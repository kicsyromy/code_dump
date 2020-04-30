#pragma once

#include "voot_global.hh"
#include "core/voot_property.hh"

#include <functional>
#include <memory>
#include <vector>

VOOT_BEGIN_NAMESPACE

class BindingBase
{
public:
    BindingBase(void *target_property,
        std::function<void(void *)> source_expression = nullptr) noexcept
      : target_property_{ target_property }
      , source_expression_{ source_expression }
    {}

protected:
    void *target_property_;
    std::function<void(void *)> source_expression_;
};

namespace binding
{
    using BindingList = std::vector<std::unique_ptr<BindingBase>>;
    BindingList &bindings() noexcept;
} // namespace binding

template<typename...> class Binding : std::false_type
{
};

template<typename TargetValueType,
    auto target_get,
    auto target_set,
    typename SourceValueType,
    auto source_get,
    auto source_set>
class Binding<Property<TargetValueType, target_get, target_set>,
    Property<SourceValueType, source_get, source_set>> : public BindingBase
{
    using PropertyTarget = Property<TargetValueType, target_get, target_set>;
    using PropertySource = Property<SourceValueType, source_get, source_set>;
    using ThisType = Binding<PropertyTarget, PropertySource>;

private:
    void on_source_property_changed(std::decay_t<SourceValueType> value)
    {
        (*static_cast<PropertyTarget *>(target_property_)) = value;
    }

public:
    Binding(PropertyTarget &target, PropertySource &source)
      : BindingBase{ &target }
    {
        source.changed.template connect<&ThisType::on_source_property_changed>(*this);
    }
};

template<typename TargetValueType, auto target_get, auto target_set, typename SourceExpressionType>
class Binding<Property<TargetValueType, target_get, target_set>, SourceExpressionType>
  : public BindingBase
{
    static_assert(std::is_invocable_r_v<TargetValueType, SourceExpressionType>,
        "Binding source must be invocable without arguments and must result in type contained in "
        "property");

    using PropertyTarget = Property<TargetValueType, target_get, target_set>;
    using ThisType = Binding<PropertyTarget, SourceExpressionType>;

public:
    Binding(PropertyTarget &target, SourceExpressionType expression)
      : BindingBase{ &target, [expression](void *result) {
                        (*static_cast<TargetValueType *>(result)) = expression();
                    } }
    {
        target.bound_getter_ = [this] {
            TargetValueType result;
            source_expression_(&result);
            return result;
        };
    }
};

template<typename TargetValueType,
    auto target_get,
    auto target_set,
    typename SourceValueType,
    auto source_get,
    auto source_set>
void bind(Property<TargetValueType, target_get, target_set> &target,
    Property<SourceValueType, source_get, source_set> &source)
{
    binding::bindings().emplace_back(
        new Binding<std::decay_t<decltype(target)>, std::decay_t<decltype(source)>>(target,
            source));
}

template<typename TargetValueType, auto target_get, auto target_set, typename SourceExpressionType>
void bind(Property<TargetValueType, target_get, target_set> &target,
    SourceExpressionType expression)
{
    binding::bindings().emplace_back(
        new Binding<std::decay_t<decltype(target)>, SourceExpressionType>(target, expression));
}

VOOT_END_NAMESPACE
