#include "TestHandler.h"

#include "Macros.h"
#include "Maths.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

TestHandler::TestHandler(Settings settings) :
	m_pMenu(new UiMenu("Test handler")),
	m_pTestScriptMenu(new UiMenu("Test scripts")),
	m_sharedLogFile(settings.m_sharedLogFilePath),
	m_pTemporaryUiIo(nullptr),
	m_shouldOutputToSharedFile(true),
	m_shouldOutputToScriptFile(true),
	m_shouldOutputToUi(true),
	m_assertResults(1)
{
	m_pMenu->AddOption(SharedPtr<UiMenu>(m_pTestScriptMenu));
}

// --------------------------------------------------------------------------------------------------------------------------------

Result TestHandler::Register(SharedPtr<ITestScript> pTestScript)
{
	m_testScripts.push_back(pTestScript);

	SharedPtr<UiOption> pTestScriptUiOption = MakeShared<UiOption>(pTestScript->GetTitle(), [=](UiIo const& uiIo)
	{
		if (RESULT_CODE_SUCCESS == uiIo.GetConfirmation(Fmt::Format("Run script \"{}\"", pTestScript->GetTitle()), true))
		{
			m_pTemporaryUiIo = &uiIo;

			Run(pTestScript);

			m_pTemporaryUiIo = nullptr;
		}
	});

	m_pTestScriptMenu->AddOption(pTestScriptUiOption);

	return RESULT_CODE_SUCCESS;
}

// --------------------------------------------------------------------------------------------------------------------------------

void TestHandler::Run(SharedPtr<ITestScript> pTestScript)
{
	assert(!m_sharedLogFile.IsOpen());
	assert(0 == m_assertIndex);

	m_sharedLogFile.Open(File::OpenMode::WRITE | File::OpenMode::APPEND);

	m_assertResults.front().Reset();

	Print(Fmt::Format("\nScript: {}\n", pTestScript->GetTitle()));

	pTestScript->Run(*this);

	AssertResult & scriptResults = m_assertResults.front(); // Note : must come after calling Run - calls to Assert resizing m_assertResults will invalidate existing references.

	Print(Fmt::Format("Asserts passed = {} / {}", scriptResults.nPasses, scriptResults.nAsserts));

	if (scriptResults.nPasses != scriptResults.nAsserts)
		Print(Fmt::Format(" ({} FAILED)", scriptResults.nAsserts - scriptResults.nPasses));

	Print("\n");

	m_sharedLogFile.Close();
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

TestHandler::IndexRange::IndexRange(size_t start, size_t end, int stepsize) :
	m_first(start),
	m_last(end),
	m_stepSize(stepsize),
	m_numIterations(ComputeNumIterations())
{
}

// --------------------------------------------------------------------------------------------------------------------------------

size_t TestHandler::IndexRange::ComputeNumIterations() const
{
	size_t numIterations = 1;

	ASSERT(0 != m_stepSize);

	if (m_first < m_last)
	{
		ASSERT(0 < m_stepSize);

		numIterations += ((m_last - m_first) / static_cast<size_t>(m_stepSize));
	}
	else if (m_last < m_first)
	{
		ASSERT(m_stepSize < 0);

		numIterations += ((m_first - m_last) / static_cast<size_t>(Maths::Abs(m_stepSize)));
	}

	return numIterations;
}

// --------------------------------------------------------------------------------------------------------------------------------

std::vector<size_t> TestHandler::IndexRange::GetIndexSequence(IndexRange const& indexRange)
{
	std::vector<size_t> sequence(indexRange.m_numIterations);

	for (size_t i = 0; i < indexRange.m_numIterations; ++i)
	{
		size_t testIndex = indexRange.m_first;

		if (0 < indexRange.m_stepSize)
			testIndex += i * static_cast<size_t>(indexRange.m_stepSize);
		else
			testIndex -= i * static_cast<size_t>(Maths::Abs(indexRange.m_stepSize));

		sequence[i] = testIndex;
	}

	return sequence;
}

// --------------------------------------------------------------------------------------------------------------------------------

size_t TestHandler::IndexRange::GetIndexPosition(IndexRange const& indexRange, size_t index)
{
	if (indexRange.m_first == index)
		return 0;

	size_t sequenceIndex;

	if (0 < indexRange.m_stepSize)
		sequenceIndex = (index - indexRange.m_first) / static_cast<size_t>(indexRange.m_stepSize);
	else
		sequenceIndex = (indexRange.m_first - index) / static_cast<size_t>(Maths::Abs(indexRange.m_stepSize));

	return sequenceIndex;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

TestHandlerTestScript::TestHandlerTestScript() :
	ITestScript("TestHandler")
{
}

// --------------------------------------------------------------------------------------------------------------------------------

TestHandlerTestScript::~TestHandlerTestScript()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void TestHandlerTestScript::RunImpl(TestHandler & testHandler)
{
	// 1
	testHandler.Assert<size_t, size_t>([](size_t index) { return index; },
		TestHandler::FRangeIndex(), TestHandler::FRangeIndex(), "Test single assert", { 1, 1 });

	// 2
	testHandler.Assert<size_t, size_t>([](size_t index) { return index; },
		TestHandler::FRangeIndex(), TestHandler::FRangeIndex(), "Test single assert", { 1, 1 });

	// 3
	testHandler.Assert<size_t, size_t>([](size_t index) { return index; },
		TestHandler::FRangeIndex(), TestHandler::FRangeIndex(), "Test single assert", { 1, 1 });

	// 4.1 - 4.10
	testHandler.Assert<size_t, size_t>([](size_t index) { return index; },
		TestHandler::FRangeIndex(), TestHandler::FRangeIndex(), "Test assert range", { 1, 10 });

	// 5.1
	testHandler.Assert<size_t, size_t>([&](size_t index)
	{
		testHandler.Assert<size_t, size_t>([](size_t index) { return index; },
			TestHandler::FRangeIndex(), TestHandler::FRangeIndex(), "Single sub-assert", { 1, 1 });

		return index;

	}, TestHandler::FRangeIndex(), TestHandler::FRangeIndex(), "Test single sub-assert", { 1, 1 });

	// 6.1.1
	testHandler.Assert<size_t, size_t>([&](size_t index)
	{
		testHandler.Assert<size_t, size_t>([&](size_t index)
		{
			testHandler.Assert<size_t, size_t>([](size_t index) { return index; },
				TestHandler::FRangeIndex(), TestHandler::FRangeIndex(), "Single sub-sub-assert", { 1, 1 });

			return index;

		}, TestHandler::FRangeIndex(), TestHandler::FRangeIndex(), "Single sub-assert", { 1, 1 });

		return index;

	}, TestHandler::FRangeIndex(), TestHandler::FRangeIndex(), "Test single sub-sub-assert", { 1, 1 });

	// 7.1.1 - 7.1.10
	testHandler.Assert<size_t, size_t>([&](size_t index)
	{
		testHandler.Assert<size_t, size_t>([](size_t index) { return index; },
			TestHandler::FRangeIndex(), TestHandler::FRangeIndex(), "Sub-assert range", { 1, 10 });

		return index;

	}, TestHandler::FRangeIndex(), TestHandler::FRangeIndex(), "Test sub-assert range", { 1, 1 });

	// 8.1.1 - 8.10.1
	testHandler.Assert<size_t, size_t>([&](size_t index)
	{
		testHandler.Assert<size_t, size_t>([](size_t index) { return index; },
			TestHandler::FRangeIndex(), TestHandler::FRangeIndex(), "Single sub-assert", { 1, 1 });

		return index;

	}, TestHandler::FRangeIndex(), TestHandler::FRangeIndex(), "Test range of single sub-asserts", { 1, 10 });

	// AssertException handler
	testHandler.Assert<size_t, size_t>([](size_t index)
	{
		ASSERT_THROW(false, RESULT_CODE_FAILURE, "Test AssertException");
		return index;

	}, TestHandler::FRangeIndex(), TestHandler::FRangeIndex(), "Test AssertException handling", { 1, 1 });

	// Exception handler
	testHandler.Assert<size_t, size_t>([](size_t index)
	{
		throw Exception(RESULT_CODE_FAILURE, "Test Exception");
		return index;

	}, TestHandler::FRangeIndex(), TestHandler::FRangeIndex(), "Test Exception handling", { 1, 1 });

	// Random index sequence
	TestHandler::IndexRange indexRange = { 0, 10 };
	std::vector<size_t> const indexSequence = TestHandler::IndexRange::GetIndexSequence(indexRange);
	std::set<size_t> previousIndices;

	testHandler.Assert<bool, size_t>([&](size_t index)
	{
		bool const isInSequence = (indexSequence.cend() != std::find(indexSequence.cbegin(), indexSequence.cend(), index));
		bool const isNewIndex = (previousIndices.end() == previousIndices.find(index));

		previousIndices.insert(index);

		return (isInSequence && isNewIndex);

	}, TestHandler::FRangeRandomOrder({ 0, 10 }), [](size_t){ return true; }, "Test FRangeRandomOrder", { 0, 10 });
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
