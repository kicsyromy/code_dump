#pragma once

#include "voot_global.hh"
#include "core/voot_signal.hh"

#define VT_PROPERTY(type, name, klass, getter, setter, notify)       \
    voot::Property<type, klass, &klass::getter, &klass::setter> name \
    {                                                                \
        notify, this                                                 \
    }

#define VT_CONST_PROPERTY(type, name, klass, getter, setter)
#define VT_READONLY_PROPERTY(type, name, klass, getter, notify)
#define VT_READONLY_CONST_PROPERTY(type, name, klass, getter)

VOOT_BEGIN_NAMESPACE

template<typename T, typename Class> using GetMethod = const std::decay_t<T> &(Class::*)() const;
template<typename T, typename Class> using SetMethod = void (Class::*)(const std::decay_t<T> &);

template<typename T,
    typename MemberOf,
    GetMethod<T, MemberOf> getter,
    SetMethod<T, MemberOf> setter>
class Property
{
public:
    using ValueType = std::decay_t<T>;

public:
    Property(Signal<ValueType> &notify, MemberOf *parent_object)
      : notify_{ notify }
      , parent_object_{ parent_object }
      , getter_{ [](const void *obj) noexcept(
                     std::is_nothrow_invocable_v<decltype(getter), void>) -> const ValueType & {
          const auto *object = static_cast<const MemberOf *>(obj);
          return (object->*getter)();
      } }
      , setter_{ [](const ValueType &v, void *obj) noexcept(
                     std::is_nothrow_invocable_v<decltype(setter), const ValueType &>) {
          auto *object = static_cast<MemberOf *>(obj);
          (object->*setter)(v);
      } }
    {}

public:
    Property &operator=(const ValueType &v) noexcept(
        std::is_nothrow_invocable_v<decltype(setter), const ValueType &>)
    {
        setter_(v, parent_object_);
        return *this;
    }

    const ValueType &operator()() const
        noexcept(std::is_nothrow_invocable_v<decltype(getter), void>)
    {
        return getter_(parent_object_);
    }

private:
    Signal<ValueType> &notify_;

    void *parent_object_;

    using GetFunction = const ValueType &(*)(const void *);
    GetFunction getter_;

    using SetFunction = void (*)(const ValueType &, void *);
    SetFunction setter_;
};

VOOT_END_NAMESPACE
