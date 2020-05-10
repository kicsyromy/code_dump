#pragma once

#include "voot_global.hh"
#include "core/voot_property.hh"

#include <functional>
#include <memory>
#include <vector>

VT_BEGIN_NAMESPACE

class BindingBase
{
};

namespace binding
{
    using BindingList = std::vector<std::unique_ptr<BindingBase>>;
    VOOT_API BindingList &bindings() noexcept;
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
void bind(Property<TargetValueType, target_get, target_set> &target,
    Property<SourceValueType, source_get, source_set> &source)
{
    using PropertyTarget = Property<TargetValueType, target_get, target_set>;
    using PropertySource = Property<SourceValueType, source_get, source_set>;

    static_assert(std::is_same_v<typename PropertyTarget::ValueTypeGet,
                      typename PropertySource::ValueTypeGet> ||
                      (!std::is_reference_v<typename PropertyTarget::ValueTypeGet> &&
                          std::is_same_v<std::decay_t<typename PropertyTarget::ValueTypeGet>,
                              std::decay_t<typename PropertySource::ValueTypeGet>> &&
                          std::is_convertible_v<typename PropertySource::ValueTypeGet,
                              typename PropertyTarget::ValueTypeGet>),
        "Binding target and source get methods should return the same thing");

    if (static_cast<void *>(&target) == static_cast<void *>(&source))
    {
        VT_LOG_ERROR("Cannot bind property to itself");
        return;
    }

    static_cast<PropertyBase *>(&target)->set_get_expression(
        [&source, lifeline = source.get_lifeline()](void *result) {
            if (lifeline.expired())
            {
                VT_LOG_ERROR("Binding source property destroyed, undefined behaviour ahead...");
                return;
            }

            typename PropertyTarget::ValueTypeGet expr_result = source();

            if constexpr (!std::is_reference_v<typename PropertyTarget::ValueTypeGet>)
            {
                auto *r =
                    static_cast<std::decay_t<typename PropertyTarget::ValueTypeGet> *>(result);
                *r = std::move(expr_result);
            }
            else
            {
                auto **r =
                    static_cast<std::decay_t<typename PropertyTarget::ValueTypeGet> **>(result);
                *r = const_cast<std::decay_t<decltype(*r)>>(&expr_result);
            }
        });
    source.changed.connect(target.changed);
}

template<typename TargetValueType, typename SourceValueType, auto source_get, auto source_set>
void bind(Property<TargetValueType> &target,
    Property<SourceValueType, source_get, source_set> &source)
{
    using PropertySource = Property<SourceValueType, source_get, source_set>;

    static_assert(
        std::is_same_v<TargetValueType, typename PropertySource::ValueTypeGet> ||
            (!std::is_reference_v<TargetValueType> &&
                std::is_same_v<std::decay_t<TargetValueType>,
                    std::decay_t<typename PropertySource::ValueTypeGet>> &&
                std::is_convertible_v<typename PropertySource::ValueTypeGet, TargetValueType>),
        "Binding target type and source get method should be compatible");

    if (static_cast<void *>(&target) == static_cast<void *>(&source))
    {
        VT_LOG_ERROR("Cannot bind property to itself");
        return;
    }

    static_cast<PropertyBase *>(&target)->set_get_expression(
        [&source, lifeline = source.get_lifeline()](void *result) {
            if (lifeline.expired())
            {
                VT_LOG_ERROR("Binding source property destroyed, undefined behaviour ahead...");
                return;
            }

            TargetValueType expr_result = source();

            if constexpr (!std::is_reference_v<TargetValueType>)
            {
                auto *r = static_cast<std::decay_t<TargetValueType> *>(result);
                *r = std::move(expr_result);
            }
            else
            {
                auto **r = static_cast<std::decay_t<TargetValueType> **>(result);
                *r = const_cast<std::decay_t<decltype(*r)>>(&expr_result);
            }
        });
    source.changed.connect(target.changed);
}

template<typename TargetValueType, auto target_get, auto target_set, typename SourceValueType>
void bind(Property<TargetValueType, target_get, target_set> &target,
    Property<SourceValueType> &source)
{
    using PropertyTarget = Property<TargetValueType, target_get, target_set>;

    static_assert(
        std::is_same_v<typename PropertyTarget::ValueTypeGet, SourceValueType> ||
            (!std::is_reference_v<typename PropertyTarget::ValueTypeGet> &&
                std::is_same_v<std::decay_t<typename PropertyTarget::ValueTypeGet>,
                    std::decay_t<SourceValueType>> &&
                std::is_convertible_v<SourceValueType, typename PropertyTarget::ValueTypeGet>),
        "Binding target get method and source property type should be compatible");

    if (static_cast<void *>(&target) == static_cast<void *>(&source))
    {
        VT_LOG_ERROR("Cannot bind property to itself");
        return;
    }

    static_cast<PropertyBase *>(&target)->set_get_expression(
        [&source, lifeline = source.get_lifeline()](void *result) {
            if (lifeline.expired())
            {
                VT_LOG_ERROR("Binding source property destroyed, undefined behaviour ahead...");
                return;
            }

            typename PropertyTarget::ValueTypeGet expr_result = source();

            if constexpr (!std::is_reference_v<typename PropertyTarget::ValueTypeGet>)
            {
                auto *r =
                    static_cast<std::decay_t<typename PropertyTarget::ValueTypeGet> *>(result);
                *r = std::move(expr_result);
            }
            else
            {
                auto **r =
                    static_cast<std::decay_t<typename PropertyTarget::ValueTypeGet> **>(result);
                *r = const_cast<std::decay_t<decltype(*r)>>(&expr_result);
            }
        });
    source.changed.connect(target.changed);
}

template<typename TargetValueType, typename SourceValueType>
void bind(Property<TargetValueType> &target, Property<SourceValueType> &source)
{
    static_assert(
        std::is_same_v<TargetValueType, SourceValueType> ||
            (!std::is_reference_v<TargetValueType> &&
                std::is_same_v<std::decay_t<TargetValueType>, std::decay_t<SourceValueType>> &&
                std::is_convertible_v<SourceValueType, TargetValueType>),
        "Binding target type and source type should be compatible");

    if (static_cast<void *>(&target) == static_cast<void *>(&source))
    {
        VT_LOG_ERROR("Cannot bind property to itself");
        return;
    }

    static_cast<PropertyBase *>(&target)->set_get_expression(
        [&source, lifeline = source.get_lifeline()](void *result) {
            if (lifeline.expired())
            {
                VT_LOG_ERROR("Binding source property destroyed, undefined behaviour ahead...");
                return;
            }

            TargetValueType expr_result = source();

            if constexpr (!std::is_reference_v<TargetValueType>)
            {
                auto *r = static_cast<std::decay_t<TargetValueType> *>(result);
                *r = std::move(expr_result);
            }
            else
            {
                auto **r = static_cast<std::decay_t<TargetValueType> **>(result);
                *r = const_cast<std::decay_t<decltype(*r)>>(&expr_result);
            }
        });

    source.changed.connect(target.changed);
}

template<typename TargetValueType, auto target_get, auto target_set, typename SourceExpressionType>
void bind(Property<TargetValueType, target_get, target_set> &target,
    SourceExpressionType &&expression)
{
    using PropertyTarget = Property<TargetValueType, target_get, target_set>;

    static_assert(std::is_same_v<typename PropertyTarget::ValueTypeGet,
                      std::invoke_result_t<SourceExpressionType>>,
        "Binding source must be invocable without arguments and must result in the return type "
        "of the get method");

    static_cast<PropertyBase *>(&target)->set_get_expression(
        [expression = std::forward<SourceExpressionType>(expression)](void *result) {
            typename PropertyTarget::ValueTypeGet expr_result = expression();

            if constexpr (!std::is_reference_v<typename PropertyTarget::ValueTypeGet>)
            {
                auto *r =
                    static_cast<std::decay_t<typename PropertyTarget::ValueTypeGet> *>(result);
                *r = std::move(expr_result);
            }
            else
            {
                auto **r =
                    static_cast<std::decay_t<typename PropertyTarget::ValueTypeGet> **>(result);
                *r = const_cast<std::decay_t<decltype(*r)>>(&expr_result);
            }
        });
}

template<typename TargetValueType, typename SourceExpressionType>
void bind(Property<TargetValueType> &target, SourceExpressionType &&expression)
{
    static_assert(std::is_same_v<TargetValueType, std::invoke_result_t<SourceExpressionType>>,
        "Binding source must be invocable without arguments and must result in the return type "
        "of the simple property");

    static_cast<PropertyBase *>(&target)->set_get_expression(
        [expression = std::forward<SourceExpressionType>(expression)](void *result) {
            TargetValueType expr_result = expression();

            if constexpr (!std::is_reference_v<TargetValueType>)
            {
                auto *r = static_cast<std::decay_t<TargetValueType> *>(result);
                *r = std::move(expr_result);
            }
            else
            {
                auto **r = static_cast<std::decay_t<TargetValueType> **>(result);
                *r = const_cast<std::decay_t<decltype(*r)>>(&expr_result);
            }
        });
}

VT_END_NAMESPACE
