#include "OrbitalSystem.h"

#include "Particle.h"
#include "ScalingSpace.h"
#include "TestHandler.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

namespace Orbital // --------------------------------------------------------------------------------------------------------------
{

System::System(float hostMass, float hostSpaceTrueRadius) :
	m_hostMass(hostMass),
	m_pHostSpace(MakeShared<ScalingSpace>(hostMass, hostSpaceTrueRadius)),
	m_scalingSpaces{ m_pHostSpace }
{
}

// --------------------------------------------------------------------------------------------------------------------------------

SharedPtr<ScalingSpace> System::CreateScalingSpace(float trueRadius)
{
	float radius = trueRadius / m_pHostSpace->GetTrueRadius();

	SharedPtr<ScalingSpace> pNewScalingSpace = MakeShared<ScalingSpace>(m_hostMass, trueRadius, radius);

	m_scalingSpaces.Insert(pNewScalingSpace);

	return pNewScalingSpace;
}

// --------------------------------------------------------------------------------------------------------------------------------

SharedPtr<Particle> System::CreateParticle(float mass, Vector3 position, Vector3 velocity, SharedPtr<ScalingSpace> pHostSpace)
{
	SharedPtr<Particle> pNewParticle = MakeShared<Particle>(Particle::State{ mass, position, velocity }, pHostSpace);

	pHostSpace->m_particles.push_back(pNewParticle);

	assert(false); // TODO ...

	return nullptr;
}

// --------------------------------------------------------------------------------------------------------------------------------

SharedPtr<ScalingSpace> System::CreateScalingSpace(float trueRadius, SharedPtr<Particle> pHostParticle)
{
	assert(false); // TODO ...

	return nullptr;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

SystemTestScript::SystemTestScript() :
	ITestScript("OrbitalSystem")
{
}

// --------------------------------------------------------------------------------------------------------------------------------

SystemTestScript::~SystemTestScript()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void SystemTestScript::RunImpl(TestHandler & testHandler)
{
	static const float hostMass = 1e30f;
	static const float hostSpaceTrueRadius = 8e12f;

	TestHandler::OutputMode outputMode = testHandler.SetOutputMode(TestHandler::VERBOSE);

	System orbitalSystem(hostMass, hostSpaceTrueRadius);

	SharedPtr<ScalingSpace> pHostSpace = orbitalSystem.GetHostSpace();

	// Host space.
	testHandler.Assert(pHostSpace->GetRadius(), 1.f, "Host space radius");
	testHandler.Assert(pHostSpace->GetTrueRadius(), hostSpaceTrueRadius, "Host space true radius");

	ScalingSpace::List const& hostScalingSpaces = orbitalSystem.GetScalingSpaces();

	testHandler.Assert(hostScalingSpaces.size(), 1ull, "Host space list size");

	// Adding host spaces.
	testHandler.Assert<int, int>([&](int index)
	{
		float const trueRadius = hostSpaceTrueRadius / powf(2.f, static_cast<float>(index));
		float const radius = 1.f / powf(2.f, static_cast<float>(index));

		SharedPtr<ScalingSpace> pNewHostSpace = orbitalSystem.CreateScalingSpace(trueRadius);

		testHandler.Assert(pNewHostSpace->GetTrueRadius(), trueRadius, "True radius");
		testHandler.Assert(pNewHostSpace->GetRadius(), radius, "Radius");

		testHandler.Assert((*(--hostScalingSpaces.cend()))->m_uuid, pNewHostSpace->m_uuid, "List position");

		return static_cast<int>(hostScalingSpaces.size());

	}, TestHandler::FRangeIndex<int>(), TestHandler::FRangeIndex<int>(1), "Adding host spaces", TestHandler::IndexRange<int>{ 1, 10 });

	// Creating particles.
	SharedPtr<Particle> pNewParticle = orbitalSystem.CreateParticle(1.f, { 0.75f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, pHostSpace);

	testHandler.SetOutputMode(outputMode);
}

} // namespace Orbital ------------------------------------------------------------------------------------------------------------

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
