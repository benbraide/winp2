#include "../app/app_object.h"

winp::thread::object::object()
	: object(*app::object::get_main()){}

winp::thread::object::object(app::object &app)
	: app_(app), queue_(*this), id_(std::this_thread::get_id()), local_id_(GetCurrentThreadId()){
	app_.add_thread_(*this);
}

winp::thread::object::~object(){
	app_.remove_thread_(local_id_);
}

int winp::thread::object::run(){
	if (!is_thread_context())
		throw utility::error_code::outside_thread_context;

	MSG msg{};
	while (true){
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

winp::thread::queue &winp::thread::object::get_queue(){
	return queue_;
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
