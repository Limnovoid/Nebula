#ifndef NEUTRON_ORBIT_H
#define NEUTRON_ORBIT_H

#include "NebulaTypes.h"
#include "ITestScript.h"
#include "Types.h"
#include "NeutronTime.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

/* Forward Declarations */
class ScalingSpace;

using namespace Nebula;

// --------------------------------------------------------------------------------------------------------------------------------

class Orbit
{
	friend class ParticleBase;

public:
	enum class Type
	{
		Circle,
		Ellipse,
		Parabola,
		Hyperbola
	};

	class Elements
	{
	public:
		Elements() = default;

		/// <summary>
		/// Compute orbit elements from an initial position and velocity relative to the primary.
		/// Assumes the orbiter's mass is vanishingly small relative to the primary.
		/// </summary>
		/// <param name="gravityParameter"> The gravity parameter of the primary as defined by the orbiter's host Sphere. </param>
		/// <param name="position"> Initial position of the orbiter relative to the primary, in the units of the orbiter's host Sphere. </param>
		/// <param name="velocity"> Initial velocity of the orbiter relative to the primary, in the units of the orbiter's host Sphere. </param>
		/// <exception cref="ApiException"> Angular momentum evaluted to zero. </exception>
		void Compute(Unit::Relative const& gravityParameter, RelVector3 const& position, RelVector3 const& velocity);

		Unit::Relative		m_angularMomentum			= Unit::Relative(0.f);	/// Orbital specific angular momentum
		float				m_eccentricity				= 0.f;					/// Eccentricity (unitless)
		Type				m_type						= Type::Circle;			/// Type of orbit - defined by eccentricity, indicates the type of shape which describes the orbit path

		/* Dimensions */
		Unit::Relative		m_semiMajor					= Unit::Relative(0.f);
		Unit::Relative		m_semiMinor					= Unit::Relative(0.f);
		Unit::Relative		m_centreOffset				= Unit::Relative(0.f);	/// Signed distance from occupied focus to centre, measured along perifocal frame's x-axis.
		Time::Microseconds	m_period					= 0;					/// Orbit period, measured in microseconds.
		Unit::Relative		m_parameter					= Unit::Relative(0.f);	/// Orbit parameter, or semi-latus rectum:   h^2 / mu

		/* Perifocal frame */
		RelVector3			m_perifocalX				= RelVector3::ZERO;		/// The direction of the major axis.
		RelVector3			m_perifocalY				= RelVector3::ZERO;		/// The direction of the minor axis.
		RelVector3			m_perifocalZ				= RelVector3::ZERO;		/// The direction of the normal.

		/* Orientation */
		Unit::Radians		m_inclination				= Unit::Radians(0);		/// Inclination.
		RelVector3			m_ascendingNodeDirection	= RelVector3::ZERO;		/// Direction of ascending node.
		Unit::Radians		m_rightAscension			= Unit::Radians(0);		/// Right ascension of ascending node.
		Unit::Radians		m_argumentPeriapsis			= Unit::Radians(0);		/// Argument of periapsis.
		//Quaternion		m_orientation;										/// Quaternion orientation of the perifocal frame relative to the reference frame.

		/* Constants of integration */
		Unit::Relative		m_kVelocity = Unit::Relative(0.f);	/// Constant factor of orbital velocity:             mu / h
		Unit::Relative		m_kMass = Unit::Relative(0.f);		/// Constant factor of mean anomaly for e >= 1:      mu^2 / h^3
	};

	class Section
	{
	public:
		Section() = default;

		Elements	m_elements;
		double		m_trueAnomalyEntry	= 0.0;
		double		m_trueAnomalyExit	= 0.0;
	};

	Orbit();

	Section & GetCurrentSection();
	Section const& GetCurrentSection() const;

private:
	using SectionList = std::deque<UniquePtr<Section>>;

	SectionList	m_sections;
	size_t		m_currentSectionIndex;
	double		m_trueAnomaly;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline Orbit::Section & Orbit::GetCurrentSection()
{
	assert(m_currentSectionIndex < m_sections.size());

	return *m_sections[m_currentSectionIndex];
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Orbit::Section const& Orbit::GetCurrentSection() const
{
	assert(m_currentSectionIndex < m_sections.size());

	return *m_sections[m_currentSectionIndex];
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class OrbitTestScript : public ITestScript
{
public:
	OrbitTestScript();
	virtual ~OrbitTestScript();

protected:
	virtual void RunImpl(TestHandler & testHandler) override;
};

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_ORBIT_H
