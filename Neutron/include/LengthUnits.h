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
	constexpr explicit Absolute(const float value);
	Absolute(const Relative relative, ScalingSphereBase const& scalingSphereBase);

	float Get() const;

	Relative ToRelative(ScalingSphereBase const& scalingSphereBase) const;
	float ToRelativeValue(ScalingSphereBase const& scalingSphereBase) const;

	void Set(const float value);
	void Set(const Relative relative, ScalingSphereBase const& scalingSphereBase);

	operator float() const;

	const Absolute operator+(const float rhs) const;
	const Absolute operator-(const float rhs) const;
	const Absolute operator*(const float rhs) const;
	const Absolute operator/(const float rhs) const;

	Absolute operator+=(const float rhs);
	Absolute operator-=(const float rhs);
	Absolute operator*=(const float rhs);
	Absolute operator/=(const float rhs);

private:
	float	m_value;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr Absolute::Absolute(const float value) :
	m_value(value)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

inline float Absolute::Get() const
{
	return m_value;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void Absolute::Set(const float value)
{
	m_value = value;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Absolute::operator float() const
{
	return m_value;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline const Absolute Absolute::operator+(const float rhs) const
{
	return Absolute(m_value + rhs);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline const Absolute Absolute::operator-(const float rhs) const
{
	return Absolute(m_value - rhs);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline const Absolute Absolute::operator*(const float rhs) const
{
	return Absolute(m_value * rhs);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline const Absolute Absolute::operator/(const float rhs) const
{
	return Absolute(m_value / rhs);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Absolute Absolute::operator+=(const float rhs)
{
	m_value += rhs;
	return *this;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Absolute Absolute::operator-=(const float rhs)
{
	m_value -= rhs;
	return *this;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Absolute Absolute::operator*=(const float rhs)
{
	m_value *= rhs;
	return *this;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Absolute Absolute::operator/=(const float rhs)
{
	m_value /= rhs;
	return *this;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class Relative
{
public:
	constexpr explicit Relative(const float value);
	Relative(const Absolute absolute, ScalingSphereBase const& scalingSphereBase);

	float Get() const;

	Absolute ToAbsolute(ScalingSphereBase const& scalingSphereBase) const;
	float ToAbsoluteValue(ScalingSphereBase const& scalingSphereBase) const;

	void Set(const float value);
	void Set(const Absolute absolute, ScalingSphereBase const& scalingSphereBase);

	operator float() const;

private:
	float	m_value;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr Relative::Relative(const float value) :
	m_value(value)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

inline float Relative::Get() const
{
	return m_value;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void Relative::Set(const float value)
{
	m_value = value;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Relative::operator float() const
{
	return m_value;
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_LENGTH_UNITS_H
