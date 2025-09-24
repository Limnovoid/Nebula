#ifndef NEUTRON_TYPES_H
#define NEUTRON_TYPES_H

#include "Units.h"
#include "Vector3.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

class AbsVector3 : public TVector3<Unit::Absolute>
{
	using Base = TVector3<Unit::Absolute>;
public:
	using TVector3::TVector3;

	constexpr AbsVector3(const float x, const float y, const float z);

	const Vector3 Get() const;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr AbsVector3::AbsVector3(const float x, const float y, const float z) :
	Base(Unit::Absolute(x), Unit::Absolute(y), Unit::Absolute(z))
{
}

// --------------------------------------------------------------------------------------------------------------------------------

inline const Vector3 AbsVector3::Get() const
{
	return Vector3(X().Get(), Y().Get(), Z().Get());
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class RelVector3 : public TVector3<Unit::Relative>
{
	using Base = TVector3<Unit::Relative>;

public:
	using TVector3::TVector3;

	constexpr RelVector3(const float x, const float y, const float z);

	const Vector3 Get() const;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr RelVector3::RelVector3(const float x, const float y, const float z) :
	Base(Unit::Relative(x), Unit::Relative(y), Unit::Relative(z))
{
}

// --------------------------------------------------------------------------------------------------------------------------------

inline const Vector3 RelVector3::Get() const
{
	return Vector3(X().Get(), Y().Get(), Z().Get());
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_TYPES_H
