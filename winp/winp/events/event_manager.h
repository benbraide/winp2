#pragma once

#include <any>

#include "../utility/traits.h"

#include "event_object.h"
#include "general_events.h"
#include "io_events.h"
#include "menu_events.h"
#include "control_events.h"

namespace winp::events{
	class handler_base{
	public:
		virtual void call(object &object) const = 0;

		virtual const std::type_info *get_type_info() const = 0;
	};

	template <class object_type, typename return_type>
	class handler : public handler_base{
	public:
		using m_return_type = return_type;
		using m_object_type = object_type;
		using callback_type = std::function<return_type(m_object_type)>;

		explicit handler(const callback_type &callback)
			: callback_(callback){}

		virtual void call(object &object) const override{
			call_<return_type>(object, std::bool_constant<std::is_void_v<return_type>>());
		}

		virtual const std::type_info *get_type_info() const override{
			return &typeid(object_type);
		}

	private:
		template <typename unused>
		void call_(object &object, std::true_type) const{
			callback_(dynamic_cast<std::remove_cv_t<std::remove_reference_t<m_object_type>> &>(object));
		}

		template <typename unused>
		void call_(object &object, std::false_type) const{
			object.set_result(callback_(dynamic_cast<std::remove_cv_t<std::remove_reference_t<m_object_type>> &>(object)));
		}

		callback_type callback_;
	};

	template <class owner_type>
	class manager{
	public:
		using m_owner_type = owner_type;

		struct handler_info{
			unsigned __int64 id;
			std::shared_ptr<handler_base> handler;
		};

		using key_type = unsigned __int64;
		using handler_list_type = std::list<handler_info>;
		using map_type = std::unordered_map<key_type, std::pair<handler_list_type, unsigned int>>;

		template <class return_type, class object_type, std::size_t>
		struct bind_helper;

		template <class return_type, class object_type>
		struct bind_helper<return_type, object_type, 1u>{
			static unsigned __int64 bind(manager &m, const std::function<return_type(object_type &)> &handler){
				return m.bind_<return_type, object_type>(handler);
			}

			static unsigned __int64 bind(manager &m, const std::function<return_type(const object_type &)> &handler){
				return m.bind_<return_type, object_type>(handler);
			}
		};

		template <class return_type>
		struct bind_helper<return_type, void, 1u>{
			template <typename object_type>
			static unsigned __int64 bind(manager &m, const std::function<return_type(object_type &)> &handler){
				return m.bind_<return_type, object_type>(handler);
			}

			template <typename object_type>
			static unsigned __int64 bind(manager &m, const std::function<return_type(const object_type &)> &handler){
				return m.bind_<return_type, object_type>(handler);
			}
		};

		template <class return_type, class object_type>
		struct bind_helper<return_type, object_type, 0u>{
			static unsigned __int64 bind(manager &m, const std::function<return_type()> &handler){
				return bind_helper<return_type, object_type, 1u>::template bind(m, [handler](object_type &){
					return handler();
				});
			}
		};

		template <class return_type>
		struct bind_helper<return_type, void, 0u>{
			static unsigned __int64 bind(manager &m, const std::function<return_type()> &handler){
				return bind_helper<return_type, unhandled, 1u>::template bind(m, [handler](unhandled &){
					return handler();
				});
			}
		};

		explicit manager(m_owner_type &owner)
			: owner_(owner){}

		~manager(){
			count_ = 0u;
		}

		const m_owner_type &get_owner() const{
			return owner_;
		}

		m_owner_type &get_owner(){
			return owner_;
		}
		
		template <typename object_type>
		bool is_supported() const{
			return owner_.compute_task_inside_thread_context([&]{
				return owner_.event_is_supported_(get_key<object_type>());
			});
		}

		template <typename handler_type>
		unsigned __int64 bind(const handler_type &handler){
			return owner_.compute_task_inside_thread_context([&]{
				using traits_type = utility::object_to_function_traits::traits<handler_type>;
				return bind_helper<typename traits_type::m_return_type, void, traits_type::template arg_count>::template bind(*this, utility::object_to_function_traits::get(handler));
			});
		}

		template <typename object_type, typename handler_type>
		unsigned __int64 bind(const handler_type &handler){
			return owner_.compute_task_inside_thread_context([&]{
				using traits_type = utility::object_to_function_traits::traits<handler_type>;
				return bind_helper<typename traits_type::m_return_type, object_type, traits_type::template arg_count>::template bind(*this, utility::object_to_function_traits::get(handler));
			});
		}

		void unbind(unsigned __int64 id){
			owner_.execute_task_inside_thread_context([&]{
				unbind_(id);
			});
		}

		template <typename object_type>
		std::size_t get_bound_count() const{
			return owner_.compute_task_inside_thread_context([&]{
				if (auto it = handlers_.find(get_key<object_type>()); it != handlers_.end())
					return it->second.first.size();
				return static_cast<std::size_t>(0);
			});
		}

		template <typename object_type>
		unsigned int get_states() const{
			return owner_.compute_task_inside_thread_context([&]{
				if (auto it = handlers_.find(get_key<object_type>()); it != handlers_.end())
					return it->second.second;
				return state_nil;
			});
		}

		static key_type get_key(const std::type_info &info){
			return reinterpret_cast<key_type>(&info);
		}

		template <typename object_type>
		static key_type get_key(object_type &e){
			return get_key(typeid(e));
		}

		template <typename object_type>
		static key_type get_key(){
			return get_key(typeid(object_type));
		}

		template <typename object_type, typename... others>
		static bool is_equal_key(key_type key){
			return is_equal_key_<object_type, others...>(key);
		}

		static constexpr unsigned int state_nil							= (0u << 0x0000);
		static constexpr unsigned int state_disable_bounding			= (1u << 0x0000);
		static constexpr unsigned int state_disable_triggering			= (1u << 0x0001);

	private:
		friend owner_type;
		
		template <typename return_type, typename object_type>
		unsigned __int64 bind_(const std::function<return_type(object_type &)> &handler){
			auto key = get_key<object_type>();
			if (!owner_.adding_event_handler_(*this, key))
				return 0u;//Rejected

			auto handler_object = std::make_shared<events::handler<object_type &, return_type>>(handler);
			if (handler_object == nullptr)
				return 0u;

			auto id = reinterpret_cast<unsigned __int64>(handler_object.get());
			auto &handler_list = handlers_[key];

			if ((handler_list.second & state_disable_bounding) != 0u)
				return 0u;//Bounding disabled

			handler_list.first.push_back(handler_info{
				id,
				handler_object
			});

			++count_;
			owner_.added_event_handler_(*this, key, id);

			activity e(key, (handler_list.first.size() - 1u), handler_list.first.size(), owner_);
			trigger_(e, 0u);

			return id;
		}
		
		template <typename return_type, typename object_type>
		unsigned __int64 bind_(const std::function<return_type(const object_type &)> &handler){
			return bind_<return_type, object_type>([handler](object_type &e){
				return handler(e);
			});
		}

		void unbind_(unsigned __int64 id){
			if (count_ == 0u || handlers_.empty())
				return;

			for (auto &info : handlers_){
				for (auto it = info.second.first.begin(); it != info.second.first.end(); ++it){
					if (it->id != id)
						continue;

					auto key = get_key(*it->handler->get_type_info());
					info.second.first.erase(it);

					--count_;
					owner_.removed_event_handler_(*this, key, id);

					activity e(key, (info.second.first.size() + 1u), info.second.first.size(), owner_);
					trigger_(e, 0u);

					return;
				}
			}
		}

		void trigger_(object &e, unsigned __int64 id) const{
			if (count_ == 0u || handlers_.empty())
				return;

			auto it = handlers_.find(get_key(e));
			if (it == handlers_.end())
				return;

			if ((it->second.second & state_disable_triggering) != 0u)
				return;//Triggering disabled

			std::size_t index = 0u;
			std::list<std::size_t> unbind_list;

			for (auto &handler_info : it->second.first){
				if (id != 0u && handler_info.id != id)
					continue;

				handler_info.handler->call(e);
				if ((e.states_ & object::state_unbind_on_exit) != 0u){//Unbind
					unbind_list.push_back(index);
					e.states_ &= ~object::state_unbind_on_exit;
				}

				++index;
				if (id != 0u || (e.states_ & object::state_propagation_stopped) != 0u)
					break;//Propagation stopped
			}

			for (auto unbind_item : unbind_list)
				it->second.first.erase(std::next(it->second.first.begin(), unbind_item));
		}

		template <typename object_type>
		void set_state_(unsigned int state, bool combine){
			if (auto it = handlers_.find(get_key<object_type>()); it != handlers_.end()){
				if (combine)
					it->second.second |= state;
				else//Replace
					it->second.second = state;
			}
		}

		template <typename first_object_type, typename second_object_type, typename... other_object_types>
		void set_state_(unsigned int state, bool combine){
			set_state_<first_object_type>(state, combine);
			set_state_<second_object_type, other_object_types...>(state, combine);
		}

		template <typename object_type>
		void remove_state_(unsigned int state){
			if (auto it = handlers_.find(get_key<object_type>()); it != handlers_.end())
				it->second.second &= ~state;
		}

		template <typename first_object_type, typename second_object_type, typename... other_object_types>
		void remove_state_(unsigned int state){
			remove_state_<first_object_type>(state);
			remove_state_<second_object_type, other_object_types...>(state);
		}

		template <typename object_type>
		unsigned int get_states_() const{
			if (auto it = handlers_.find(get_key<object_type>()); it != handlers_.end())
				return it->second.second;
			return state_nil;
		}

		template <typename object_type>
		static bool is_equal_key_(key_type key){
			return (get_key<object_type>() == key);
		}

		template <typename first_object_type, typename second_object_type, typename... others>
		static bool is_equal_key_(key_type key){
			return (is_equal_key_<first_object_type>(key) || is_equal_key_<second_object_type, others...>(key));
		}

		m_owner_type &owner_;
		std::size_t count_ = 0u;
		mutable map_type handlers_;
	};
}
