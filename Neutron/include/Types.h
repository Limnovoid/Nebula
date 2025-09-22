#ifndef NEUTRON_TYPES_H
#define NEUTRON_TYPES_H

#include "LengthUnits.h"
#include "Vector3.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

class AbsVector3 : public TVector3<Length::Absolute>
{
	using Base = TVector3<Length::Absolute>;
public:
	using TVector3::TVector3;

	constexpr AbsVector3(const float x, const float y, const float z);

	const Vector3 Get() const;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline AbsVector3::AbsVector3(const float x, const float y, const float z) :
	Base(Length::Absolute(x), Length::Absolute(y), Length::Absolute(z))
{
}

// --------------------------------------------------------------------------------------------------------------------------------

inline const Vector3 AbsVector3::Get() const
{
	return Vector3(X().Get(), Y().Get(), Z().Get());
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class RelVector3 : public TVector3<Length::Relative>
{
	using Base = TVector3<Length::Relative>;

public:
	using TVector3::TVector3;

	constexpr RelVector3(const float x, const float y, const float z);

	const Vector3 Get() const;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline RelVector3::RelVector3(const float x, const float y, const float z) :
	Base(Length::Relative(x), Length::Relative(y), Length::Relative(z))
{
}

// --------------------------------------------------------------------------------------------------------------------------------

inline const Vector3 RelVector3::Get() const
{
	return Vector3(X().Get(), Y().Get(), Z().Get());
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_TYPES_H
