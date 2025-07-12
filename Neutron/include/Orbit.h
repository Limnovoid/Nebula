#ifndef NEUTRON_ORBIT_H
#define NEUTRON_ORBIT_H

#include "NebulaTypes.h"
#include "ITestScript.h"
#include "Vector3.h"
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
		/// Compute orbit elements from the given initial position and velocity relative to the primary.
		/// Assumes the orbiter's mass is vanishingly small relative to the primary.
		/// </summary>
		/// <param name="gravityParameter"> The gravity parameter of the primary. </param>
		/// <param name="position"> Initial position of the orbiter relative to the primary. </param>
		/// <param name="velocity"> Initial velocity of the orbiter relative to the primary. </param>
		/// <exception cref="ApiException"> Angular momentum evaluted to zero. </exception>
		void Compute(float gravityParameter, Vector3 const& position, Vector3 const& velocity);

		float				m_angularMomentum			= 0.f;			/// Orbital specific angular momentum
		float				m_eccentricity				= 0.f;			/// Eccentricity

		float				m_velocityK					= 0.f;			/// Constant factor of orbital velocity:             mu / h
		float				m_massK						= 0.f;			/// Constant factor of mean anomaly for e >= 1:      mu^2 / h^3

		Type				m_type						= Type::Circle;	/// Type of orbit - defined by eccentricity, indicates the type of shape which describes the orbit path

		/* Dimensions */
		float				m_semiMajor					= 0.f;
		float				m_semiMinor					= 0.f;
		float				m_centreOffset				= 0.f;			/// Signed distance from occupied focus to centre, measured along perifocal frame's x-axis.
		Time::Microseconds	m_period					= 0;			/// Orbit period, measured in microseconds.
		float				m_parameter					= 0.f;			/// Orbit parameter, or semi-latus rectum:   h^2 / mu

		/* Perifocal frame */
		Vector3				m_perifocalX				= { 0.f };		/// The direction of the major axis.
		Vector3				m_perifocalY				= { 0.f };		/// The direction of the minor axis.
		Vector3				m_perifocalZ				= { 0.f };		/// The direction of the normal.

		/* Orientation */
		float				m_inclination				= 0.f;			/// Inclination.
		Vector3				m_ascendingNodeDirection	= { 0.f };		/// Direction of ascending node.
		float				m_rightAscension			= 0.f;			/// Right ascension of ascending node.
		float				m_argumentPeriapsis			= 0.f;			/// Argument of periapsis.

		//Quaternion m_orientation;										/// Quaternion orientation of the perifocal frame relative to the reference frame.
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
