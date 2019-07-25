#pragma once

#include "ui_hook.h"

namespace winp::ui{
	class auto_create_hook : public hook{
	public:
		explicit auto_create_hook(object &target);

		virtual ~auto_create_hook();
	};

	class animation_hook : public hook{
	public:
		using easing_type = std::function<float(float)>;
		using key_type = thread::item::event_manager_type::key_type;

		struct key_info{
			bool is_allowed;
			bool is_active;

			easing_type easing;
			std::chrono::microseconds duration;

			std::any value;
			unsigned __int64 count;
		};

		explicit animation_hook(object &target);

		animation_hook(object &target, const easing_type &easing);

		animation_hook(object &target, const std::chrono::microseconds &duration);

		animation_hook(object &target, const easing_type &easing, const std::chrono::microseconds &duration);

		virtual ~animation_hook();

		virtual utility::error_code set_easing(const easing_type &value, const std::function<void(animation_hook &, utility::error_code)> &callback = nullptr);

		utility::error_code set_easing(key_type key, const easing_type &value, const std::function<void(animation_hook &, utility::error_code)> &callback = nullptr);

		template <typename target_type>
		utility::error_code set_easing(const easing_type &value, const std::function<void(animation_hook &, utility::error_code)> &callback = nullptr){
			return set_easing(thread::item::event_manager_type::get_key<target_type>(), value, callback);
		}

		virtual const easing_type &get_easing(const std::function<void(const easing_type &)> &callback = nullptr) const;

		virtual const easing_type &get_easing(key_type key, const std::function<void(const easing_type &)> &callback = nullptr) const;

		template <typename target_type>
		const easing_type &get_easing(const std::function<void(const easing_type &)> &callback = nullptr) const{
			return get_easing(thread::item::event_manager_type::get_key<target_type>(), callback);
		}

		virtual utility::error_code set_duration(const std::chrono::microseconds &value, const std::function<void(animation_hook &, utility::error_code)> &callback = nullptr);

		utility::error_code set_duration(key_type key, const std::chrono::microseconds &value, const std::function<void(animation_hook &, utility::error_code)> &callback = nullptr);

		template <typename target_type>
		utility::error_code set_duration(const std::chrono::microseconds &value, const std::function<void(animation_hook &, utility::error_code)> &callback = nullptr){
			return set_duration(thread::item::event_manager_type::get_key<target_type>(), value, callback);
		}

		virtual const std::chrono::microseconds &get_duration(const std::function<void(const std::chrono::microseconds &)> &callback = nullptr) const;

		virtual const std::chrono::microseconds &get_duration(key_type key, const std::function<void(const std::chrono::microseconds &)> &callback = nullptr) const;

		template <typename target_type>
		const std::chrono::microseconds &get_duration(const std::function<void(const std::chrono::microseconds &)> &callback = nullptr) const{
			return get_duration(thread::item::event_manager_type::get_key<target_type>(), callback);
		}

		virtual void set_allowed_state(bool is_allowed);

		template <typename target_type>
		void set_allowed_state(bool is_allowed){
			set_allowed_state(thread::item::event_manager_type::get_key<target_type>(), is_allowed);
		}

		virtual void set_allowed_state(key_type key, bool is_allowed);

		virtual bool is_allowed() const;

		template <typename target_type>
		bool is_allowed() const{
			return is_allowed(thread::item::event_manager_type::get_key<target_type>());
		}

		virtual bool is_allowed(key_type key) const;

		template <typename target_type>
		bool is_active() const{
			return is_active(thread::item::event_manager_type::get_key<target_type>());
		}

		virtual bool is_active(key_type key) const;

		virtual bool is_active() const;

	protected:
		friend class object;

		virtual utility::error_code set_easing_(const easing_type &value);

		virtual utility::error_code set_easing_(key_type key, const easing_type &value);

		template <typename target_type>
		const easing_type &get_easing_() const{
			get_easing_(thread::item::event_manager_type::get_key<target_type>());
		}

		virtual const easing_type &get_easing_(key_type key) const;

		virtual utility::error_code set_duration_(const std::chrono::microseconds &value);

		virtual utility::error_code set_duration_(key_type key, const std::chrono::microseconds &value);

		template <typename target_type>
		const std::chrono::microseconds &get_duration_() const{
			get_duration_(thread::item::event_manager_type::get_key<target_type>());
		}

		virtual const std::chrono::microseconds &get_duration_(key_type key) const;

		template <typename target_type>
		void cancel_(){
			return cancel_(thread::item::event_manager_type::get_key<target_type>());
		}

		void cancel_(key_type key);

		template <typename target_type>
		key_info &get_() const{
			return get_(thread::item::event_manager_type::get_key<target_type>());
		}

		key_info &get_(key_type key) const;

		template <typename target_type>
		key_info *get_existing_() const{
			return get_existing_(thread::item::event_manager_type::get_key<target_type>());
		}

		key_info *get_existing_(key_type key) const;

		easing_type easing_;
		std::chrono::microseconds duration_;

		bool is_allowed_ = true;
		mutable std::unordered_map<key_type, key_info> info_list_;
	};

	class animation_suppression_hook : public hook{
	public:
		using key_type = thread::item::event_manager_type::key_type;

		explicit animation_suppression_hook(object &target, bool once = true);

		virtual ~animation_suppression_hook();

		virtual bool is_once(const std::function<void(bool)> &callback = nullptr) const;

		template <typename target_type>
		void suppress_type(){
			suppress_type(thread::item::event_manager_type::get_key<target_type>());
		}

		virtual void suppress_type(key_type key);

		template <typename target_type>
		void unsuppress_type(){
			unsuppress_type(thread::item::event_manager_type::get_key<target_type>());
		}

		virtual void unsuppress_type(key_type key);

		template <typename target_type>
		bool type_is_suppressed() const{
			return type_is_suppressed(thread::item::event_manager_type::get_key<target_type>());
		}

		virtual bool type_is_suppressed(key_type key) const;

	protected:
		bool once_;
		std::unordered_map<key_type, bool> suppressed_list_;
	};
}
