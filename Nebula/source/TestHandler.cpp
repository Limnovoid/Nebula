#include "TestHandler.h"

#include "Macros.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

TestHandler::TestHandler(Settings settings) :
	m_pMenu(new UiMenu("Test handler")),
	m_pTestProgramMenu(new UiMenu("Test programs")),
	m_sharedLogFile(settings.m_sharedLogFilePath),
	m_pTemporaryUiIo(nullptr),
	m_shouldOutputToSharedFile(true),
	m_shouldOutputToProgramFile(true),
	m_shouldOutputToUi(true)
{
	m_pMenu->AddOption(SharedPtr<UiMenu>(m_pTestProgramMenu));
}

// --------------------------------------------------------------------------------------------------------------------------------

Result TestHandler::Register(SharedPtr<ITestProgram> pTestProgram)
{
	m_testPrograms.push_back(pTestProgram);

	SharedPtr<UiOption> pTestProgramUiOption = MakeShared<UiOption>(pTestProgram->GetTitle(), [=](UiIo const& uiIo)
	{
		if (RESULT_CODE_SUCCESS == uiIo.GetConfirmation("Run program"))
		{
			m_pTemporaryUiIo = &uiIo;

			Run(pTestProgram);

			m_pTemporaryUiIo = nullptr;
		}
	});

	m_pTestProgramMenu->AddOption(pTestProgramUiOption);

	return RESULT_CODE_SUCCESS;
}

// --------------------------------------------------------------------------------------------------------------------------------

void TestHandler::Run(SharedPtr<ITestProgram> pTestProgram)
{
	assert(!m_sharedLogFile.IsOpen());

	m_sharedLogFile.Open(File::OpenMode::WRITE | File::OpenMode::APPEND);

	m_currentProgramStats.Reset();

	Print(Fmt::Format("\nProgram: {}\n", pTestProgram->GetTitle()));

	pTestProgram->Run(*this);

	Print(Fmt::Format("Asserts passed / total (failed) = {} / {} ({})\n",
		m_currentProgramStats.GetNumAssertsPassed(), m_currentProgramStats.GetNumAsserts(),
		m_currentProgramStats.GetNumAsserts() - m_currentProgramStats.GetNumAssertsPassed()));

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
	ASSERT_THROW(start <= end, RESULT_CODE_INVALID_PARAMETER,
		Fmt::Format("Invalid range definition: [{},{}]; end index must be greater than or equal to start index.", start, end));
}

// --------------------------------------------------------------------------------------------------------------------------------

size_t TestHandler::IndexRange::ComputeNumIterations() const
{
	size_t numIterations = 1;

	if (m_first != m_last)
		numIterations += ((m_last - m_first) / m_stepSize);

	return numIterations;
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
