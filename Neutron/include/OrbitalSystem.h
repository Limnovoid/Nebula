#ifndef NEUTRON_ORBITAL_SYSTEM_2_H
#define NEUTRON_ORBITAL_SYSTEM_2_H

#include "ITestScript.h"
#include "NebulaTypes.h"
#include "ParticleBase.h"
#include "ScalingSphereBase.h"
#include "Vector3.h"
#include "Orbit.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class OrbitalSystem
{
	friend class OrbitalSystemTestScript;
	friend class ScalingSphereListTestScript;

	class HostParticle : public ParticleBase
	{
	public:
		HostParticle(float hostMass);
		virtual ~HostParticle() override = default;

		virtual void SetPosition(Vector3 const& position) override;
		virtual void SetVelocity(Vector3 const& velocity) override;
		virtual void Rescale(const float rescaleFactor) override;
		virtual void Initialize() override;

		virtual bool IsInfluencing() const override;
		virtual ScalingSphereBase * GetSphereOfInfluence() const override;
		virtual Vector3 const& GetPosition() const override;
		virtual Vector3 const& GetVelocity() const override;
		virtual Orbit const* GetOrbit() const override;
	};

	class InfluencingSpace : public ScalingSphereBase
	{
	public:
		using ScalingSphereBase::ScalingSphereBase;

		virtual bool IsInfluencing() const override;
		virtual ParticleBase const* GetPrimary() const override;
		virtual Vector3 const& GetPrimaryPosition() const override;
		virtual Vector3 const& GetPrimaryVelocity() const override;
	};

	class NonInfluencingSpace : public ScalingSphereBase
	{
	public:
		NonInfluencingSpace(ParticleBase * pHostParticle, float trueRadius);

		virtual void Initialize() override;

		virtual bool IsInfluencing() const override;
		virtual ParticleBase const* GetPrimary() const override;
		virtual Vector3 const& GetPrimaryPosition() const override;
		virtual Vector3 const& GetPrimaryVelocity() const override;

	private:
		static ParticleBase * FindPrimary(ScalingSphereBase const* pScaledSpace);
		static void ComputePrimaryKinetics(ScalingSphereBase const* pScaledSpace, Vector3 & position, Vector3 & velocity);

		ParticleBase *	m_pPrimary;			// Pointer to the local primary.
		Vector3			m_primaryPosition;	// Locally scaled position of the primary relative to this space.
		Vector3			m_primaryVelocity;	// Locally scaled velocity of the primary relative to this space.
	};

	class PassiveParticle : public ParticleBase
	{
	public:
		PassiveParticle(ScalingSphereBase * pHostSpace, float mass, Vector3 position, Vector3 velocity);
		virtual ~PassiveParticle() override = default;

		virtual void SetPosition(Vector3 const& position) override;
		virtual void SetVelocity(Vector3 const& velocity) override;
		virtual void Rescale(const float rescaleFactor) override;
		virtual void Initialize() override;

		virtual bool IsInfluencing() const override;
		virtual ScalingSphereBase * GetSphereOfInfluence() const override;
		virtual Vector3 const& GetPosition() const override;
		virtual Vector3 const& GetVelocity() const override;
		virtual Orbit const* GetOrbit() const override;

	protected:
		Vector3				m_position;
		Vector3				m_velocity;

		UniquePtr<Orbit>	m_pOrbit;
	};

	class InfluencingParticle : public PassiveParticle
	{
	public:
		using PassiveParticle::GetPosition;
		using PassiveParticle::GetVelocity;

		InfluencingParticle(ScalingSphereBase * pHostSpace, float mass, Vector3 position, Vector3 velocity);
		virtual ~InfluencingParticle() override = default;

		virtual void Initialize() override;

		virtual bool IsInfluencing() const override;
		virtual ScalingSphereBase * GetSphereOfInfluence() const override;
		virtual Orbit const* GetOrbit() const override;

	private:
		InfluencingSpace *	m_pSphereOfInfluence;
	};

public:
	OrbitalSystem(float hostMass, float hostSpaceTrueRadius);

	ParticleBase * GetHostParticle();
	ScalingSphereBase * GetHostSpace();

	/// <summary> Create a scaled space. </summary>
	/// <param name="hostParticle"> The particle to which the new space will be attached. </param>
	/// <param name="trueRadius"> The true radius (meters). </param>
	/// <returns> Reference to the created space. </returns>
	/// <exception cref="ApiException"> Invalid parameter. </exception>
	ScalingSphereBase * CreateScaledSpace(ParticleBase & hostParticle, float trueRadius);

	/// <summary> Create a particle. </summary>
	/// <param name="hostSpace"> The scaled space in which the particle will be placed. </param>
	/// <param name="mass"> The particle mass (kg). </param>
	/// <param name="position"> The particle initial position. </param>
	/// <param name="velocity"> The particle initial velocity. </param>
	/// <param name="isInfluencing"> Whether the particle has a sphere of influence (an influencing scaled space). </param>
	/// <returns> Reference to the created particle. </returns>
	/// <exception cref="ApiException"> Invalid parameter. </exception>
	ParticleBase * CreateParticle(ScalingSphereBase & hostSpace, float mass, Vector3 position, Vector3 velocity, bool isInfluencing);

	/// <summary> Create a particle with circular orbit. </summary>
	/// <param name="hostSpace"> The scaled space in which the particle will be placed. </param>
	/// <param name="mass"> The particle mass (kg). </param>
	/// <param name="position"> The particle initial position. </param>
	/// <param name="velocity"> The particle initial velocity. </param>
	/// <param name="isInfluencing"> Whether the particle has a sphere of influence (an influencing scaled space). </param>
	/// <returns> Reference to the created particle. </returns>
	/// <exception cref="ApiException"> Invalid parameter. </exception>
	ParticleBase * CreateParticle(ScalingSphereBase & hostSpace, float mass, Vector3 position, bool isInfluencing);

	/// <summary> Destroy a particle in this orbital system. </summary>
	/// <param name="pParticleBase"> Pointer to the particle to be destroyed. </param>
	void DestroyParticle(ParticleBase * pParticleBase);

	Result ResizeScalingSphere(ScalingSphereBase * pScalingSphereBase, const float trueRadius);

private:
	ScalingSphereBase * CreateScaledSpaceImpl(ParticleBase * pHostParticle, float trueRadius, bool isInfluencing);

	UniquePtr<HostParticle>	m_pHostParticle;	// Pointer to the interface of the host particle around which all other particles in the system orbit.
};

// --------------------------------------------------------------------------------------------------------------------------------

inline ParticleBase * OrbitalSystem::GetHostParticle()
{
	return static_cast<ParticleBase *>(m_pHostParticle.get());
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ScalingSphereBase * OrbitalSystem::GetHostSpace()
{
	return m_pHostParticle->GetScalingSphereList().Front().get();
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

inline void OrbitalSystem::HostParticle::SetPosition(Vector3 const& position)
{
	assert(false); // We shouldn't be here!
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void OrbitalSystem::HostParticle::SetVelocity(Vector3 const& velocity)
{
	assert(false); // We shouldn't be here!
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void OrbitalSystem::HostParticle::Rescale(const float rescaleFactor)
{
	// Nothing to do.
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void OrbitalSystem::HostParticle::Initialize()
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
	return nullptr;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Vector3 const& OrbitalSystem::HostParticle::GetPosition() const
{
	return Vector3::Zero();
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Vector3 const& OrbitalSystem::HostParticle::GetVelocity() const
{
	return Vector3::Zero();
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Orbit const* OrbitalSystem::HostParticle::GetOrbit() const
{
	return nullptr;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

inline bool OrbitalSystem::InfluencingSpace::IsInfluencing() const
{
	return true;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ParticleBase const* OrbitalSystem::InfluencingSpace::GetPrimary() const
{
	return m_pHostParticle;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Vector3 const& OrbitalSystem::InfluencingSpace::GetPrimaryPosition() const
{
	return Vector3::Zero();
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Vector3 const& OrbitalSystem::InfluencingSpace::GetPrimaryVelocity() const
{
	return Vector3::Zero();
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

inline OrbitalSystem::NonInfluencingSpace::NonInfluencingSpace(ParticleBase * pHostParticle, float trueRadius) :
	ScalingSphereBase(pHostParticle, trueRadius),
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

inline Vector3 const& OrbitalSystem::NonInfluencingSpace::GetPrimaryPosition() const
{
	return m_primaryPosition;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Vector3 const& OrbitalSystem::NonInfluencingSpace::GetPrimaryVelocity() const
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

inline void OrbitalSystem::NonInfluencingSpace::ComputePrimaryKinetics(ScalingSphereBase const* pScaledSpace, Vector3 & position,
	Vector3 & velocity)
{
	assert(nullptr != pScaledSpace);

	position = 0.f;
	velocity = 0.f;

	float scaleFactor = 1.f;
	ParticleBase const* pHostParticle = pScaledSpace->GetHostParticle();

	while (!pScaledSpace->IsInfluencing())
	{
		ParticleBase const*const pHostParticle = pScaledSpace->GetHostParticle();
		ScalingSphereBase const*const pHostSpace = pHostParticle->GetHostSphere();

		scaleFactor *= (pHostSpace->GetTrueRadius() / pScaledSpace->GetTrueRadius());

		position -= pHostParticle->GetPosition() * scaleFactor;
		velocity -= pHostParticle->GetVelocity() * scaleFactor;

		pScaledSpace = pHostSpace;
	}
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

inline void OrbitalSystem::PassiveParticle::SetPosition(Vector3 const& position)
{
	m_position = position;

	m_needsInitializationHelper.Set();
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void OrbitalSystem::PassiveParticle::SetVelocity(Vector3 const& velocity)
{
	m_velocity = velocity;

	m_needsInitializationHelper.Set();
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void OrbitalSystem::PassiveParticle::Rescale(const float rescaleFactor)
{
	m_position *= rescaleFactor;
	m_velocity *= rescaleFactor;

	m_needsInitializationHelper.Set();
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

inline Vector3 const& OrbitalSystem::PassiveParticle::GetPosition() const
{
	return m_position;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Vector3 const& OrbitalSystem::PassiveParticle::GetVelocity() const
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

class OrbitalSystemTestScript2 : public ITestScript
{
public:
	OrbitalSystemTestScript2();
	virtual ~OrbitalSystemTestScript2();

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

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_ORBITAL_SYSTEM_2_H
