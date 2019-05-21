#include "../app/app_object.h"

winp::thread::object::object()
	: queue_(*this), item_manager_(*this, GetCurrentThreadId()), id_(std::this_thread::get_id()), local_id_(GetCurrentThreadId()){
	app::object::current_thread_ = this;
	message_hwnd_ = CreateWindowExW(0, app::object::get_class_name().data(), L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, GetModuleHandleW(nullptr), nullptr);
}

winp::thread::object::~object(){
	queue_.add_id_to_black_list(reinterpret_cast<unsigned __int64>(this));
}

int winp::thread::object::run(){
	if (!is_thread_context())
		throw utility::error_code::outside_thread_context;

	std::thread([this]{//Run animation loop
		running_animation_loop_ = true;

		DEVMODEW info{};
		info.dmSize = sizeof(DEVMODEW);
		info.dmDriverExtra = 0;

		DWORD refresh_rate = 60;
		if (EnumDisplaySettingsW(NULL, ENUM_CURRENT_SETTINGS, &info) != FALSE)
			refresh_rate = info.dmDisplayFrequency;

		auto interval = (1000000 / refresh_rate);
		while (running_animation_loop_){
			std::this_thread::sleep_for(std::chrono::microseconds(interval));
			if (!running_animation_loop_)
				break;

			queue_.post_task([this]{
				if (!running_animation_loop_ || animation_callbacks_.empty())
					return;//Ignore

				auto animation_callbacks = animation_callbacks_;
				animation_callbacks_.clear();

				for (auto &entry : animation_callbacks)
					entry.second(std::chrono::high_resolution_clock::now());

			}, queue::urgent_task_priority, reinterpret_cast<unsigned __int64>(this));
		}
	}).detach();

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

		if (GetMessageW(&msg, nullptr, 0u, 0u) == -1){
			running_animation_loop_ = false;
			throw utility::error_code::thread_get_message_failure;
		}

		if (msg.message == WM_QUIT){
			running_animation_loop_ = false;
			return static_cast<int>(msg.wParam);
		}

		if (msg.hwnd == nullptr || !item_manager_.is_dialog_message_(msg)){
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	running_animation_loop_ = false;
	return 0;
}

void winp::thread::object::stop(int exit_code){
	if (is_thread_context())
		PostQuitMessage(exit_code);
	else//Post message to thread queue
		post_message(WM_QUIT, exit_code);
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

unsigned __int64 winp::thread::object::request_animation_frame(const animation_frame_callback_type &callback, unsigned __int64 cancel_frame){
	return queue_.compute_task([&]{
		cancel_animation_frame_(cancel_frame);
		return request_animation_frame_(callback);
	}, queue::urgent_task_priority, reinterpret_cast<unsigned __int64>(this));
}

void winp::thread::object::cancel_animation_frame(unsigned __int64 id){
	queue_.post_task([=]{
		cancel_animation_frame_(id);
	}, queue::urgent_task_priority, reinterpret_cast<unsigned __int64>(this));
}

void winp::thread::object::animate(const std::function<float(float)> &timing, const std::function<bool(float)> &callback, const std::chrono::microseconds &duration){
	animate(timing, [=](float progress, bool){
		return callback(progress);
	}, duration);
}

void winp::thread::object::animate(const std::function<float(float)> &timing, const std::function<bool(float, bool)> &callback, const std::chrono::microseconds &duration){
	queue_.execute_task([&]{
		animate_(timing, callback, duration);
	}, queue::urgent_task_priority, reinterpret_cast<unsigned __int64>(this));
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

winp::events::object &winp::thread::object::get_current_event() const{
	if (!is_thread_context())
		throw utility::error_code::outside_thread_context;

	return *current_event_;
}

HWND winp::thread::object::get_message_handle() const{
	return message_hwnd_;
}

bool winp::thread::object::post_message(item &target, const MSG &msg) const{
	return (PostMessageW(message_hwnd_, WINP_WM_POST_MESSAGE, reinterpret_cast<WPARAM>(new MSG(msg)), reinterpret_cast<LPARAM>(&target)) != FALSE);
}

float winp::thread::object::convert_pixel_to_dip_x(int x){
	if (dpi_scale_.x == 0.0f)
		initialize_dpi_scale_();
	return ((dpi_scale_.x == 0.0f) ? static_cast<float>(x) : (x / dpi_scale_.x));
}

float winp::thread::object::convert_pixel_to_dip_y(int y){
	if (dpi_scale_.y == 0.0f)
		initialize_dpi_scale_();
	return ((dpi_scale_.y == 0.0f) ? static_cast<float>(y) : (y / dpi_scale_.y));
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

void winp::thread::object::add_timer_(const std::chrono::milliseconds &duration, const std::function<void()> &callback, unsigned __int64 id){
	item_manager_.add_timer_(duration, callback, id);
}

void winp::thread::object::remove_timer_(unsigned __int64 id){
	item_manager_.remove_timer_(id);
}

WNDPROC winp::thread::object::get_class_entry_(const std::wstring &class_name) const{
	if (&class_name == &app::object::get_class_name())
		return DefWindowProcW;

	if (auto it = class_info_map_.find(class_name); it != class_info_map_.end())
		return it->second;

	if (WNDCLASSEXW class_info{ sizeof(WNDCLASSEXW) }; GetClassInfoExW(nullptr, class_name.data(), &class_info) != FALSE){
		class_info_map_[class_name] = class_info.lpfnWndProc;
		return class_info.lpfnWndProc;
	}

	return nullptr;
}

unsigned __int64 winp::thread::object::request_animation_frame_(const animation_frame_callback_type &callback){
	animation_callbacks_[++animation_frame_id_] = callback;
	return animation_frame_id_;
}

void winp::thread::object::cancel_animation_frame_(unsigned __int64 id){
	if (!animation_callbacks_.empty())
		animation_callbacks_.erase(id);
}

void winp::thread::object::animate_(const std::function<float(float)> &timing, const std::function<bool(float, bool)> &callback, const std::chrono::nanoseconds &duration){
	if (duration.count() == 0)
		callback(1.0f, false);
	else if (callback(timing(0.0f), true))
		animate_(std::chrono::high_resolution_clock::now(), timing, callback, duration);
}

void winp::thread::object::animate_(const std::chrono::time_point<std::chrono::steady_clock> &start, const std::function<float(float)> &timing, const std::function<bool(float, bool)> &callback, const std::chrono::nanoseconds &duration){
	request_animation_frame_([=](const std::chrono::time_point<std::chrono::steady_clock> &time){
		auto elapsed_time = (time - start);
		if (duration.count() <= elapsed_time.count()){
			callback(1.0f, false);
			return;
		}

		auto time_fraction = (static_cast<float>(elapsed_time.count()) / duration.count());
		auto progress = timing(time_fraction);

		if (callback(progress, true))
			animate_(start, timing, callback, duration);
	});
}

void winp::thread::object::begin_draw_(){
	if (++begin_draw_count_ == 1u){
		if (auto render_target = get_device_render_target(); render_target != nullptr)
			render_target->BeginDraw();
	}
}

void winp::thread::object::end_draw_(){
	if (0u < begin_draw_count_ && --begin_draw_count_ == 0u && device_render_target_ != nullptr){
		switch (device_render_target_->EndDraw()){
		case D2DERR_RECREATE_TARGET:
			discard_d2d_resources();
			break;
		default:
			break;
		}
	}
}

void winp::thread::object::initialize_dpi_scale_(){
	if (draw_factory_ == nullptr)
		D2D1CreateFactory(D2D1_FACTORY_TYPE::D2D1_FACTORY_TYPE_SINGLE_THREADED, &draw_factory_);

	if (draw_factory_ == nullptr)
		return;

	float dpi_x, dpi_y;
	draw_factory_->GetDesktopDpi(&dpi_x, &dpi_y);

	dpi_scale_.x = (dpi_x / 96.0f);
	dpi_scale_.y = (dpi_y / 96.0f);
}
