#ifndef NEUTRON_PHYSICS_ENGINE_H
#define NEUTRON_PHYSICS_ENGINE_H

#include "TestHandler.h"
#include "PriorityQueue.h"
#include "Vector3.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;
using namespace Nova;

// --------------------------------------------------------------------------------------------------------------------------------

class PhysicsEngine
{
public:
	PhysicsEngine();

	void Tick(double dT)
	{
	}

private:
	class Orbit
	{
	public:
		enum class Type
		{
			Circle,
			Ellipse,
			Hyperbola
		};

		class Parameters
		{
		public:
			Result Compute();

			double	m_massParameter;				/// Orbital specific angular momentum
			double	m_eccentricity;					/// Eccentricity

			double	m_vK = 0.f;						/// Constant factor of orbital velocity:             mu / h
			double	m_mK = 0.f;						/// Constant factor of mean anomaly for e >= 1:      mu^2 / h^3

			Type m_type = Type::Circle;		/// Type of orbit - defined by eccentricity, indicates the type of shape which describes the orbit path

			/* Dimensions */
			float m_semiMajor = 0.f;
			float m_semiMinor = 0.f;
			float m_c = 0.f;						/// Signed distance from occupied focus to centre, measured along perifocal frame's x-axis
			double m_t = 0.0;						/// Orbit period, measured in seconds
			float m_p = 0.f;						/// Orbit parameter, or semi-latus rectum:   h^2 / mu

			/* Orientation */
			float I = 0.f;							/// Inclination
			Vector3 N = { 0.f };					/// Direction of ascending node
			float Omega = 0.f;						/// Right ascension of ascending node
			float ArgPeriapsis = 0.f;				/// Argument of periapsis

			/* Perifocal frame */
			Vector3 PerifocalX = { 0.f };
			Vector3 PerifocalY = { 0.f };
			Vector3 PerifocalNormal = { 0.f };
			//Quaternion PerifocalOrientation;					/// Orientation of the perifocal frame relative to the reference frame
			// etc ...
		};

		class Section
		{
		public:
			Parameters	m_parameters;
			double		m_trueAnomalyEntry;
			double		m_trueAnomalyExit;
		};

		class Path
		{
		public:
			std::deque<Section>	m_sections;
		};
	};

	/*
	class Body;

	class ScaledSpace
	{
	public:
		float m_parameterizedRadius;

		std::list<SharedPtr<Body>>	m_bodies;
	};

	class Body
	{
	public:
		// Physical attributes...
		double	m_mass;
		Vec3	m_parameterizedPosition;
		Vec3	m_parameterizedVelocity;
	};

	class Primary : public Body
	{
		std::list<ScaledSpace>	m_scaledSpaces; // Linked list of scaling spaces ordered by radius, highest to lowest.
	};

	// Sorted list of bodies to update (earliest update first) implemented as a priority queue.
	PriorityQueue<SharedPtr<Body>>	m_updateQueue;
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
