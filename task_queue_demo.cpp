#include <functional>
#include <thread>
#include <condition_variable>
#include <iostream>

class task_queue {

public:

	explicit task_queue(int worker_count) {
		// TODO: multiple workers
		worker = std::thread([this]() {
			std::unique_lock<std::mutex> l(m);
			for (;;) {
				if (done)
					break;
				cv.wait(l);
			}
		});
	}

	~task_queue() {
		{
			std::lock_guard<std::mutex> l(m);
			done = true;
			cv.notify_all();
		}
		worker.join();
	}

	void enqueue(std::function<void()> task) {
		// TODO
	}

private:

	std::thread worker;

	std::mutex m;
	std::condition_variable cv;
	bool done = false;

};

// TODO proper tests

int main() {
	task_queue q(10);
	for (int i = 0; i != 20; ++i)
		q.enqueue([i]() { std::cout << "hello world " << i << "\n"; });
	return 0;
}
