#include "Vector3.h"

#include "TestHandler.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;

// --------------------------------------------------------------------------------------------------------------------------------

Vector3TestScript::Vector3TestScript() :
	ITestScript("Vector3")
{
}

// --------------------------------------------------------------------------------------------------------------------------------

Vector3TestScript::~Vector3TestScript()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void Vector3TestScript::RunImpl(TestHandler & testHandler)
{
	testHandler.Assert<String, size_t>([](size_t index) { return Fmt::Format("{}", Vector3(Maths::Sqrt((float)index))); }, TestHandler::FRangeIndex(),
		[](size_t index) { return Fmt::Format(Vector3::FORMAT_STRING.Get(), Maths::Sqrt((float)index), Maths::Sqrt((float)index), Maths::Sqrt((float)index)); },
		"Fmt::Format with Vector3 argument", {1, 5});

	testHandler.Assert<Vector3, int>([](int) { return Vector3::Zero(); }, 0, Vector3(0, 0, 0), "Vector3::Zero() is (0 0 0)");
	testHandler.Assert<bool, int>([](int) { return Vector3::Zero().IsZero(); }, 0, true, "Vector3::Zero().IsZero()");

	testHandler.Assert<Vector3, size_t>([&](size_t index)
	{
		Vector3 vector((float)index);

		testHandler.Assert<float, int>([&](int) { return vector.X(); }, 0, (float)index, "Vector3::X()");
		testHandler.Assert<float, int>([&](int) { return vector.Y(); }, 0, (float)index, "Vector3::Y()");
		testHandler.Assert<float, int>([&](int) { return vector.Z(); }, 0, (float)index, "Vector3::Z()");

		return vector;

	}, TestHandler::FRangeIndex(), [](size_t index) { return Vector3((float)index); }, "Vector3 construction and equality", { 0, 4 });

	testHandler.Assert<Vector3, size_t>([&](size_t index)
	{
		Vector3 vector((float)index);

		vector += vector;

		return vector;

	}, TestHandler::FRangeIndex(), [](size_t index) { return Vector3((float)(index + index)); }, "Vector3 operator+=", { 0, 4 });

	testHandler.Assert<Vector3, size_t>([&](size_t index)
	{
		Vector3 vector((float)index);

		vector -= vector;

		return vector;

	}, TestHandler::FRangeIndex(), [](size_t index) { return Vector3::Zero(); }, "Vector3 operator-=", { 0, 4 });

	testHandler.Assert<Vector3, size_t>([&](size_t index)
	{
		Vector3 vector((float)index);

		vector *= (float)index;

		return vector;

	}, TestHandler::FRangeIndex(), [](size_t index) { return Vector3((float)(index * index)); }, "Vector3 operator*=", { 0, 4 });

	testHandler.Assert<Vector3, size_t>([&](size_t index)
	{
		Vector3 vector((float)index);

		vector /= (float)index;

		return vector;

	}, TestHandler::FRangeIndex(), [](size_t index) { return Vector3(1.f); }, "Vector3 operator/=", { 1, 5 });

	// Dot.
	testHandler.Assert<float, size_t>([&](size_t index)
	{
		Vector3 lhs((float)index, 0, 0);
		Vector3 rhs((float)index, 0, 0);

		return lhs.Dot(rhs);

	}, TestHandler::FRangeIndex(), [](size_t index) { return (float)Maths::Pow(index, 2); }, "Vector3 Dot of equal vectors", { 1, 5 });

	testHandler.Assert<float, int>([&](int index)
	{
		Vector3 lhs((float)index, 0, 0);
		Vector3 rhs(0, (float)(index * ((index % 2) ? 1 : -1)), 0);

		return lhs.Dot(rhs);

	}, TestHandler::FRangeIndex<int>(), TestHandler::FRangeZero<int, float>(), "Vector3 Dot of perpendicular vectors", TestHandler::IndexRange<int>(1, 10));

	testHandler.Assert<float, int64_t>([&](int64_t index)
	{
		Vector3 lhs((float)index, 0, 0);
		Vector3 rhs((float)(index * ((index % 2) ? 1 : -1)), 0, 0);

		return lhs.Dot(rhs);

	}, TestHandler::FRangeIndex(), [](int64_t index) { return (float)(Maths::Pow(index, 2) * ((index % 2) ? 1 : -1)); }, "Vector3 Dot of parallel and anti-parallel vectors", { 1, 5 });

	testHandler.Assert<bool, int64_t>([&](int64_t index)
	{
		Vector3 lhs((float)index, ((index % 2) ? 1.f : -1.f), 0);
		Vector3 rhs(1.f, 0.f, 0);

		return (0.f < lhs.Dot(rhs));

	}, TestHandler::FRangeIndex(), [](int64_t index) { return (0 < index); }, "Vector3 Dot product has correct sign", { -5, 5, 2 });

	// Cross.
	testHandler.Assert<Vector3, int>([&](int index)
	{
		Vector3 lhs((float)index, 0, 0);
		Vector3 rhs(0, (float)(index * ((index % 2) ? 1 : -1)), 0);

		return lhs.Cross(rhs);

	}, TestHandler::FRangeIndex<int>(), [](int index) { return Vector3(0, 0, (float)(Maths::Pow(index, 2) * ((index % 2) ? 1 : -1))); },
		"Vector3 Cross of perpendicular vectors", TestHandler::IndexRange<int>(1, 10));

	testHandler.Assert<Vector3, int>([&](int index)
	{
		Vector3 lhs((float)index, 0, 0);
		Vector3 rhs((index % 2) ? 1.f : -1.f, 0.f, 0);

		return lhs.Cross(rhs);

	}, TestHandler::FRangeIndex<int>(), [](int index) { return Vector3(); }, "Vector3 Cross of parallel and anti-parallel vectors", TestHandler::IndexRange<int>(1, 10));

	// Precise Cross.
	testHandler.Assert<Vector3, int>([&](int index)
	{
		Vector3 lhs((float)index, 0, 0);
		Vector3 rhs(0, (float)(index * ((index % 2) ? 1 : -1)), 0);

		return lhs.PreciseCross(rhs);

	}, TestHandler::FRangeIndex<int>(), [](int index) { return Vector3(0, 0, (float)(Maths::Pow(index, 2) * ((index % 2) ? 1 : -1))); },
		"Vector3 Precise Cross of perpendicular vectors", TestHandler::IndexRange<int>(1, 10));

	testHandler.Assert<Vector3, int>([&](int index)
	{
		Vector3 lhs((float)index, 0, 0);
		Vector3 rhs((index % 2) ? 1.f : -1.f, 0.f, 0);

		return lhs.PreciseCross(rhs);

	}, TestHandler::FRangeIndex<int>(), [](int index) { return Vector3(); }, "Vector3 Precise Cross of parallel and anti-parallel vectors", TestHandler::IndexRange<int>(1, 10));

	testHandler.Assert<Vector3, int>([&](int index)
	{
		Vector3 lhs((float)index, 0, 0);
		Vector3 rhs(5 < index ? 1.f : 0.f, (float)(index * ((index % 2) ? 1 : -1)), 0);

		return lhs.PreciseCross(rhs);

	}, TestHandler::FRangeIndex<int>(), [](int index)
	{
		Vector3 lhs((float)index, 0, 0);
		Vector3 rhs(5 < index ? 1.f : 0.f, (float)(index * ((index % 2) ? 1 : -1)), 0);

		return lhs.Cross(rhs);

	}, "Vector3 Precise Cross equals Cross", TestHandler::IndexRange<int>(1, 10));

	// Approx. parallel
	assert(false); // TODO ...
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
