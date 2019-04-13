#ifndef RML_SEMAPHORE_HH
#define RML_SEMAPHORE_HH

/* Implementation based on pastebin.com/raw/Q7p6e7Xc and https://vorbrodt.blog/2019/02/05/fast-semaphore */

#include <mutex>
#include <atomic>
#include <condition_variable>
#include <cassert>

namespace rml
{
    namespace utilities
    {
        class semaphore
        {
        public:
            semaphore(int count) noexcept
              : count_(count)
            {
                assert(count > -1);
            }
        
            void increment() noexcept
            {
                {
                    std::unique_lock<std::mutex> lock(mutex_);
                    ++count_;
                }
                cv_.notify_one();
            }
        
            void decrement() noexcept
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [&]() { return count_ != 0; });
                --count_;
            }
        
        private:
            int count_;
            std::mutex mutex_;
            std::condition_variable cv_;
        };
        
        class fast_semaphore
        {
        public:
            fast_semaphore(int count) noexcept
              : count_(count)
              , semaphore_(0) {}
        
            void increment()
            {
                const auto count = count_.fetch_add(1, std::memory_order_release);
                if (count < 0) { semaphore_.increment(); }
            }
        
            void decrement()
            {
                const auto count = count_.fetch_sub(1, std::memory_order_acquire);
                if (count < 1) { semaphore_.decrement(); }
            }
        
        private:
            std::atomic<int> count_;
            semaphore semaphore_;
        };

        template<typename semaphore_t>
        struct binary_semaphore_lock
        {
            binary_semaphore_lock(semaphore_t &semaphore)
              : binary_semaphore_(semaphore)
            {
                binary_semaphore_.decrement();
            }

            ~binary_semaphore_lock()
            {
                binary_semaphore_.increment();
            }

        private:
            semaphore_t &binary_semaphore_;
        };
    }
}

#endif /* RML_SEMAPHORE_HH */