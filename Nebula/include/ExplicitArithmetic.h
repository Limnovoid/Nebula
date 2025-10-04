#ifndef NEBULA_EXPLICIT_ARITHMETIC_H
#define NEBULA_EXPLICIT_ARITHMETIC_H

#include "ITestScript.h"

namespace Nebula // -----------------------------------------------------------------------------------------------------------------
{

template<typename T, typename CRTP>
class TExplicitArithmetic
{
public:
	using ArithmeticType = T;

	TExplicitArithmetic() = default;
	constexpr explicit TExplicitArithmetic(const T value);

	constexpr const T Get() const;

	void Set(const T value);

	constexpr const T operator*() const;

	constexpr bool operator==(TExplicitArithmetic const& rhs) const							{ return m_value == rhs.m_value; }
	//constexpr bool operator!=(TExplicitArithmetic const& rhs) const						{ return m_value != rhs.m_value; }
	constexpr bool operator<(TExplicitArithmetic const& rhs) const							{ return m_value < rhs.m_value; }
	constexpr bool operator>(TExplicitArithmetic const& rhs) const							{ return m_value > rhs.m_value; }
	constexpr bool operator<=(TExplicitArithmetic const& rhs) const							{ return m_value <= rhs.m_value; }
	constexpr bool operator>=(TExplicitArithmetic const& rhs) const							{ return m_value >= rhs.m_value; }
	constexpr bool operator==(const T rhs) const											{ return m_value == rhs; }
	//constexpr bool operator!=(const T rhs) const											{ return m_value != rhs; }
	constexpr bool operator<(const T rhs) const												{ return m_value < rhs; }
	constexpr bool operator>(const T rhs) const												{ return m_value > rhs; }
	constexpr bool operator<=(const T rhs) const											{ return m_value <= rhs; }
	constexpr bool operator>=(const T rhs) const											{ return m_value >= rhs; }

	constexpr const CRTP operator-() const													{ return CRTP(-m_value); }

	constexpr const CRTP operator+(TExplicitArithmetic const& rhs) const					{ return CRTP(m_value + rhs.m_value); }
	constexpr const CRTP operator-(TExplicitArithmetic const& rhs) const					{ return CRTP(m_value - rhs.m_value); }
	constexpr const CRTP operator*(TExplicitArithmetic const& rhs) const					{ return CRTP(m_value * rhs.m_value); }
	constexpr const CRTP operator/(TExplicitArithmetic const& rhs) const					{ return CRTP(m_value / rhs.m_value); }
	constexpr const CRTP operator+(const T rhs) const										{ return CRTP(m_value + rhs); }
	constexpr const CRTP operator-(const T rhs) const										{ return CRTP(m_value - rhs); }
	constexpr const CRTP operator*(const T rhs) const										{ return CRTP(m_value * rhs); }
	constexpr const CRTP operator/(const T rhs) const										{ return CRTP(m_value / rhs); }

	constexpr CRTP & operator+=(TExplicitArithmetic const& rhs);
	constexpr CRTP & operator-=(TExplicitArithmetic const& rhs);
	constexpr CRTP & operator*=(TExplicitArithmetic const& rhs);
	constexpr CRTP & operator/=(TExplicitArithmetic const& rhs);
	constexpr CRTP & operator+=(const T rhs);
	constexpr CRTP & operator-=(const T rhs);
	constexpr CRTP & operator*=(const T rhs);
	constexpr CRTP & operator/=(const T rhs);

	friend constexpr const CRTP operator+(const T lhs, TExplicitArithmetic const& rhs);
	friend constexpr const CRTP operator-(const T lhs, TExplicitArithmetic const& rhs);
	friend constexpr const CRTP operator*(const T lhs, TExplicitArithmetic const& rhs);
	friend constexpr const CRTP operator/(const T lhs, TExplicitArithmetic const& rhs);

private:
	T	m_value;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline constexpr TExplicitArithmetic<T, CRTP>::TExplicitArithmetic(const T value) :
	m_value(value)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline constexpr const T TExplicitArithmetic<T, CRTP>::Get() const
{
	return m_value;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline void TExplicitArithmetic<T, CRTP>::Set(const T value)
{
	m_value = value;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
constexpr const T TExplicitArithmetic<T, CRTP>::operator*() const
{
	return m_value;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline constexpr CRTP & TExplicitArithmetic<T, CRTP>::operator+=(const TExplicitArithmetic<T, CRTP> &rhs)
{
	m_value += rhs.m_value;
	return *static_cast<CRTP *>(this);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline constexpr CRTP & TExplicitArithmetic<T, CRTP>::operator-=(const TExplicitArithmetic<T, CRTP> &rhs)
{
	m_value -= rhs.m_value;
	return *static_cast<CRTP *>(this);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline constexpr CRTP & TExplicitArithmetic<T, CRTP>::operator*=(const TExplicitArithmetic<T, CRTP> &rhs)
{
	m_value *= rhs.m_value;
	return *static_cast<CRTP *>(this);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline constexpr CRTP & TExplicitArithmetic<T, CRTP>::operator/=(const TExplicitArithmetic<T, CRTP> &rhs)
{
	m_value /= rhs.m_value;
	return *static_cast<CRTP *>(this);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline constexpr CRTP & TExplicitArithmetic<T, CRTP>::operator+=(const T rhs)
{
	m_value += rhs;
	return *static_cast<CRTP *>(this);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline constexpr CRTP & TExplicitArithmetic<T, CRTP>::operator-=(const T rhs)
{
	m_value -= rhs;
	return *static_cast<CRTP *>(this);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline constexpr CRTP & TExplicitArithmetic<T, CRTP>::operator*=(const T rhs)
{
	m_value *= rhs;
	return *static_cast<CRTP *>(this);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline constexpr CRTP & TExplicitArithmetic<T, CRTP>::operator/=(const T rhs)
{
	m_value /= rhs;
	return *static_cast<CRTP *>(this);
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline constexpr bool operator<(const T lhs, TExplicitArithmetic<T, CRTP> const& rhs)
{
	return rhs > lhs;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline constexpr bool operator>(const T lhs, TExplicitArithmetic<T, CRTP> const& rhs)
{
	return rhs < lhs;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline constexpr bool operator<=(const T lhs, TExplicitArithmetic<T, CRTP> const& rhs)
{
	return rhs >= lhs;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline constexpr bool operator>=(const T lhs, TExplicitArithmetic<T, CRTP> const& rhs)
{
	return rhs <= lhs;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline constexpr const CRTP operator+(const T lhs, TExplicitArithmetic<T, CRTP> const& rhs)
{
	return CRTP(lhs + rhs.m_value);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline constexpr const CRTP operator-(const T lhs, TExplicitArithmetic<T, CRTP> const& rhs)
{
	return CRTP(lhs - rhs.m_value);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline constexpr const CRTP operator*(const T lhs, TExplicitArithmetic<T, CRTP> const& rhs)
{
	return CRTP(lhs * rhs.m_value);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline constexpr const CRTP operator/(const T lhs, TExplicitArithmetic<T, CRTP> const& rhs)
{
	return CRTP(lhs / rhs.m_value);
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

#endif//NEBULA_EXPLICIT_ARITHMETIC_H
