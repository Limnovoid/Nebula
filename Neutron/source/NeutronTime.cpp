#include "NeutronTime.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

template<Time::Granularity TFromPerSecond, Time::Granularity TToPerSecond>
struct FRangePeriodConvert
{
	Time::Period<TToPerSecond> operator()(size_t index)
	{
		return Time::Period<TToPerSecond>::Convert(Time::Period<TFromPerSecond>(index));
	}
};

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

TimeTestScript::TimeTestScript() :
	ITestScript("Time")
{
}

// --------------------------------------------------------------------------------------------------------------------------------

TimeTestScript::~TimeTestScript()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void TimeTestScript::RunImpl(TestHandler & testHandler)
{
	using namespace Time;

	// 1 second -> other.
	testHandler.Assert<Seconds, size_t>(FRangePeriodConvert<Second, Second>(), TestHandler::FRangeIndex(),
		[](size_t index) { return Seconds(index); }, "1 second == 1 second", { 0, 1000, 100 });

	testHandler.Assert<Milliseconds, size_t>(FRangePeriodConvert<Second, Millisecond>(), TestHandler::FRangeIndex(),
		[](size_t index) { return Milliseconds(index * Millisecond); }, "1 second == 1,000 ms", { 0, 1000, 100 });

	testHandler.Assert<Microseconds, size_t>(FRangePeriodConvert<Second, Microsecond>(), TestHandler::FRangeIndex(),
		[](size_t index) { return Microseconds(index * Microsecond); }, "1 second == 1,000,000 us", { 0, 1000, 100 });

	testHandler.Assert<Nanoseconds, size_t>(FRangePeriodConvert<Second, Nanosecond>(), TestHandler::FRangeIndex(),
		[](size_t index) { return Nanoseconds(index * Nanosecond); }, "1 second == 1,000,000,000 ns", { 0, 1000, 100 });

	// ms -> s
	testHandler.Assert<Seconds, size_t>(FRangePeriodConvert<Millisecond, Second>(), TestHandler::FRangeIndex(),
		TestHandler::FRangeZero(), "[1-999] milliseconds == 0s", { 0, 999, 100 });

	testHandler.Assert<Seconds, size_t>(FRangePeriodConvert<Millisecond, Second>(), TestHandler::FRangeIndex(),
		TestHandler::FRangeConstant(1ull), "[1000-1999] milliseconds == 1s", { 1000, 1999, 100 });

	// us -> ms
	testHandler.Assert<Milliseconds, size_t>(FRangePeriodConvert<Microsecond, Millisecond>(), TestHandler::FRangeIndex(),
		TestHandler::FRangeZero(), "[1-999] microseconds == 0ms", { 0, 999, 100 });

	testHandler.Assert<Milliseconds, size_t>(FRangePeriodConvert<Microsecond, Millisecond>(), TestHandler::FRangeIndex(),
		TestHandler::FRangeConstant(1ull), "[1000-1999] microseconds == 1ms", { 1000, 1999, 100 });

	// ns -> us
	testHandler.Assert<Microseconds, size_t>(FRangePeriodConvert<Nanosecond, Microsecond>(), TestHandler::FRangeIndex(),
		TestHandler::FRangeZero(), "[1-999] nanoseconds == 0us", { 0, 999, 100 });

	testHandler.Assert<Microseconds, size_t>(FRangePeriodConvert<Nanosecond, Microsecond>(), TestHandler::FRangeIndex(),
		TestHandler::FRangeConstant(1ull), "[1000-1999] nanoseconds == 1us", { 1000, 1999, 100 });

	// Arithmetic
	testHandler.Assert<Seconds, size_t>([](size_t index) { return Seconds(index) + Seconds(index); }, TestHandler::FRangeIndex(),
		[](size_t index) { return Seconds(index * 2); }, "operator+", { 0, 1000, 100 });

	testHandler.Assert<Seconds, size_t>([](size_t index) { return Seconds(index) - Seconds(index); }, TestHandler::FRangeIndex(),
		TestHandler::FRangeZero(), "operator-", { 0, 1000, 100 });

	return;

	Seconds seconds1(1);
	Seconds seconds2(1);

	seconds1 += seconds2;

	Microseconds microseconds1(Period<Microsecond>::Convert(seconds1)); // error

	//microseconds1 += seconds1; // error

	microseconds1.AddEqual(seconds1);

	Microseconds microseconds2 = microseconds1.Add(seconds2);
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
