#ifndef NEUTRON_LENGTH_UNITS_H
#define NEUTRON_LENGTH_UNITS_H

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

class Relative;
class ScalingSphereBase;

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class Absolute
{
public:
	explicit Absolute(const float value);
	Absolute(Relative const& relative, ScalingSphereBase const& scalingSphereBase);

	float GetValue() const;

	Relative ToRelative(ScalingSphereBase const& scalingSphereBase) const;
	float ToRelativeValue(ScalingSphereBase const& scalingSphereBase) const;

	void SetValue(const float value);
	void SetValue(Relative const& relative, ScalingSphereBase const& scalingSphereBase);

private:
	float	m_value;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline float Absolute::GetValue() const
{
	return m_value;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class Relative
{
public:
	explicit Relative(const float value);
	Relative(const Absolute absolute, ScalingSphereBase const& scalingSphereBase);

	float GetValue() const;

	Absolute ToAbsolute(ScalingSphereBase const& scalingSphereBase) const;
	float ToAbsoluteValue(ScalingSphereBase const& scalingSphereBase) const;

	void SetValue(const float value);
	void SetValue(const Absolute absolute, ScalingSphereBase const& scalingSphereBase);

private:
	float						m_value;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline float Relative::GetValue() const
{
	return m_value;
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_LENGTH_UNITS_H
