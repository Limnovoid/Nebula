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
	Nova::RedBlackTree		m_updateQueue;
};

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_PHYSICS_ENGINE_H
