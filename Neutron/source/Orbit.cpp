#include "Orbit.h"

#include "ScalingSpace.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

using namespace Orbital;

// --------------------------------------------------------------------------------------------------------------------------------

Result Orbit::Elements::Compute(ScalingSpace const& scalingSpace, Vector3 const& localPosition, Vector3 const& localVelocity)
{
	Vector3 positionFromPrimary = localPosition - scalingSpace.GetPrimaryPosition();
	Vector3 velocityFromPrimary = localVelocity - scalingSpace.GetPrimaryVelocity();

	Vector3 angularMomentumVector = positionFromPrimary.PreciseCross(velocityFromPrimary);

	return RESULT_CODE_NOT_IMPLEMENTED;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

OrbitTestScript::OrbitTestScript() :
	ITestScript("Orbit")
{
}

// --------------------------------------------------------------------------------------------------------------------------------

OrbitTestScript::~OrbitTestScript()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void OrbitTestScript::RunImpl(TestHandler & testHandler)
{
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
