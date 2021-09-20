#include <functional>
#include <thread>
#include <condition_variable>
#include <iostream>

class task_queue {

public:
	explicit task_queue(int worker_count) 
	{
		for (int i = 0; i < worker_count; i++) 
		{
			workers.push_back(std::thread([this, i]() {

				std::unique_lock<std::mutex> l(m);

				for (;;) 
				{
					if (tasks.size()) 
					{
						auto task_to_do = tasks.front();

						tasks.erase(tasks.begin());

						l.unlock();
						task_to_do();
						l.lock();
					}

					else if (!done) 
					{
						cv.wait(l);
					}

					else 
					{
						break;
					}
				}
			}));
		}
	}
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
