#ifndef NEUTRON_PHYSICS_ENGINE_H
#define NEUTRON_PHYSICS_ENGINE_H

#include "TestHandler.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;

// --------------------------------------------------------------------------------------------------------------------------------

class PhysicsEngine
{
public:
	void Tick(double dT)
	{
	}

	/*
private:
	class Orbit
	{
	public:
		class Parameters
		{
			double	m_massParameter;
			double	m_eccentricity;
			// etc ...
		};

		class Section
		{
		public:
			Parameters	m_parameters;
		};

		class Path
		{
		public:
			std::list<Section>	m_sections;
		};
	};

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
