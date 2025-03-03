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
#include "Macros.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

class TestHandler
{
public:
	struct Settings
	{
		String	m_sharedLogFilePath;
	};

	template<IsInt TIndex = int64_t>
	class IndexRange
	{
	public:
		/// <returns> The sequence of indices defined by the given index range. </returns>
		static std::vector<TIndex> GetIndexSequence(IndexRange const& indexRange);

		/// <returns> The position of the given index in the sequence defined by this range of indices. </returns>
		static size_t GetIndexPosition(IndexRange const& indexRange, TIndex index);

		/// <param name="first">Index of first test iteration to execute.</param>
		/// <param name="last">Index of last test iteration to execute.</param>
		/// <param name="stepsize">Number by which index is incremented between executed test iterations. Use a negative for decrementing.</param>
		/// <exception cref="std::exception">Range is ill-defined.</exception>
		IndexRange(TIndex first = 0, TIndex last = 0, int stepsize = 1);

		/// <returns> The sequence of indices defined by the given index range. </returns>
		std::vector<TIndex> GetIndexSequence() const;

		/// <returns> The position of the given index in the sequence defined by this range of indices. </returns>
		size_t GetIndexPosition(TIndex index) const;

		TIndex const	m_first;
		TIndex const	m_last;
		int const		m_stepSize;
		size_t const	m_numIterations;

	private:
		size_t ComputeNumIterations() const;
	};

	template<typename TIndex = int64_t>
	struct FRangeIndex
	{
		FRangeIndex(TIndex constantOffset = 0) : m_offset(constantOffset) {}

		TIndex operator()(TIndex index)
		{
			return m_offset + index;
		}

		TIndex const		m_offset;
	};

	template<typename TIndex = int64_t, typename TReturn = TIndex>
	struct FRangeZero
	{
		TReturn operator()(TIndex) { return 0; }
	};

	template<typename TIndex = int64_t>
	struct FRangeConstant
	{
		FRangeConstant(TIndex constant) : m_constant(constant) {}

		TIndex operator()(TIndex) { return m_constant; }

		TIndex const		m_constant;
	};

	template<typename TIndex = int64_t>
	struct FRangeRandomOrder
	{
		/// <param name="indexRange"> The index range passed into TestHandler::Assert. </param>
		FRangeRandomOrder(IndexRange<TIndex> const& indexRange) :
			m_indexRange(indexRange),
			m_indexSequence(IndexRange<TIndex>::GetIndexSequence(indexRange))
		{
			Random::Shuffle(m_indexSequence);
		}

		TIndex operator()(TIndex index)
		{
			return m_indexSequence[IndexRange<TIndex>::GetIndexPosition(m_indexRange, index)];
		}

		IndexRange<TIndex> const		m_indexRange;
		std::vector<TIndex>			m_indexSequence;
	};

	enum OutputMode
	{
		SILENT,
		QUIET,
		VERBOSE
	};

	TestHandler(Settings settings);

	// Assert implementation.
	template<typename TReturn, typename TParameters, typename TFuncGetParameters, typename TFuncGetExpected, IsInt TIndex = int64_t>
		requires (IsInvocable<TFuncGetParameters, TIndex> && IsInvocable<TFuncGetExpected, TIndex>)
	Result Assert(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, TFuncGetParameters funcGetParameters,
		TFuncGetExpected funcGetExpected, IndexRange<TIndex> const& testRange = {});

	// Assert with IUnitTest and arrays.
	template<typename TReturn, typename TParameters, IsInt TIndex = int64_t>
	Result Assert(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, TParameters * pParameters, TReturn const* pExpected,
		IndexRange<TIndex> const& testRange = {});

	// Assert with IUnitTest and references.
	template<typename TReturn, typename TParameters, IsInt TIndex = int64_t>
	Result Assert(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, TParameters const& parameters, TReturn const& expected,
		IndexRange<TIndex> const& testRange = {});

	// Assert with function and functors (force title).
	template<typename TReturn, typename TParameters, typename TFuncGetParameters, typename TFuncGetExpected, IsInt TIndex = int64_t>
		requires (IsInvocable<TFuncGetParameters, TIndex> && IsInvocable<TFuncGetExpected, TIndex>)
	Result Assert(std::function<TReturn(TParameters)> const& unitTestFunc, TFuncGetParameters funcGetParameters,
		TFuncGetExpected funcGetExpected, StringView title, IndexRange<TIndex> const& testRange = {});

	// Assert with function and functors (no title).
	template<typename TReturn, typename TParameters, typename TFuncGetParameters, typename TFuncGetExpected, IsInt TIndex = int64_t>
		requires (IsInvocable<TFuncGetParameters, TIndex> && IsInvocable<TFuncGetExpected, TIndex>)
	Result Assert(std::function<TReturn(TParameters)> const& unitTestFunc, TFuncGetParameters funcGetParameters,
		TFuncGetExpected funcGetExpected, IndexRange<TIndex> const& testRange = {});

	// Assert with function and references (force title).
	template<typename TReturn, typename TParameters, IsInt TIndex = int64_t>
	Result Assert(std::function<TReturn(TParameters)> const& unitTestFunc, TParameters const& parameters, TReturn const& expected,
		StringView title, IndexRange<TIndex> const& testRange = {});

	// Assert with function and references (no title).
	template<typename TReturn, typename TParameters, IsInt TIndex= int64_t>
	Result Assert(std::function<TReturn(TParameters)> const& unitTestFunc, TParameters const& parameters, TReturn const& expected,
		IndexRange<TIndex> const& testRange = {});

	template<typename TReturn>
	Result Assert(TReturn const& value, TReturn const& expected, StringView title = "");

	/// <summary> Register a test script so it can be run from the test handler UI. </summary>
	/// <param name="pTestScript"> Pointer to the test script. </param>
	/// <param name="testGroupName"> Optional test group name - if specified, the test will be placed in a UI submenu of the same name. </param>
	Result Register(SharedPtr<ITestScript> pTestScript, StringView testGroupName = "");

	void Run(SharedPtr<ITestScript> pTestScript);

	SharedPtr<UiMenu> GetMenu();

	/// <summary> Set the output mode. </summary>
	/// <param name="outputMode"> The mode to be used after this function returns. </param>
	/// <returns> The mode being used at the time this function is called. </returns>
	OutputMode SetOutputMode(OutputMode outputMode);


	/// TestHandler UI options...
	void UiSetOutputMode(UiIo const& uiIo);

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

	template<typename TReturn, typename TParameters, typename TFuncGetParameters, typename TFuncGetExpected, IsInt TIndex>
		requires (IsInvocable<TFuncGetParameters, TIndex> && IsInvocable<TFuncGetExpected, TIndex>)
	AssertResult AssertSingle(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, TFuncGetParameters funcGetParameters,
		TFuncGetExpected funcGetExpected, TIndex testIndex, String & resultMessage);

	void Print(StringView message);

	template<typename TReturn, typename TParameters, IsInt TIndex>
	void OutputPreamble(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, IndexRange<TIndex> const& testRange);

	template<typename TReturn, typename TParameters>
	void OutputFailed(size_t const iteration, TParameters const& parameters, TReturn const& expectedValue, TReturn const& computedValue, Result const& result, bool isSeries = false);

	template<typename TReturn, typename TParameters>
	void OutputFailedWithException(size_t const iteration, TParameters const& parameters, TReturn const& expectedValue, StringView exceptionString, bool isSeries = false);

	template<typename TReturn, typename TParameters, IsInt TIndex>
	void OutputPassed(size_t const iteration, TParameters const& parameters, TReturn const& expectedValue, bool isSeries = false);

	template<IsInt TIndex>
	void OutputSummary(size_t const nPassed, IndexRange<TIndex> const& testRange);

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

	OutputMode					m_outputMode;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters, typename TFuncGetParameters, typename TFuncGetExpected, IsInt TIndex>
	requires (IsInvocable<TFuncGetParameters, TIndex> && IsInvocable<TFuncGetExpected, TIndex>)
inline Result TestHandler::Assert(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, TFuncGetParameters funcGetParameters,
	TFuncGetExpected funcGetExpected, IndexRange<TIndex> const& testRange)
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

		auto indexSequence = testRange.GetIndexSequence();
		for (TIndex index : indexSequence)
		{
			resultMessage.clear();

			AssertResult assertResult = AssertSingle(pUnitTest, funcGetParameters, funcGetExpected, index, resultMessage);

			switch (m_outputMode)
			{
			case (OutputMode::SILENT): break;
			case (OutputMode::QUIET):
				if (assertResult.IsPass())
					break;
				// Fall through.
			case (OutputMode::VERBOSE):
				fPrintAssertId(m_assertIndex - 1);
				fPrintResult(m_assertIndex - 1, resultMessage, isSubAssert);

				break;
			}

			m_assertResults[m_assertIndex - 1] += assertResult;

			// Assert ID complete...
			m_assertResults[m_assertIndex].Reset();
			// /Assert ID complete
		}

		--m_assertIndex;

		fPrintAssertId(m_assertIndex - 1);

		AssertResult & seriesResult = m_assertResults[m_assertIndex]; // Note : must come after the loop in case sub-asserts cause the vector to be reallocated.

		Print(Fmt::Format("Series passed = {} / {}{}", seriesResult.nPasses, seriesResult.nAsserts,
			(seriesResult.IsPass() ? "" : Fmt::Format(" ({} failed)", seriesResult.nAsserts - seriesResult.nPasses))));

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

template<typename TReturn, typename TParameters, IsInt TIndex>
inline Result TestHandler::Assert(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, TParameters * pParameters,
	TReturn const* pExpected, IndexRange<TIndex> const& testRange)
{
	struct FuncGetParameters
	{
		TParameters const& operator()(TIndex index)
		{
			return m_pParameters[index];
		}

		TParameters *	m_pParameters;
	};

	struct FuncGetExpected
	{
		TReturn const& operator()(TIndex index)
		{
			return m_pExpected[index];
		}

		TReturn const*	m_pExpected;
	};

	return Assert(pUnitTest, FuncGetParameters(pParameters), FuncGetExpected(pExpected), testRange);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters, IsInt TIndex>
inline Result TestHandler::Assert(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, TParameters const& parameters,
	TReturn const& expected, IndexRange<TIndex> const& testRange)
{
	struct FuncGetParameters
	{
		TParameters const& operator()(TIndex index)
		{
			return m_parameters;
		}

		TParameters const&	m_parameters;
	};

	struct FuncGetExpected
	{
		TReturn const& operator()(TIndex index)
		{
			return m_expected;
		}

		TReturn const&	m_expected;
	};

	return Assert(pUnitTest, FuncGetParameters(parameters), FuncGetExpected(expected), testRange);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters, typename TFuncGetParameters, typename TFuncGetExpected, IsInt TIndex>
	requires (IsInvocable<TFuncGetParameters, TIndex> && IsInvocable<TFuncGetExpected, TIndex>)
inline Result TestHandler::Assert(std::function<TReturn(TParameters)> const& unitTestFunc, TFuncGetParameters funcGetParameters,
	TFuncGetExpected funcGetExpected, StringView title, IndexRange<TIndex> const& testRange)
{
	using UnitTest = UnitTest<TReturn, TParameters>;
	using IUnitTest = IUnitTest<TReturn, TParameters>;

	SharedPtr<UnitTest> pUnitTest = MakeShared<UnitTest>(unitTestFunc, title);

	return Assert(StaticPtrCast<IUnitTest>(pUnitTest), funcGetParameters, funcGetExpected, testRange);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters, typename TFuncGetParameters, typename TFuncGetExpected, IsInt TIndex>
	requires (IsInvocable<TFuncGetParameters, TIndex> && IsInvocable<TFuncGetExpected, TIndex>)
inline Result TestHandler::Assert(std::function<TReturn(TParameters)> const& unitTestFunc, TFuncGetParameters funcGetParameters,
	TFuncGetExpected funcGetExpected, IndexRange<TIndex> const& testRange)
{
	return Assert(unitTestFunc, funcGetParameters, funcGetExpected, "", testRange);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters, IsInt TIndex>
inline Result TestHandler::Assert(std::function<TReturn(TParameters)> const& unitTestFunc, TParameters const& parameters,
	TReturn const& expected, StringView title, IndexRange<TIndex> const& testRange)
{
	using UnitTest = UnitTest<TReturn, TParameters>;
	using IUnitTest = IUnitTest<TReturn, TParameters>;

	SharedPtr<UnitTest> pUnitTest = MakeShared<UnitTest>(unitTestFunc, title);

	return Assert(StaticPtrCast<IUnitTest>(pUnitTest), parameters, expected, testRange);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters, IsInt TIndex>
inline Result TestHandler::Assert(std::function<TReturn(TParameters)> const& unitTestFunc, TParameters const& parameters,
	TReturn const& expected, IndexRange<TIndex> const& testRange)
{
	return Assert(unitTestFunc, parameters, expected, "", testRange);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn>
inline Result TestHandler::Assert(TReturn const& value, TReturn const& expected, StringView title)
{
	using TParameters = int; // Placeholder, parameters are ignored.

	auto fReturnValue = [&value](TParameters) { return value; };

	return Assert<TReturn, TParameters>(fReturnValue, 0, expected, title);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline SharedPtr<UiMenu> TestHandler::GetMenu()
{
	return m_pMenu;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline TestHandler::OutputMode TestHandler::SetOutputMode(OutputMode outputMode)
{
	OutputMode const currentMode = m_outputMode;

	m_outputMode = outputMode;

	return currentMode;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters, typename TFuncGetParameters, typename TFuncGetExpected, IsInt TIndex>
	requires (IsInvocable<TFuncGetParameters, TIndex> && IsInvocable<TFuncGetExpected, TIndex>)
inline TestHandler::AssertResult TestHandler::AssertSingle(SharedPtr<IUnitTest<TReturn, TParameters>> pUnitTest, TFuncGetParameters funcGetParameters,
	TFuncGetExpected funcGetExpected, TIndex testIndex, String & resultMessage)
{
	AssertResult assertResult = AssertResult::SingleFail();

	try
	{
		TParameters const& parameters = funcGetParameters(testIndex);
		TReturn const& expected = funcGetExpected(testIndex);

		TReturn returnValue(expected);

		Result result = pUnitTest->Invoke(parameters, returnValue);

		resultMessage += GetEvaluationString(parameters, returnValue);

		if ((RESULT_CODE_SUCCESS == result) && (expected == returnValue))
		{
			assertResult = AssertResult::SinglePass();
			resultMessage += " - Pass";
		}
		else
		{
			resultMessage += Fmt::Format(" - FAIL with code {} - expected {}", result, expected);
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

template<IsInt TIndex>
inline TestHandler::IndexRange<TIndex>::IndexRange(TIndex start, TIndex end, int stepsize) :
	m_first(start),
	m_last(end),
	m_stepSize(stepsize),
	m_numIterations(ComputeNumIterations())
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<IsInt TIndex>
inline size_t TestHandler::IndexRange<TIndex>::ComputeNumIterations() const
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

template<IsInt TIndex>
inline std::vector<TIndex> TestHandler::IndexRange<TIndex>::GetIndexSequence(IndexRange const& indexRange)
{
	std::vector<TIndex> sequence(indexRange.m_numIterations);

	size_t sequenceIndex = 0;

	if (0 < indexRange.m_stepSize)
	{
		for (TIndex i = indexRange.m_first; i <= indexRange.m_last; i += static_cast<TIndex>(indexRange.m_stepSize))
			sequence[sequenceIndex++] = i;
	}
	else
	{
		for (TIndex i = indexRange.m_first; indexRange.m_last <= i; i -= static_cast<TIndex>(Maths::Abs(indexRange.m_stepSize)))
			sequence[sequenceIndex++] = i;
	}

	return sequence;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<IsInt TIndex>
inline size_t TestHandler::IndexRange<TIndex>::GetIndexPosition(IndexRange const& indexRange, TIndex index)
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

template<IsInt TIndex>
std::vector<TIndex> TestHandler::IndexRange<TIndex>::GetIndexSequence() const
{
	return GetIndexSequence(*this);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<IsInt TIndex>
size_t TestHandler::IndexRange<TIndex>::GetIndexPosition(TIndex index) const
{
	return GetIndexPosition(*this, index);
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
