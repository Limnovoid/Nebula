#include "NeutronTime.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

namespace Time // -----------------------------------------------------------------------------------------------------------------
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

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

template<Time::Granularity TFromPerSecond, Time::Granularity TToPerSecond>
struct IndexConverter
{
	Time::Period<TToPerSecond> operator()(const ITest::Sequence::Index index)
	{
		return Time::Period<TToPerSecond>::Convert(Time::Period<TFromPerSecond>(index));
	}
};

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

void TimeTest::Run() const
{
	Sequence sequence(0, 1000, 100);

	// 1 second -> other.
	AssertSequence("Convert seconds to seconds", sequence, [this](const Sequence::Index index)
	{
		Time::Seconds lhs = IndexConverter<Second, Second>()(index), rhs = Seconds(index * Second);
		AssertOperator(Fmt::Format("<{}> seconds == <{}> seconds", index, rhs), lhs, Equal(rhs));
	});

	AssertSequence("Convert seconds to milliseconds", sequence, [this](const Sequence::Index index)
	{
		Time::Milliseconds lhs = IndexConverter<Second, Millisecond>()(index), rhs = Milliseconds(index * Millisecond);
		AssertOperator(Fmt::Format("<{}> seconds == <{}> milliseconds", index, rhs), lhs, Equal(rhs));
	});

	AssertSequence("Convert seconds to microseconds", sequence, [this](const Sequence::Index index)
	{
		Time::Microseconds lhs = IndexConverter<Second, Microsecond>()(index), rhs = Microseconds(index * Microsecond);
		AssertOperator(Fmt::Format("<{}> seconds == <{}> microseconds", index, rhs), lhs, Equal(rhs));
	});

	AssertSequence("Convert seconds to nanoseconds", sequence, [this](const Sequence::Index index)
	{
		Time::Nanoseconds lhs = IndexConverter<Second, Nanosecond>()(index), rhs = Nanoseconds(index * Nanosecond);
		AssertOperator(Fmt::Format("<{}> seconds == <{}> nanoseconds", index, rhs), lhs, Equal(rhs));
	});

	// TEMP
	AssertSequence("INTENTIONAL FAIL - Convert seconds to seconds", sequence, [this](const Sequence::Index index)
	{
		Time::Seconds lhs(0), rhs = Seconds(index * Second);
		AssertOperator(Fmt::Format("INTENTIONAL FAIL - <{}> seconds == <{}> seconds", index, rhs), lhs, Equal(rhs));
	});
}

} // namespace Time ---------------------------------------------------------------------------------------------------------------

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
