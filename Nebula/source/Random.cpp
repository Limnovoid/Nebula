#include "Random.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

static std::random_device	g_randomDevice;
static std::mt19937			g_generator(g_randomDevice());

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

std::vector<size_t> GetRandomSequence(size_t const minIndex, size_t const maxIndex)
{
	assert(minIndex < maxIndex);

	std::vector<size_t> sequence(maxIndex - minIndex);
	std::iota(sequence.begin(), sequence.end(), minIndex);
	std::shuffle(sequence.begin(), sequence.end(), g_generator);

	return sequence;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<IsInt T>
T Integer(T min, T max)
{
	std::uniform_int_distribution<T> distribution(min, max);

	return distribution(g_generator);
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
