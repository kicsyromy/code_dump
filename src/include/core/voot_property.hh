#pragma once

#include "voot_global.hh"
#include "core/voot_signal.hh"

#include <type_traits>
#include <variant>

#define VT_PROPERTY(type, name, getter, setter)                                                    \
    voot::Property<type, voot::utility::GetClassType<decltype(getter)>::Type, getter, setter> name \
    {                                                                                              \
        this                                                                                       \
    }

#define VT_READONLY_PROPERTY(type, name, getter)                                           \
    voot::Property<type, voot::utility::GetClassType<decltype(getter)>::Type, getter> name \
    {                                                                                      \
        this                                                                               \
    }

VOOT_BEGIN_NAMESPACE

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

template<typename T, typename MemberOf, auto getter, auto setter = nullptr> class Property
{
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

private:
    using GetFunction = ValueTypeGet (*)(void *);
    using SetFunction = bool (*)(ValueTypeSet, void *);

public:
    Property(MemberOf *parent_object)
      : parent_object_{ parent_object }
      , getter_{ [](void *obj) noexcept(
                     std::is_nothrow_invocable_v<decltype(getter), void>) -> ValueTypeGet {
          if constexpr (get_ref)
          {
              auto *object = static_cast<MemberOf *>(obj);
              return (object->*getter)();
          }
          else
          {
              const auto *object = static_cast<const MemberOf *>(obj);
              return (object->*getter)();
          }
      } }
      , setter_{ nullptr }
    {
        if constexpr (setter != nullptr)
        {
            setter_ = [](ValueTypeSet v, void *obj) noexcept(
                          std::is_nothrow_invocable_v<decltype(setter), ValueTypeSet>) -> bool {
                auto *object = static_cast<MemberOf *>(obj);
                return (object->*setter)(v);
            };
        }
    }

public:
    Property &operator=(ValueTypeSet v) noexcept(
        std::is_nothrow_invocable_v<decltype(setter), ValueTypeSet>)
    {
        static_assert(setter != nullptr, "Cannot assign to a readonly property");
        const bool value_set = setter_(v, parent_object_);
        if (value_set)
        {
            changed.emit(v);
        }
        return *this;
    }

    ValueTypeGet operator()() const noexcept(std::is_nothrow_invocable_v<decltype(getter), void>)
    {
        return getter_(parent_object_);
    }

public:
    Signal<std::decay_t<T>> changed;

private:
    void *parent_object_;

    GetFunction getter_;
    SetFunction setter_;
};

VOOT_END_NAMESPACE
