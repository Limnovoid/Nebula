#include "Types.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

const RelVector3 AbsVector3::ToRelative(ScalingSphereBase const& scalingSphereBase) const
{
	return RelVector3(ToRelativeValue(scalingSphereBase));
}

// --------------------------------------------------------------------------------------------------------------------------------

const Vector3 AbsVector3::ToRelativeValue(ScalingSphereBase const& scalingSphereBase) const
{
	return Vector3(X().ToRelativeValue(scalingSphereBase), Y().ToRelativeValue(scalingSphereBase), Z().ToRelativeValue(scalingSphereBase));
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

const AbsVector3 RelVector3::ToAbsolute(ScalingSphereBase const& scalingSphereBase) const
{
	return AbsVector3(ToAbsoluteValue(scalingSphereBase));
}

// --------------------------------------------------------------------------------------------------------------------------------

const Vector3 RelVector3::ToAbsoluteValue(ScalingSphereBase const& scalingSphereBase) const
{
	return Vector3(X().ToAbsoluteValue(scalingSphereBase), Y().ToAbsoluteValue(scalingSphereBase), Z().ToAbsoluteValue(scalingSphereBase));
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
