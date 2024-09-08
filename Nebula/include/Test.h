#ifndef NEBULA_ORION_TEST_H
#define NEBULA_ORION_TEST_H

#include "NebulaTypes.h"
#include "UiIo.h"
#include "UiMenu.h"
#include "Result.h"
#include "Format.h"

namespace Nebula
{

template<typename TReturn, typename TParameters>
class IUnitTest
{
public:
	IUnitTest(StringView title);

	virtual ~IUnitTest();

	StringView GetTitle();

	virtual Result Invoke(TParameters const& parameters, TReturn & returned) = 0;

private:
	String	m_title;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
inline IUnitTest<TReturn, TParameters>::IUnitTest(StringView title) :
	m_title(title)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
inline IUnitTest<TReturn, TParameters>::~IUnitTest()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
inline StringView IUnitTest<TReturn, TParameters>::GetTitle()
{
	return MakeStringView(m_title);
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class ITestProgram;

// --------------------------------------------------------------------------------------------------------------------------------

class TestHandler
{
public:
	class IndexRange
	{
	public:
		/// <param name="first">Index of first test iteration to execute.</param>
		/// <param name="last">Index of last test iteration to execute.</param>
		/// <param name="stepsize">Number by which index is incremented between executed test iterations.</param>
		/// <exception cref="std::exception">Range is ill-defined.</exception>
		IndexRange(size_t first = 0, size_t last = 0, size_t stepsize = 1);

		size_t GetNumIterations() const;

		size_t const	m_first;
		size_t const	m_last;
		size_t const	m_stepSize;
	};

	TestHandler(UiIo const& uiIo);

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

	Result Register(SharedPtr<ITestProgram> pTestProgram);

	SharedPtr<UiMenu> GetMenu();

private:
	template<typename TReturn, typename TParameters>
	void PrintAssertion(size_t const iteration, TParameters const& parameters, TReturn const& expectedValue);

	template<typename TReturn, typename TParameters>
	void PrintFailed(size_t const iteration, TParameters const& parameters, TReturn const& expectedValue, TReturn const& computedValue, Result const& result);

	template<typename TReturn, typename TParameters>
	void PrintPassed(size_t const iteration, TParameters const& parameters, TReturn const& expectedValue);

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

	UiIo const&								m_io;
	std::vector<SharedPtr<ITestProgram>>	m_testPrograms;

	SharedPtr<UiMenu>						m_pMenu;
	SharedPtr<UiMenu>						m_pTestProgramMenu;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters, typename TFuncGetParameters, typename TFuncGetExpected>
	requires (IsInvocable<TFuncGetParameters, size_t> && IsInvocable<TFuncGetExpected, size_t>)
inline Result TestHandler::Assert(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, TFuncGetParameters funcGetParameters,
	TFuncGetExpected funcGetExpected, IndexRange testRange)
{
	const size_t nIterations = testRange.GetNumIterations();

	m_io << "Unit test " << pUnitTest->GetTitle();
	if (1 < nIterations)
	{
		m_io << Fmt::Format(", iterating {} - {} (step size {})",
			testRange.m_first, testRange.m_last, testRange.m_stepSize);
	}
	m_io << '\n';

	size_t nPassed = 0;
	Result result;
	TReturn returnValue;

	for (size_t i = testRange.m_first; i <= testRange.m_last; i += testRange.m_stepSize)
	{
		result = pUnitTest->Invoke(funcGetParameters(i), returnValue);

		if ((RESULT_CODE_SUCCESS != result) || (funcGetExpected(i) != returnValue))
		{
			PrintFailed(i, funcGetParameters(i), funcGetExpected(i), returnValue, result);
		}
		else
		{
			++nPassed;

			PrintPassed(i, funcGetParameters(i), funcGetExpected(i));
		}

		m_io << '\n';
	}

	if (1 < nIterations)
		m_io << Fmt::Format("Summary: passed {} / {} (failed {})", nPassed, nIterations, (nIterations - nPassed)) << '\n';

	m_io << '\n';

	return ((nIterations == nPassed) ? RESULT_CODE_SUCCESS : RESULT_CODE_FAILURE);
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

inline SharedPtr<UiMenu> TestHandler::GetMenu()
{
	return m_pMenu;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
inline void TestHandler::PrintAssertion(size_t const iteration, TParameters const& parameters, TReturn const& expectedValue)
{
	m_io << Fmt::Format("{0:>4}: Assert ", iteration);

	PrintEvaluation(parameters, expectedValue);

	m_io << ": ";
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
inline void TestHandler::PrintFailed(size_t const iteration, TParameters const& parameters, TReturn const& expectedValue, TReturn const& computedValue, Result const& result)
{
	PrintAssertion(iteration, parameters, expectedValue);

	m_io << "failed, code " << result.GetString() << ", computed ";

	PrintEvaluation(parameters, computedValue);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
inline void TestHandler::PrintPassed(size_t const iteration, TParameters const& parameters, TReturn const& expectedValue)
{
	PrintAssertion(iteration, parameters, expectedValue);

	m_io << "passed";
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
	requires (IsWriteable<TParameters> && IsWriteable<TReturn>)
inline void TestHandler::PrintEvaluation(TParameters const& parameters, TReturn const& value)
{
	m_io << Fmt::Format("f({}) -> {}", parameters, value);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
	requires (IsWriteable<TParameters> && !IsWriteable<TReturn>)
inline void TestHandler::PrintEvaluation(TParameters const& parameters, TReturn const& value)
{
	m_io << Fmt::Format("f({}) -> !", parameters);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
	requires (!IsWriteable<TParameters> && IsWriteable<TReturn>)
inline void TestHandler::PrintEvaluation(TParameters const& parameters, TReturn const& value)
{
	m_io << Fmt::Format("f(!) -> {}", value);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
	requires (!IsWriteable<TParameters> && !IsWriteable<TReturn>)
inline void TestHandler::PrintEvaluation(TParameters const& parameters, TReturn const& value)
{
	m_io << Fmt::Format("f(!) -> !", parameters, value);
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

inline size_t TestHandler::IndexRange::GetNumIterations() const
{
	size_t numIterations = 1;

	if (m_first != m_last)
		numIterations += ((m_last - m_first) / m_stepSize);

	return numIterations;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class ITestProgram
{
public:
	ITestProgram(StringView title);

	virtual ~ITestProgram();

	virtual void Run(TestHandler & testHandler) = 0;

	StringView GetTitle();

private:
	String		m_title;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline StringView ITestProgram::GetTitle()
{
	return MakeStringView(m_title);
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_ORION_TEST_H
