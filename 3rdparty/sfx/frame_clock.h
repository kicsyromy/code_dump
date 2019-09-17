////////////////////////////////////////////////////////////
//
// FrameClock - Utility class for tracking frame statistics.
// Copyright (C) 2013 Lee R (lee-r@outlook.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

#ifndef FRAMECLOCK_H_INCLUDED
#define FRAMECLOCK_H_INCLUDED

#include <sys/timeb.h>

#include <limits>
#include <cassert>
#include <algorithm>

namespace sfx
{
    namespace detail
    {
        // Utility function to disambiguate from macro 'max'.
        template<typename T>
        inline T limit()
        {
            return (std::numeric_limits<T>::max)();
        }
    }	// namespace detail

    struct time_t : public timeb
    {
        constexpr time_t operator -(const time_t &t2) const noexcept
        {
            return {
                time - t2.time,
                static_cast<unsigned short>(millitm - t2.millitm),
                dstflag,
                timezone
            };
        }

        constexpr time_t &operator -=(const time_t &t2) noexcept
        {
            time -= t2.time;
            millitm -= t2.millitm;

            return *this;
        }

        constexpr time_t operator +(const time_t &t2) const noexcept
        {
            return {
                time + t2.time,
                static_cast<unsigned short>(millitm + t2.millitm),
                dstflag,
                timezone
            };
        }

        constexpr time_t &operator +=(const time_t &t2) noexcept
        {
            time += t2.time;
            millitm += t2.millitm;

            return *this;
        }

        constexpr time_t &operator =(std::uint32_t milliseconds) noexcept
        {
            time = milliseconds / 1000;
            millitm = milliseconds % 1000;

            return *this;
        }

        constexpr bool operator <(const time_t &t2) noexcept
        {
            if (time == t2.time && millitm < t2.millitm) { return true; }
            if (time < t2.time) { return true; }

            return false;
        }

        constexpr bool operator >(const time_t &t2) noexcept
        {
            if (time == t2.time && millitm > t2.millitm) { return true; }
            if (time > t2.time) { return true; }

            return false;
        }

        constexpr float asSeconds() noexcept
        {
            return (1000.f * time + millitm) / 1000.f;
        }

        constexpr std::uint32_t asMilliseconds() noexcept
        {
            return 1000 * time + millitm;
        }
    };

    struct clock_t
    {
        time_t start;

        void restart() noexcept
        {
            ftime(&start);
        }

        time_t getElapsedTime() noexcept
        {
            time_t end;
            ftime(&end);

            return end - start;
        }
    };

    class FrameClock
    {
    public:

        // Constructs a FrameClock object with sample depth 'depth'.
        explicit FrameClock(std::size_t depth = 100)
        {
            assert(depth >= 1);
            m_sample.data.resize(depth);

            m_freq.minimum = detail::limit<float>();
            m_time.minimum = time_t {
                    detail::limit<::time_t>(),
                    detail::limit<unsigned short>(),
                    0, 0
            };
        }

        // Resets all times to zero and discards accumulated samples.
        void clear()
        {
            FrameClock(getSampleDepth()).swap(*this);
        }

        // Begin frame timing.
        // Should be called once at the start of each new frame.
        void beginFrame()
        {
            m_clock.restart();
        }

        // End frame timing.
        // Should be called once at the end of each frame.
        // Returns: Time elapsed since the matching FrameClock::beginFrame.
        time_t endFrame()
        {
            m_time.current = m_clock.getElapsedTime();

            m_sample.accumulator -= m_sample.data[m_sample.index];
            m_sample.data[m_sample.index] = m_time.current;

            m_sample.accumulator += m_time.current;
            m_time.elapsed       += m_time.current;

            if (++m_sample.index >= getSampleDepth())
            {
                m_sample.index = 0;
            }

            if (m_time.current.asMilliseconds() != 0)
            {
                m_freq.current = 1.0f / m_time.current.asSeconds();
            }

            if (m_sample.accumulator.asMilliseconds() != 0)
            {
                const float smooth = static_cast<float>(getSampleDepth());
                m_freq.average = smooth / m_sample.accumulator.asSeconds();
            }

            const std::int64_t smooth = static_cast<std::int64_t>(getSampleDepth());
            m_time.average = m_sample.accumulator.asMilliseconds() / smooth;

            if (m_freq.current < m_freq.minimum)
                m_freq.minimum = m_freq.current;
            if (m_freq.current > m_freq.maximum)
                m_freq.maximum = m_freq.current;

            if (m_time.current < m_time.minimum)
                m_time.minimum = m_time.current;
            if (m_time.current > m_time.maximum)
                m_time.maximum = m_time.current;

            ++m_freq.elapsed;

            return m_time.current;
        }

        // Sets the number of frames to be sampled for averaging.
        // 'depth' must be greater than or equal to 1.
        void setSampleDepth(std::size_t depth)
        {
            assert(depth >= 1);
            FrameClock(depth).swap(*this);
        }

        // Returns: The number of frames to be sampled for averaging.
        std::size_t getSampleDepth() const
        {
            return m_sample.data.size();
        }

        // Returns: The total accumulated frame time.
        time_t getTotalFrameTime() const
        {
            return m_time.elapsed;
        }

        // Returns: The total accumulated number of frames.
        std::uint64_t getTotalFrameCount() const
        {
            return m_freq.elapsed;
        }

        // Returns: Time elapsed during the last 'FrameClock::beginFrame/endFrame' pair.
        time_t getLastFrameTime() const
        {
            return m_time.current;
        }

        // Returns: The shortest measured frame time.
        time_t getMinFrameTime() const
        {
            return m_time.minimum;
        }

        // Returns: The longest measured frame time.
        time_t getMaxtFrameTime() const
        {
            return m_time.maximum;
        }

        // Returns: Average frame time over the last getSampleDepth() frames.
        time_t getAverageFrameTime() const
        {
            return m_time.average;
        }

        // Returns: Frames per second, considering the pervious frame only.
        float getFramesPerSecond() const
        {
            return m_freq.current;
        }

        // Returns: The lowest measured frames per second.
        float getMinFramesPerSecond() const
        {
            return m_freq.minimum;
        }

        // Returns: The highest measured frames per second.
        float getMaxFramesPerSecond() const
        {
            return m_freq.maximum;
        }

        // Returns: Average frames per second over the last getSampleDepth() frames.
        float getAverageFramesPerSecond() const
        {
            return m_freq.average;
        }

        // Swaps the value of this FrameClock instance with 'other'.
        void swap(FrameClock& other)
        {
            this->m_time.swap(other.m_time);
            this->m_freq.swap(other.m_freq);
            this->m_sample.swap(other.m_sample);
            std::swap(this->m_clock, other.m_clock);
        }

    private:

        template<typename T, typename U>
        struct Range
        {
            Range()
                : minimum()
                , maximum()
                , average()
                , current()
                , elapsed()
            {}
            void swap(Range& other)
            {
                std::swap(this->minimum, other.minimum);
                std::swap(this->maximum, other.maximum);
                std::swap(this->average, other.average);
                std::swap(this->current, other.current);
                std::swap(this->elapsed, other.elapsed);
            }
            T minimum;
            T maximum;
            T average;
            T current;
            U elapsed;
        };

        Range<time_t, time_t> m_time;
        Range<float, std::uint64_t>  m_freq;

        struct SampleData
        {
            SampleData()
                : accumulator()
                , data()
                , index()
            {}
            time_t accumulator;
            std::vector<time_t> data;
            std::vector<time_t>::size_type index;
            void swap(SampleData& other)
            {
                std::swap(this->accumulator, other.accumulator);
                std::swap(this->data,        other.data);
                std::swap(this->index,       other.index);
            }
        } m_sample;

        clock_t m_clock;
    };
} // namespace sfx

#endif	// FRAMECLOCK_H_INCLUDED
