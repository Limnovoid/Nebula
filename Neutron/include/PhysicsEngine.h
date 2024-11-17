#ifndef NEUTRON_PHYSICS_ENGINE_H
#define NEUTRON_PHYSICS_ENGINE_H

#include "RedBlackTree.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

class PhysicsEngine
{
public:
	void Tick(double dT)
	{
	}

private:
	/*

	class Body;

	class ScaledSpace
	{
		float m_parameterizedRadius;
	}

	class Body
	{
		// Physical attributes...
		double	m_mass;
		Vec3	m_parameterizedPosition;
		Vec3	m_parameterizedVelocity;
	}

	class Primary : public Body
	{
		std::list<ScalingSpace> m_scalingSpaces; // Linked list of scaling spaces ordered by radius, highest to lowest.
	}

	*/
};

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_PHYSICS_ENGINE_H
