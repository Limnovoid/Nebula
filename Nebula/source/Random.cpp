#include "Random.h"

#include "Macros.h"
#include "TestHandler.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

std::random_device	Random::g_randomDevice;
std::seed_seq		Random::g_seedSequence({ g_randomDevice(), g_randomDevice(), g_randomDevice() });
std::mt19937		Random::g_generator(g_seedSequence);

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

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

RandomTestScript::RandomTestScript() :
	ITestScript("Random")
{
}

// --------------------------------------------------------------------------------------------------------------------------------

RandomTestScript::~RandomTestScript()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void RandomTestScript::RunImpl(TestHandler & testHandler)
{
	std::unordered_map<uint64_t, size_t> uuidCounts;

	TestHandler::OutputMode const outputMode = testHandler.SetOutputMode(TestHandler::OutputMode::QUIET);

	testHandler.Assert<bool, int, int>([&](int)
	{
		auto pair = uuidCounts.try_emplace(Random::Integer<uint64_t>(), 0);

		if (!pair.second)
			pair.first->second = 1 + pair.first->second;

		return pair.second;

	}, 0, true, "Is unique", { 0, 1000000 });

	testHandler.SetOutputMode(outputMode);
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
