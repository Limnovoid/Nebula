#ifndef NEUTRON_PHYSICS_ENGINE_H
#define NEUTRON_PHYSICS_ENGINE_H

#include "TestHandler.h"
#include "PriorityQueue.h"
#include "Vector3.h"
#include "NeutronTime.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;
using namespace Nova;

// --------------------------------------------------------------------------------------------------------------------------------

class PhysicsEngine
{
public:
	PhysicsEngine();

	void Tick(Time::Microseconds dT);

private:
	/* Forward declarations. */
	class Body;
	class LocalSpace;

	class Orbit
	{
	public:
		enum class Type
		{
			Circle,
			Ellipse,
			Hyperbola
		};

		class Elements
		{
		public:
			Result Compute(LocalSpace const& localSpace, Vector3 const& localPosition, Vector3 const& localVelocity);

			double	m_angularMomentum;				/// Orbital specific angular momentum
			double	m_eccentricity;					/// Eccentricity

			double	m_velocityK = 0.f;				/// Constant factor of orbital velocity:             mu / h
			double	m_massK = 0.f;					/// Constant factor of mean anomaly for e >= 1:      mu^2 / h^3

			Type m_type = Type::Circle;				/// Type of orbit - defined by eccentricity, indicates the type of shape which describes the orbit path

			/* Dimensions */
			float m_semiMajor = 0.f;
			float m_semiMinor = 0.f;
			float m_centreOffset = 0.f;				/// Signed distance from occupied focus to centre, measured along perifocal frame's x-axis.
			Time::Microseconds m_period = 0;		/// Orbit period, measured in microseconds.
			float m_parameter = 0.f;				/// Orbit parameter, or semi-latus rectum:   h^2 / mu

			/* Orientation */
			float m_inclination = 0.f;				/// Inclination.
			Vector3 m_ascendingNode = { 0.f };		/// Direction of ascending node.
			float m_rightAscension = 0.f;			/// Right ascension of ascending node.
			float m_argumentPeriapsis = 0.f;		/// Argument of periapsis.

			/* Perifocal frame */
			Vector3 PerifocalX = { 0.f };
			Vector3 PerifocalY = { 0.f };
			Vector3 PerifocalZ = { 0.f };
			//Quaternion PerifocalOrientation;		/// Orientation of the perifocal frame relative to the reference frame
			// etc ...
		};

		class Section
		{
		public:
			Elements	m_elements;
			double		m_trueAnomalyEntry;
			double		m_trueAnomalyExit;
		};

		class Path
		{
		public:
			std::deque<Section>	m_sections;
		};
	};

	class LocalSpace
	{
	public:
		/// <returns> The position of the local primary relative to this local space. Distances are parameterized to this local space. </returns>
		Vector3 GetLocalPrimaryPosition() const;

		/// <returns> The velocity of the local primary relative to this local space. Distances are parameterized to this local space. </returns>
		Vector3 GetLocalPrimaryVelocity() const;

	private:
		int64_t	m_trueRadius;			// Radius in meters.
		float	m_radius;				// Radius relative to parent space.
		double	m_gravityParameter;		// Gravitational parameter = M * G | G = gravitational constant, M = mass of locally influencing body.

		SharedPtr<Body>				m_pHost;
		std::list<SharedPtr<Body>>	m_bodies;
	};

	class Body
	{
	public:
		// Physical attributes...
		double			m_mass;
		Vector3			m_localPosition;
		Vector3			m_localVelocity;

		Orbit::Elements	m_elements;
	};

	/*
	class Primary : public Body
	{
		std::list<LocalSpace>	m_scaledSpaces; // Linked list of scaling spaces ordered by radius, highest to lowest.
	};

	// Sorted list of bodies to update (earliest update first) implemented as a priority queue.
	PriorityQueue<SharedPtr<Body>>	m_updateQueue;

	std::list<LocalSpace>	m_rootSpaces;
	*/
};

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class PhysicsEngineTestScript : public ITestScript
{
public:
	PhysicsEngineTestScript();
	virtual ~PhysicsEngineTestScript();

protected:
	virtual void RunImpl(TestHandler & testHandler) override;
};

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_PHYSICS_ENGINE_H
