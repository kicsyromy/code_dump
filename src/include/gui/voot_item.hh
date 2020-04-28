#pragma once

#include "voot_global.hh"
#include "core/voot_signal.hh"
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

public:
    template<typename ChildItem>
    Item(ChildItem *) noexcept
      : render_function_{ [](NVGcontext *vg, const void *child) {
          constexpr RenderMethod<ChildItem> render_method = &ChildItem::render;
          (static_cast<const ChildItem *>(child)->*render_method)(vg);
      } }
    {}

    explicit Item(NVGcontext *context) noexcept
      : drawing_context_{ context }
    {}

    ~Item() noexcept;

public:
    Signal<MouseButton, int, int> mouse_button_pressed;
    Signal<MouseButton, int, int> mouse_button_released;
    Signal<MouseButton, int, int> mouse_clicked;

public:
    constexpr int x() const noexcept
    {
        if (parent_item() != nullptr)
        {
            return x_ - parent_item()->x();
        }

        return x_;
    }

    template<typename Integer> constexpr void set_x(Integer x) noexcept
    {
        static_assert(std::is_integral_v<Integer>);
        const auto new_x = static_cast<int>(x);
        if (parent_item() != nullptr)
        {
            x_ = parent_item()->x() + new_x;
        }
        else
        {
            x_ = new_x;
        }
    }

    constexpr int y() const noexcept
    {
        if (parent_item() != nullptr)
        {
            return y_ - parent_item()->y();
        }

        return y_;
    }

    template<typename Integer> constexpr void set_y(Integer y) noexcept
    {
        static_assert(std::is_integral_v<Integer>);
        const auto new_y = static_cast<int>(y);
        if (parent_item() != nullptr)
        {
            y_ = parent_item()->y() + new_y;
        }
        else
        {
            y_ = new_y;
        }
    }

    constexpr int z() const noexcept
    {
        return z_;
    }

    template<typename Integer> inline void set_z(Integer z) noexcept
    {
        static_assert(std::is_integral_v<Integer>);
        const auto old_z = z_;
        const auto new_z = static_cast<int>(z);

        z_ = new_z;
        update_z_ordering(new_z, old_z);
    }

    constexpr auto width() const noexcept
    {
        return width_;
    }

    template<typename Integer> constexpr void set_width(Integer width) noexcept
    {
        static_assert(std::is_integral_v<Integer>);
        width_ = static_cast<std::uint16_t>(width);
    }

    constexpr auto height() const noexcept
    {
        return height_;
    }

    template<typename Integer> constexpr void set_height(Integer height) noexcept
    {
        static_assert(std::is_integral_v<Integer>);
        height_ = static_cast<std::uint16_t>(height);
    }

public:
    constexpr Item *parent_item() const noexcept
    {
        return parent_;
    }

    /* For now just call render, but might do more in the future */
    inline void update() const noexcept
    {
        render();
    }

protected:
    template<typename Child> void set_parent_item(Item *parent, Child * = nullptr) noexcept
    {
        static_assert(std::is_base_of_v<Item, Child>);

        drawing_context_ = parent->drawing_context_;
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
    void render() const noexcept;

private:
    NVGcontext *drawing_context_{ nullptr };
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
