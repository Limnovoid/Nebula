#include "PhysicsEngine.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;

// --------------------------------------------------------------------------------------------------------------------------------

PhysicsEngine::PhysicsEngine()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void PhysicsEngine::Tick(Time::Microseconds dT)
{
	assert(false); // TODO ...
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

Result PhysicsEngine::Orbit::Elements::Compute(LocalSpace const& localSpace, Vector3 const& localPosition, Vector3 const& localVelocity)
{
	Vector3 positionFromPrimary = localPosition - localSpace.GetLocalPrimaryPosition();
	Vector3 velocityFromPrimary = localVelocity - localSpace.GetLocalPrimaryVelocity();

	Vector3 angularMomentumVector = positionFromPrimary.PreciseCross(velocityFromPrimary);

	return RESULT_CODE_NOT_IMPLEMENTED;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

Vector3 PhysicsEngine::LocalSpace::GetLocalPrimaryPosition() const
{
	assert(false); // TODO ...

	return {};
}

// --------------------------------------------------------------------------------------------------------------------------------

Vector3 PhysicsEngine::LocalSpace::GetLocalPrimaryVelocity() const
{
	assert(false); // TODO ...

	return {};
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

PhysicsEngineTestScript::PhysicsEngineTestScript() :
	ITestScript("PhysicsEngine")
{

}

// --------------------------------------------------------------------------------------------------------------------------------

PhysicsEngineTestScript::~PhysicsEngineTestScript()
{

}

// --------------------------------------------------------------------------------------------------------------------------------

void PhysicsEngineTestScript::RunImpl(TestHandler & testHandler)
{
	testHandler.Assert<float, int>([](int)
	{
		float a = FLT_EPSILON, b = 1.f;

		return a + b;
	}, 0, 1.f + FLT_EPSILON, "1.f + epsilon");

	testHandler.Assert<float, int>([](int)
	{
		float a = FLT_EPSILON * 0.1f, b = 1.f;

		return a + b;
	}, 0, 1.f, "1.f + (epsilon * 0.1f)");

	testHandler.Assert<float, int>([](int)
	{
		float a = FLT_EPSILON + (FLT_EPSILON * 0.1f), b = 1.f;

		return a + b;
	}, 0, 1.f + FLT_EPSILON, "1.f + (epsilon + (epsilon * 0.1f))");

	testHandler.Assert<float, int>([](int)
	{
		float a = FLT_EPSILON, b = 1.f;

		return a - ((a + b) - b);

	}, 0, 0.f, "a - ((a + b) - b) | a = epsilon, b = 1.f");

	testHandler.Assert<float, int>([](int)
	{
		float a = FLT_EPSILON * 0.1f, b = 1.f;

		return a - ((a + b) - b);

	}, 0, FLT_EPSILON * 0.1f, "a - ((a + b) - b) | a = epsilon * 0.1f, b = 1.f");

	testHandler.Assert<float, int>([](int)
	{
		float a = FLT_EPSILON + (FLT_EPSILON * 0.1f), b = 1.f;

		return a - ((a + b) - b);

	}, 0, FLT_EPSILON * 0.1f, "a - ((a + b) - b) | a = epsilon + (epsilon * 0.1f), b = 1.f");
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
