#ifndef HARBOR_PANEL_HH
#define HARBOR_PANEL_HH

#include <memory>

namespace harbor::panel
{
    class panel_private;

    class panel
    {
    public:
        panel() noexcept;

    public:
        void show() noexcept;
        void close() noexcept;

    private:
        std::unique_ptr<panel_private> private_{ nullptr };
    };
} // namespace harbor::panel

#endif /* !HARBOR_PANEL_HH */
