#include "ScalingSpace.h"

#include "Particle.h"
#include "TestHandler.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

namespace Orbital // --------------------------------------------------------------------------------------------------------------
{

ScalingSpace::ScalingSpace(float hostMass, float trueRadius, float radius) :
	m_trueRadius(trueRadius),
	m_radius(radius),
	m_gravityParameter(ComputeScaledGravityParameter(trueRadius, hostMass)),
	m_isInfluencing(true)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

ScalingSpace::ScalingSpace(float trueRadius, bool isInfluencing, SharedPtr<Particle> pHost) :
	m_trueRadius(trueRadius),
	m_gravityParameter(ComputeScaledGravityParameter(trueRadius, pHost->GetState().m_mass)),
	m_isInfluencing(isInfluencing),
	m_pHost(pHost)
{
	assert(false); // TODO - compute radius
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSpace::ComputePrimaryKinetics()
{
	m_primaryPosition = 0.f;
	m_primaryVelocity = 0.f;

	ScalingSpace const* pScalingSpace = this;
	float scaling = 1.f;

	while ((nullptr != pScalingSpace) && !pScalingSpace->m_isInfluencing)
	{
		scaling /= pScalingSpace->m_radius;

		SharedPtr<Particle> pHost = pScalingSpace->m_pHost;

		m_primaryPosition -= pHost->GetState().m_localPosition * scaling;
		m_primaryVelocity -= pHost->GetState().m_localVelocity * scaling;

		pScalingSpace = pHost->GetHostSpace().get();
	}
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

ScalingSpaceTestScript::ScalingSpaceTestScript() :
	ITestScript("ScalingSpace")
{
}

// --------------------------------------------------------------------------------------------------------------------------------

ScalingSpaceTestScript::~ScalingSpaceTestScript()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSpaceTestScript::RunImpl(TestHandler & testHandler)
{
	ScalingSpace::List list;

	testHandler.Assert<int64_t, int64_t>([&](int64_t index) -> int64_t
	{
		float const radius = static_cast<float>(index) / 10.f;

		SharedPtr<ScalingSpace> pScalingSpace = MakeShared<ScalingSpace>(1.f, radius, radius);

		list.Insert(pScalingSpace);

		return list.size();

	}, TestHandler::FRangeRandomOrder({ 1, 10 }), TestHandler::FRangeIndex(), "List insertion", { 1, 10 });

	int radius = 10;
	for (ScalingSpace::List::const_iterator iter = list.cbegin(); list.cend() != iter; ++iter)
		testHandler.Assert((*iter)->GetRadius(), static_cast<float>(radius--) / 10.f, "List ordering");
}

} // namespace Orbital ------------------------------------------------------------------------------------------------------------

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
