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
#include "Random.h"

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
		/// <returns> The sequence of indices defined by the given index range. </returns>
		static std::vector<size_t> GetIndexSequence(IndexRange const& indexRange);

		/// <returns> The position of the given index in the sequence defined by this range of indices. </returns>
		static size_t GetIndexPosition(IndexRange const& indexRange, size_t index);

		/// <param name="first">Index of first test iteration to execute.</param>
		/// <param name="last">Index of last test iteration to execute.</param>
		/// <param name="stepsize">Number by which index is incremented between executed test iterations. Use a negative for decrementing.</param>
		/// <exception cref="std::exception">Range is ill-defined.</exception>
		IndexRange(size_t first = 0, size_t last = 0, int stepsize = 1);

		size_t const	m_first;
		size_t const	m_last;
		int const		m_stepSize;
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

		size_t const		m_constant;
	};

	struct FRangeRandomOrder
	{
		/// <param name="indexRange"> The index range passed into TestHandler::Assert. </param>
		FRangeRandomOrder(IndexRange const& indexRange) :
			m_indexRange(indexRange),
			m_indexSequence(IndexRange::GetIndexSequence(indexRange))
		{
			Random::Shuffle(m_indexSequence);
		}

		size_t operator()(size_t index)
		{
			return m_indexSequence[IndexRange::GetIndexPosition(m_indexRange, index)];
		}

		IndexRange const	m_indexRange;
		std::vector<size_t>	m_indexSequence;
	};

	template<typename T>
	struct FConstant
	{
		FConstant(T const& constant) : m_constant(constant) {}

		T const& operator()(size_t index) { return m_constant; }

		T const				m_constant;
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

	std::vector<AssertResult>	m_assertResults;
	size_t						m_assertIndex = 0;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters, typename TFuncGetParameters, typename TFuncGetExpected>
	requires (IsInvocable<TFuncGetParameters, size_t> && IsInvocable<TFuncGetExpected, size_t>)
inline Result TestHandler::Assert(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, TFuncGetParameters funcGetParameters,
	TFuncGetExpected funcGetExpected, IndexRange const& testRange)
{
	// Print the IDs up to and including assertIndex.
	auto fPrintAssertId = [&](size_t assertIndex)
	{
		for (size_t i = 0; i < assertIndex + 1; ++i)
			Print(Fmt::Format("{}.", m_assertResults[i].nAsserts));
		Print(" ");
	};

	// Print the result of the assert with ID assertIndex.
	auto fPrintResult = [&](size_t assertIndex, StringView resultMessage, bool const isSubAssert)
	{
		if (0 < m_assertResults[assertIndex + 1].nAsserts) // Had sub-assert(s)
		{
			Print("\n");
			fPrintAssertId(assertIndex);
		}

		Print(resultMessage);

		if (!isSubAssert)
			Print("\n");
	};

	Result result = RESULT_CODE_FAILURE;

	bool const isSubAssert = (0 < m_assertIndex);

	if (isSubAssert)
		Print("\n");

	fPrintAssertId(m_assertIndex);

	// Assert ID increment...
	++m_assertIndex;

	if (m_assertResults.size() == m_assertIndex)
		m_assertResults.emplace_back();

	assert(0 == m_assertResults[m_assertIndex].nAsserts);
	// /Assert ID increment

	// Title
	if (!pUnitTest->GetTitle().empty())
		Print(Fmt::Format("{}. ", pUnitTest->GetTitle()));

	String resultMessage;

	// Result(s)
	if (testRange.m_first == testRange.m_last)
	{
		AssertResult assertResult = AssertSingle(pUnitTest, funcGetParameters, funcGetExpected, testRange.m_first, resultMessage);

		fPrintResult(m_assertIndex - 1, resultMessage, isSubAssert);

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

		for (size_t i = 0; i < testRange.m_numIterations; ++i)
		{
			size_t testIndex = testRange.m_first;

			if (0 < testRange.m_stepSize)
				testIndex += i * static_cast<size_t>(testRange.m_stepSize);
			else
				testIndex -= i * static_cast<size_t>(Maths::Abs(testRange.m_stepSize));

			fPrintAssertId(m_assertIndex - 1);

			AssertResult assertResult = AssertSingle(pUnitTest, funcGetParameters, funcGetExpected, testIndex, resultMessage);

			fPrintResult(m_assertIndex - 1, resultMessage, isSubAssert);

			m_assertResults[m_assertIndex - 1] += assertResult;

			// Assert ID complete...
			m_assertResults[m_assertIndex].Reset();
			// /Assert ID complete
		}

		--m_assertIndex;

		fPrintAssertId(m_assertIndex - 1);

		AssertResult & seriesResult = m_assertResults[m_assertIndex]; // Note : must come after the loop in case sub-asserts cause the vector to be reallocated.

		resultMessage = Fmt::Format("Series passed = {} / {}{}", seriesResult.nPasses, seriesResult.nAsserts,
			(seriesResult.IsPass() ? "" : Fmt::Format(" ({} failed)", seriesResult.nAsserts - seriesResult.nPasses)));

		Print(resultMessage);
		Print("\n");

		if (seriesResult.IsPass())
			result = RESULT_CODE_SUCCESS;
	}

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

	try
	{
		Result result = pUnitTest->Invoke(funcGetParameters(testIndex), returnValue);

		resultMessage = GetEvaluationString(funcGetParameters(testIndex), returnValue);

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
