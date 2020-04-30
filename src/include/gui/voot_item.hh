#pragma once

#include "voot_global.hh"
#include "core/voot_property.hh"
#include "events/voot_mouse_events.hh"

#include <algorithm>
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

VOOT_BEGIN_NAMESPACE

class Item
{
public:
    using RenderFunction = void (*)(NVGcontext *, const void *);
    template<typename ChildItem> using RenderMethod = void (ChildItem::*)(NVGcontext *) const;

protected:
    template<typename ChildItem>
    Item(ChildItem *) noexcept
      : render_function_{ [](NVGcontext *vg, const void *child) {
          constexpr RenderMethod<ChildItem> render_method = &ChildItem::render;
          (static_cast<const ChildItem *>(child)->*render_method)(vg);
      } }
    {}

public:
    Item() noexcept
    {}

    ~Item() noexcept;

public:
    constexpr int x_abs() const noexcept
    {
        if (parent_ != nullptr)
        {
            return parent_->x_abs() + x();
        }
        return x_;
    }

    constexpr int y_abs() const noexcept
    {
        if (parent_ != nullptr)
        {
            return parent_->y_abs() + y();
        }

        return y_;
    }

private:
    constexpr int get_x() const noexcept
    {
        return x_;
    }

    constexpr bool set_x(int value) noexcept
    {
        if (x_ != value)
        {
            x_ = value;
            return true;
        }

        return false;
    }

    constexpr int get_y() const noexcept
    {
        return y_;
    }

    constexpr bool set_y(int value) noexcept
    {
        if (y_ != value)
        {
            y_ = value;
            return true;
        }

        return false;
    }

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

    constexpr std::uint16_t get_width() const noexcept
    {
        return width_;
    }

    constexpr bool set_width(std::uint16_t value) noexcept
    {
        if (width_ != value)
        {
            width_ = value;
            return true;
        }

        return false;
    }

    constexpr std::uint16_t get_height() const noexcept
    {
        return height_;
    }

    constexpr bool set_height(std::uint16_t value) noexcept
    {
        if (height_ != value)
        {
            height_ = value;
            return true;
        }

        return false;
    }

public:
    Signal<MouseButton, int, int> mouse_button_pressed;
    Signal<MouseButton, int, int> mouse_button_released;
    Signal<MouseButton, int, int> mouse_clicked;

    VT_PROPERTY(int, x, &Item::get_x, &Item::set_x);
    VT_PROPERTY(int, y, &Item::get_y, &Item::set_y);
    VT_PROPERTY(int, z, &Item::get_z, &Item::set_z);
    VT_PROPERTY(std::uint16_t, width, &Item::get_width, &Item::set_width);
    VT_PROPERTY(std::uint16_t, height, &Item::get_height, &Item::set_height);

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

        set_x(x_);
        set_y(y_);

        update_z_ordering(z_, z_, true, [](Item *self) {
            delete static_cast<Child *>(self);
        });
    }

    int x_{ 0 };
    int y_{ 0 };
    int z_{ 0 };
    std::uint16_t width_{ 0 };
    std::uint16_t height_{ 0 };

private:
    using ItemDeleter = void (*)(Item *);
    void update_z_ordering(
        int new_z,
        int old_z,
        bool force = false,
        ItemDeleter item_deleter = [](Item *i) {
            delete i;
        }) noexcept;

private:
    friend class Window;
    bool handle_mouse_button_pressed(MouseButton button, int x, int y) noexcept;
    bool handle_mouse_button_released(MouseButton button, int x, int y) noexcept;
    void render(NVGcontext *vg) const noexcept;

private:
    RenderFunction render_function_{ nullptr };
    Item *parent_{ nullptr };

    int z_min_{ 0 };
    int z_max_{ 0 };

    using ItemPointer = std::unique_ptr<Item, ItemDeleter>;
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
};

VOOT_END_NAMESPACE
