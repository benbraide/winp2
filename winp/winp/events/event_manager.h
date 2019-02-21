#pragma once

#include <any>

#include "../utility/random_number_generator.h"

#include "event_object.h"

namespace winp::events{
	class handler_base{
	public:
		virtual void call(void *object) const = 0;
	};

	template <class object_type>
	class handler : public handler_base{
	public:
		using m_object_type = object_type;
		using callback_type = std::function<void(m_object_type)>;

		explicit handler(const callback_type &callback)
			: callback_(callback){}

		virtual void call(void *object) const override{
			callback_(*static_cast<std::remove_cv_t<std::remove_reference_t<m_object_type>> *>(object));
		}

	private:
		callback_type callback_;
	};

	template <class owner>
	class manager{
	public:
		struct handler_info{
			unsigned __int64 id;
			std::shared_ptr<handler_base> handler;
		};

		using list_type = std::list<handler_info>;
		using map_type = std::unordered_map<const std::type_info *, list_type>;

		~manager(){
			count_ = 0u;
		}

	private:
		friend owner;

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

		template <typename object_type>
		unsigned __int64 bind_(const std::function<void()> &handler){
			return bind_([handler](object_type &){
				handler();
			});
		}

		void unbind_(unsigned __int64 id){
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

		template <typename object_type>
		void trigger_(object_type &e) const{
			if (count_ == 0u)
				return;

			if (auto it = handlers_.find(&typeid(object_type)); it != handlers_.end()){
				for (auto &handler_info : it->second){
					handler_info.handler->call(&e);
					if ((e.get_states() & object::state_propagation_stopped) != 0u)
						break;//Propagation stopped
				}
			}
		}

		map_type handlers_;
		std::size_t count_ = 0u;
		utility::random_integral_number_generator random_generator_;
	};
}
