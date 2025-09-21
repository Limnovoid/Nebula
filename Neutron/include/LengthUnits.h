#ifndef NEUTRON_LENGTH_UNITS_H
#define NEUTRON_LENGTH_UNITS_H

#include "ExplicitArithmetic.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

class ScalingSphereBase;

namespace Length // --------------------------------------------------------------------------------------------------------------
{

struct ExplicitArithmeticTokenAbsolute {};
struct ExplicitArithmeticTokenRelative {};
class Relative;

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class Absolute : public Nebula::TExplicitArithmetic<float, ExplicitArithmeticTokenAbsolute>
{
	using Base = Nebula::TExplicitArithmetic<float, ExplicitArithmeticTokenAbsolute>;

public:
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

class Relative : public Nebula::TExplicitArithmetic<float, ExplicitArithmeticTokenRelative>
{
	using Base = Nebula::TExplicitArithmetic<float, ExplicitArithmeticTokenRelative>;

public:
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

} // namespace Length -------------------------------------------------------------------------------------------------------------

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_LENGTH_UNITS_H
