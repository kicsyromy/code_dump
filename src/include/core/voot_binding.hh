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
        std::function<void(void *)> &&source_expression = nullptr) noexcept
      : target_property_{ target_property }
    {
        if (source_expression != nullptr)
        {
            static_cast<PropertyBase *>(target_property)
                ->set_get_expression(std::move(source_expression));
        }
    }

protected:
    void *target_property_;
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
    using PropertyTarget = Property<TargetValueType, target_get, target_set>;
    using ThisType = Binding<PropertyTarget, SourceExpressionType>;

    static_assert(std::is_same_v<typename PropertyTarget::ValueTypeGet,
                      std::invoke_result_t<SourceExpressionType>>,
        "Binding source must be invocable without arguments and must result in the return type of "
        "the get method");

public:
    Binding(PropertyTarget &target, SourceExpressionType &&expression)
      : BindingBase{ &target,
          [expression = std::move(expression)](void *result) {
              typename PropertyTarget::ValueTypeGet expr_result = expression();

              if constexpr (!std::is_reference_v<typename PropertyTarget::ValueTypeGet>)
              {
                  auto *r =
                      static_cast<std::decay_t<typename PropertyTarget::ValueTypeGet> *>(result);
                  *r = std::move(expr_result);
              }

              if constexpr (std::is_reference_v<typename PropertyTarget::ValueTypeGet>)
              {
                  auto **r =
                      static_cast<std::decay_t<typename PropertyTarget::ValueTypeGet> **>(result);
                  *r = const_cast<std::decay_t<decltype(*r)>>(&expr_result);
              }
          } }
    {
        static_cast<void>(expression);
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
    SourceExpressionType &&expression)
{
    binding::bindings().emplace_back(
        new Binding<std::decay_t<decltype(target)>, SourceExpressionType>(target,
            std::move(expression)));
}

VOOT_END_NAMESPACE
