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

ResultCode PhysicsEngine::CreateBody(Body::Attributes const& attributes, SharedPtr<LocalSpace> pHostSpace)
{
	SharedPtr<Body> pNewBody = MakeShared<Body>(attributes, pHostSpace);

	assert(false); // TODO ...

	return RESULT_CODE_NOT_IMPLEMENTED;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

PhysicsEngine::Orbit::Elements::Elements(LocalSpace const& localSpace, Vector3 const& localPosition, Vector3 const& localVelocity)
{
	ASSERT_THROW_RESULT(SUCCESS, !=, Compute(localSpace, localPosition, localVelocity),
		Fmt::Format("Failed to construct elements from arguments: LocalSpace {}, position {}, velocity {}",
			localSpace.m_uuid, localPosition, localVelocity));
}

// --------------------------------------------------------------------------------------------------------------------------------

Result PhysicsEngine::Orbit::Elements::Compute(LocalSpace const& localSpace, Vector3 const& localPosition, Vector3 const& localVelocity)
{
	Vector3 positionFromPrimary = localPosition - localSpace.m_primaryAttributes.m_localPosition;
	Vector3 velocityFromPrimary = localVelocity - localSpace.m_primaryAttributes.m_localVelocity;

	Vector3 angularMomentumVector = positionFromPrimary.PreciseCross(velocityFromPrimary);

	return RESULT_CODE_NOT_IMPLEMENTED;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

PhysicsEngine::Node::Node(Uuid const uuid) :
	m_uuid(uuid)
{
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

bool PhysicsEngine::LocalSpace::IsInfluencing() const
{
	assert(false); // TODO ...

	return true;
}

// --------------------------------------------------------------------------------------------------------------------------------

Result PhysicsEngine::LocalSpace::OnHostTick()
{
	(void) ComputeLocalPrimaryKinetics();

	assert(false); // TODO ...
}

// --------------------------------------------------------------------------------------------------------------------------------

PhysicsEngine::LocalSpace::PrimaryAttributes const& PhysicsEngine::LocalSpace::ComputeLocalPrimaryKinetics()
{
	m_primaryAttributes.m_localPosition = 0.f;
	m_primaryAttributes.m_localVelocity = 0.f;

	LocalSpace * pHostSpace = this;
	float scaling = 1.f;

	while ((nullptr != pHostSpace) && !pHostSpace->IsInfluencing())
	{
		scaling /= pHostSpace->m_radius;

		SharedPtr<Body> pHost = pHostSpace->m_pHost;

		m_primaryAttributes.m_localPosition -= pHost->m_attributes.m_localPosition * scaling;
		m_primaryAttributes.m_localVelocity -= pHost->m_attributes.m_localVelocity * scaling;

		pHostSpace = pHost->m_pHostSpace.get();
	}

	return m_primaryAttributes;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

PhysicsEngine::Body::Body(Attributes const& attributes, SharedPtr<LocalSpace> pHostSpace) :
	m_attributes(attributes),
	m_elements(*pHostSpace, attributes.m_localPosition, attributes.m_localVelocity),
	m_pHostSpace(pHostSpace)
{
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

PhysicsEngine::System::System(float hostMass, float hostSpaceTrueRadius) :
	m_hostMass(hostMass),
	m_pHostSpace(MakeShared<LocalSpace>(hostMass, hostSpaceTrueRadius))
{
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
