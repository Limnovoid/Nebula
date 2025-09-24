#include "LengthUnits.h"

#include "ScalingSphereBase.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

namespace Length // --------------------------------------------------------------------------------------------------------------
{

Absolute::Absolute(Relative const& relative, ScalingSphereBase const& scalingSphereBase) :
	Base(relative.ToAbsoluteValue(scalingSphereBase))
{
}

// --------------------------------------------------------------------------------------------------------------------------------

const Relative Absolute::ToRelative(ScalingSphereBase const& scalingSphereBase) const
{
	return Relative(ToRelativeValue(scalingSphereBase));
}

// --------------------------------------------------------------------------------------------------------------------------------

const float Absolute::ToRelativeValue(ScalingSphereBase const& scalingSphereBase) const
{
	return Get() / scalingSphereBase.GetAbsoluteRadius().Get();
}

// --------------------------------------------------------------------------------------------------------------------------------

void Absolute::Set(Relative const& relative, ScalingSphereBase const& scalingSphereBase)
{
	Base::Set(relative.ToAbsoluteValue(scalingSphereBase));
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

Relative::Relative(Absolute const& absolute, ScalingSphereBase const& scalingSphereBase) :
	Base(absolute.ToRelativeValue(scalingSphereBase))
{
}

// --------------------------------------------------------------------------------------------------------------------------------

const Absolute Relative::ToAbsolute(ScalingSphereBase const& scalingSphereBase) const
{
	return Absolute(ToAbsoluteValue(scalingSphereBase));
}

// --------------------------------------------------------------------------------------------------------------------------------

const float Relative::ToAbsoluteValue(ScalingSphereBase const& scalingSphereBase) const
{
	return Get() * scalingSphereBase.GetAbsoluteRadius().Get();
}

// --------------------------------------------------------------------------------------------------------------------------------

void Relative::Set(Absolute const& relative, ScalingSphereBase const& scalingSphereBase)
{
	Base::Set(relative.ToRelativeValue(scalingSphereBase));
}

} // namespace Length -------------------------------------------------------------------------------------------------------------

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
