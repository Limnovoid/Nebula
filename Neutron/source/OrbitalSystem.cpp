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
	m_pHostSpace(MakeShared<ScalingSpace>(hostMass, hostSpaceTrueRadius))
{
	m_scalingSpaces.push_back(m_pHostSpace);
}

// --------------------------------------------------------------------------------------------------------------------------------

SharedPtr<ScalingSpace> System::CreateScalingSpace(float trueRadius)
{
	float radius = trueRadius / m_pHostSpace->GetTrueRadius();

	SharedPtr<ScalingSpace> pNewScalingSpace = MakeShared<ScalingSpace>(m_hostMass, trueRadius, radius);

	return nullptr;
}

// --------------------------------------------------------------------------------------------------------------------------------

SharedPtr<Particle> System::CreateParticle(float mass, Vector3 position, Vector3 velocity, SharedPtr<ScalingSpace> pHostSpace)
{
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

	System orbitalSystem(hostMass, hostSpaceTrueRadius);

	SharedPtr<ScalingSpace> pHostSpace = orbitalSystem.GetHostSpace();

	testHandler.Assert(pHostSpace->GetRadius(), 1.f, "Host space radius = 1");
	testHandler.Assert(pHostSpace->GetTrueRadius(), hostSpaceTrueRadius, "Host space true radius = 1");
}

} // namespace Orbital ------------------------------------------------------------------------------------------------------------

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
