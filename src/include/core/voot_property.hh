#pragma once

#include "voot_global.hh"
#include "core/voot_signal.hh"
#include "core/voot_lifeline.hh"
#include "core/voot_logger.hh"

#include <type_traits>
#include <variant>

#define VT_PROPERTY(type, name, getter, setter) \
    voot::Property<type, getter, setter> name   \
    {                                           \
        this                                    \
    }

#define VT_SIMPLE_PROPERTY(type, name) voot::Property<type> name

#define VT_READONLY_PROPERTY(type, name, getter) \
    voot::Property<type, getter, nullptr> name   \
    {                                            \
        this                                     \
    }

VT_BEGIN_NAMESPACE

namespace property
{
    template<typename T, typename Class> using GetMethod = std::decay_t<T> (Class::*)() const;
    template<typename T, typename Class> using GetMethodRef = std::decay_t<T> &(Class::*)();
    template<typename T, typename Class>
    using GetMethodConstRef = const std::decay_t<T> &(Class::*)() const;

    template<typename T, typename Class> using SetMethod = bool (Class::*)(std::decay_t<T>);
    template<typename T, typename Class> using SetMethodRef = bool (Class::*)(std::decay_t<T> &);
    template<typename T, typename Class>
    using SetMethodConstRef = bool (Class::*)(const std::decay_t<T> &);

    template<typename...> struct GetValueType : std::false_type
    {
    };
    template<typename T, typename Class> struct GetValueType<T (Class::*)() const>
    {
        using Type = T;
    };
    template<typename T, typename Class> struct GetValueType<T (Class::*)() const noexcept>
    {
        using Type = T;
    };
    template<typename T, typename Class> struct GetValueType<T &(Class::*)()>
    {
        using Type = T &;
    };
    template<typename T, typename Class> struct GetValueType<T &(Class::*)() noexcept>
    {
        using Type = T &;
    };
    template<typename T, typename Class> struct GetValueType<const T &(Class::*)() const>
    {
        using Type = const T &;
    };
    template<typename T, typename Class> struct GetValueType<const T &(Class::*)() const noexcept>
    {
        using Type = const T &;
    };

    template<typename...> struct SetValueType : std::false_type
    {
        using Type = void *;
    };
    template<typename T, typename Class> struct SetValueType<bool (Class::*)(T)>
    {
        using Type = T;
    };
    template<typename T, typename Class> struct SetValueType<bool (Class::*)(T) noexcept>
    {
        using Type = T;
    };
    template<typename T, typename Class> struct SetValueType<bool (Class::*)(T &)>
    {
        using Type = T &;
    };
    template<typename T, typename Class> struct SetValueType<bool (Class::*)(T &) noexcept>
    {
        using Type = T &;
    };
    template<typename T, typename Class> struct SetValueType<bool (Class::*)(const T &)>
    {
        using Type = const T &;
    };
    template<typename T, typename Class> struct SetValueType<bool (Class::*)(const T &) noexcept>
    {
        using Type = const T &;
    };
} // namespace property

class PropertyBase
{
    VT_ADD_LIFELINE

public:
    template<typename Callable> void set_get_expression(Callable &&callable)
    {
        static_assert(std::is_invocable_v<Callable, void *>);
        get_expression_ = std::move(callable);
    }

protected:
    std::function<void(void *)> get_expression_{ nullptr };
};

template<typename T, auto...> class Property : std::false_type
{
};

template<typename T, auto getter, auto setter>
class Property<T, getter, setter> : public PropertyBase
{
    using MemberOf = typename utility::GetClassType<decltype(getter)>::Type;

    static constexpr bool get_simple =
        std::is_convertible_v<decltype(getter), property::GetMethod<T, MemberOf>>;
    static constexpr bool get_ref =
        std::is_convertible_v<decltype(getter), property::GetMethodRef<T, MemberOf>>;
    static constexpr bool get_const_ref =
        std::is_convertible_v<decltype(getter), property::GetMethodConstRef<T, MemberOf>>;

    static constexpr bool set_simple =
        std::is_convertible_v<decltype(setter), property::SetMethod<T, MemberOf>>;
    static constexpr bool set_ref =
        std::is_convertible_v<decltype(setter), property::SetMethodRef<T, MemberOf>>;
    static constexpr bool set_const_ref =
        std::is_convertible_v<decltype(setter), property::SetMethodConstRef<T, MemberOf>>;
    static constexpr bool set_null = std::is_same_v<decltype(setter), std::nullptr_t>;

    static_assert(get_simple || get_ref || get_const_ref, "Invalid get function");
    static_assert(set_simple || set_ref || set_const_ref || set_null, "Invalid set function");

public:
    using ValueTypeGet = typename property::GetValueType<decltype(getter)>::Type;
    using ValueTypeSet = typename property::SetValueType<decltype(setter)>::Type;

public:
    Property(MemberOf *parent_object)
      : parent_object_{ parent_object }
    {}

public:
    Property &operator=(ValueTypeSet v) noexcept(
        std::is_nothrow_invocable_v<decltype(setter), ValueTypeSet>)
    {
        static_assert(!set_null, "Cannot assign to a readonly property");
        if (get_expression_ != nullptr)
        {
            VT_LOG_WARN("Breaking property binding...");
            get_expression_ = nullptr;
        }
        const bool value_set = (parent_object_->*setter)(v);
        if (value_set)
        {
            changed.emit(v);
        }
        return *this;
    }

    ValueTypeGet operator()() const
    {
        if (get_expression_ != nullptr)
        {
            if constexpr (get_simple)
            {
                ValueTypeGet result;
                get_expression_(&result);
                return result;
            }

            if constexpr (get_ref)
            {
                std::decay_t<ValueTypeGet> *result;
                get_expression_(&result);
                return *result;
            }

            if constexpr (get_const_ref)
            {
                const std::decay_t<ValueTypeGet> *result;
                get_expression_(&result);
                return *result;
            }
        }

        return (parent_object_->*getter)();
    }

public:
    Signal<ValueTypeGet> changed;

private:
    MemberOf *parent_object_;
};

template<typename T> class Property<T> : public PropertyBase
{
public:
    using ValueType = std::remove_const_t<std::remove_reference_t<T>>;

public:
    Property &operator=(const ValueType &v) noexcept
    {
        if (get_expression_ != nullptr)
        {
            VT_LOG_WARN("Breaking property binding...");
            get_expression_ = nullptr;
        }

        if (v != value_)
        {
            value_ = v;
            changed.emit(v);
        }

        return *this;
    }

    T operator()() const
    {
        if (get_expression_ != nullptr)
        {
            if constexpr (!std::is_reference_v<T>)
            {
                T result;
                get_expression_(&result);
                return result;
            }
            else
            {
                std::decay_t<T> *result;
                get_expression_(&result);
                return *result;
            }
        }

        return value_;
    }

public:
    Signal<T> changed;

private:
    ValueType value_{};
};

VT_END_NAMESPACE
