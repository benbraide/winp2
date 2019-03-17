#include "../app/app_collection.h"

winp::thread::object::object()
	: object(*((app::collection::get_main() == nullptr) ? app::collection::get_current_thread_app() : app::collection::get_main())){}

winp::thread::object::object(app::object &app)
	: app_(app), queue_(*this), item_manager_(*this, GetCurrentThreadId()), id_(std::this_thread::get_id()), local_id_(GetCurrentThreadId()){
	app::object::current_thread_ = this;
	app_.add_thread_(*this);
	message_hwnd_ = CreateWindowExW(0, app_.get_class_name().data(), L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, GetModuleHandleW(nullptr), nullptr);
}

winp::thread::object::~object(){
	app_.remove_thread_(local_id_);
	app::object::current_thread_ = nullptr;
}

int winp::thread::object::run(){
	if (!is_thread_context())
		throw utility::error_code::outside_thread_context;

	MSG msg{};
	while (!item_manager_.top_level_windows_.empty()){
		if (queue_.run_next_(queue::urgent_task_priority))
			continue;//Task ran

		if (auto peek_status = PeekMessageW(&msg, nullptr, 0u, 0u, PM_NOREMOVE); peek_status != FALSE){//Message found in queue
			if ((msg.message == WM_TIMER || msg.message == WM_PAINT || msg.message == WM_NCPAINT) && queue_.run_next_(queue::default_task_priority, queue::condition_type::less, true))
				continue;
		}
		else if (queue_.run_next_())
			continue;//Task ran

		if (GetMessageW(&msg, nullptr, 0u, 0u) == -1)
			throw utility::error_code::thread_get_message_failure;

		if (msg.message == WM_QUIT)
			return static_cast<int>(msg.wParam);

		if (msg.hwnd == nullptr || !item_manager_.is_dialog_message_(msg)){
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	return 0;
}

void winp::thread::object::stop(int exit_code){
	if (is_thread_context())
		PostQuitMessage(exit_code);
	else//Post message to thread queue
		post_message(WM_QUIT, exit_code);
}

winp::app::object &winp::thread::object::get_app(){
	return app_;
}

const winp::app::object &winp::thread::object::get_app() const{
	return app_;
}

winp::thread::queue &winp::thread::object::get_queue(){
	return queue_;
}

const winp::thread::queue &winp::thread::object::get_queue() const{
	return queue_;
}

winp::thread::item_manager &winp::thread::object::get_item_manager(){
	return item_manager_;
}

const winp::thread::item_manager &winp::thread::object::get_item_manager() const{
	return item_manager_;
}

std::thread::id winp::thread::object::get_id() const{
	return id_;
}

DWORD winp::thread::object::get_local_id() const{
	return local_id_;
}

bool winp::thread::object::is_thread_context() const{
	return (GetCurrentThreadId() == local_id_);
}

void winp::thread::object::discard_d2d_resources(){
	if (!is_thread_context())
		throw utility::error_code::outside_thread_context;

	if (color_brush_ != nullptr){
		color_brush_->Release();
		color_brush_ = nullptr;
	}

	if (device_render_target_ != nullptr){
		device_render_target_->Release();
		device_render_target_ = nullptr;
	}

	if (write_interop_ != nullptr){
		write_interop_->Release();
		write_interop_ = nullptr;
	}
}

ID2D1Factory *winp::thread::object::get_draw_factory() const{
	if (!is_thread_context())
		throw utility::error_code::outside_thread_context;

	if (draw_factory_ == nullptr)
		D2D1CreateFactory(D2D1_FACTORY_TYPE::D2D1_FACTORY_TYPE_SINGLE_THREADED, &draw_factory_);

	return draw_factory_;
}

IDWriteFactory *winp::thread::object::get_write_factory() const{
	if (!is_thread_context())
		throw utility::error_code::outside_thread_context;

	if (write_factory_ == nullptr)
		DWriteCreateFactory(DWRITE_FACTORY_TYPE::DWRITE_FACTORY_TYPE_ISOLATED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&write_factory_));

	return write_factory_;
}

IDWriteGdiInterop *winp::thread::object::get_write_interop() const{
	if (auto factory = get_write_factory(); write_interop_ == nullptr && factory != nullptr)
		factory->GetGdiInterop(&write_interop_);
	return write_interop_;
}

ID2D1DCRenderTarget *winp::thread::object::get_device_render_target() const{
	if (auto factory = get_draw_factory(); device_render_target_ == nullptr && factory != nullptr){
		auto props = D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(
				DXGI_FORMAT_B8G8R8A8_UNORM,
				D2D1_ALPHA_MODE_PREMULTIPLIED
			),
			0,
			0,
			D2D1_RENDER_TARGET_USAGE_NONE,
			D2D1_FEATURE_LEVEL_DEFAULT
		);

		factory->CreateDCRenderTarget(&props, &device_render_target_);
	}

	return device_render_target_;
}

ID2D1SolidColorBrush *winp::thread::object::get_color_brush() const{
	if (auto render_target = get_device_render_target(); color_brush_ == nullptr && render_target != nullptr){
		render_target->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black, 1.0f),
			D2D1::BrushProperties(),
			&color_brush_
		);
	}

	return color_brush_;
}

HWND winp::thread::object::get_message_handle() const{
	return message_hwnd_;
}

bool winp::thread::object::post_message(item &target, const MSG &msg) const{
	return (PostMessageW(message_hwnd_, WINP_WM_POST_MESSAGE, reinterpret_cast<WPARAM>(new MSG(msg)), reinterpret_cast<LPARAM>(&target)) != FALSE);
}

void winp::thread::object::init_control(const std::wstring &class_name, DWORD control_id){
	queue_.execute_task([&]{
		if (class_info_map_.find(class_name) == class_info_map_.end()){
			WNDCLASSEXW class_info{ sizeof(WNDCLASSEXW) };
			if (GetClassInfoExW(nullptr, class_name.data(), &class_info) != FALSE)
				class_info_map_[class_name] = class_info.lpfnWndProc;
		}

		if ((control_ids_ & control_id) == 0u){
			INITCOMMONCONTROLSEX info{
				sizeof(INITCOMMONCONTROLSEX),
				control_id
			};

			if (InitCommonControlsEx(&info) != FALSE)
				control_ids_ |= control_id;
		}
	}, queue::urgent_task_priority, 0);
}

WNDPROC winp::thread::object::get_class_entry(const std::wstring &class_name) const{
	return queue_.compute_task([&]() -> WNDPROC{
		return get_class_entry_(class_name);
	}, queue::urgent_task_priority, 0);
}

WNDPROC winp::thread::object::get_message_entry(){
	return item_manager::entry_;
}

void winp::thread::object::add_item_(item &item){
	if (&item.thread_ == this && is_thread_context())
		items_[item.id_] = &item;
	else
		throw utility::error_code::outside_thread_context;
}

void winp::thread::object::remove_item_(unsigned __int64 id){
	if (!is_thread_context())
		throw utility::error_code::outside_thread_context;

	if (!items_.empty())//Inside thread context
		items_.erase(id);
}

WNDPROC winp::thread::object::get_class_entry_(const std::wstring &class_name) const{
	if (&class_name == &app_.get_class_name())
		return DefWindowProcW;

	if (auto it = class_info_map_.find(class_name); it != class_info_map_.end())
		return it->second;

	if (WNDCLASSEXW class_info{ sizeof(WNDCLASSEXW) }; GetClassInfoExW(nullptr, class_name.data(), &class_info) != FALSE){
		class_info_map_[class_name] = class_info.lpfnWndProc;
		return class_info.lpfnWndProc;
	}

	return nullptr;
}
