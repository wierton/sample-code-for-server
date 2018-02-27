#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <memory>
#include <utility>
#include <array>
#include "ThreadSafeQueue.h"


template<size_t N>
class ThreadPool {
	std::array<std::unique_ptr<std::thread>, N> pool;
	ThreadSafeQueue<std::function<void()>> tasks;

public:

	~ThreadPool() {
		for(auto &pthread : pool) { pthread.reset(); }
	}

	ThreadPool() {
		auto runner = [this]() {
			while(1) {
				try {
					auto task = this->tasks.dequeue();
					task();
				} catch(std::exception &e) {
					// do nothing
				}
			}
		};

		for(auto &pthread : pool) {
			pthread.reset(new std::thread(runner));
		}
	}

	template<class Func, class...Args>
	void submitTask(Func &&func, Args&&...args) {
		auto task = [=]() {
			return func(args...);
		};
		tasks.enqueue(task);
	}
};


#endif
