#ifndef FPS_HH
#define FPS_HH

#include <cstdint>

#include <sys/timeb.h>

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

#endif /* !FPS_HH */
