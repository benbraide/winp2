#pragma once

#include <any>

#include "../utility/traits.h"
#include "../utility/random_number_generator.h"

#include "event_object.h"

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

		using list_type = std::list<handler_info>;
		using map_type = std::unordered_map<const std::type_info *, list_type>;
		using change_map_type = std::unordered_map<const std::type_info *, std::list<std::function<void(std::size_t, std::size_t)>>>;

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

		/*template <typename handler_type>
		unsigned __int64 bind(const handler_type &handler){
			return owner_.compute_task_inside_thread_context([&]{
				return bind_<typename utility::object_to_function_traits::traits<handler_type>::m_return_type>(utility::object_to_function_traits::get(handler));
			});
		}
		
		template <typename object_type>
		unsigned __int64 bind(const std::function<void()> &handler){
			return bind([handler](object_type &){
				handler();
			});
		}*/

		void unbind(unsigned __int64 id){
			owner_.execute_task_inside_thread_context([&]{
				unbind_(id);
			});
		}

	private:
		friend owner_type;
		
		template <typename return_type, typename object_type>
		unsigned __int64 bind_(const std::function<return_type(object_type &)> &handler){
			unsigned __int64 id = random_generator_;
			handlers_[&typeid(object_type)].push_back(handler_info{
				id,
				std::make_shared<events::handler<object_type &, return_type>>(handler)
			});

			++count_;
			if (auto it = change_handlers_.find(&typeid(object_type)); it != change_handlers_.end()){//Call handlers
				for (auto &handler : it->second)
					handler(handlers_.size(), (handlers_.size() - 1u));
			}

			return id;
		}
		
		template <typename return_type, typename object_type>
		unsigned __int64 bind_(const std::function<return_type(const object_type &)> &handler){
			return bind_<return_type, object_type>([handler](object_type &e){
				return handler(e);
			});
		}

		void unbind_(unsigned __int64 id){
			if (count_ == 0u)
				return;

			for (auto &info : handlers_){
				for (auto it = info.second.begin(); it != info.second.end(); ++it){
					if (it->id != id)
						continue;

					info.second.erase(it);
					--count_;

					if (auto it = change_handlers_.find(info.first); it != change_handlers_.end()){//Call handlers
						for (auto &handler : it->second)
							handler(info.second.size(), (info.second.size() + 1u));
					}

					return;
				}
			}
		}

		void trigger_(object &e) const{
			if (count_ == 0u)
				return;

			if (auto it = handlers_.find(&typeid(e)); it != handlers_.end()){
				for (auto &handler_info : it->second){
					handler_info.handler->call(e);
					if ((e.get_states() & object::state_propagation_stopped) != 0u)
						break;//Propagation stopped
				}
			}
		}

		template <typename object_type>
		void add_change_handler_(const std::function<void(std::size_t, std::size_t)> &handler){
			change_handlers_[&typeid(object_type)].push_back(handler);
		}

		m_owner_type &owner_;
		std::size_t count_ = 0u;

		map_type handlers_;
		change_map_type change_handlers_;
		
		utility::random_integral_number_generator random_generator_;
	};
}
