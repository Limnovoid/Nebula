#include "Nebula.h"

#include "NebulaTypes.h"
#include "ToType.h"
#include "ToString.h"
#include "Format.h"
#include "UiMenu.h"
#include "TextArt.h"

namespace Nebula
{

//template<typename TReturn, typename... TParams>
//class UnitTest
//{
//public:
//	struct Iteration
//	{
//		Iteration(size_t index) : m_index(index) {}
//
//		Iteration & operator+=(Iteration rhs)
//		{
//			m_index += rhs.m_index;
//			return *this;
//		}
//
//		bool operator!=(Iteration rhs) const
//		{
//			return (m_index != rhs.m_index);
//		}
//
//		size_t		m_index;
//	};
//
//	struct Parameters
//	{
//		Iteration	m_first;
//		Iteration	m_last;
//		Iteration	m_step;
//
//		TReturn *	m_expectedResults;
//
//	};
//
//	UnitTest() {}
//
//	bool Run(TReturn expectedResult, TParams... parameters)
//	{
//		return (Invoke(std::forward<TParams>(parameters)...) == expectedResult);
//	}
//
//	bool RunSeries(Iteration first, Iteration last, Iteration step, TReturn * expectedResults, TParams... parameters)
//	{
//		bool overallResult = true;
//		Iteration iteration = first;
//		do
//		{
//			TReturn unitTestReturnValue = InvokeIteration(iteration, std::forward<TParams>(parameters)...);
//
//			if (unitTestReturnValue != expectedResults[iteration.m_index])
//			{
//				std::cout << Fmt::Format("Failed iteration {}: unit returned {}, expected {}",
//					iteration.m_index, unitTestReturnValue, expectedResults[iteration.m_index]) << std::endl;
//
//				overallResult = false;
//			}
//
//			iteration += step;
//		}
//		while (iteration != last);
//
//		return overallResult;
//	}
//
//protected:
//	virtual TReturn Invoke(TParams...) { return TReturn(); }
//	virtual TReturn InvokeIteration(Iteration, TParams...) { return TReturn(); }
//};
//
//class UnitTestHelper
//{
//public:
//	template<typename TFunc, typename TReturn, typename TParameters>
//	static Result Run(TFunc functor, TReturn const expectedReturn, TParameters const& parameters)
//	{
//		bool isExpectedReturn = (expectedReturn == functor(parameters);
//
//		return isExpectedReturn ? RESULT_CODE_SUCCESS : RESULT_CODE_FAILURE;
//	}
//};
//
//template<typename TFunc, typename TReturn, typename... TArgs>
//class UiUnitTest : public IOption, UnitTestHelper
//{
//public:
//	UiUnitTest(char const* prompt) :
//		m_prompt(prompt)
//	{
//	}
//
//	virtual StringView GetPrompt()
//	{
//		return MakeStringView(m_prompt);
//	}
//
//	virtual void Execute(UiIo const& uiIo)
//	{
//		TReturn expected;
//		uiIo.Get("Expected return value", expected);
//
//		// args????
//
//		UnitTestHelper::Run(TFunc(), expected, )
//	}
//
//private:
//	String		m_prompt;
//};

template<typename TReturn, class TParameters>
class IUnitTest
{
public:
	struct Iteration
	{
		Iteration(size_t index) : m_index(index) {}

		Iteration & operator+=(Iteration rhs)
		{
			m_index += rhs.m_index;
			return *this;
		}

		bool operator!=(Iteration rhs) const
		{
			return (m_index != rhs.m_index);
		}

		size_t		m_index;
	};

	virtual void Initialize(Iteration const iteration, TParameters & parameters) = 0;
	virtual Result Execute(Iteration const iteration, TParameters const& parameters, TReturn & returned) = 0;
	virtual Result Compare(Iteration const iteration, TReturn const& returned) = 0;
};

template<typename TFunc>
concept IsTestUnit = requires (TFunc & func)
{
	func();
};

class TestHandler
{
public:
	template<typename TFunc, typename TParams, typename TReturn>// requires IsTestUnit<TFunc>
	bool Assert(TFunc & func, TParams const& parameters, TReturn const& expectedReturn)
	{
		return (func(parameters) == expectedReturn);
	}
};

template<typename TTo> requires IsInt<TTo>
class UnitTestToType : public IUnitTest<TTo, char>
{
	using Iteration = IUnitTest<TTo, char>::Iteration;
	using TParameters = char;
	using TReturn = TTo;

	UnitTestToType() {}

	virtual void Initialize(Iteration const iteration, TParameters & parameters) override
	{
		parameters += static_cast<uint8_t>(iteration.m_index);
	}

	virtual Result Execute(Iteration const iteration, TParameters const& parameters, TReturn & returned) override
	{
		return ToType<int>(parameters, returned);
	}

	virtual Result Compare(Iteration const iteration, TReturn const& returned) override
	{

	}
};

void UiTestTypeConversion(UiIo const& uiIo)
{
	Result result;

	int myInt;
	result = ToType<int>('9', myInt);

	struct ToTypeIntFunctor
	{
		int operator()(char c)
		{
			int convertedInt;
			Result result = ToType<int>(c, convertedInt);
			return convertedInt;
		}
	};


	uiIo << "Test: ToType<unsigned>(char)" << '\n';

	static const unsigned TEST_RANGE_START = 0, TEST_RANGE_END = 100, TEST_RANGE_STEP = 1;

	uiIo << Fmt::Format("Iterations: {} - {} (step {})", TEST_RANGE_START, TEST_RANGE_END, TEST_RANGE_STEP) << '\n';

	bool allPassed = true;
	for (unsigned i = TEST_RANGE_START; i < TEST_RANGE_END; i += TEST_RANGE_STEP)
	{
		unsigned returnValue;
		Result result = ToType<unsigned>(static_cast<char>(static_cast<unsigned>('0') + i), returnValue);

		if ((RESULT_CODE_SUCCESS != result) || (i != returnValue))
		{
			allPassed = false;
			uiIo << Fmt::Format("Failed iteration {}: expected {}, returned {}", i, i, returnValue) << '\n';
		}
	}

	uiIo << "Result: " << (allPassed ? "pass" : "fail") << '\n';

	//class ToTypeUnitTest : public UnitTest<int, char>
	//{
	//public:
	//	ToTypeUnitTest() {}

	//	virtual int Invoke(char c) override
	//	{
	//		int convertedInt;
	//		Result result = ToType<int>(c, convertedInt);
	//		return convertedInt;
	//	}

	//	virtual int InvokeIteration(Iteration i, char c) override
	//	{
	//		c += static_cast<uint8_t>(i.m_index);

	//		int convertedInt;
	//		Result result = ToType<int>(c, convertedInt);
	//		return convertedInt;
	//	}
	//};

	//ToTypeUnitTest test;
	//test.Run(9, '9');

	//int expectedResults[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	//test.RunSeries(0, 9, 1, expectedResults, '0');

	//// -------------------------

	//struct ToTypeIntFunctor
	//{
	//	int operator()(char c)
	//	{
	//		int convertedInt;
	//		Result result = ToType<int>(c, convertedInt);
	//		return convertedInt;
	//	}
	//};

	//struct ToTypeIntFunctor2
	//{
	//	std::pair<Result, int> operator()(char c)
	//	{
	//		int convertedInt;
	//		Result result = ToType<int>(c, convertedInt);
	//		return { result, convertedInt };
	//	}
	//};

	//uiIo << "ToType<int>(char):" << ToString(UnitTestHelper::Run(ToTypeIntFunctor(), int(9), '9')) << '\n';
	//uiIo << "ToType<int>(char) 2:" << ToString(UnitTestHelper::Run(ToTypeIntFunctor2(), std::pair<Result, int>(RESULT_CODE_SUCCESS, 9), '9')) << '\n';

	/*unsigned res2 = ToType<unsigned>('9');
	String str1000 = ToString<unsigned>(1000);
	String strMax = ToString<unsigned>(std::numeric_limits<unsigned>::max());

	std::vector<unsigned> myUIntVector;
	for (unsigned i = 0; i < 100; ++i)
	{
		String myIntegerString = ToString<unsigned>(i);
		myUIntVector.push_back(ToType<unsigned>(myIntegerString));
	}

	for (unsigned i = 0; i < 100; ++i)
		assert(myUIntVector[i] == i);

	std::vector<int> myIntVector;
	for (int i = -50; i < 50; ++i)
	{
		String myIntegerString = ToString<int>(i);
		myIntVector.push_back(ToType<int>(myIntegerString));
	}

	for (int i = -50; i < 50; ++i)
		assert(myIntVector[50 + i] == i);*/
}

// ---------------------------------------------------------------------------------------------------------------------------------

void TestFormat()
{
	String formattedString = Fmt::Format("{}", 0);
	std::cerr << formattedString << std::endl;
}

// --------------------------------------------------------------------------------------------------------------------------------

void UiAddTextArtMenu(UiMenu & uiMenu)
{
	SharedPtr<UiMenu> pTextArtMenu = MakeShared<UiMenu>("Text art");

	pTextArtMenu->AddOption("Display text art 1", [](UiIo const& uiIo) { uiIo << '\n' << GetTextArt(); });
	pTextArtMenu->AddOption("Display text art 2", [](UiIo const& uiIo) { uiIo << '\n' << GetTextArt2(); });

	uiMenu.AddOption(pTextArtMenu);
}

// --------------------------------------------------------------------------------------------------------------------------------

void UiAddOptions(UiMenu & uiMenu)
{
	//String message1 = "test message 1";
	//String message2 = "test message 2";
	//
	//Orion::TestResult testResult1(false, message1);
	//Orion::TestResult testResult2(false, std::move(message2));
	//
	//assert(message2.empty());
	//
	//testResult1.SetMessage(message1);
	//testResult2.SetMessage(std::move(message1));
	//
	//assert(message1.empty());

	UiAddTextArtMenu(uiMenu);

	SharedPtr<UiMenu> pTestMenu = MakeShared<UiMenu>("Nebula tests");

	pTestMenu->AddOption("Type conversion", UiTestTypeConversion);
	pTestMenu->AddOption("Format", [](UiIo const&) { TestFormat(); });

	uiMenu.AddOption(pTestMenu);
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
