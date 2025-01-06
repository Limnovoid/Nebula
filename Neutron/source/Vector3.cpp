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
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
