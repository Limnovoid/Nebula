#include "PhysicsEngine.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;

// --------------------------------------------------------------------------------------------------------------------------------

PhysicsEngine::PhysicsEngine()
{
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

Result PhysicsEngine::Orbit::Parameters::Compute()
{
	assert(false);
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

}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
