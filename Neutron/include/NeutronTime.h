#ifndef NEUTRON_TIME_H
#define NEUTRON_TIME_H

#include "TestHandler.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;

// --------------------------------------------------------------------------------------------------------------------------------

namespace Time // -----------------------------------------------------------------------------------------------------------------
{

enum Granularity : uint32_t
{
	Second			= 1,
	Millisecond		= 1000,
	Microsecond		= 1000000,
	Nanosecond		= 1000000000
};

// --------------------------------------------------------------------------------------------------------------------------------

template<Granularity NPerSecond>
class Period
{
	template<Granularity NOtherPerSecond>
	friend class Period;

public:
	template<Granularity NFromPerSecond>
	static Period Convert(Period<NFromPerSecond> const fromPeriod);

	Period(int64_t value);

	template<IsInt T = int64_t>
	int64_t Get() const;

	template<Granularity NToPerSecond>
	Period<NToPerSecond> Convert() const;

	template<Granularity NOtherPerSecond>
	Period Add(Period<NOtherPerSecond> otherPeriod);

	template<Granularity NOtherPerSecond>
	Period & AddEqual(Period<NOtherPerSecond> otherPeriod);

	bool operator==(Period const& rhs) const;
	Period operator+(Period const& rhs) const;
	Period operator-(Period const& rhs) const;
	Period & operator+=(Period const& rhs);
	Period & operator-=(Period const& rhs);

private:
	template<Granularity NToPerSecond, Granularity NFromPerSecond>
	static int64_t Convert(int64_t const fromValue);

	int64_t		m_value;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<Granularity NPerSecond>
template<Granularity NFromPerSecond>
inline Period<NPerSecond> Period<NPerSecond>::Convert(Period<NFromPerSecond> const fromPeriod)
{
	return { Convert<NPerSecond, NFromPerSecond>(fromPeriod.m_value) };
}

// --------------------------------------------------------------------------------------------------------------------------------

template<Granularity NPerSecond>
inline Period<NPerSecond>::Period(int64_t value) :
	m_value(value)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<Granularity NPerSecond>
template<IsInt T>
inline int64_t Period<NPerSecond>::Get() const
{
	return static_cast<T>(m_value);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<Granularity NPerSecond>
template<Granularity NToPerSecond>
inline Period<NToPerSecond> Period<NPerSecond>::Convert() const
{
	return { Convert<NToPerSecond, NPerSecond>(m_value) };
}

// --------------------------------------------------------------------------------------------------------------------------------

template<Granularity NPerSecond>
template<Granularity NOtherPerSecond>
inline Period<NPerSecond> Period<NPerSecond>::Add(Period<NOtherPerSecond> otherPeriod)
{
	int64_t returnValue = m_value;

	if constexpr (NPerSecond < NOtherPerSecond)
	{
		returnValue *= (NOtherPerSecond / NPerSecond);
	}
	else if constexpr (NOtherPerSecond < NPerSecond)
	{
		otherPeriod.m_value *= (NPerSecond / NOtherPerSecond);
	}

	returnValue += otherPeriod.m_value;

	if constexpr (NPerSecond < NOtherPerSecond)
	{
		returnValue /= (NOtherPerSecond / NPerSecond);
	}

	return { returnValue };
}

// --------------------------------------------------------------------------------------------------------------------------------

template<Granularity NPerSecond>
template<Granularity NOtherPerSecond>
inline Period<NPerSecond> & Period<NPerSecond>::AddEqual(Period<NOtherPerSecond> otherPeriod)
{
	if constexpr (NPerSecond < NOtherPerSecond)
	{
		m_value *= (NOtherPerSecond / NPerSecond);
	}
	else if constexpr (NOtherPerSecond < NPerSecond)
	{
		otherPeriod.m_value *= (NPerSecond / NOtherPerSecond);
	}

	m_value += otherPeriod.m_value;

	if constexpr (NPerSecond < NOtherPerSecond)
	{
		m_value /= (NOtherPerSecond / NPerSecond);
	}

	return *this;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<Granularity NPerSecond>
inline bool Period<NPerSecond>::operator==(Period const& rhs) const
{
	return m_value == rhs.m_value;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<Granularity NPerSecond>
inline Period<NPerSecond> Period<NPerSecond>::operator+(Period const& rhs) const
{
	return { m_value + rhs.m_value };
}

// --------------------------------------------------------------------------------------------------------------------------------

template<Granularity NPerSecond>
inline Period<NPerSecond> Period<NPerSecond>::operator-(Period const& rhs) const
{
	return { m_value - rhs.m_value };
}

// --------------------------------------------------------------------------------------------------------------------------------

template<Granularity NPerSecond>
inline Period<NPerSecond> & Period<NPerSecond>::operator+=(Period const& rhs)
{
	m_value += rhs.m_value;

	return *this;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<Granularity NPerSecond>
inline Period<NPerSecond> & Period<NPerSecond>::operator-=(Period const& rhs)
{
	m_value -= rhs.m_value;

	return *this;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<Granularity NPerSecond>
template<Granularity NToPerSecond, Granularity NFromPerSecond>
inline int64_t Period<NPerSecond>::Convert(int64_t const fromValue)
{
	if constexpr (NToPerSecond < NFromPerSecond)
	{
		return fromValue / (NFromPerSecond / NToPerSecond);
	}
	else if constexpr (NFromPerSecond < NToPerSecond)
	{
		return fromValue * (NToPerSecond / NFromPerSecond);
	}

	return fromValue;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

typedef Period<Granularity::Second>			Seconds;
typedef Period<Granularity::Millisecond>	Milliseconds;
typedef Period<Granularity::Microsecond>	Microseconds;
typedef Period<Granularity::Nanosecond>		Nanoseconds;

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class TimeTestScript : public ITestScript
{
public:
	TimeTestScript();
	virtual ~TimeTestScript();

protected:
	virtual void RunImpl(TestHandler & testHandler) override;
};

} // namespace Time ---------------------------------------------------------------------------------------------------------------

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

template<>
struct std::formatter<Neutron::Time::Seconds> : std::formatter<uint8_t>
{
	auto format(Neutron::Time::Seconds const seconds, std::format_context & ctx) const
	{
		return std::format_to(ctx.out(), "{}", seconds.Get());
	}
};

// --------------------------------------------------------------------------------------------------------------------------------

template<>
struct std::formatter<Neutron::Time::Milliseconds> : std::formatter<uint8_t>
{
	auto format(Neutron::Time::Milliseconds const milliseconds, std::format_context & ctx) const
	{
		return std::format_to(ctx.out(), "{}", milliseconds.Get());
	}
};

// --------------------------------------------------------------------------------------------------------------------------------

template<>
struct std::formatter<Neutron::Time::Microseconds> : std::formatter<uint8_t>
{
	auto format(Neutron::Time::Microseconds const microseconds, std::format_context & ctx) const
	{
		return std::format_to(ctx.out(), "{}", microseconds.Get());
	}
};

// --------------------------------------------------------------------------------------------------------------------------------

template<>
struct std::formatter<Neutron::Time::Nanoseconds> : std::formatter<uint8_t>
{
	auto format(Neutron::Time::Nanoseconds const nanoseconds, std::format_context & ctx) const
	{
		return std::format_to(ctx.out(), "{}", nanoseconds.Get());
	}
};

#endif//NEUTRON_TIME_H
