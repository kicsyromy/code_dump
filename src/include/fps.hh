#pragma once

#include <cstdint>

#include <features.h>
#include <bits/types/time_t.h>

__BEGIN_DECLS

/* Structure returned by the `ftime' function.  */
struct timeb
{
    time_t time;                /* Seconds since epoch, as from `time'.  */
    unsigned short int millitm; /* Additional milliseconds.  */
    short int timezone;         /* Minutes west of GMT.  */
    short int dstflag;          /* Nonzero if Daylight Savings Time used.  */
};

/* Fill in TIMEBUF with information about the current time.  */
extern int ftime(struct timeb *__timebuf) __nonnull((1));

__END_DECLS

class fps_counter
{
    class clock
    {
    public:
        inline clock() { ftime(&start_); }

    public:
        inline std::uint16_t value() const
        {
            timeb end;
            ftime(&end);

            return static_cast<std::uint16_t>(1000 * (end.time - start_.time) +
                                              (end.millitm - start_.millitm));
        }

        inline void reset() { ftime(&start_); }

    private:
        timeb start_;
    };

public:
    void update()
    {
        fpscount_++;

        if (interval_.value() > 1000)
        {
            fps_ = fpscount_;

            reset();
        }
    }

    void reset()
    {
        fpscount_ = 0;
        interval_.reset();
    }

    unsigned int get() const { return fps_; }

private:
    std::uint16_t fps_{ 0 };
    std::uint16_t fpscount_{ 0 };
    clock interval_;
};
