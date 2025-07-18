#ifndef NEUTRON_ORBITAL_SYSTEM_2_H
#define NEUTRON_ORBITAL_SYSTEM_2_H

#include "ITestScript.h"
#include "NebulaTypes.h"
#include "ParticleBase.h"
#include "ScaledSpaceBase.h"
#include "Vector3.h"
#include "Orbit.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class OrbitalSystem2
{
	friend class ParticleBase;
	friend class OrbitalSystem2TestScript;

	class HostParticle : public ParticleBase
	{
	public:
		HostParticle(OrbitalSystem2 & orbitalSystem, float hostMass);
		virtual ~HostParticle() override = default;

		virtual Vector3 const& GetPosition() const override;
		virtual Vector3 const& GetVelocity() const override;
		virtual bool IsInfluencing() const override;
		virtual ScaledSpaceBase * GetSpaceOfInfluence() const override;
		virtual Orbit const* GetOrbit() const override;
	};

	class InfluencingSpace : public ScaledSpaceBase
	{
	public:
		using ScaledSpaceBase::ScaledSpaceBase;

		virtual bool IsInfluencing() const override;
		virtual ParticleBase const* GetPrimary() const override;
		virtual Vector3 const& GetPrimaryPosition() const override;
		virtual Vector3 const& GetPrimaryVelocity() const override;
	};

	class NonInfluencingSpace : public ScaledSpaceBase
	{
	public:
		NonInfluencingSpace(ParticleBase * pHostParticle, float trueRadius);

		virtual void Initialize(float radius) override;

		virtual bool IsInfluencing() const override;
		virtual ParticleBase const* GetPrimary() const override;
		virtual Vector3 const& GetPrimaryPosition() const override;
		virtual Vector3 const& GetPrimaryVelocity() const override;

	private:
		static ParticleBase * FindPrimary(ScaledSpaceBase const* pScaledSpace);
		static void ComputePrimaryKinetics(ScaledSpaceBase const* pScaledSpace, Vector3 & position, Vector3 & velocity);

		ParticleBase *	m_pPrimary;			// Pointer to the local primary.
		Vector3			m_primaryPosition;	// Locally scaled position of the primary relative to this space.
		Vector3			m_primaryVelocity;	// Locally scaled velocity of the primary relative to this space.
	};

	class Particle : public ParticleBase
	{
	public:
		Particle(OrbitalSystem2 & orbitalSystem, ScaledSpaceBase * pHostSpace, float mass, Vector3 position, Vector3 velocity);
		virtual ~Particle() override = default;

		virtual Vector3 const& GetPosition() const override;
		virtual Vector3 const& GetVelocity() const override;
		virtual bool IsInfluencing() const override;
		virtual ScaledSpaceBase * GetSpaceOfInfluence() const override;
		virtual Orbit const* GetOrbit() const override;

	protected:
		Vector3				m_position;
		Vector3				m_velocity;

		UniquePtr<Orbit>	m_pOrbit;
	};

	class InfluencingParticle : public Particle
	{
	public:
		using Particle::GetPosition;
		using Particle::GetVelocity;

		InfluencingParticle(OrbitalSystem2 & orbitalSystem, ScaledSpaceBase * pHostSpace, float mass, Vector3 position, Vector3 velocity);
		virtual ~InfluencingParticle() override = default;

		virtual bool IsInfluencing() const override;
		virtual ScaledSpaceBase * GetSpaceOfInfluence() const override;
		virtual Orbit const* GetOrbit() const override;

	private:
		InfluencingSpace *	m_pSpaceOfInfluence;
	};

public:
	OrbitalSystem2(float hostMass, float hostSpaceTrueRadius);

	ParticleBase * GetHostParticle();
	ScaledSpaceBase * GetHostSpace();

	/// <summary> Create a scaled space. </summary>
	/// <param name="hostParticle"> The particle to which the new space will be attached. </param>
	/// <param name="trueRadius"> The true radius (meters). </param>
	/// <returns> Reference to the created space. </returns>
	/// <exception cref="ApiException"> Invalid parameter. </exception>
	ScaledSpaceBase * CreateScaledSpace(ParticleBase & hostParticle, float trueRadius);

	/// <summary> Create a particle. </summary>
	/// <param name="hostSpace"> The scaled space in which the particle will be placed. </param>
	/// <param name="mass"> The particle mass (kg). </param>
	/// <param name="position"> The particle initial position. </param>
	/// <param name="velocity"> The particle initial velocity. </param>
	/// <param name="isInfluencing"> Whether the particle has a sphere of influence (an influencing scaled space). </param>
	/// <returns> Reference to the created particle. </returns>
	/// <exception cref="ApiException"> Invalid parameter. </exception>
	ParticleBase * CreateParticle(ScaledSpaceBase & hostSpace, float mass, Vector3 position, Vector3 velocity, bool isInfluencing);

	/// <summary> Create a particle with circular orbit. </summary>
	/// <param name="hostSpace"> The scaled space in which the particle will be placed. </param>
	/// <param name="mass"> The particle mass (kg). </param>
	/// <param name="position"> The particle initial position. </param>
	/// <param name="velocity"> The particle initial velocity. </param>
	/// <param name="isInfluencing"> Whether the particle has a sphere of influence (an influencing scaled space). </param>
	/// <returns> Reference to the created particle. </returns>
	/// <exception cref="ApiException"> Invalid parameter. </exception>
	ParticleBase * CreateParticle(ScaledSpaceBase & hostSpace, float mass, Vector3 position, bool isInfluencing);

	/// <summary> Destroy a particle in this orbital system. </summary>
	/// <param name="pParticleBase"> Pointer to the particle to be destroyed. </param>
	void DestroyParticle(ParticleBase * pParticleBase);

private:
	ScaledSpaceBase * CreateScaledSpaceImpl(ParticleBase * pHostParticle, float trueRadius, bool isInfluencing);

	UniquePtr<HostParticle>	m_pHostParticle;	// Pointer to the interface of the host particle around which all other particles in the system orbit.
};

// --------------------------------------------------------------------------------------------------------------------------------

inline ParticleBase * OrbitalSystem2::GetHostParticle()
{
	return static_cast<ParticleBase *>(m_pHostParticle.get());
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ScaledSpaceBase * OrbitalSystem2::GetHostSpace()
{
	return m_pHostParticle->GetSpaceOfInfluence();
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

inline Vector3 const& OrbitalSystem2::HostParticle::GetPosition() const
{
	return Vector3::Zero();
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Vector3 const& OrbitalSystem2::HostParticle::GetVelocity() const
{
	return Vector3::Zero();
}

// --------------------------------------------------------------------------------------------------------------------------------

inline bool OrbitalSystem2::HostParticle::IsInfluencing() const
{
	return true;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ScaledSpaceBase * OrbitalSystem2::HostParticle::GetSpaceOfInfluence() const
{
	return m_attachedSpaces.begin()->get();
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Orbit const* OrbitalSystem2::HostParticle::GetOrbit() const
{
	return nullptr;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

inline bool OrbitalSystem2::InfluencingSpace::IsInfluencing() const
{
	return true;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ParticleBase const* OrbitalSystem2::InfluencingSpace::GetPrimary() const
{
	return m_pHostParticle;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Vector3 const& OrbitalSystem2::InfluencingSpace::GetPrimaryPosition() const
{
	return Vector3::Zero();
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Vector3 const& OrbitalSystem2::InfluencingSpace::GetPrimaryVelocity() const
{
	return Vector3::Zero();
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

inline OrbitalSystem2::NonInfluencingSpace::NonInfluencingSpace(ParticleBase * pHostParticle, float trueRadius) :
	ScaledSpaceBase(pHostParticle, trueRadius),
	m_pPrimary(FindPrimary(this))
{
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void OrbitalSystem2::NonInfluencingSpace::Initialize(float radius)
{
	ScaledSpaceBase::Initialize(radius);

	ComputePrimaryKinetics(this, m_primaryPosition, m_primaryVelocity);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline bool OrbitalSystem2::NonInfluencingSpace::IsInfluencing() const
{
	return false;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ParticleBase const* OrbitalSystem2::NonInfluencingSpace::GetPrimary() const
{
	return m_pPrimary;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Vector3 const& OrbitalSystem2::NonInfluencingSpace::GetPrimaryPosition() const
{
	return m_primaryPosition;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Vector3 const& OrbitalSystem2::NonInfluencingSpace::GetPrimaryVelocity() const
{
	return m_primaryVelocity;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ParticleBase * OrbitalSystem2::NonInfluencingSpace::FindPrimary(ScaledSpaceBase const* pScaledSpace)
{
	ParticleBase * pHostParticle = pScaledSpace->GetHostParticle();

	while ((nullptr != pScaledSpace) && !pScaledSpace->IsInfluencing());
	{
		assert(nullptr != pHostParticle);

		pScaledSpace = pHostParticle->GetHostSpace();

		pHostParticle = pScaledSpace->GetHostParticle();
	}

	return pHostParticle;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void OrbitalSystem2::NonInfluencingSpace::ComputePrimaryKinetics(ScaledSpaceBase const* pScaledSpace, Vector3 & position,
	Vector3 & velocity)
{
	assert(nullptr != pScaledSpace);

	position = 0.f;
	velocity = 0.f;

	float scaleFactor = 1.f;
	ParticleBase const* pHostParticle = pScaledSpace->GetHostParticle();

	while (!pScaledSpace->IsInfluencing())
	{
		ParticleBase const* pHostParticle = pScaledSpace->m_pHostParticle;
		ScaledSpaceBase const* pHostSpace = pHostParticle->m_pHostSpace;

		scaleFactor *= (pHostSpace->m_trueRadius / pScaledSpace->m_trueRadius);

		position -= pHostParticle->GetPosition() * scaleFactor;
		velocity -= pHostParticle->GetVelocity() * scaleFactor;

		pScaledSpace = pHostSpace;
	}
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

inline Vector3 const& OrbitalSystem2::Particle::GetPosition() const
{
	return m_position;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Vector3 const& OrbitalSystem2::Particle::GetVelocity() const
{
	return m_velocity;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline bool OrbitalSystem2::Particle::IsInfluencing() const
{
	return false;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ScaledSpaceBase * OrbitalSystem2::Particle::GetSpaceOfInfluence() const
{
	return nullptr;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Orbit const* OrbitalSystem2::Particle::GetOrbit() const
{
	return m_pOrbit.get();
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

inline bool OrbitalSystem2::InfluencingParticle::IsInfluencing() const
{
	return true;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ScaledSpaceBase * OrbitalSystem2::InfluencingParticle::GetSpaceOfInfluence() const
{
	return m_pSpaceOfInfluence;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Orbit const* OrbitalSystem2::InfluencingParticle::GetOrbit() const
{
	return m_pOrbit.get();
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class OrbitalSystem2TestScript : public ITestScript
{
public:
	OrbitalSystem2TestScript();
	virtual ~OrbitalSystem2TestScript();

protected:
	virtual void RunImpl(TestHandler & testHandler) override;
};

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_ORBITAL_SYSTEM_2_H
