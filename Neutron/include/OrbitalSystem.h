#ifndef NEUTRON_ORBITAL_SYSTEM_2_H
#define NEUTRON_ORBITAL_SYSTEM_2_H

#include "ITestScript.h"
#include "NebulaTypes.h"
#include "ParticleBase.h"
#include "ScalingSphereBase.h"
#include "Orbit.h"
#include "Types.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class OrbitalSystem
{
	friend class OrbitalSystemTestScript;
	friend class ScalingSphereListTestScript;

	class InfluencingSphere : public ScalingSphereBase
	{
	public:
		using ScalingSphereBase::ScalingSphereBase;

		virtual bool IsInfluencing() const override;
		virtual ParticleBase const* GetPrimary() const override;
		virtual RelVector3 const& GetPrimaryPosition() const override;
		virtual RelVector3 const& GetPrimaryVelocity() const override;
	};

	class NonInfluencingSpace : public ScalingSphereBase
	{
	public:
		NonInfluencingSpace(ParticleBase * pHostParticle, Unit::Absolute const& radius);

		virtual void Initialize() override;

		virtual bool IsInfluencing() const override;
		virtual ParticleBase const* GetPrimary() const override;
		virtual RelVector3 const& GetPrimaryPosition() const override;
		virtual RelVector3 const& GetPrimaryVelocity() const override;

	private:
		static ParticleBase * FindPrimary(ScalingSphereBase const* pScaledSpace);
		static void ComputePrimaryKinetics(ScalingSphereBase const* pScaledSpace, RelVector3 & position, RelVector3 & velocity);

		ParticleBase *	m_pPrimary;			// Pointer to the local primary.
		RelVector3		m_primaryPosition;	// Locally scaled position of the primary relative to this space.
		RelVector3		m_primaryVelocity;	// Locally scaled velocity of the primary relative to this space.
	};

	class HostParticle : public ParticleBase
	{
	public:
		HostParticle(const float mass, Unit::Absolute const& hostSphereAbsoluteRadius);
		virtual ~HostParticle() override = default;

		virtual void SetPosition(RelVector3 const& position) override;
		virtual void SetVelocity(RelVector3 const& velocity) override;
		virtual void Rescale(Unit::Relative const& rescaleFactor) override;
		virtual void Initialize() override;

		virtual bool IsInfluencing() const override;
		virtual ScalingSphereBase * GetSphereOfInfluence() const override;
		virtual RelVector3 const& GetPosition() const override;
		virtual RelVector3 const& GetVelocity() const override;
		virtual Orbit const* GetOrbit() const override;

	private:
		InfluencingSphere *	m_pHostSphere;
	};

	class PassiveParticle : public ParticleBase
	{
	public:
		PassiveParticle(ScalingSphereBase * pHostSpace, float mass, RelVector3 const& position, RelVector3 const& velocity);
		virtual ~PassiveParticle() override = default;

		virtual void SetPosition(RelVector3 const& position) override;
		virtual void SetVelocity(RelVector3 const& velocity) override;
		virtual void Rescale(Unit::Relative const& rescaleFactor) override;
		virtual void Initialize() override;

		virtual bool IsInfluencing() const override;
		virtual ScalingSphereBase * GetSphereOfInfluence() const override;
		virtual RelVector3 const& GetPosition() const override;
		virtual RelVector3 const& GetVelocity() const override;
		virtual Orbit const* GetOrbit() const override;

	protected:
		RelVector3			m_position;
		RelVector3			m_velocity;

		UniquePtr<Orbit>	m_pOrbit;
	};

	class InfluencingParticle : public PassiveParticle
	{
	public:
		using PassiveParticle::GetPosition;
		using PassiveParticle::GetVelocity;

		InfluencingParticle(ScalingSphereBase * pHostSpace, float mass, RelVector3 const& position, RelVector3 const& velocity);
		virtual ~InfluencingParticle() override = default;

		virtual void Initialize() override;

		virtual bool IsInfluencing() const override;
		virtual ScalingSphereBase * GetSphereOfInfluence() const override;
		virtual Orbit const* GetOrbit() const override;

	private:
		InfluencingSphere *	m_pSphereOfInfluence;
	};

public:
	OrbitalSystem(const float hostMass, Unit::Absolute const& hostSphereAbsoluteRadius);

	ParticleBase * GetHostParticle();
	ScalingSphereBase * GetHostSphere();

	/// <summary> Create a scaled space. </summary>
	/// <param name="hostParticle"> The particle to which the new space will be attached. </param>
	/// <param name="trueRadius"> The true radius (meters). </param>
	/// <returns> Reference to the created space. </returns>
	/// <exception cref="ApiException"> Invalid parameter. </exception>
	ScalingSphereBase * CreateScalingSphere(ParticleBase * pHostParticle, Unit::Absolute const& radius);
	ScalingSphereBase * CreateScalingSphere(ScalingSphereBase * pOuterSphere, Unit::Relative const& radius);

	/// <summary> Create a particle. </summary>
	/// <param name="hostSpace"> The scaled space in which the particle will be placed. </param>
	/// <param name="mass"> The particle mass (kg). </param>
	/// <param name="position"> The particle initial position. </param>
	/// <param name="velocity"> The particle initial velocity. </param>
	/// <param name="isInfluencing"> Whether the particle has a sphere of influence (an influencing scaled space). </param>
	/// <returns> Reference to the created particle. </returns>
	/// <exception cref="ApiException"> Invalid parameter. </exception>
	ParticleBase * CreateParticle(ScalingSphereBase * pHostSphere, float mass, RelVector3 const& position, RelVector3 const& velocity, const bool isInfluencing);

	/// <summary> Create a particle with circular orbit. </summary>
	/// <param name="hostSpace"> The scaled space in which the particle will be placed. </param>
	/// <param name="mass"> The particle mass (kg). </param>
	/// <param name="position"> The particle initial position. </param>
	/// <param name="velocity"> The particle initial velocity. </param>
	/// <param name="isInfluencing"> Whether the particle has a sphere of influence (an influencing scaled space). </param>
	/// <returns> Reference to the created particle. </returns>
	/// <exception cref="ApiException"> Invalid parameter. </exception>
	ParticleBase * CreateParticle(ScalingSphereBase * pHostSphere, float mass, RelVector3 const& position, const bool isInfluencing);

	/// <summary> Destroy a particle in this orbital system. </summary>
	/// <param name="pParticleBase"> Pointer to the particle to be destroyed. </param>
	void DestroyParticle(ParticleBase * pParticleBase);

	Result ResizeScalingSphere(ScalingSphereBase * pScalingSphereBase, Unit::Absolute const& radius);

	/// <summary> Remove the scaling sphere from its host Particle and destroy it. </summary>
	/// <param name="pScalingSphereBase"> Pointer to the ScalingSphere to destroy. </param>
	/// <param name="shouldDonateParticles"> Whether to donate the Sphere's Particles to its neighbouring Spheres. Absolute Particle positions/velocities are preserved. </param>
	void DestroyScalingSphere(ScalingSphereBase * pScalingSphereBase, const bool shouldDonateParticles);

private:
	ScalingSphereBase * CreateScalingSphere(ParticleBase * pHostParticle, Unit::Absolute const& radius, const bool isInfluencing);

	UniquePtr<HostParticle>	m_pHostParticle;	// Pointer to the interface of the host particle around which all other particles in the system orbit.
};

// --------------------------------------------------------------------------------------------------------------------------------

inline ParticleBase * OrbitalSystem::GetHostParticle()
{
	return static_cast<ParticleBase *>(m_pHostParticle.get());
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ScalingSphereBase * OrbitalSystem::GetHostSphere()
{
	return m_pHostParticle->GetSphereOfInfluence();
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

inline bool OrbitalSystem::InfluencingSphere::IsInfluencing() const
{
	return true;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ParticleBase const* OrbitalSystem::InfluencingSphere::GetPrimary() const
{
	return m_pHostParticle;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline RelVector3 const& OrbitalSystem::InfluencingSphere::GetPrimaryPosition() const
{
	return RelVector3::ZERO;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline RelVector3 const& OrbitalSystem::InfluencingSphere::GetPrimaryVelocity() const
{
	return RelVector3::ZERO;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

inline OrbitalSystem::NonInfluencingSpace::NonInfluencingSpace(ParticleBase * pHostParticle, Unit::Absolute const& radius) :
	ScalingSphereBase(pHostParticle, radius),
	m_pPrimary(FindPrimary(this))
{
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void OrbitalSystem::NonInfluencingSpace::Initialize()
{
	ScalingSphereBase::Initialize();

	ComputePrimaryKinetics(this, m_primaryPosition, m_primaryVelocity);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline bool OrbitalSystem::NonInfluencingSpace::IsInfluencing() const
{
	return false;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ParticleBase const* OrbitalSystem::NonInfluencingSpace::GetPrimary() const
{
	return m_pPrimary;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline RelVector3 const& OrbitalSystem::NonInfluencingSpace::GetPrimaryPosition() const
{
	return m_primaryPosition;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline RelVector3 const& OrbitalSystem::NonInfluencingSpace::GetPrimaryVelocity() const
{
	return m_primaryVelocity;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ParticleBase * OrbitalSystem::NonInfluencingSpace::FindPrimary(ScalingSphereBase const* pScaledSpace)
{
	ParticleBase * pHostParticle = pScaledSpace->GetHostParticle();

	while ((nullptr != pScaledSpace) && !pScaledSpace->IsInfluencing())
	{
		assert(nullptr != pHostParticle);

		pScaledSpace = pHostParticle->GetHostSphere();

		pHostParticle = pScaledSpace->GetHostParticle();
	}

	return pHostParticle;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void OrbitalSystem::NonInfluencingSpace::ComputePrimaryKinetics(ScalingSphereBase const* pScalingSphere, RelVector3 & position,
	RelVector3 & velocity)
{
	assert(nullptr != pScalingSphere);

	position = RelVector3::ZERO;
	velocity = RelVector3::ZERO;

	Unit::Relative relativeScaleFactor(1.f);

	while (!pScalingSphere->IsInfluencing())
	{
		ParticleBase const*const pHostParticle = pScalingSphere->GetHostParticle();
		ScalingSphereBase const*const pHostSphere = pHostParticle->GetHostSphere();

		relativeScaleFactor *= pHostSphere->GetAbsoluteRadius().ToRelative(*pScalingSphere);

		position -= pHostParticle->GetPosition() * relativeScaleFactor;
		velocity -= pHostParticle->GetVelocity() * relativeScaleFactor;

		pScalingSphere = pHostSphere;
	}
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

inline void OrbitalSystem::HostParticle::SetPosition(RelVector3 const& position)
{
	assert(false); // We shouldn't be here!
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void OrbitalSystem::HostParticle::SetVelocity(RelVector3 const& velocity)
{
	assert(false); // We shouldn't be here!
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void OrbitalSystem::HostParticle::Rescale(Unit::Relative const& rescaleFactor)
{
	// Nothing to do.
}

// --------------------------------------------------------------------------------------------------------------------------------

inline bool OrbitalSystem::HostParticle::IsInfluencing() const
{
	return true;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ScalingSphereBase * OrbitalSystem::HostParticle::GetSphereOfInfluence() const
{
	return static_cast<ScalingSphereBase *>(m_pHostSphere);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline RelVector3 const& OrbitalSystem::HostParticle::GetPosition() const
{
	return RelVector3::ZERO;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline RelVector3 const& OrbitalSystem::HostParticle::GetVelocity() const
{
	return RelVector3::ZERO;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Orbit const* OrbitalSystem::HostParticle::GetOrbit() const
{
	return nullptr;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

inline void OrbitalSystem::PassiveParticle::SetPosition(RelVector3 const& position)
{
	m_position = position;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void OrbitalSystem::PassiveParticle::SetVelocity(RelVector3 const& velocity)
{
	m_velocity = velocity;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void OrbitalSystem::PassiveParticle::Rescale(Unit::Relative const& rescaleFactor)
{
	m_position *= rescaleFactor;
	m_velocity *= rescaleFactor;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline bool OrbitalSystem::PassiveParticle::IsInfluencing() const
{
	return false;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ScalingSphereBase * OrbitalSystem::PassiveParticle::GetSphereOfInfluence() const
{
	return nullptr;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline RelVector3 const& OrbitalSystem::PassiveParticle::GetPosition() const
{
	return m_position;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline RelVector3 const& OrbitalSystem::PassiveParticle::GetVelocity() const
{
	return m_velocity;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Orbit const* OrbitalSystem::PassiveParticle::GetOrbit() const
{
	return m_pOrbit.get();
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

inline bool OrbitalSystem::InfluencingParticle::IsInfluencing() const
{
	return true;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ScalingSphereBase * OrbitalSystem::InfluencingParticle::GetSphereOfInfluence() const
{
	return static_cast<ScalingSphereBase *>(m_pSphereOfInfluence);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Orbit const* OrbitalSystem::InfluencingParticle::GetOrbit() const
{
	return m_pOrbit.get();
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class OrbitalSystemTestScript : public ITestScript
{
public:
	OrbitalSystemTestScript();
	virtual ~OrbitalSystemTestScript();

protected:
	virtual void RunImpl(TestHandler & testHandler) override;
};

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class ResizeScalingSpheresTestScript : public ITestScript
{
public:
	ResizeScalingSpheresTestScript();
	virtual ~ResizeScalingSpheresTestScript();

protected:
	virtual void RunImpl(TestHandler & testHandler) override;
};

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class ScalingSphereListTestScript : public Nebula::ITestScript
{
public:
	ScalingSphereListTestScript();
	virtual ~ScalingSphereListTestScript();

protected:
	virtual void RunImpl(Nebula::TestHandler & testHandler) override;
};

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class RelAbsVectorTestScript : public Nebula::ITestScript
{
public:
	RelAbsVectorTestScript();
	virtual ~RelAbsVectorTestScript();

protected:
	virtual void RunImpl(Nebula::TestHandler & testHandler) override;
};

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_ORBITAL_SYSTEM_2_H
