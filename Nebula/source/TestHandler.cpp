#include "TestHandler.h"

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
	TestHandler::IndexRange<size_t> indexRange = { 0, 10 };
	std::vector<size_t> const indexSequence = TestHandler::IndexRange<>::GetIndexSequence(indexRange);
	std::set<size_t> previousIndices;

	testHandler.Assert<bool, size_t>([&](size_t index)
	{
		bool const isInSequence = (indexSequence.cend() != std::find(indexSequence.cbegin(), indexSequence.cend(), index));
		bool const isNewIndex = (previousIndices.end() == previousIndices.find(index));

		previousIndices.insert(index);

		return (isInSequence && isNewIndex);

	}, TestHandler::FRangeRandomOrder({ 0, 10 }), [](size_t){ return true; }, "Test FRangeRandomOrder", { 0, 10 });

	// Negative indexes.
	std::array<int, 5> expectedIndices = { -10, -5, 0, 5, 10 };
	size_t iteration = 0;
	testHandler.Assert<int, int>([&](int index) { return index; },TestHandler::FRangeIndex<int>(),
		[&](int) { return expectedIndices[iteration++]; },"Negative indices increasing", TestHandler::IndexRange<int>(-10, 10, 5));
	//testHandler.Assert<int, int>([](int index){ return index; }, 0, [&](int index) { return expectedIndices[iteration++]; }, "Negative indices increasing", {-10, 10, 5});
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
