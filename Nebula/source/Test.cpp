#include "Test.h"

namespace Nebula
{

TestHandler::TestHandler(UiIo const& uiIo) :
	m_io(uiIo),
	m_pMenu(new UiMenu("Test handler")),
	m_pTestProgramMenu(new UiMenu("Test programs"))
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
			pTestProgram->Run(*this);
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
