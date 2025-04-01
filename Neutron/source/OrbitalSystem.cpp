#include "OrbitalSystem.h"

#include "Particle.h"
#include "ScalingSpace.h"
#include "TestHandler.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

OrbitalSystem::OrbitalSystem(float hostMass, float hostSpaceTrueRadius) :
	m_pHostParticle(MakeUnique<Particle>(hostMass, hostSpaceTrueRadius))
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void OrbitalSystem::Reset(float hostMass, float hostSpaceTrueRadius)
{
	m_pHostParticle = MakeUnique<Particle>(hostMass, hostSpaceTrueRadius);
}

// --------------------------------------------------------------------------------------------------------------------------------

void OrbitalSystem::OnUpdate(Time::Microseconds dT)
{
	// TODO - update particles ...
}

// --------------------------------------------------------------------------------------------------------------------------------

ScalingSpace & OrbitalSystem::CreateScalingSpace(float trueRadius)
{
	Particle::ScalingSpaceList::iterator scalingSpaceListIter = EmplaceScalingSpace(trueRadius, *m_pHostParticle);

	ScalingSpace & scalingSpace = **scalingSpaceListIter;

	float const radius = (nullptr == scalingSpace.m_pOuterSpace) ? 1.f : trueRadius / scalingSpace.m_pOuterSpace->m_trueRadius;

	InitializeScalingSpace(scalingSpace);

	return scalingSpace;
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

	float const radiusOfInfluence = ComputeRadiusOfInfluence(pNewParticle->m_pElements->m_semiMajor, pNewParticle->m_state.m_mass,
		hostSpace.GetPrimary().m_state.m_mass);

	if (kMinimumRadiusOfInfluence < radiusOfInfluence)
	{
		float const trueRadiusOfInfluence = radiusOfInfluence * hostSpace.GetTrueRadius();

		ScalingSpace & spaceOfInfluence =
			**(pNewParticle->m_attachedSpaces.Emplace(std::move(MakeUnique<ScalingSpace>(pNewParticle.get(), trueRadiusOfInfluence))));

		spaceOfInfluence.Initialize(radiusOfInfluence, true);
	}

	return *pNewParticle;
}

// --------------------------------------------------------------------------------------------------------------------------------

ScalingSpace & OrbitalSystem::CreateScalingSpace(float trueRadius, Particle & hostParticle)
{
	Particle::ScalingSpaceList::iterator scalingSpaceListIter = EmplaceScalingSpace(trueRadius, hostParticle);

	ScalingSpace & scalingSpace = **scalingSpaceListIter;

	InitializeScalingSpace(scalingSpace);

	return scalingSpace;
}

// --------------------------------------------------------------------------------------------------------------------------------

Particle::ScalingSpaceList::iterator OrbitalSystem::EmplaceScalingSpace(float trueRadius, Particle & hostParticle)
{
	Particle::ScalingSpaceList::iterator scalingSpaceListIter =
		hostParticle.m_attachedSpaces.Emplace(std::move(MakeUnique<ScalingSpace>(&hostParticle, trueRadius)));

	if (hostParticle.m_attachedSpaces.begin() != scalingSpaceListIter)
	{
		Particle::ScalingSpaceList::iterator outerScalingSpaceListIter = scalingSpaceListIter;
		--outerScalingSpaceListIter;

		(*scalingSpaceListIter)->m_pOuterSpace = outerScalingSpaceListIter->get();
		(*outerScalingSpaceListIter)->m_pInnerSpace = scalingSpaceListIter->get();
	}
	else
	{
		(*scalingSpaceListIter)->m_pOuterSpace = hostParticle.m_pHostSpace; // Note: equals nullptr if host particle is the system host.
	}

	Particle::ScalingSpaceList::iterator innerScalingSpaceListIter = scalingSpaceListIter;
	++innerScalingSpaceListIter;
	if (hostParticle.m_attachedSpaces.end() != innerScalingSpaceListIter)
	{
		(*scalingSpaceListIter)->m_pInnerSpace = innerScalingSpaceListIter->get();
		(*innerScalingSpaceListIter)->m_pOuterSpace = scalingSpaceListIter->get();
	}

	return scalingSpaceListIter;
}

// --------------------------------------------------------------------------------------------------------------------------------

void OrbitalSystem::InitializeScalingSpace(ScalingSpace & scalingSpace)
{
	ScalingSpace const*const pSpaceOfInfluence = scalingSpace.m_pHost->GetSpaceOfInfluence();

	bool const isInfluencing = (nullptr != pSpaceOfInfluence) &&
		((pSpaceOfInfluence->m_uuid == scalingSpace.m_uuid) || (scalingSpace.m_trueRadius < pSpaceOfInfluence->m_trueRadius));

	if (nullptr == scalingSpace.m_pOuterSpace)
	{
		scalingSpace.Initialize(1.f, true);
	}
	else
	{
		float const radius = scalingSpace.m_trueRadius / scalingSpace.m_pOuterSpace->m_trueRadius;
		float const rescaleFactor = scalingSpace.m_radius / radius;

		scalingSpace.Initialize(radius, isInfluencing);

		for (UniquePtr<Particle> & pParticle : scalingSpace.m_particles)
		{
			Vector3 rescaledPosition = pParticle->m_state.m_localPosition * rescaleFactor;
			Vector3 rescaledVelocity = pParticle->m_state.m_localVelocity * rescaleFactor;

			if (ShouldAscend(sqrtf(rescaledPosition.SqareMagnitude())))
			{
				rescaledPosition *= radius;
				rescaledVelocity *= radius;

				InitializeParticle(*pParticle, rescaledPosition, rescaledVelocity, *scalingSpace.m_pOuterSpace);
			}
			else
			{
				InitializeParticle(*pParticle, rescaledPosition, rescaledVelocity, scalingSpace);
			}
		}

		for (UniquePtr<Particle> & pParticle : scalingSpace.m_pOuterSpace->m_particles)
		{
			float const radialDistance = sqrtf(pParticle->m_state.m_localPosition.SqareMagnitude());

			if (ShouldDescend(radialDistance, scalingSpace))
			{
				Vector3 const rescaledPosition = pParticle->m_state.m_localPosition / radius;
				Vector3 const rescaledVelocity = pParticle->m_state.m_localVelocity / radius;

				InitializeParticle(*pParticle, rescaledPosition, rescaledVelocity, scalingSpace);
			}
		}
	}

	if (nullptr != scalingSpace.m_pInnerSpace)
	{
		if (isInfluencing && !scalingSpace.m_pInnerSpace->m_isInfluencing)
			InitializeScalingSpace(*scalingSpace.m_pInnerSpace);
		else
			RecomputeRadius(*scalingSpace.m_pInnerSpace);
	}
}

// --------------------------------------------------------------------------------------------------------------------------------

void OrbitalSystem::RecomputeRadius(ScalingSpace & scalingSpace)
{
	float const newRadius = (nullptr == scalingSpace.m_pOuterSpace) ? 1.f :
		scalingSpace.m_trueRadius / scalingSpace.m_pOuterSpace->m_trueRadius;

	scalingSpace.Initialize(newRadius, scalingSpace.m_isInfluencing);

	if (!scalingSpace.m_isInfluencing)
	{
		for (UniquePtr<Particle> & pParticle : scalingSpace.m_particles)
			InitializeParticle(*pParticle, pParticle->m_state.m_localPosition, pParticle->m_state.m_localVelocity, scalingSpace);
	}
}

// --------------------------------------------------------------------------------------------------------------------------------

void OrbitalSystem::InitializeParticle(Particle & particle, Vector3 const& position, Vector3 const& velocity,
	ScalingSpace & scalingSpace)
{
	particle.Set(position, velocity, &scalingSpace);

	float const radiusOfInfluence =
		ComputeRadiusOfInfluence(particle.m_pElements->m_semiMajor, particle.m_state.m_mass, scalingSpace.GetPrimary().m_state.m_mass);

	float const trueRadiusOfInfluence = radiusOfInfluence * scalingSpace.GetTrueRadius();

	Particle::ScalingSpaceList::iterator spaceOfInfluenceIter = particle.m_attachedSpaces.FindSpaceOfInfluence();
	if (particle.m_attachedSpaces.end() == spaceOfInfluenceIter)
	{
		if (kMinimumRadiusOfInfluence < radiusOfInfluence)
		{
			spaceOfInfluenceIter = EmplaceScalingSpace(trueRadiusOfInfluence, particle);

			ScalingSpace & spaceOfInfluence = **spaceOfInfluenceIter;

			spaceOfInfluence.m_isInfluencing = true;

			InitializeScalingSpace(spaceOfInfluence);
		}
	}
	else
	{
		ScalingSpace & spaceOfInfluence = **spaceOfInfluenceIter;

		if (kMinimumRadiusOfInfluence < radiusOfInfluence)
		{
			spaceOfInfluence.m_trueRadius = trueRadiusOfInfluence;

			particle.m_attachedSpaces.Sort(spaceOfInfluenceIter);

			InitializeScalingSpace(spaceOfInfluence);
		}
		else
		{
			assert(false); // TODO - set influence and all smaller spaces to non-influencing ...
		}
	}

	assert(false); // TODO ...
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
	Particle & newParticle = orbitalSystem.CreateParticle(1.f, { 0.75f, 0.f, 0.f },
		{ 0.f, hostSpace.CircularOrbitSpeed(0.75f), 0.f }, hostSpace);

	// Adding scaling space between existing spaces.

	testHandler.SetOutputMode(outputMode);
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
