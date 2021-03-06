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

		struct outbound_event_info{
			item *target;
			item::event_manager_type::key_type key;
		};

		struct item_info{
			item *target;
			std::unordered_map<unsigned __int64, outbound_event_info> outbound_events_;
		};

		~object();

		int run();

		void stop(int exit_code);

		queue &get_queue();

		const queue &get_queue() const;

		item_manager &get_item_manager();

		const item_manager &get_item_manager() const;

		std::thread::id get_id() const;

		DWORD get_local_id() const;

		bool is_thread_context() const;

		void enable_full_mouse_feature();

		void disable_full_mouse_feature();

		bool full_mouse_feature_is_enabled() const;

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

		template <typename result_type = LRESULT, typename wparam_type = WPARAM, typename lparam_type = LPARAM>
		result_type send_message(UINT message, wparam_type wparam = wparam_type(0), lparam_type lparam = lparam_type(0)) const{
			if (is_thread_context())
				return (result_type)SendMessageW(message_hwnd_, message, (WPARAM)wparam, (LPARAM)lparam);

			return queue_.compute_task([&]{
				return (result_type)SendMessageW(message_hwnd_, message, (WPARAM)wparam, (LPARAM)lparam);
			}, queue::default_task_priority, reinterpret_cast<unsigned __int64>(this));
		}

		template <typename result_type = LRESULT>
		result_type send_message(item &target, MSG &msg) const{
			if (is_thread_context())
				return (result_type)SendMessageW(message_hwnd_, WINP_WM_SEND_MESSAGE, reinterpret_cast<WPARAM>(&msg), reinterpret_cast<LPARAM>(&target));
			
			return queue_.compute_task([&]{
				return (result_type)SendMessageW(message_hwnd_, WINP_WM_SEND_MESSAGE, reinterpret_cast<WPARAM>(&msg), reinterpret_cast<LPARAM>(&target));
			}, queue::default_task_priority, reinterpret_cast<unsigned __int64>(this));
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

		template <typename wparam_type = WPARAM, typename lparam_type = LPARAM>
		bool post_message(UINT message, wparam_type wparam = wparam_type(0), lparam_type lparam = lparam_type(0)) const{
			return (PostThreadMessageW(local_id_, message, (WPARAM)wparam, (LPARAM)lparam) != FALSE);
		}

		template <typename target_type>
		target_type generate_random_integer(target_type from, target_type to){
			if (is_thread_context())
				return random_generator_(from, to);

			return queue_.compute_task([&]{
				return random_generator_(from, to);
			}, queue::default_task_priority, reinterpret_cast<unsigned __int64>(this));
		}

		template <typename target_type>
		target_type generate_random_integer(target_type from){
			if (is_thread_context())
				return random_generator_(from);

			return queue_.compute_task([&]{
				return random_generator_(from);
			}, queue::default_task_priority, reinterpret_cast<unsigned __int64>(this));
		}

		template <typename target_type>
		target_type generate_random_integer(){
			if (is_thread_context())
				return random_generator_.operator()<target_type>();

			return queue_.compute_task([&]{
				return random_generator_.operator()<target_type>();
			}, queue::default_task_priority, reinterpret_cast<unsigned __int64>(this));
		}

		float convert_pixel_to_dip_x(int x);

		float convert_pixel_to_dip_y(int y);

		void init_control(const std::wstring &class_name, DWORD control_id);

		WNDPROC get_class_entry(const std::wstring &class_name) const;

		static WNDPROC get_message_entry();

	private:
		friend class item;
		friend class item_manager;

		friend class app::object;
		friend class events::object;
		friend class events::draw;
		friend class events::non_client_paint;

		explicit object();

		void add_item_(item &item);

		void remove_item_(item &item);

		void add_timer_(const std::chrono::milliseconds &duration, const std::function<void()> &callback, unsigned __int64 id);

		void remove_timer_(unsigned __int64 id);

		WNDPROC get_class_entry_(const std::wstring &class_name) const;

		unsigned __int64 request_animation_frame_(const animation_frame_callback_type &callback);

		void cancel_animation_frame_(unsigned __int64 id);

		void animate_(const std::function<float(float)> &timing, const std::function<bool(float, bool)> &callback, const std::chrono::nanoseconds &duration);

		void animate_(const std::chrono::time_point<std::chrono::steady_clock> &start, const std::function<float(float)> &timing, const std::function<bool(float, bool)> &callback, const std::chrono::nanoseconds &duration);

		void begin_draw_();

		void end_draw_();

		void initialize_dpi_scale_();

		queue queue_;
		item_manager item_manager_;

		std::thread::id id_;
		DWORD local_id_;

		HWND message_hwnd_ = nullptr;
		std::unordered_map<unsigned __int64, item_info> items_;
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

		unsigned int begin_draw_count_ = 0u;
		events::object *current_event_ = nullptr;
		D2D1_POINT_2F dpi_scale_{};

		HRGN dummy_source_rgn_ = nullptr;
		HRGN dummy_destination_rgn_ = nullptr;
	};
}
