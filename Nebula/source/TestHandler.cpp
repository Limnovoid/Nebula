#include "TestHandler.h"

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
		if (RESULT_CODE_SUCCESS == uiIo.GetConfirmation("Run test"))
		{
			assert(!m_sharedLogFile.IsOpen());

			m_sharedLogFile.Open(File::OpenMode::WRITE | File::OpenMode::APPEND);
			m_pTemporaryUiIo = &uiIo;

			pTestProgram->Run(*this);

			m_pTemporaryUiIo = nullptr;
			m_sharedLogFile.Close();
		}
	});

	m_pTestProgramMenu->AddOption(pTestProgramUiOption);

	return RESULT_CODE_SUCCESS;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

TestHandler::IndexRange::IndexRange(size_t start, size_t end, size_t stepsize) :
	m_first(start),
	m_last(end),
	m_stepSize(stepsize),
	m_numIterations(ComputeNumIterations())
{
	if (end < start)
		throw std::exception(); // TODO : custom exception with message
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
