#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>


template<class T>
class ThreadSafeQueue{
private:
    mutable std::mutex mut;
    mutable std::condition_variable condition;
	std::queue<T> data;
public:

    ThreadSafeQueue() = default;
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    template<typename Iterator>
    ThreadSafeQueue(Iterator first, Iterator last){
        for(auto it=first; it != last; ++it){
            data.push(*it);
        }
    }

    ThreadSafeQueue(std::initializer_list<T> list) :
		ThreadSafeQueue(list.begin(),list.end())
	{
    }

    void enqueue(const T &value){
        std::lock_guard<std::mutex>lock(mut);
        data.push(value);
        condition.notify_one();
    }

    void enqueue(T &&value){
        std::lock_guard<std::mutex>lock(mut);
        data.push(std::move(value));
        condition.notify_one();
    }

	T dequeue(){
        std::unique_lock<std::mutex> lock(mut);
        condition.wait(lock, [this]{return !this->data.empty();});
        auto value=std::move(data.front());
        data.pop();
        return value;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mut);
        return data.empty();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mut);
        return data.size();
    }
};


#endif
