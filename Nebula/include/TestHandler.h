#ifndef NEBULA_TEST_HANDLER_H
#define NEBULA_TEST_HANDLER_H

#include "Exception.h"
#include "File.h"
#include "Format.h"
#include "ITestProgram.h"
#include "IUnitTest.h"
#include "NebulaTypes.h"
#include "Result.h"
#include "UiIo.h"
#include "UiMenu.h"
#include "UnitTest.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

class TestHandler
{
public:
	struct Settings
	{
		String	m_sharedLogFilePath;
	};

	class IndexRange
	{
	public:
		/// <param name="first">Index of first test iteration to execute.</param>
		/// <param name="last">Index of last test iteration to execute.</param>
		/// <param name="stepsize">Number by which index is incremented between executed test iterations.</param>
		/// <exception cref="std::exception">Range is ill-defined.</exception>
		IndexRange(size_t first = 0, size_t last = 0, size_t stepsize = 1);

		size_t const	m_first;
		size_t const	m_last;
		size_t const	m_stepSize;
		size_t const	m_numIterations;

	private:
		size_t ComputeNumIterations() const;
	};

	TestHandler(Settings settings);

	template<typename TReturn, typename TParameters, typename TFuncGetParameters, typename TFuncGetExpected>
		requires (IsInvocable<TFuncGetParameters, size_t> && IsInvocable<TFuncGetExpected, size_t>)
	Result Assert(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, TFuncGetParameters funcGetParameters,
		TFuncGetExpected funcGetExpected, IndexRange testRange = IndexRange());

	template<typename TReturn, typename TParameters>
	Result Assert(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, TParameters * pParameters, TReturn const* pExpected,
		IndexRange const testRange = IndexRange());

	template<typename TReturn, typename TParameters>
	Result Assert(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, TParameters const& parameters, TReturn const& expected,
		IndexRange const testRange = IndexRange());

	template<typename TReturn, typename TParameters>
	Result Assert(std::function<TReturn(TParameters)> unitTestFunc, TParameters const& parameters, TReturn const& expected,
		IndexRange const testRange = IndexRange());

	Result Register(SharedPtr<ITestProgram> pTestProgram);

	SharedPtr<UiMenu> GetMenu();

private:
	void Print(StringView message);

	template<typename TReturn, typename TParameters>
	void OutputPreamble(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, IndexRange const& testRange);

	template<typename TReturn, typename TParameters>
	void OutputFailed(size_t const iteration, TParameters const& parameters, TReturn const& expectedValue, TReturn const& computedValue, Result const& result);

	template<typename TReturn, typename TParameters>
	void OutputPassed(size_t const iteration, TParameters const& parameters, TReturn const& expectedValue);

	void OutputSummary(size_t const nPassed, IndexRange const& testRange);

	template<typename TReturn, typename TParameters>
	void PrintAssertion(size_t const iteration, TParameters const& parameters, TReturn const& expectedValue);

	template<typename TReturn, typename TParameters>
		requires (IsWriteable<TParameters> && IsWriteable<TReturn>)
	void PrintEvaluation(TParameters const& parameters, TReturn const& value);

	template<typename TReturn, typename TParameters>
		requires (IsWriteable<TParameters> && !IsWriteable<TReturn>)
	void PrintEvaluation(TParameters const& parameters, TReturn const& value);

	template<typename TReturn, typename TParameters>
		requires (!IsWriteable<TParameters> && IsWriteable<TReturn>)
	void PrintEvaluation(TParameters const& parameters, TReturn const& value);

	template<typename TReturn, typename TParameters>
		requires (!IsWriteable<TParameters> && !IsWriteable<TReturn>)
	void PrintEvaluation(TParameters const& parameters, TReturn const& value);

	using TestProgramList = std::vector<SharedPtr<ITestProgram>>;

	TestProgramList			m_testPrograms;
	SharedPtr<UiMenu>		m_pMenu;
	SharedPtr<UiMenu>		m_pTestProgramMenu;

	File					m_sharedLogFile;
	UiIo const*				m_pTemporaryUiIo;

	bool					m_shouldOutputToSharedFile;
	bool					m_shouldOutputToProgramFile;
	bool					m_shouldOutputToUi;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters, typename TFuncGetParameters, typename TFuncGetExpected>
	requires (IsInvocable<TFuncGetParameters, size_t> && IsInvocable<TFuncGetExpected, size_t>)
inline Result TestHandler::Assert(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, TFuncGetParameters funcGetParameters,
	TFuncGetExpected funcGetExpected, IndexRange testRange)
{
	OutputPreamble(pUnitTest, testRange);

	size_t nPassed = 0;
	Result result = RESULT_CODE_NOT_INITIALIZED;
	TReturn returnValue;

	for (size_t i = testRange.m_first; i <= testRange.m_last; i += testRange.m_stepSize)
	{
		result = pUnitTest->Invoke(funcGetParameters(i), returnValue);

		if ((RESULT_CODE_SUCCESS != result) || (funcGetExpected(i) != returnValue))
		{
			OutputFailed(i, funcGetParameters(i), funcGetExpected(i), returnValue, result);
		}
		else
		{
			++nPassed;

			OutputPassed(i, funcGetParameters(i), funcGetExpected(i));
		}
	}

	OutputSummary(nPassed, testRange);

	return ((testRange.m_numIterations == nPassed) ? RESULT_CODE_SUCCESS : RESULT_CODE_FAILURE);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
inline Result TestHandler::Assert(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, TParameters * pParameters,
	TReturn const* pExpected, IndexRange const testRange)
{
	struct FuncGetParameters
	{
		TParameters const& operator()(size_t index)
		{
			return m_pParameters[index];
		}

		TParameters *	m_pParameters;
	};

	struct FuncGetExpected
	{
		TReturn const& operator()(size_t index)
		{
			return m_pExpected[index];
		}

		TReturn const*	m_pExpected;
	};

	return Assert(pUnitTest, FuncGetParameters(pParameters), FuncGetExpected(pExpected), testRange);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
inline Result TestHandler::Assert(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, TParameters const& parameters,
	TReturn const& expected, IndexRange const testRange)
{
	struct FuncGetParameters
	{
		TParameters const& operator()(size_t index)
		{
			return m_parameters;
		}

		TParameters const&	m_parameters;
	};

	struct FuncGetExpected
	{
		TReturn const& operator()(size_t index)
		{
			return m_expected;
		}

		TReturn const&	m_expected;
	};

	return Assert(pUnitTest, FuncGetParameters(parameters), FuncGetExpected(expected), testRange);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
inline Result TestHandler::Assert(std::function<TReturn(TParameters)> unitTestFunc, TParameters const& parameters,
	TReturn const& expected, IndexRange const testRange)
{
	using UnitTest = UnitTest<TReturn, TParameters>;
	using IUnitTest = IUnitTest<TReturn, TParameters>;

	SharedPtr<UnitTest> pUnitTest = MakeShared<UnitTest>(unitTestFunc, "UnitTest");

	return Assert(StaticPtrCast<IUnitTest>(pUnitTest), parameters, expected, testRange);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline SharedPtr<UiMenu> TestHandler::GetMenu()
{
	return m_pMenu;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void TestHandler::Print(StringView message)
{
	if (m_shouldOutputToProgramFile)
	{
	}

	if (m_shouldOutputToSharedFile)
	{
		(void) m_sharedLogFile.Write(message);
	}

	if (m_shouldOutputToUi)
	{
		if (nullptr != m_pTemporaryUiIo)
			m_pTemporaryUiIo->Print(message);
	}
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
inline void TestHandler::OutputPreamble(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, IndexRange const& testRange)
{
	Print(Fmt::Format("Unit test {}", pUnitTest->GetTitle()));

	if (1 < testRange.m_numIterations)
	{
		Print(Fmt::Format(", iterating {} - {} (step size {})",
			testRange.m_first, testRange.m_last, testRange.m_stepSize));
	}

	Print("\n");
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
inline void TestHandler::OutputFailed(size_t const iteration, TParameters const& parameters, TReturn const& expectedValue, TReturn const& computedValue, Result const& result)
{
	PrintAssertion(iteration, parameters, expectedValue);

	Print(Fmt::Format("failed, code {}, computed ", result.GetString()));

	PrintEvaluation(parameters, computedValue);

	Print("\n");
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
inline void TestHandler::OutputPassed(size_t const iteration, TParameters const& parameters, TReturn const& expectedValue)
{
	PrintAssertion(iteration, parameters, expectedValue);

	Print("passed\n");
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void TestHandler::OutputSummary(size_t const nPassed, IndexRange const& testRange)
{
	if (1 < testRange.m_numIterations)
	{
		Print(Fmt::Format("Summary: passed {} / {} (failed {})",
			nPassed, testRange.m_numIterations, (testRange.m_numIterations - nPassed)));

		Print("\n");
	}

	Print("\n");
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
inline void TestHandler::PrintAssertion(size_t const iteration, TParameters const& parameters, TReturn const& expectedValue)
{
	Print(Fmt::Format("{0:>4}: Assert ", iteration));

	PrintEvaluation(parameters, expectedValue);

	Print(": ");
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
	requires (IsWriteable<TParameters> && IsWriteable<TReturn>)
inline void TestHandler::PrintEvaluation(TParameters const& parameters, TReturn const& value)
{
	Print(Fmt::Format("f({}) -> {}", parameters, value));
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
	requires (IsWriteable<TParameters> && !IsWriteable<TReturn>)
inline void TestHandler::PrintEvaluation(TParameters const& parameters, TReturn const& value)
{
	Print(Fmt::Format("f({}) -> !", parameters));
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
	requires (!IsWriteable<TParameters> && IsWriteable<TReturn>)
inline void TestHandler::PrintEvaluation(TParameters const& parameters, TReturn const& value)
{
	Print(Fmt::Format("f(!) -> {}", value));
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
	requires (!IsWriteable<TParameters> && !IsWriteable<TReturn>)
inline void TestHandler::PrintEvaluation(TParameters const& parameters, TReturn const& value)
{
	Print(Fmt::Format("f(!) -> !", parameters, value));
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_TEST_HANDLER_H
