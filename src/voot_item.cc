#include "gui/voot_item.hh"

VOOT_BEGIN_NAMESPACE

Item::~Item() noexcept
{}

void Item::set_parent_item(Item *parent) noexcept
{
    drawing_context_ = parent->drawing_context_;
    parent_ = parent;

    auto it = parent->children_.find(z_);
    if (it != parent->children_.end())
    {
        auto &self = it->second.emplace_back();
        self.reset(this);
    }
    else
    {
        std::vector<std::unique_ptr<Item>> children;
        auto &self = children.emplace_back();
        self.reset(this);
        parent->children_.emplace(z_, std::move(children));
    }
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

VOOT_END_NAMESPACE
