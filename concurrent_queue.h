#ifndef _CONCURRENT_QUEUE_H_
#define _CONCURRENT_QUEUE_H_

#include <deque>
#include <mutex>
#include <condition_variable>

template <typename DataType>
class concurrent_queue
{
    std::deque<DataType> the_queue;
    std::mutex the_mutex;
    std::condition_variable the_condition_variable;
    bool isFinished = false;

    public:

    void close()
    {
        {
            std::lock_guard<std::mutex> lock(the_mutex);
            isFinished = true;
        }

        the_condition_variable.notify_one();

    }

    template<class... Args>
        void emplace(Args&&... args)
        {
            {
                std::lock_guard<std::mutex> lock(the_mutex);
                the_queue.emplace_back(std::forward<Args>(args)...);
            }

            the_condition_variable.notify_one();
        }

    void push(DataType const &data)
    {
        {
            std::lock_guard<std::mutex> lock(the_mutex);
            the_queue.push_back(data);
        }

        the_condition_variable.notify_one();
    }


    bool empty() const
    {
        std::lock_guard<std::mutex> lock(the_mutex);
        return the_queue.empty();
    }

    bool try_pop(DataType& popped_value)
    {
        std::unique_lock<std::mutex> lock(the_mutex);

        the_condition_variable.wait(lock, [this]{ return (the_queue.empty() || !isFinished); });

        if(the_queue.empty() && !isFinished)
        {
            return false;
        }

        popped_value=the_queue.front();
        the_queue.pop_front();
        return true;
    }

    void wait_and_pop(DataType& popped_value)
    {
        std::unique_lock<std::mutex> lock(the_mutex);

        the_condition_variable.wait(lock, [this]{ return (the_queue.empty() || !isFinished); });

        popped_value=the_queue.front();
        the_queue.pop_front();
    }
};

#endif
