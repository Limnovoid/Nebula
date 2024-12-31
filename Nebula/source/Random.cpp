#include "Random.h"

#include "Macros.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

std::random_device	Random::g_randomDevice;
std::mt19937		Random::g_generator(g_randomDevice());

// --------------------------------------------------------------------------------------------------------------------------------

std::vector<size_t> Random::GetRandomSequence(size_t const min, size_t const max)
{
	ASSERT(min < max);

	std::vector<size_t> sequence(max - min);

	std::iota(sequence.begin(), sequence.end(), min);

	std::shuffle(sequence.begin(), sequence.end(), g_generator);

	return sequence;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<IsInt T>
T Random::Integer(T min, T max)
{
	std::uniform_int_distribution<T> distribution(min, max);

	return distribution(g_generator);
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
