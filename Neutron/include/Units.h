#ifndef NEUTRON_LENGTH_UNITS_H
#define NEUTRON_LENGTH_UNITS_H

#include "ExplicitArithmetic.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

class ScalingSphereBase;

namespace Unit // -----------------------------------------------------------------------------------------------------------------
{

class Relative;

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class Absolute : public Nebula::TExplicitArithmetic<float, Absolute>
{
	using Base = Nebula::TExplicitArithmetic<float, Absolute>;

public:
	using ArithmeticType = float;

	constexpr explicit Absolute(const float value);
	Absolute(Relative const& relative, ScalingSphereBase const& scalingSphereBase);

	const Relative ToRelative(ScalingSphereBase const& scalingSphereBase) const;
	const float ToRelativeValue(ScalingSphereBase const& scalingSphereBase) const;

	void Set(Relative const& relative, ScalingSphereBase const& scalingSphereBase);
};

// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr Absolute::Absolute(const float value) :
	Base(value)
{
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class Relative : public Nebula::TExplicitArithmetic<float, Relative>
{
	using Base = Nebula::TExplicitArithmetic<float, Relative>;

public:
	using ArithmeticType = float;

	constexpr explicit Relative(const float value);
	Relative(Absolute const& absolute, ScalingSphereBase const& scalingSphereBase);

	const Absolute ToAbsolute(ScalingSphereBase const& scalingSphereBase) const;
	const float ToAbsoluteValue(ScalingSphereBase const& scalingSphereBase) const;

	void Set(Absolute const& absolute, ScalingSphereBase const& scalingSphereBase);
};

// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr Relative::Relative(const float value) :
	Base(value)
{
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class Radians : public Nebula::TExplicitArithmetic<float, Radians>
{
	using Base = Nebula::TExplicitArithmetic<float, Radians>;

public:
	using Base::Base;
};

} // namespace Unit ---------------------------------------------------------------------------------------------------------------

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

template<>
struct std::formatter<Neutron::Unit::Absolute> : std::formatter<uint8_t>
{
	auto format(Neutron::Unit::Absolute const& relative, std::format_context & ctx) const
	{
		return std::format_to(ctx.out(), "{}", relative.Get());
	}
};

template<>
struct std::formatter<Neutron::Unit::Relative> : std::formatter<uint8_t>
{
	auto format(Neutron::Unit::Relative const& relative, std::format_context & ctx) const
	{
		return std::format_to(ctx.out(), "{}", relative.Get());
	}
};

#endif//NEUTRON_LENGTH_UNITS_H
