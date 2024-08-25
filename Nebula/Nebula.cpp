#include "Nebula.h"

#include "NebulaTypes.h"
#include "ToType.h"
#include "ToString.h"
#include "Format.h"
#include "UiMenu.h"
#include "TextArt.h"

namespace Nebula
{

template<typename TReturn, typename... TParams>
class UnitTest
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

		size_t	m_index;
	};

	UnitTest() {}

	bool Run(TReturn expectedResult, TParams... parameters)
	{
		return (Invoke(std::forward<TParams>(parameters)...) == expectedResult);
	}

	bool RunSeries(Iteration first, Iteration last, Iteration step, TReturn * expectedResults, TParams... parameters)
	{
		bool overallResult = true;
		Iteration iteration = first;
		do
		{
			TReturn unitTestReturnValue = InvokeIteration(iteration, std::forward<TParams>(parameters)...);

			if (unitTestReturnValue != expectedResults[iteration.m_index])
			{
				std::cout << Fmt::Format("Failed iteration {}: unit returned {}, expected {}",
					iteration.m_index, unitTestReturnValue, expectedResults[iteration.m_index]) << std::endl;

				overallResult = false;
			}

			iteration += step;
		}
		while (iteration != last);

		return overallResult;
	}

protected:
	virtual TReturn Invoke(TParams...) { return TReturn(); }
	virtual TReturn InvokeIteration(Iteration, TParams...) { return TReturn(); }
};

void TestTypeConversion()
{
	Result result;

	int myInt;
	result = ToType<int>('9', myInt);

	class ToTypeUnitTest : public UnitTest<int, char>
	{
	public:
		ToTypeUnitTest() {}

		virtual int Invoke(char c) override
		{
			int convertedInt;
			Result result = ToType<int>(c, convertedInt);
			return convertedInt;
		}

		virtual int InvokeIteration(Iteration i, char c) override
		{
			c += static_cast<uint8_t>(i.m_index);

			int convertedInt;
			Result result = ToType<int>(c, convertedInt);
			return convertedInt;
		}
	};

	ToTypeUnitTest test;

	test.Run(9, '9');

	int expectedResults[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	test.RunSeries(0, 9, 1, expectedResults, '0');

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

	pTestMenu->AddOption("Type conversion", [](UiIo const&) { TestTypeConversion(); });
	pTestMenu->AddOption("Format", [](UiIo const&) { TestFormat(); });

	uiMenu.AddOption(pTestMenu);
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
