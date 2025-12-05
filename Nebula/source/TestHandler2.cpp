#include "TestHandler2.h"

#include "Random.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

void Suite::TestOption::Execute(UiIo const& ui)
{
	ui.Print(Fmt::Format("Running test class '{}'...\n", m_testName));

	m_testInterface.Run();

	ui.Print(Fmt::Format("Test class '{}' complete.\n", m_testName));
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

TestRegister::SuiteMap & TestRegister::GetSuiteMap()
{
	static SuiteMap s_testSuiteMapSingleton{};

	const  SuiteMap::const_iterator rootSuiteIter = s_testSuiteMapSingleton.find(ROOT_SUITE_NAME.Get());
	if (s_testSuiteMapSingleton.cend() == rootSuiteIter)
		s_testSuiteMapSingleton.emplace(ROOT_SUITE_NAME.Get(), MakeShared<Suite>(ROOT_SUITE_MENU_NAME.Get()));

	return s_testSuiteMapSingleton;
}

// --------------------------------------------------------------------------------------------------------------------------------

SharedPtr<UiMenu> TestRegister::GetMenu()
{
	return GetSuiteMap()[ROOT_SUITE_NAME.Get()]->GetMenu();
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

ITest::Sequence::Sequence(const Index min, const Index max, const size_t stepSize, const SequenceType type) :
	m_indices{ 0 },
	m_maxIndex(max),
	m_minIndex(min)
{
	ASSERT_THROW(min < max, RESULT_CODE_INVALID_PARAMETER,
		Fmt::Format("Index 'min' <{}> cannot be less than 'max' <{}>", min, max));

	ASSERT_THROW(0 != stepSize, RESULT_CODE_INVALID_PARAMETER,
		Fmt::Format("Step size cannot be zero", min, max));

	const size_t numSteps = 1 + ((max - min) / stepSize);

	m_indices.resize(numSteps);

	Index index = (SEQUENCE_TYPE_DECREMENTAL == type) ? max : min;
	for (size_t i = 0; i < numSteps; ++i)
	{
		m_indices[i] = index;

		if (SEQUENCE_TYPE_DECREMENTAL == type)
			index -= stepSize;
		else
			index += stepSize;
	}

	if (SEQUENCE_TYPE_RANDOMIZED == type)
		Random::Shuffle(m_indices);
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
