#pragma once

#include "voot_global.hh"
#include "core/voot_property.hh"

VOOT_BEGIN_NAMESPACE

class BindingBase
{
public:
    constexpr BindingBase(void *target_property,
        void (*slot)(void *) = nullptr,
        void *object = nullptr) noexcept
      : target_property_{ target_property }
      , slot_{ slot }
      , object_{ object }
    {}

protected:
    void *target_property_;
    void (*slot_)(void *);
    void *object_;
};

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

VOOT_END_NAMESPACE
