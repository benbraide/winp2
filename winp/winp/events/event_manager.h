#pragma once

#include <any>

#include "../utility/traits.h"
#include "../utility/random_number_generator.h"

#include "event_object.h"

namespace winp::events{
	class handler_base{
	public:
		virtual void call(object &object) const = 0;
	};

	template <class object_type>
	class handler : public handler_base{
	public:
		using m_object_type = object_type;
		using callback_type = std::function<void(m_object_type)>;

		explicit handler(const callback_type &callback)
			: callback_(callback){}

		virtual void call(object &object) const override{
			callback_(dynamic_cast<std::remove_cv_t<std::remove_reference_t<m_object_type>> &>(object));
		}

	private:
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
				return bind_(utility::object_to_function_traits::get(handler));
			});
		}
		
		template <typename object_type>
		unsigned __int64 bind(const std::function<void()> &handler){
			return bind([handler](object_type &){
				handler();
			});
		}

		void unbind(unsigned __int64 id){
			owner_.execute_task_inside_thread_context([&]{
				unbind_(id);
			});
		}

	private:
		friend owner_type;
		
		template <typename object_type>
		unsigned __int64 bind_(const std::function<void(object_type &)> &handler){
			unsigned __int64 id = random_generator_;
			handlers_[&typeid(object_type)].push_back(handler_info{
				id,
				std::make_shared<events::handler<object_type &>>(handler)
			});
			++count_;

			return id;
		}
		
		template <typename object_type>
		unsigned __int64 bind_(const std::function<void(const object_type &)> &handler){
			unsigned __int64 id = random_generator_;
			handlers_[&typeid(object_type)].push_back(handler_info{
				id,
				std::make_shared<events::handler<const object_type &>>(handler)
			});
			++count_;

			return id;
		}

		void unbind_(unsigned __int64 id){
			if (count_ == 0u)
				return;

			for (auto &info : handlers_){
				for (auto it = info.second.begin(); it != info.second.end(); ++it){
					if (it->id == id){//Handler found
						info.second.erase(it);
						--count_;
						return;
					}
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

		m_owner_type &owner_;
		map_type handlers_;
		std::size_t count_ = 0u;
		utility::random_integral_number_generator random_generator_;
	};
}
