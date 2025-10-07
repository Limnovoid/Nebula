#ifndef NEUTRON_TYPES_H
#define NEUTRON_TYPES_H

#include "Units.h"
#include "Vector3.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

class RelVector3;

// --------------------------------------------------------------------------------------------------------------------------------

class AbsVector3 : public TVector3<Unit::Absolute, AbsVector3>
{
	using Base = TVector3<Unit::Absolute, AbsVector3>;

	friend class Base;

public:
	using Base::Base;

	//constexpr AbsVector3(Base const& rhs);
	//constexpr AbsVector3(AbsVector3 const& rhs);
	constexpr AbsVector3(const float value);
	constexpr AbsVector3(const float x, const float y, const float z);

	const Vector3 Get() const;

	const RelVector3 ToRelative(ScalingSphereBase const& scalingSphereBase) const;
	const Vector3 ToRelativeValue(ScalingSphereBase const& scalingSphereBase) const;
};

// --------------------------------------------------------------------------------------------------------------------------------

//inline constexpr AbsVector3::AbsVector3(Base const& rhs) :
//	Base(rhs)
//{
//}

// --------------------------------------------------------------------------------------------------------------------------------

//inline constexpr AbsVector3::AbsVector3(AbsVector3 const& rhs) :
//	Base(static_cast<Base const&>(rhs))
//{
//}

// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr AbsVector3::AbsVector3(const float value) :
	Base(Unit::Absolute(value))
{
}

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

class RelVector3 : public TVector3<Unit::Relative, RelVector3>
{
	using Base = TVector3<Unit::Relative, RelVector3>;

	friend class Base;

public:
	using Base::Base;

	//constexpr RelVector3();
	//constexpr RelVector3(Base const& rhs);
	//constexpr RelVector3(RelVector3 const& rhs);
	constexpr RelVector3(const float value);
	constexpr RelVector3(const float x, const float y, const float z);
	//constexpr RelVector3(Unit::Relative const& value);
	//constexpr RelVector3(Unit::Relative const& x, Unit::Relative const& y, Unit::Relative const& z);

	const Vector3 Get() const;

	const AbsVector3 ToAbsolute(ScalingSphereBase const& scalingSphereBase) const;
	const Vector3 ToAbsoluteValue(ScalingSphereBase const& scalingSphereBase) const;
};

// --------------------------------------------------------------------------------------------------------------------------------

//inline constexpr RelVector3::RelVector3() :
//	Base()
//{
//}

// --------------------------------------------------------------------------------------------------------------------------------

//inline constexpr RelVector3::RelVector3(Base const& rhs) :
//	Base(rhs)
//{
//}

// --------------------------------------------------------------------------------------------------------------------------------

//inline constexpr RelVector3::RelVector3(RelVector3 const& rhs) :
//	Base(rhs)
//{
//}

// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr RelVector3::RelVector3(const float value) :
	Base(Unit::Relative(value))
{
}

// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr RelVector3::RelVector3(const float x, const float y, const float z) :
	Base(Unit::Relative(x), Unit::Relative(y), Unit::Relative(z))
{
}

// --------------------------------------------------------------------------------------------------------------------------------

//inline constexpr RelVector3::RelVector3(Unit::Relative const& value) :
//	Base(value)
//{
//}

// --------------------------------------------------------------------------------------------------------------------------------

//inline constexpr RelVector3::RelVector3(Unit::Relative const& x, Unit::Relative const& y, Unit::Relative const& z) :
//	Base(x, y, z)
//{
//}

// --------------------------------------------------------------------------------------------------------------------------------

inline const Vector3 RelVector3::Get() const
{
	return Vector3(X().Get(), Y().Get(), Z().Get());
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

template<>
struct std::formatter<Neutron::AbsVector3> : std::formatter<Neutron::Vector3>
{
	auto format(Neutron::AbsVector3 const& relVector3, std::format_context & ctx) const
	{
		return std::format_to(ctx.out(), Neutron::Vector3::FORMAT_STRING.Get(), relVector3.X(), relVector3.Y(), relVector3.Z());
	}
};

template<>
struct std::formatter<Neutron::RelVector3> : std::formatter<Neutron::Vector3>
{
	auto format(Neutron::RelVector3 const& absVector3, std::format_context & ctx) const
	{
		return std::format_to(ctx.out(), Neutron::Vector3::FORMAT_STRING.Get(), absVector3.X(), absVector3.Y(), absVector3.Z());
	}
};

#endif//NEUTRON_TYPES_H
