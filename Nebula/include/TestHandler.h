#ifndef NEBULA_TEST_HANDLER_H
#define NEBULA_TEST_HANDLER_H

#include "Exception.h"
#include "File.h"
#include "Format.h"
#include "ITestScript.h"
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

	struct FRangeIndex
	{
		size_t operator()(size_t index) { return index; }
	};

	struct FRangeZero
	{
		size_t operator()(size_t index) { return 0; }
	};

	struct FRangeConstant
	{
		FRangeConstant(size_t constant) : m_constant(constant) {}

		size_t operator()(size_t index) { return m_constant; }

		size_t const	m_constant;
	};

	TestHandler(Settings settings);

	template<typename TReturn, typename TParameters, typename TFuncGetParameters, typename TFuncGetExpected>
		requires (IsInvocable<TFuncGetParameters, size_t> && IsInvocable<TFuncGetExpected, size_t>)
	Result Assert(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, TFuncGetParameters funcGetParameters,
		TFuncGetExpected funcGetExpected, IndexRange const& testRange = IndexRange());

	template<typename TReturn, typename TParameters>
	Result Assert(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, TParameters * pParameters, TReturn const* pExpected,
		IndexRange const& testRange = IndexRange());

	template<typename TReturn, typename TParameters>
	Result Assert(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, TParameters const& parameters, TReturn const& expected,
		IndexRange const& testRange = IndexRange());

	template<typename TReturn, typename TParameters, typename TFuncGetParameters, typename TFuncGetExpected>
		requires (IsInvocable<TFuncGetParameters, size_t> && IsInvocable<TFuncGetExpected, size_t>)
	Result Assert(std::function<TReturn(TParameters)> unitTestFunc, TFuncGetParameters funcGetParameters,
		TFuncGetExpected funcGetExpected, StringView title, IndexRange const& testRange = IndexRange());

	template<typename TReturn, typename TParameters>
	Result Assert(std::function<TReturn(TParameters)> unitTestFunc, TParameters const& parameters, TReturn const& expected,
		StringView title, IndexRange const& testRange = IndexRange());

	template<typename TReturn, typename TParameters>
	Result Assert(std::function<TReturn(TParameters)> unitTestFunc, TParameters const& parameters, TReturn const& expected,
		IndexRange const& testRange = IndexRange());

	Result Register(SharedPtr<ITestScript> pTestScript);

	void Run(SharedPtr<ITestScript> pTestScript);

	SharedPtr<UiMenu> GetMenu();

private:
	class ScriptStats
	{
	public:
		ScriptStats() :
			m_numAsserts(0),
			m_numAssertsPassed(0)
		{
		}

		void Reset()
		{
			m_numAsserts = 0;
			m_numAssertsPassed = 0;
		}

		void Update(size_t numPasses, IndexRange const& indexRange)
		{
			m_numAssertsPassed += numPasses;
			m_numAsserts += indexRange.m_numIterations;
		}

		size_t GetNumAsserts() const
		{
			return m_numAsserts;
		}

		size_t GetNumAssertsPassed() const
		{
			return m_numAssertsPassed;
		}

	private:
		size_t	m_numAsserts;
		size_t	m_numAssertsPassed;
	};

	struct AssertResult
	{
		size_t		nPasses		= 0;
		size_t		nAsserts	= 0;

		static AssertResult SinglePass() { return { 1, 1 }; }
		static AssertResult SingleFail() { return { 0, 1 }; }

		void Reset()
		{
			nPasses = 0;
			nAsserts = 0;
		}

		bool IsPass() const
		{
			return ((nAsserts != 0) && (nPasses == nAsserts));
		}

		AssertResult & operator+=(AssertResult const& rhs)
		{
			nPasses += rhs.nPasses;
			nAsserts += rhs.nAsserts;

			return *this;
		}
	};

	class AssertStack
	{
	public:
		struct Node
		{
			String			idString = "";
			AssertResult	assertResult = { 0, 0 };
		};

		AssertStack()
		{
			Node baseNode;
			baseNode.idString = "0";

			m_nodes.emplace_back(std::move(baseNode));
		}

		/// <summary> Push a new assert to the stack. </summary>
		Node & Push()
		{
			Node newNode;

			Node & parentNode = m_nodes.back();

			newNode.idString = Fmt::Format("{}.{}", parentNode.idString, parentNode.assertResult.nAsserts);

			return m_nodes.emplace_back(std::move(newNode));
		}

		/// <summary> Remove the top of the assert stack. </summary>
		void Pop()
		{
			assert(1 < m_nodes.size());

			m_nodes.pop_back();
		}

		Node & Back()
		{
			return m_nodes.back();
		}

		size_t Size() const
		{
			return m_nodes.size();
		}

		using Iterator = std::deque<Node>::reverse_iterator;

		Iterator Top()
		{
			return m_nodes.rbegin();
		}

		void Update(Iterator iter, AssertResult latest)
		{
			iter->assertResult += latest;

			if (1 == m_nodes.size())
			{
				iter->idString = Fmt::Format("{}", ++(iter->assertResult.nAsserts));
			}
			else
			{
				Iterator nextIter = iter;
				++nextIter;

				iter->idString = Fmt::Format("{}.{}", nextIter->idString, ++(iter->assertResult.nAsserts));
			}
		}

	private:
		std::deque<Node>	m_nodes;
	};

	template<typename TReturn, typename TParameters, typename TFuncGetParameters, typename TFuncGetExpected>
		requires (IsInvocable<TFuncGetParameters, size_t> && IsInvocable<TFuncGetExpected, size_t>)
	AssertResult AssertSingle(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, TFuncGetParameters funcGetParameters,
		TFuncGetExpected funcGetExpected, size_t testIndex, String & resultMessage);

	void Print(StringView message);

	template<typename TReturn, typename TParameters>
	void OutputPreamble(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, IndexRange const& testRange);

	template<typename TReturn, typename TParameters>
	void OutputFailed(size_t const iteration, TParameters const& parameters, TReturn const& expectedValue, TReturn const& computedValue, Result const& result, bool isSeries = false);

	template<typename TReturn, typename TParameters>
	void OutputFailedWithException(size_t const iteration, TParameters const& parameters, TReturn const& expectedValue, StringView exceptionString, bool isSeries = false);

	template<typename TReturn, typename TParameters>
	void OutputPassed(size_t const iteration, TParameters const& parameters, TReturn const& expectedValue, bool isSeries = false);

	void OutputSummary(size_t const nPassed, IndexRange const& testRange);

	template<typename TReturn, typename TParameters>
	void PrintAssertion(size_t const iteration, TParameters const& parameters, TReturn const& expectedValue, TReturn const& returnValue);

	template<typename TReturn, typename TParameters>
		requires (IsFormattable<TParameters> && IsFormattable<TReturn>)
	void PrintEvaluation(TParameters const& parameters, TReturn const& value);

	template<typename TReturn, typename TParameters>
		requires (IsFormattable<TParameters> && IsFormattable<TReturn>)
	String GetEvaluationString(TParameters const& parameters, TReturn const& value);

	template<typename TReturn, typename TParameters>
		requires (IsFormattable<TParameters> && !IsFormattable<TReturn>)
	String GetEvaluationString(TParameters const& parameters, TReturn const& value);

	template<typename TReturn, typename TParameters>
		requires (!IsFormattable<TParameters> && IsFormattable<TReturn>)
	String GetEvaluationString(TParameters const& parameters, TReturn const& value);

	template<typename TReturn, typename TParameters>
		requires (!IsFormattable<TParameters> && !IsFormattable<TReturn>)
	String GetEvaluationString(TParameters const& parameters, TReturn const& value);

	using TestScriptList = std::vector<SharedPtr<ITestScript>>;

	TestScriptList				m_testScripts;
	SharedPtr<UiMenu>			m_pMenu;
	SharedPtr<UiMenu>			m_pTestScriptMenu;

	File						m_sharedLogFile;
	UiIo const*					m_pTemporaryUiIo;

	bool						m_shouldOutputToSharedFile;
	bool						m_shouldOutputToScriptFile;
	bool						m_shouldOutputToUi;

	ScriptStats					m_currentScriptStats;

	std::vector<AssertResult>	m_assertResults;
	size_t						m_assertIndex = 0;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters, typename TFuncGetParameters, typename TFuncGetExpected>
	requires (IsInvocable<TFuncGetParameters, size_t> && IsInvocable<TFuncGetExpected, size_t>)
inline Result TestHandler::Assert(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, TFuncGetParameters funcGetParameters,
	TFuncGetExpected funcGetExpected, IndexRange const& testRange)
{
	auto fPrintAssertId = [&](size_t assertIndex)
	{
		for (size_t i = 0; i < assertIndex; ++i)
			Print(Fmt::Format("{}.", m_assertResults[i].nAsserts));
		Print(" ");
	};

	Result result = RESULT_CODE_FAILURE;

	bool const isSubAssert = (0 < m_assertIndex);

	if (isSubAssert)
		Print("\n");

	// Assert ID increment...
	++m_assertIndex;

	if (m_assertResults.size() == m_assertIndex)
		m_assertResults.emplace_back();

	assert(0 == m_assertResults[m_assertIndex].nAsserts);
	// /Assert ID increment

	fPrintAssertId(m_assertIndex);

	// Title
	if (!pUnitTest->GetTitle().empty())
		Print(Fmt::Format("{}. ", pUnitTest->GetTitle()));

	String resultMessage;

	// Result(s)
	if (testRange.m_first == testRange.m_last)
	{
		AssertResult assertResult = AssertSingle(pUnitTest, funcGetParameters, funcGetExpected, testRange.m_first, resultMessage);

		if (isSubAssert)
			fPrintAssertId(m_assertIndex);

		Print(resultMessage);
		Print("\n");

		if (assertResult.IsPass())
			result = RESULT_CODE_SUCCESS;
	}
	else
	{
		Print(Fmt::Format("Range = [{}, {}], stepsize {}:\n", testRange.m_first, testRange.m_last, testRange.m_stepSize));

		// Assert ID increment...
		++m_assertIndex;

		if (m_assertResults.size() == m_assertIndex)
			m_assertResults.emplace_back();
		// /Assert ID increment

		AssertResult & seriesResult = m_assertResults[m_assertIndex - 1];

		for (size_t i = testRange.m_first; i <= testRange.m_last; i += testRange.m_stepSize)
		{
			fPrintAssertId(m_assertIndex);

			seriesResult += AssertSingle(pUnitTest, funcGetParameters, funcGetExpected, i, resultMessage);

			Print(resultMessage);
			Print("\n");

			// Assert ID complete...
			m_assertResults[m_assertIndex].Reset();
			// /Assert ID complete
		}

		--m_assertIndex;

		fPrintAssertId(m_assertIndex);

		resultMessage = Fmt::Format("Series passed = {} / {}{}", seriesResult.nPasses, seriesResult.nAsserts,
			(seriesResult.IsPass() ? "" : Fmt::Format(" ({} failed)", seriesResult.nAsserts - seriesResult.nPasses)));

		Print(resultMessage);
		Print("\n");

		if (seriesResult.IsPass())
			result = RESULT_CODE_SUCCESS;
	}

	if (isSubAssert)
		fPrintAssertId(m_assertIndex - 1);

	// Assert ID complete...
	m_assertResults[m_assertIndex].Reset();
	--m_assertIndex;
	m_assertResults[m_assertIndex] += (RESULT_CODE_SUCCESS == result) ? AssertResult::SinglePass() : AssertResult::SingleFail();
	// /Assert ID complete

	return result;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
inline Result TestHandler::Assert(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, TParameters * pParameters,
	TReturn const* pExpected, IndexRange const& testRange)
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
	TReturn const& expected, IndexRange const& testRange)
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

template<typename TReturn, typename TParameters, typename TFuncGetParameters, typename TFuncGetExpected>
	requires (IsInvocable<TFuncGetParameters, size_t> && IsInvocable<TFuncGetExpected, size_t>)
inline Result TestHandler::Assert(std::function<TReturn(TParameters)> unitTestFunc, TFuncGetParameters funcGetParameters,
	TFuncGetExpected funcGetExpected, StringView title, IndexRange const& testRange)
{
	using UnitTest = UnitTest<TReturn, TParameters>;
	using IUnitTest = IUnitTest<TReturn, TParameters>;

	SharedPtr<UnitTest> pUnitTest = MakeShared<UnitTest>(unitTestFunc, title);

	return Assert(StaticPtrCast<IUnitTest>(pUnitTest), funcGetParameters, funcGetExpected, testRange);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
inline Result TestHandler::Assert(std::function<TReturn(TParameters)> unitTestFunc, TParameters const& parameters,
	TReturn const& expected, StringView title, IndexRange const& testRange)
{
	using UnitTest = UnitTest<TReturn, TParameters>;
	using IUnitTest = IUnitTest<TReturn, TParameters>;

	SharedPtr<UnitTest> pUnitTest = MakeShared<UnitTest>(unitTestFunc, title);

	return Assert(StaticPtrCast<IUnitTest>(pUnitTest), parameters, expected, testRange);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
inline Result TestHandler::Assert(std::function<TReturn(TParameters)> unitTestFunc, TParameters const& parameters,
	TReturn const& expected, IndexRange const& testRange)
{
	return Assert(unitTestFunc, parameters, expected, "", testRange);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline SharedPtr<UiMenu> TestHandler::GetMenu()
{
	return m_pMenu;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters, typename TFuncGetParameters, typename TFuncGetExpected>
	requires (IsInvocable<TFuncGetParameters, size_t> && IsInvocable<TFuncGetExpected, size_t>)
inline TestHandler::AssertResult TestHandler::AssertSingle(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, TFuncGetParameters funcGetParameters,
	TFuncGetExpected funcGetExpected, size_t testIndex, String & resultMessage)
{
	AssertResult assertResult = AssertResult::SingleFail();

	TReturn returnValue(funcGetExpected(testIndex));

	resultMessage = GetEvaluationString(funcGetExpected(testIndex), returnValue);

	try
	{
		Result result = pUnitTest->Invoke(funcGetParameters(testIndex), returnValue);

		if ((RESULT_CODE_SUCCESS == result) && (funcGetExpected(testIndex) == returnValue))
		{
			assertResult = AssertResult::SinglePass();
			resultMessage += " - Pass";
		}
		else
		{
			resultMessage += Fmt::Format(" - FAIL with code {} - expected {}", result, funcGetExpected(testIndex));
		}
	}
	catch (AssertionException const& exception)
	{
		resultMessage += Fmt::Format(" - FAIL with exception, {}", exception.ToString());
	}
	catch (Exception const& exception)
	{
		resultMessage += Fmt::Format(" - FAIL with exception, {}", exception.ToString());
	}

	return assertResult;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void TestHandler::Print(StringView message)
{
	if (m_shouldOutputToScriptFile)
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

//template<typename TReturn, typename TParameters>
//inline void TestHandler::OutputPreamble(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, IndexRange const& testRange)
//{
//	if (nullptr != m_pAssertStackNodeTop->m_pPrev)
//		Print("\n");
//
//	Print(Fmt::Format("{}", m_pAssertStackNodeTop->m_assertId));
//
//	if (!pUnitTest->GetTitle().empty())
//		Print(Fmt::Format(", \"{}\"", pUnitTest->GetTitle()));
//
//	if (1 < testRange.m_numIterations)
//	{
//		Print(Fmt::Format(", range [{}, {}] (step size {})",
//			testRange.m_first, testRange.m_last, testRange.m_stepSize));
//	}
//}
//
//// --------------------------------------------------------------------------------------------------------------------------------
//
//template<typename TReturn, typename TParameters>
//inline void TestHandler::OutputFailed(size_t const iteration, TParameters const& parameters, TReturn const& expectedValue,
//	TReturn const& computedValue, Result const& result, bool isSeries)
//{
//	if (isSeries)
//		Print(Fmt::Format("\n{0:>4}", iteration));
//
//	Print(": ");
//
//	PrintAssertion(iteration, parameters, expectedValue);
//
//	Print(Fmt::Format("failed, code {}, computed ", result.GetString()));
//
//	PrintEvaluation(parameters, computedValue);
//}
//
//// --------------------------------------------------------------------------------------------------------------------------------
//
//template<typename TReturn, typename TParameters>
//inline void TestHandler::OutputFailedWithException(size_t const iteration, TParameters const& parameters,
//	TReturn const& expectedValue, StringView exceptionString, bool isSeries)
//{
//	if (isSeries)
//		Print(Fmt::Format("\n{0:>4}", iteration));
//
//	Print(": ");
//
//	PrintAssertion(iteration, parameters, expectedValue);
//
//	Print(Fmt::Format("failed with exception, {}", exceptionString));
//}
//
//// --------------------------------------------------------------------------------------------------------------------------------
//
//template<typename TReturn, typename TParameters>
//inline void TestHandler::OutputPassed(size_t const iteration, TParameters const& parameters, TReturn const& expectedValue,
//	bool isSeries)
//{
//	if (isSeries)
//		Print(Fmt::Format("\n{0:>4}", iteration));
//
//	Print(": ");
//
//	PrintAssertion(iteration, parameters, expectedValue);
//
//	Print("passed");
//}
//
//// --------------------------------------------------------------------------------------------------------------------------------
//
//inline void TestHandler::OutputSummary(size_t const nPassed, IndexRange const& testRange)
//{
//	if (1 < testRange.m_numIterations)
//	{
//		Print(Fmt::Format("\nSummary: passed {} / {} (failed {})",
//			nPassed, testRange.m_numIterations, (testRange.m_numIterations - nPassed)));
//	}
//
//	Print("\n");
//}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
inline void TestHandler::PrintAssertion(size_t const iteration, TParameters const& parameters, TReturn const& expectedValue,
	TReturn const& returnValue)
{
	PrintEvaluation(parameters, returnValue);

}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
	requires (IsFormattable<TParameters> && IsFormattable<TReturn>)
inline void TestHandler::PrintEvaluation(TParameters const& parameters, TReturn const& value)
{
	Print(GetEvaluationString(parameters, value));
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
	requires (IsFormattable<TParameters> && IsFormattable<TReturn>)
inline String TestHandler::GetEvaluationString(TParameters const& parameters, TReturn const& value)
{
	return Fmt::Format("f({}) -> {}", parameters, value);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
	requires (IsFormattable<TParameters> && !IsFormattable<TReturn>)
inline String TestHandler::GetEvaluationString(TParameters const& parameters, TReturn const& value)
{
	return Fmt::Format("f({}) -> !", parameters);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
	requires (!IsFormattable<TParameters> && IsFormattable<TReturn>)
inline String TestHandler::GetEvaluationString(TParameters const& parameters, TReturn const& value)
{
	return Fmt::Format("f(!) -> {}", value);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
	requires (!IsFormattable<TParameters> && !IsFormattable<TReturn>)
inline String TestHandler::GetEvaluationString(TParameters const& parameters, TReturn const& value)
{
	return Fmt::Format("f(!) -> !", parameters, value);
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class TestHandlerTestScript : public ITestScript
{
public:
	TestHandlerTestScript();
	virtual ~TestHandlerTestScript();

protected:
	virtual void RunImpl(TestHandler & testHandler) override;
};

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_TEST_HANDLER_H
