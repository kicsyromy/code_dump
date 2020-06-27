#pragma once

#include "voot_global.hh"
#include "core/voot_property.hh"
#include "events/voot_mouse_events.hh"

#include <algorithm>
#include <chrono>
#include <memory>
#include <unordered_map>
#include <vector>

struct NVGcontext;

#define VT_ITEM                                              \
public:                                                      \
    inline void set_parent_item(Item *item) noexcept         \
    {                                                        \
        using ThisType = std::decay_t<decltype(*this)>;      \
        ItemBase<ThisType>::set_parent_item<ThisType>(item); \
    }

VT_BEGIN_NAMESPACE

class Item
{
public:
    using RenderFunction = void (*)(NVGcontext *, const void *);
    template<typename ChildItem> using RenderMethod = void (ChildItem::*)(NVGcontext *) const;

public:
    enum MouseEventFilter : std::uint8_t
    {
        MouseEventFilterNone = 0,
        MouseEventFilterButton = 1 << 0,
        MouseEventFilterMotion = 1 << 1,
        MouseEventFilterWheel = 1 << 2,
        MouseEventFilterDrag = 1 << 3,
    };

protected:
    template<typename ChildItem>
    Item(ChildItem *) noexcept
      : render_function_{ [](NVGcontext *vg, const void *child) {
          constexpr RenderMethod<ChildItem> render_method = &ChildItem::render;
          (static_cast<const ChildItem *>(child)->*render_method)(vg);
      } }
    {
        focus = false;
        can_focus = true;
    }

public:
    Item() noexcept
    {
        focus = false;
        can_focus = true;
    }

    ~Item() noexcept;

public:
    constexpr int x_abs() const noexcept
    {
        if (parent_ != nullptr)
        {
            return parent_->x_abs() + x();
        }
        return 0;
    }

    constexpr int y_abs() const noexcept
    {
        if (parent_ != nullptr)
        {
            return parent_->y_abs() + y();
        }

        return 0;
    }

private:
    constexpr int get_z() const noexcept
    {
        return z_;
    }

    inline bool set_z(int value) noexcept
    {
        const auto old_z = z_;
        const auto new_z = value;

        z_ = new_z;
        update_z_ordering(new_z, old_z);

        return new_z != old_z;
    }

    constexpr bool has_focus() const noexcept
    {
        return has_focus_;
    }

    inline bool set_focus(bool value) noexcept
    {
        if (has_focus_ != value)
        {
            has_focus_ = value;
            return true;
        }

        return false;
    }

public:
    Signal<MouseButton, int, int> mouse_button_pressed;
    Signal<MouseButton, int, int> mouse_button_released;
    Signal<MouseButton, int, int> mouse_clicked;
    Signal<MouseButton, int, int> mouse_double_clicked;

    VT_SIMPLE_PROPERTY(int, x);
    VT_SIMPLE_PROPERTY(int, y);
    VT_PROPERTY(int, z, &Item::get_z, &Item::set_z);

    VT_SIMPLE_PROPERTY(std::uint16_t, width);
    VT_SIMPLE_PROPERTY(std::uint16_t, height);

    VT_SIMPLE_PROPERTY(bool, can_focus);
    VT_PROPERTY(bool, focus, &Item::has_focus, &Item::set_focus);

public:
    constexpr Item *parent_item() const noexcept
    {
        return parent_;
    }

    /* For now just call render, but might do more in the future */
    inline void update() const noexcept
    {
        render(nullptr);
    }

protected:
    template<typename Child> void set_parent_item(Item *parent, Child * = nullptr) noexcept
    {
        static_assert(std::is_base_of_v<Item, Child>);

        parent_ = parent;

        update_z_ordering(z_, z_, true, [](Item *self) {
            delete static_cast<Child *>(self);
        });
    }

    int z_{ 0 };

    using ItemDeleter = void (*)(Item *);
    using ItemPointer = std::unique_ptr<Item, ItemDeleter>;

    bool has_focus_{ false };
    Item *focused_child_{ nullptr };
    std::vector<ItemPointer>::iterator focused_child_it_{};
    int focused_child_z_{ 0 };

    /* Mouse handling */
    std::uint8_t mouse_event_filter_{ MouseEventFilterNone };

private:
    void update_z_ordering(
        int new_z,
        int old_z,
        bool force = false,
        ItemDeleter item_deleter = [](Item *i) {
            delete i;
        }) noexcept;

    bool update_focus();

private:
    friend class Window;
    void render(NVGcontext *vg) const noexcept;

    /* Mouse handling */
    bool handle_mouse_button_pressed(MouseButton button, int x, int y) noexcept;
    bool handle_mouse_button_released(MouseButton button, int x, int y) noexcept;

    using TimePoint = std::chrono::high_resolution_clock::time_point;
    TimePoint mouse_button_press_start_{ std::chrono::high_resolution_clock::now() };
    TimePoint mouse_button_click_end_{ std::chrono::high_resolution_clock::now() };

private:
    RenderFunction render_function_{ nullptr };
    Item *parent_{ nullptr };

    int z_min_{ 0 };
    int z_max_{ 0 };

    std::unordered_map<int, std::vector<ItemPointer>> children_{};
};

template<typename ChildItem> class ItemBase : public Item
{
    friend ChildItem;

public:
    ItemBase(Item *parent = nullptr) noexcept
      : Item(static_cast<const ChildItem *>(nullptr))
    {
        if (parent != nullptr)
        {
            Item::set_parent_item<ChildItem>(parent);
        }
    }

protected:
    constexpr void set_mouse_event_filter(std::uint8_t flags) noexcept
    {
        mouse_event_filter_ = flags;
    }
};

VT_END_NAMESPACE
