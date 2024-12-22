#include "TestHandler.h"

#include "Macros.h"

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

	m_sharedLogFile.Open(File::OpenMode::WRITE | File::OpenMode::APPEND);

	m_currentScriptStats.Reset();

	Print(Fmt::Format("\nScript: {}\n", pTestScript->GetTitle()));

	pTestScript->Run(*this);

	Print(Fmt::Format("Asserts passed = {} / {}",
		m_currentScriptStats.GetNumAssertsPassed(), m_currentScriptStats.GetNumAsserts()));

	if (m_currentScriptStats.GetNumAssertsPassed() != m_currentScriptStats.GetNumAsserts())
		Print(Fmt::Format(" ({} FAILED)", m_currentScriptStats.GetNumAsserts() - m_currentScriptStats.GetNumAssertsPassed()));

	Print("\n");

	m_sharedLogFile.Close();
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

TestHandler::IndexRange::IndexRange(size_t start, size_t end, size_t stepsize) :
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

	if (m_first != m_last)
		numIterations += ((m_last - m_first) / m_stepSize);

	return numIterations;
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
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
