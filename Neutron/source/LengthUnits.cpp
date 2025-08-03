#include "LengthUnits.h"

#include "ScalingSphereBase.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

Absolute::Absolute(const Relative relative, ScalingSphereBase const& scalingSphereBase) :
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

void Absolute::Set(const Relative relative, ScalingSphereBase const& scalingSphereBase)
{
	m_value = relative.ToAbsoluteValue(scalingSphereBase);
}

// --------------------------------------------------------------------------------------------------------------------------------
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

// --------------------------------------------------------------------------------------------------------------------------------

void Relative::Set(const Absolute relative, ScalingSphereBase const& scalingSphereBase)
{
	m_value = relative.ToRelativeValue(scalingSphereBase);
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
