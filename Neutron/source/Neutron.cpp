#include "Neutron.h"

// Include all project headers for library building.
#include "PhysicsEngine.h"
#include "NebulaTypes.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

void AddTests(Nebula::TestHandler & testHandler)
{
	using namespace Nebula;

	testHandler.Register(MakeShared<PhysicsEngineTestScript>());

	// Pool exploration.
	std::pmr::pool_options poolOptions;
	poolOptions.largest_required_pool_block;
	poolOptions.max_blocks_per_chunk;

	std::pmr::unsynchronized_pool_resource unsynchronizedPoolResource(poolOptions);

	//std::allocator<std::pair<uint32_t, String>> allocator(&unsynchronizedPoolResource);
	//std::unordered_map<uint32_t, String> unorderedMap(&unsynchronizedPoolResource);

	std::pmr::vector<String> vector(&unsynchronizedPoolResource);

	std::pmr::unordered_map<uint32_t, String> map(&unsynchronizedPoolResource);
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
