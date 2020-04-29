#pragma once

#include "voot_global.hh"
#include "core/voot_signal.hh"

#include <type_traits>

#define VT_PROPERTY(type, name, getter, setter)                                                    \
    voot::                                                                                         \
        Property<type, voot::property::impl::GetClassType<decltype(getter)>::Type, getter, setter> \
            name                                                                                   \
    {                                                                                              \
        this                                                                                       \
    }

#define VT_READONLY_PROPERTY(type, klass, getter)    \
    voot::Property<type, klass, &klass::getter> name \
    {                                                \
        this                                         \
    }

VOOT_BEGIN_NAMESPACE

namespace property
{
    template<typename T, typename Class>
    using GetMethod = const std::decay_t<T> &(Class::*)() const;
    template<typename T, typename Class> using SetMethod = bool (Class::*)(const std::decay_t<T> &);

    namespace impl
    {
        template<typename...> struct GetClassType : std::false_type
        {
        };
        template<typename R, typename Class> struct GetClassType<R (Class::*)() const>
        {
            using Type = Class;
        };
        template<typename R, typename Class> struct GetClassType<R (Class::*)() const noexcept>
        {
            using Type = Class;
        };
    } // namespace impl
} // namespace property

template<typename T,
    typename MemberOf,
    property::GetMethod<T, MemberOf> getter,
    property::SetMethod<T, MemberOf> setter = nullptr>
class Property
{
public:
    using ValueType = std::decay_t<T>;

private:
    using GetFunction = const ValueType &(*)(const void *);
    using SetFunction = bool (*)(const ValueType &, void *);

public:
    Property(MemberOf *parent_object)
      : parent_object_{ parent_object }
      , getter_{ [](const void *obj) noexcept(
                     std::is_nothrow_invocable_v<decltype(getter), void>) -> const ValueType & {
          const auto *object = static_cast<const MemberOf *>(obj);
          return (object->*getter)();
      } }
      , setter_{ nullptr }
    {
        if constexpr (setter != nullptr)
        {
            setter_ =
                [](const ValueType &v, void *obj) noexcept(
                    std::is_nothrow_invocable_v<decltype(setter), const ValueType &>) -> bool {
                auto *object = static_cast<MemberOf *>(obj);
                return (object->*setter)(v);
            };
        }
    }

public:
    Property &operator=(const ValueType &v) noexcept(
        std::is_nothrow_invocable_v<decltype(setter), const ValueType &>)
    {
        static_assert(setter != nullptr, "Cannot assign to a readonly property");
        const bool value_set = setter_(v, parent_object_);
        if (value_set)
        {
            changed.emit(v);
        }
        return *this;
    }

    const ValueType &operator()() const
        noexcept(std::is_nothrow_invocable_v<decltype(getter), void>)
    {
        return getter_(parent_object_);
    }

public:
    Signal<ValueType> changed;

private:
    void *parent_object_;

    GetFunction getter_;
    SetFunction setter_;
};

VOOT_END_NAMESPACE
