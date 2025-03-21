#include "OrbitalSystem.h"

#include "Particle.h"
#include "ScalingSpace.h"
#include "TestHandler.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

OrbitalSystem::OrbitalSystem(float hostMass, float hostSpaceTrueRadius) :
	m_hostParticle(hostMass, hostSpaceTrueRadius)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

ScalingSpace & OrbitalSystem::CreateScalingSpace(float trueRadius)
{
	float const radius = trueRadius / m_hostParticle.m_pHostSpace->GetTrueRadius();

	Particle::ScalingSpaceList::iterator scalingSpaceListIter =
		m_hostParticle.m_attachedSpaces.Emplace(std::move(MakeUnique<ScalingSpace>(&m_hostParticle, radius, trueRadius, true)));

	if (m_hostParticle.m_attachedSpaces.begin() != scalingSpaceListIter)
	{
		Particle::ScalingSpaceList::iterator outerScalingSpaceListIter = scalingSpaceListIter;
		--outerScalingSpaceListIter;

		(*scalingSpaceListIter)->m_pOuterSpace = outerScalingSpaceListIter->get();
		(*outerScalingSpaceListIter)->m_pInnerSpace = scalingSpaceListIter->get();
	}

	Particle::ScalingSpaceList::iterator innerScalingSpaceListIter = scalingSpaceListIter;
	++innerScalingSpaceListIter;
	if (m_hostParticle.m_attachedSpaces.end() != innerScalingSpaceListIter)
	{
		(*scalingSpaceListIter)->m_pInnerSpace = innerScalingSpaceListIter->get();
		(*innerScalingSpaceListIter)->m_pOuterSpace = scalingSpaceListIter->get();
	}

	return **scalingSpaceListIter;
}

// --------------------------------------------------------------------------------------------------------------------------------

Particle & OrbitalSystem::CreateParticle(float mass, Vector3 position, Vector3 velocity, ScalingSpace & hostSpace)
{
	float const positionMagnitude = sqrtf(position.SqareMagnitude());

	if (hostSpace.m_pInnerSpace != nullptr)
	{
		API_ASSERT_THROW(hostSpace.m_pInnerSpace->m_radius < positionMagnitude,
			RESULT_CODE_INVALID_PARAMETER, Fmt::Format("Position ({}) overlaps inner scaling space (radius {})",
				position, hostSpace.m_pInnerSpace->m_radius));
	}

	API_ASSERT_THROW(positionMagnitude < hostSpace.m_radius, RESULT_CODE_INVALID_PARAMETER,
		Fmt::Format("Position ({}) is outside scaling space (radius {})", position, hostSpace.m_radius));

	UniquePtr<Particle> & pNewParticle =
		hostSpace.m_particles.emplace_back(
			std::move(MakeUnique<Particle>(Particle::State{ mass, position, velocity }, &hostSpace)));

	return *pNewParticle;
}

// --------------------------------------------------------------------------------------------------------------------------------

ScalingSpace & OrbitalSystem::CreateScalingSpace(float trueRadius, Particle & hostParticle)
{
	float const radius = trueRadius / hostParticle.m_pHostSpace->GetTrueRadius();

	Particle::ScalingSpaceList::iterator scalingSpaceListIter =
		hostParticle.m_attachedSpaces.Emplace(std::move(MakeUnique<ScalingSpace>(&hostParticle, radius, trueRadius, false)));

	ScalingSpace & newScalingSpace = **scalingSpaceListIter;

	if (hostParticle.m_attachedSpaces.begin() != scalingSpaceListIter)
	{
		Particle::ScalingSpaceList::iterator outerScalingSpaceListIter = scalingSpaceListIter;
		--outerScalingSpaceListIter;

		if ((*outerScalingSpaceListIter)->IsInfluencing())
			newScalingSpace.m_isInfluencing = true;

		newScalingSpace.m_pOuterSpace = outerScalingSpaceListIter->get();
		(*outerScalingSpaceListIter)->m_pInnerSpace = scalingSpaceListIter->get();
	}

	Particle::ScalingSpaceList::iterator innerScalingSpaceListIter = scalingSpaceListIter;
	++innerScalingSpaceListIter;
	if (m_hostParticle.m_attachedSpaces.end() != innerScalingSpaceListIter)
	{
		(*scalingSpaceListIter)->m_pInnerSpace = innerScalingSpaceListIter->get();
		(*innerScalingSpaceListIter)->m_pOuterSpace = scalingSpaceListIter->get();
	}

	return newScalingSpace;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

OrbitalSystemTestScript::OrbitalSystemTestScript() :
	ITestScript("OrbitalSystem")
{
}

// --------------------------------------------------------------------------------------------------------------------------------

OrbitalSystemTestScript::~OrbitalSystemTestScript()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void OrbitalSystemTestScript::RunImpl(TestHandler & testHandler)
{
	static constexpr float hostMass = 1e30f, hostSpaceTrueRadius = 8e12f;
	static float const hostGravityParameter = ScalingSpace::ComputeScaledGravityParameter(hostSpaceTrueRadius, hostMass);

	TestHandler::OutputMode const outputMode = testHandler.SetOutputMode(TestHandler::VERBOSE);

	OrbitalSystem orbitalSystem(hostMass, hostSpaceTrueRadius);

	Particle::ScalingSpaceList const& hostScalingSpaces = orbitalSystem.GetScalingSpaces();

	testHandler.Assert(hostScalingSpaces.size(), 1ull, "Host scaling space exists");

	ScalingSpace & hostSpace = orbitalSystem.GetHostSpace();

	testHandler.Assert(hostSpace.m_uuid, (*hostScalingSpaces.begin())->m_uuid, "Host space UUID");

	// Host space.
	testHandler.Assert(hostSpace.GetRadius(), 1.f, "Host space radius");
	testHandler.Assert(hostSpace.GetTrueRadius(), hostSpaceTrueRadius, "Host space true radius");
	testHandler.Assert(hostSpace.GetGravityParameter(), hostGravityParameter, "Host space gravity parameter");

	// Adding host spaces.
	testHandler.Assert<int, int>([&](int index)
	{
		float const trueRadius = hostSpaceTrueRadius / powf(2.f, static_cast<float>(index));
		float const radius = 1.f / powf(2.f, static_cast<float>(index));

		ScalingSpace & newHostSpace = orbitalSystem.CreateScalingSpace(trueRadius);

		testHandler.Assert(newHostSpace.GetTrueRadius(), trueRadius, "True radius");
		testHandler.Assert(newHostSpace.GetRadius(), radius, "Radius");

		auto newSpaceIter = --hostScalingSpaces.cend();
		testHandler.Assert((*newSpaceIter)->m_uuid, newHostSpace.m_uuid, "List position");

		auto prevSpaceIter = newSpaceIter; --prevSpaceIter;
		testHandler.Assert(reinterpret_cast<uintptr_t>(newHostSpace.m_pInnerSpace), reinterpret_cast<uintptr_t>(nullptr), "Inner space pointer");
		testHandler.Assert(reinterpret_cast<uintptr_t>(newHostSpace.m_pOuterSpace), reinterpret_cast<uintptr_t>(prevSpaceIter->get()), "Outer space pointer");
		testHandler.Assert(reinterpret_cast<uintptr_t>((*prevSpaceIter)->m_pInnerSpace), reinterpret_cast<uintptr_t>(newSpaceIter->get()), "Outer space's inner space pointer");

		return static_cast<int>(hostScalingSpaces.size());

	}, TestHandler::FRangeIndex<int>(), TestHandler::FRangeIndex<int>(1), "Adding host spaces", TestHandler::IndexRange<int>{ 1, 10 });

	// Creating particles.
	Particle & newParticle = orbitalSystem.CreateParticle(1.f, { 0.75f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, hostSpace);

	testHandler.SetOutputMode(outputMode);
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
