#ifndef NEBULA_EXPLICIT_H
#define NEBULA_EXPLICIT_H

#include "ITestScript.h"

namespace Nebula // -----------------------------------------------------------------------------------------------------------------
{

template<typename T, typename TOKEN>
class TExplicitArithmetic
{
public:
	TExplicitArithmetic() = default;
	constexpr explicit TExplicitArithmetic(const T value);

	constexpr const T Get() const;

	void Set(const T value);

	constexpr bool operator==(TExplicitArithmetic const& rhs) const							{ return m_value == rhs.m_value; }
	constexpr bool operator!=(TExplicitArithmetic const& rhs) const							{ return m_value != rhs.m_value; }
	constexpr bool operator<(TExplicitArithmetic const& rhs) const							{ return m_value < rhs.m_value; }
	constexpr bool operator>(TExplicitArithmetic const& rhs) const							{ return m_value > rhs.m_value; }
	constexpr bool operator<=(TExplicitArithmetic const& rhs) const							{ return m_value <= rhs.m_value; }
	constexpr bool operator>=(TExplicitArithmetic const& rhs) const							{ return m_value >= rhs.m_value; }
	constexpr bool operator==(const T rhs) const											{ return m_value == rhs; }
	constexpr bool operator!=(const T rhs) const											{ return m_value != rhs; }
	constexpr bool operator<(const T rhs) const												{ return m_value < rhs; }
	constexpr bool operator>(const T rhs) const												{ return m_value > rhs; }
	constexpr bool operator<=(const T rhs) const											{ return m_value <= rhs; }
	constexpr bool operator>=(const T rhs) const											{ return m_value >= rhs; }

	constexpr const TExplicitArithmetic operator-() const									{ return TExplicitArithmetic(-m_value); }

	constexpr const TExplicitArithmetic operator+(TExplicitArithmetic const& rhs) const		{ return TExplicitArithmetic(m_value + rhs.m_value); }
	constexpr const TExplicitArithmetic operator-(TExplicitArithmetic const& rhs) const		{ return TExplicitArithmetic(m_value - rhs.m_value); }
	constexpr const TExplicitArithmetic operator*(TExplicitArithmetic const& rhs) const		{ return TExplicitArithmetic(m_value * rhs.m_value); }
	constexpr const TExplicitArithmetic operator/(TExplicitArithmetic const& rhs) const		{ return TExplicitArithmetic(m_value / rhs.m_value); }
	constexpr const TExplicitArithmetic operator+(const T rhs) const						{ return TExplicitArithmetic(m_value + rhs); }
	constexpr const TExplicitArithmetic operator-(const T rhs) const						{ return TExplicitArithmetic(m_value - rhs); }
	constexpr const TExplicitArithmetic operator*(const T rhs) const						{ return TExplicitArithmetic(m_value * rhs); }
	constexpr const TExplicitArithmetic operator/(const T rhs) const						{ return TExplicitArithmetic(m_value / rhs); }

	constexpr TExplicitArithmetic & operator+=(TExplicitArithmetic const& rhs);
	constexpr TExplicitArithmetic & operator-=(TExplicitArithmetic const& rhs);
	constexpr TExplicitArithmetic & operator*=(TExplicitArithmetic const& rhs);
	constexpr TExplicitArithmetic & operator/=(TExplicitArithmetic const& rhs);
	constexpr TExplicitArithmetic & operator+=(const T rhs);
	constexpr TExplicitArithmetic & operator-=(const T rhs);
	constexpr TExplicitArithmetic & operator*=(const T rhs);
	constexpr TExplicitArithmetic & operator/=(const T rhs);

private:
	T	m_value;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TOKEN>
inline constexpr TExplicitArithmetic<T, TOKEN>::TExplicitArithmetic(const T value) :
	m_value(value)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TOKEN>
inline constexpr const T TExplicitArithmetic<T, TOKEN>::Get() const
{
	return m_value;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TOKEN>
inline void TExplicitArithmetic<T, TOKEN>::Set(const T value)
{
	m_value = value;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TOKEN>
inline constexpr TExplicitArithmetic<T, TOKEN> & TExplicitArithmetic<T, TOKEN>::operator+=(const TExplicitArithmetic<T, TOKEN> &rhs)
{
	m_value += rhs.m_value;
	return *this;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TOKEN>
inline constexpr TExplicitArithmetic<T, TOKEN> & TExplicitArithmetic<T, TOKEN>::operator-=(const TExplicitArithmetic<T, TOKEN> &rhs)
{
	m_value -= rhs.m_value;
	return *this;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TOKEN>
inline constexpr TExplicitArithmetic<T, TOKEN> & TExplicitArithmetic<T, TOKEN>::operator*=(const TExplicitArithmetic<T, TOKEN> &rhs)
{
	m_value *= rhs.m_value;
	return *this;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TOKEN>
inline constexpr TExplicitArithmetic<T, TOKEN> & TExplicitArithmetic<T, TOKEN>::operator/=(const TExplicitArithmetic<T, TOKEN> &rhs)
{
	m_value /= rhs.m_value;
	return *this;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TOKEN>
inline constexpr TExplicitArithmetic<T, TOKEN> & TExplicitArithmetic<T, TOKEN>::operator+=(const T rhs)
{
	m_value += rhs;
	return *this;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TOKEN>
inline constexpr TExplicitArithmetic<T, TOKEN> & TExplicitArithmetic<T, TOKEN>::operator-=(const T rhs)
{
	m_value -= rhs;
	return *this;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TOKEN>
inline constexpr TExplicitArithmetic<T, TOKEN> & TExplicitArithmetic<T, TOKEN>::operator*=(const T rhs)
{
	m_value *= rhs;
	return *this;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TOKEN>
inline constexpr TExplicitArithmetic<T, TOKEN> & TExplicitArithmetic<T, TOKEN>::operator/=(const T rhs)
{
	m_value /= rhs;
	return *this;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TOKEN>
inline constexpr bool operator<(const T lhs, TExplicitArithmetic<T, TOKEN> const& rhs)
{
	return rhs > lhs;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TOKEN>
inline constexpr bool operator>(const T lhs, TExplicitArithmetic<T, TOKEN> const& rhs)
{
	return rhs < lhs;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TOKEN>
inline constexpr bool operator<=(const T lhs, TExplicitArithmetic<T, TOKEN> const& rhs)
{
	return rhs >= lhs;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TOKEN>
inline constexpr bool operator>=(const T lhs, TExplicitArithmetic<T, TOKEN> const& rhs)
{
	return rhs <= lhs;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class ExplicitArithmeticTestScript : public Nebula::ITestScript
{
public:
	ExplicitArithmeticTestScript();
	virtual ~ExplicitArithmeticTestScript();

protected:
	virtual void RunImpl(Nebula::TestHandler & testHandler) override;
};

} // namespace Nebula ---------------------------------------------------------------------------------------------------------------

#endif//NEBULA_EXPLICIT_H
