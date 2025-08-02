#include "LengthUnits.h"

#include "ScalingSphereBase.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

Absolute::Absolute(const float value) :
	m_value(value)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

Absolute::Absolute(Relative const& relative, ScalingSphereBase const& scalingSphereBase) :
	m_value(relative.ToAbsoluteValue(scalingSphereBase))
{
}

// --------------------------------------------------------------------------------------------------------------------------------

Relative Absolute::ToRelative(ScalingSphereBase const& scalingSphereBase) const
{
	return Relative(ToRelativeValue(scalingSphereBase));
}

// --------------------------------------------------------------------------------------------------------------------------------

float Absolute::ToRelativeValue(ScalingSphereBase const& scalingSphereBase) const
{
	return m_value / scalingSphereBase.GetTrueRadius();
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

Relative::Relative(const float value) :
	m_value(value)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

Relative::Relative(const Absolute absolute, ScalingSphereBase const& scalingSphereBase) :
	m_value(absolute.ToRelativeValue(scalingSphereBase))
{
}

// --------------------------------------------------------------------------------------------------------------------------------

Absolute Relative::ToAbsolute(ScalingSphereBase const& scalingSphereBase) const
{
	return Absolute(ToAbsoluteValue(scalingSphereBase));
}

// --------------------------------------------------------------------------------------------------------------------------------

float Relative::ToAbsoluteValue(ScalingSphereBase const& scalingSphereBase) const
{
	return m_value * scalingSphereBase.GetTrueRadius();
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
