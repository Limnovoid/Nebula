#ifndef NEUTRON_PARTICLE_H
#define NEUTRON_PARTICLE_H

#include "Vector3.h"
#include "Uuid.h"
#include "ScalingSpace.h"
#include "Orbit.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;

/* Forward Declarations */
class ScalingSpace;

// --------------------------------------------------------------------------------------------------------------------------------

/// <summary>
/// The particle class implementing the orbital motion of a point mass in a scaled simulation space.
/// </summary>
class Particle
{
	friend class OrbitalSystem;
	friend class ParticleTestScript;

	struct ScalingSpaceListPredicate
	{
		bool operator()(UniquePtr<ScalingSpace> const& lhs, UniquePtr<ScalingSpace> const& rhs);
	};

public:
	class ScalingSpaceList : public SortedList<UniquePtr<ScalingSpace>, ScalingSpaceListPredicate>
	{
		using Base = SortedList<UniquePtr<ScalingSpace>, ScalingSpaceListPredicate>;

	public:
		using iterator = Base::iterator;
		using const_iterator = Base::const_iterator;
		using Base::SortedList;
		using Base::begin;
		using Base::end;
		using Base::cbegin;
		using Base::cend;
		using Base::size;
		using Base::empty;
		using Base::clear;

		iterator FindSpaceOfInfluence();
		const_iterator FindSpaceOfInfluence() const;

	};

	//using ScalingSpaceList = SortedList<UniquePtr<ScalingSpace>, ScalingSpaceListPredicate>;

	struct State
	{
		float		m_mass;					// Mass of the particle.
		Vector3		m_localPosition;		// Position relative/scaled to the host space.
		Vector3		m_localVelocity;		// Velocity relative/scaled to the host space.
	};

	Particle(State const& state, ScalingSpace * pHostSpace);
	Particle(float mass, float hostSpaceTrueRadius);

	void Set(Vector3 const& position, Vector3 const& velocity, ScalingSpace * pHostSpace);

	State const& GetState() const;
	ScalingSpace * GetHostSpace();
	ScalingSpace * GetSpaceOfInfluence();
	ScalingSpaceList const& GetScalingSpaceList() const;

	Uuid const							m_uuid;

private:
	State								m_state;						// Physical state of the particle.
	UniquePtr<Orbit::Elements>			m_pElements;					// Pointer to the orbit elements.

	ScalingSpace *						m_pHostSpace;					// Pointer to the scaling space in which this particle is moving, or the orbital system's host space if this particle is the system host particle.
	ScalingSpaceList					m_attachedSpaces;				// List of pointers to scaling spaces attached to this particle.
};

// --------------------------------------------------------------------------------------------------------------------------------

inline Particle::State const& Particle::GetState() const
{
	return m_state;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ScalingSpace * Particle::GetHostSpace()
{
	return m_pHostSpace;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ScalingSpace * Particle::GetSpaceOfInfluence()
{
	ScalingSpace * pSpaceOfInfluence = nullptr;

	Particle::ScalingSpaceList::iterator const spaceOfInfluenceIter = m_attachedSpaces.FindSpaceOfInfluence();

	if (m_attachedSpaces.end() != spaceOfInfluenceIter)
		pSpaceOfInfluence = (*spaceOfInfluenceIter).get();

	return pSpaceOfInfluence;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Particle::ScalingSpaceList const& Particle::GetScalingSpaceList() const
{
	return m_attachedSpaces;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

Particle::ScalingSpaceList::iterator Particle::ScalingSpaceList::FindSpaceOfInfluence()
{
	iterator spaceOfInfluenceIter = begin();

	while ((end() != spaceOfInfluenceIter) && !(*spaceOfInfluenceIter)->IsInfluencing())
		++spaceOfInfluenceIter;

	return spaceOfInfluenceIter;
}

// --------------------------------------------------------------------------------------------------------------------------------

Particle::ScalingSpaceList::const_iterator Particle::ScalingSpaceList::FindSpaceOfInfluence() const
{
	const_iterator spaceOfInfluenceIter = cbegin();

	while ((cend() != spaceOfInfluenceIter) && !(*spaceOfInfluenceIter)->IsInfluencing())
		++spaceOfInfluenceIter;

	return spaceOfInfluenceIter;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class ParticleTestScript : public ITestScript
{
public:
	ParticleTestScript();
	virtual ~ParticleTestScript();

protected:
	virtual void RunImpl(TestHandler & testHandler) override;
};

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_PARTICLE_H
