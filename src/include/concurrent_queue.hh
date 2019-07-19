#ifndef RML_CONCURRENT_QUEUE_HH
#define RML_CONCURRENT_QUEUE_HH

#include <queue>
#include <type_traits>
#include <vector>

#include "semaphore.hh"

namespace rml
{
    namespace utilities
    {
        template <typename queue_element_t,
                  typename container_t = std::deque<queue_element_t>,
                  std::size_t initial_capacity = 100>
        class concurrent_queue
        {
            static_assert(std::is_same_v<queue_element_t, typename container_t::value_type>);
            static_assert(std::is_move_assignable_v<queue_element_t>);
            static_assert(std::is_move_constructible_v<queue_element_t>);

            using semaphore_lock_t = binary_semaphore_lock<fast_semaphore>;

        public:
            void push(queue_element_t &&element)
            {
                semaphore_lock_t lock(binary_semaphore_);

                queue_.push(std::move(element));
                fill_count_.increment();
            }

            template <typename... Args> void emplace(Args &&... args)
            {
                semaphore_lock_t lock(binary_semaphore_);

                queue_.emplace(std::forward<Args>(args)...);
                fill_count_.increment();
            }

            auto pop() -> queue_element_t
            {
                fill_count_.decrement();

                {
                    semaphore_lock_t lock(binary_semaphore_);

                    queue_element_t result = std::move(queue_.front());
                    queue_.pop();

                    return result;
                }
            }

        private:
            std::queue<queue_element_t, container_t> queue_{};
            fast_semaphore binary_semaphore_{ 1 };
            fast_semaphore fill_count_{ 0 };
        };
    } // namespace utilities
} // namespace rml

#endif /* RML_CONCURRENT_QUEUE_HH */
