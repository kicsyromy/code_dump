#include "gui/voot_item.hh"

namespace
{
    constexpr bool rectangle_contains_point(int rx,
        int ry,
        int rwidth,
        int rheight,
        int x,
        int y) noexcept
    {
        return (x >= rx && x <= rwidth + rx && y >= ry && y <= rheight + ry);
    }
} // namespace

VOOT_BEGIN_NAMESPACE

Item::~Item() noexcept = default;

void Item::render() const noexcept
{
    if (render_function_ != nullptr)
    {
        render_function_(drawing_context_, this);
    }

    if (children_.empty())
        return;

    for (int zz = z_min_; zz <= z_max_; ++zz)
    {
        auto it = children_.find(zz);
        if (it != children_.end())
        {
            for (const auto &child : it->second)
            {
                child->render();
            }
        }
    }
}

void Item::update_z_ordering(int new_z, int old_z, bool force, ItemDeleter item_deleter) noexcept
{
    if (parent_ == nullptr)
    {
        return;
    }

    if (new_z > parent_->z_max_)
    {
        parent_->z_max_ = new_z;
    }

    if (new_z < parent_->z_min_)
    {
        parent_->z_min_ = new_z;
    }

    if (new_z != old_z || force)
    {
        auto new_z_children = parent_->children_.find(new_z);
        auto old_z_children = parent_->children_.find(old_z);

        ItemDeleter deleter = item_deleter;

        if (old_z_children != parent_->children_.end())
        {
            const auto it = std::find_if(old_z_children->second.begin(),
                old_z_children->second.end(),
                [this, &deleter](auto &e) noexcept {
                    if (e.get() == this)
                    {
                        deleter = e.get_deleter();
                        [[maybe_unused]] auto *self = e.release();
                        return true;
                    }
                    return false;
                });
            if (it != old_z_children->second.cend())
            {
                old_z_children->second.erase(it);
            }
        }

        if (new_z_children != parent_->children_.end())
        {
            auto &self = new_z_children->second.emplace_back(nullptr, nullptr);
            self.reset(this);
            self.get_deleter() = deleter;
        }
        else
        {
            std::vector<ItemPointer> children;
            auto &self = children.emplace_back(nullptr, nullptr);
            self.reset(this);
            self.get_deleter() = deleter;
            parent_->children_.emplace(new_z, std::move(children));
        }
    }
}

bool Item::handle_mouse_button_pressed(MouseButton button, int xx, int yy) noexcept
{
    bool event_handled = false;
    if (!children_.empty())
    {
        for (int zz = z_max_; zz >= z_min_; --zz)
        {
            auto it = children_.find(zz);
            if (it != children_.end())
            {
                for (const auto &child : it->second)
                {
                    const auto child_handled_event = rectangle_contains_point(child->x(),
                                                         child->y(),
                                                         child->width_,
                                                         child->height_,
                                                         xx,
                                                         yy) &&
                                                     child->handle_mouse_button_pressed(button,
                                                         xx - child->x(),
                                                         yy - child->y());
                    if (child_handled_event)
                    {
                        event_handled = true;
                        break;
                    }
                }

                if (event_handled)
                    break;
            }
        }
    }

    /* TODO: Every item handles mouse events, no filter is implemented yet, as such     */
    /*       this item emits the signal if the mouse event occured inbounds and always  */
    /*       return true                                                                */
    if (!event_handled) // && parent_ == nullptr)
    {
        mouse_button_pressed.emit(button, xx, yy);
    }

    return true;
}

VOOT_END_NAMESPACE
