#include <functional>
#include <thread>
#include <condition_variable>
#include <iostream>
#include <vector>
#include <atomic>

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


	~task_queue() {
		{
			std::unique_lock<std::mutex> l(m);
			done = true;
			cv.notify_all();
		}

		for (unsigned int i = 0; i < workers.size(); i++)
		{
			workers[i].detach();
			std::cout <<i+1<< " Поток высвобожден\n";
		}

		workers.clear();
		std::cout << "\tВсе потоки свободны\n";
	}

	void enqueue(std::function<void()> task) 
	{
		std::unique_lock<std::mutex> l(m);

		tasks.push_back(task);

		cv.notify_one();
	}

private:

	std::vector <std::thread> workers;
	std::vector <std::function<void()>> tasks;

	std::mutex m;
	std::condition_variable cv;
	bool done = false;

};

// TODO proper tests

int main() {

    setlocale(LC_ALL, "ru");

	std::mutex m1;

	task_queue q(5);

	for (int i = 1; i <= 25; ++i) {
		{
			q.enqueue([i, &m1]() {

				std::this_thread::sleep_for(std::chrono::milliseconds(800));
				
				std::lock_guard<std::mutex> l(m1);

				std::cout << "Итерация № " << i << "\n";
				
			});
		}
	}

	int n = 0;
	std::cin >> n;

	return 0;
}
