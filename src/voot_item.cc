#include "gui/voot_item.hh"

VOOT_BEGIN_NAMESPACE

Item::~Item() noexcept
{}

void Item::set_parent_item(Item *parent) noexcept
{
    drawing_context_ = parent->drawing_context_;
    parent_ = parent;

    set_x(x_);
    set_y(y_);

    update_z_ordering(z_, z_, true);
}

void Item::render() const noexcept
{
    if (render_function_ != nullptr)
    {
        render_function_(drawing_context_, this);
    }

    if (children_.empty())
        return;

    for (int z = z_min_; z <= z_max_; ++z)
    {
        auto it = children_.find(z);
        if (it != children_.end())
        {
            for (const auto &child : it->second)
            {
                child->render();
            }
        }
    }
}

void Item::update_z_ordering(int new_z, int old_z, bool force) noexcept
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

        if (old_z_children != parent_->children_.end())
        {
            const auto it = std::find_if(old_z_children->second.begin(),
                old_z_children->second.end(),
                [this](auto &e) noexcept {
                    if (e.get() == this)
                    {
                        e.release();
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
            auto &self = new_z_children->second.emplace_back();
            self.reset(this);
        }
        else
        {
            std::vector<std::unique_ptr<Item>> children;
            auto &self = children.emplace_back();
            self.reset(this);
            parent_->children_.emplace(new_z, std::move(children));
        }
    }
}

VOOT_END_NAMESPACE
