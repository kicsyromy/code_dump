#pragma once

#include "voot_global.hh"

#include <memory>
#include <unordered_map>
#include <vector>

#define VT_ITEM_RENDER_FUNCTION(f) \
    reinterpret_cast<voot::Item::fn_static_render_fn_type>(reinterpret_cast<void *>(f))

struct NVGcontext;

VOOT_BEGIN_NAMESPACE

class Item
{
public:
    using fn_static_render_fn_type = void (*)(NVGcontext *, const void *);

public:
    Item() noexcept = default;

    explicit Item(fn_static_render_fn_type render_function) noexcept
      : render_function_{ render_function }
    {}

    explicit Item(NVGcontext *context) noexcept
      : drawing_context_{ context }
    {}

    ~Item() noexcept;

public:
    constexpr auto x() const
    {
        return x_;
    }

    template<typename Integer> constexpr void set_x(Integer x) noexcept
    {
        static_assert(std::is_integral_v<Integer>);
        x_ = static_cast<int>(x);
    }

    constexpr auto y() const
    {
        return y_;
    }

    template<typename Integer> constexpr void set_y(Integer y) noexcept
    {
        static_assert(std::is_integral_v<Integer>);
        y_ = static_cast<int>(y);
    }

    constexpr auto z() const
    {
        return z_;
    }

    template<typename Integer> constexpr void set_z(Integer z) noexcept
    {
        static_assert(std::is_integral_v<Integer>);
        z_ = static_cast<int>(z);
    }

    constexpr auto width() const
    {
        return width_;
    }

    template<typename Integer> constexpr void set_width(Integer width) noexcept
    {
        static_assert(std::is_integral_v<Integer>);
        width_ = static_cast<std::uint16_t>(width);
    }

    constexpr auto height() const
    {
        return height_;
    }

    template<typename Integer> constexpr void set_height(Integer height) noexcept
    {
        static_assert(std::is_integral_v<Integer>);
        height_ = static_cast<std::uint16_t>(height);
    }

public:
    void set_parent_item(Item *parent) noexcept;
    constexpr Item *parent() const noexcept
    {
        return parent_;
    }

    /* For now just call render, but might do more in the future */
    void update() const noexcept
    {
        render();
    }

protected:
    int x_{ 0 };
    int y_{ 0 };
    int z_{ 0 };
    std::uint16_t width_{ 0 };
    std::uint16_t height_{ 0 };

private:
    void render() const noexcept;

private:
    NVGcontext *drawing_context_{ nullptr };
    fn_static_render_fn_type render_function_{ nullptr };
    Item *parent_{ nullptr };

    int z_min_{ 0 };
    int z_max_{ 0 };
    std::unordered_map<int, std::vector<std::unique_ptr<Item>>> children_{};
};

template<typename ChildItem> class ItemBase : public Item
{
public:
    ItemBase() noexcept
      : Item{ VT_ITEM_RENDER_FUNCTION(&ChildItem::render) }
    {}
};

VOOT_END_NAMESPACE
