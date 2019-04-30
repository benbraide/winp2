#pragma once

#include "../utility/windows.h"
#include "../utility/error_code.h"
#include "../utility/random_number_generator.h"

#include "thread_item_manager.h"

namespace winp::app{
	class object;
}

namespace winp::thread{
	class object{
	public:
		using animation_frame_callback_type = std::function<void(const std::chrono::time_point<std::chrono::steady_clock> &)>;

		object();

		explicit object(app::object &app);

		~object();

		int run();

		void stop(int exit_code);

		app::object &get_app();

		const app::object &get_app() const;

		queue &get_queue();

		const queue &get_queue() const;

		item_manager &get_item_manager();

		const item_manager &get_item_manager() const;

		std::thread::id get_id() const;

		DWORD get_local_id() const;

		bool is_thread_context() const;

		unsigned __int64 request_animation_frame(const animation_frame_callback_type &callback, unsigned __int64 cancel_frame = 0u);

		void cancel_animation_frame(unsigned __int64 id);

		void animate(const std::function<float(float)> &timing, const std::function<bool(float)> &callback, const std::chrono::microseconds &duration);

		void animate(const std::function<float(float)> &timing, const std::function<bool(float, bool)> &callback, const std::chrono::microseconds &duration);

		void discard_d2d_resources();

		ID2D1Factory *get_draw_factory() const;

		IDWriteFactory *get_write_factory() const;

		IDWriteGdiInterop *get_write_interop() const;

		ID2D1DCRenderTarget *get_device_render_target() const;

		ID2D1SolidColorBrush *get_color_brush() const;

		events::object &get_current_event() const;

		template <typename target_type>
		target_type &get_current_event_as() const{
			return dynamic_cast<target_type &>(get_current_event());
		}

		HWND get_message_handle() const;

		template <typename wparam_type = WPARAM, typename lparam_type = LPARAM>
		bool post_message(UINT message, wparam_type wparam = wparam_type(0), lparam_type lparam = lparam_type(0)) const{
			return (PostThreadMessageW(local_id_, message, (WPARAM)wparam, (LPARAM)lparam) != FALSE);
		}

		template <typename result_type = LRESULT, typename wparam_type = WPARAM, typename lparam_type = LPARAM>
		result_type send_message(UINT message, wparam_type wparam = wparam_type(0), lparam_type lparam = lparam_type(0)) const{
			return (result_type)SendMessageW(message_hwnd_, message, (WPARAM)wparam, (LPARAM)lparam);
		}

		template <typename result_type = LRESULT>
		result_type send_message(item &target, MSG &msg) const{
			return (result_type)SendMessageW(message_hwnd_, WINP_WM_SEND_MESSAGE, reinterpret_cast<WPARAM>(&msg), reinterpret_cast<LPARAM>(&target));
		}

		template <typename result_type = LRESULT, typename wparam_type = WPARAM, typename lparam_type = LPARAM>
		result_type send_message(item &target, UINT message, wparam_type wparam = wparam_type(0), lparam_type lparam = lparam_type(0)) const{
			MSG msg{ nullptr, message, (WPARAM)wparam, (LPARAM)lparam };
			return send_message<result_type>(target, msg);
		}

		bool post_message(item &target, const MSG &msg) const;

		template <typename wparam_type = WPARAM, typename lparam_type = LPARAM>
		bool post_message(item &target, UINT message, wparam_type wparam = wparam_type(0), lparam_type lparam = lparam_type(0)) const{
			return (PostMessageW(message_hwnd_, WINP_WM_POST_MESSAGE, reinterpret_cast<WPARAM>(new MSG{ nullptr, message, (WPARAM)wparam, (LPARAM)lparam }), reinterpret_cast<LPARAM>(&target)) != FALSE);
		}

		template <typename target_type>
		target_type generate_random_integer(target_type from, target_type to){
			if (is_thread_context())
				return random_generator_(from, to);

			return queue_.compute_task([&]{
				return random_generator_(from, to);
			}, queue::default_task_priority, queue::default_task_id);
		}

		template <typename target_type>
		target_type generate_random_integer(target_type from_min_to){
			if (is_thread_context())
				return random_generator_(from_min_to);

			return queue_.compute_task([&]{
				return random_generator_(from_min_to);
			}, queue::default_task_priority, queue::default_task_id);
		}

		template <typename target_type>
		target_type generate_random_integer(){
			if (is_thread_context())
				return random_generator_.operator()<target_type>();

			return queue_.compute_task([&]{
				return random_generator_.operator()<target_type>();
			}, queue::default_task_priority, queue::default_task_id);
		}

		void init_control(const std::wstring &class_name, DWORD control_id);

		WNDPROC get_class_entry(const std::wstring &class_name) const;

		static WNDPROC get_message_entry();

	private:
		friend class item;
		friend class item_manager;

		friend class app::object;
		friend class events::object;

		void add_item_(item &item);

		void remove_item_(unsigned __int64 id);

		WNDPROC get_class_entry_(const std::wstring &class_name) const;

		unsigned __int64 request_animation_frame_(const animation_frame_callback_type &callback);

		void cancel_animation_frame_(unsigned __int64 id);

		void animate_(const std::function<float(float)> &timing, const std::function<bool(float, bool)> &callback, const std::chrono::nanoseconds &duration);

		void animate_(const std::chrono::time_point<std::chrono::steady_clock> &start, const std::function<float(float)> &timing, const std::function<bool(float, bool)> &callback, const std::chrono::nanoseconds &duration);

		app::object &app_;
		queue queue_;
		item_manager item_manager_;

		std::thread::id id_;
		DWORD local_id_;

		HWND message_hwnd_ = nullptr;
		std::unordered_map<unsigned __int64, item *> items_;
		utility::random_integral_number_generator random_generator_;

		bool running_animation_loop_ = false;
		unsigned __int64 animation_frame_id_ = 0;
		std::unordered_map<unsigned __int64, animation_frame_callback_type> animation_callbacks_;

		mutable ID2D1Factory *draw_factory_ = nullptr;
		mutable IDWriteFactory *write_factory_ = nullptr;
		mutable IDWriteGdiInterop *write_interop_ = nullptr;

		mutable ID2D1DCRenderTarget *device_render_target_ = nullptr;
		mutable ID2D1SolidColorBrush *color_brush_ = nullptr;

		DWORD control_ids_ = 0;
		mutable std::unordered_map<std::wstring, WNDPROC> class_info_map_;

		events::object *current_event_ = nullptr;
	};
}
