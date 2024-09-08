#include "Nebula.h"

#include "NebulaTypes.h"
#include "ToType.h"
#include "ToString.h"
#include "Format.h"
#include "UiMenu.h"
#include "TextArt.h"
#include "Test.h"
#include "GetTypenameHelper.h"

namespace Nebula
{

template<typename TTo> requires IsInt<TTo>
class UnitTestCharToType : public IUnitTest<TTo, char>
{
public:
	using IUnitTest = IUnitTest<TTo, char>;
	using TParameters = char;
	using TReturn = TTo;

	struct GetParameters
	{
		TParameters operator()(size_t index)
		{
			assert(index < 10);
			return '0' + static_cast<char>(index);
		}
	};

	struct GetExpected
	{
		TReturn operator()(size_t index)
		{
			assert(index < 10);
			return static_cast<TReturn>(index);
		}
	};

	UnitTestCharToType(StringView title) : IUnitTest(title) {}

	virtual ~UnitTestCharToType() {}

	virtual Result Invoke(TParameters const& parameters, TReturn & returned) override
	{
		return ToType<TTo>(parameters, returned);
	}
};

// ---------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------

template<typename TTo> requires IsInt<TTo>
class TestProgramCharToType : public ITestProgram
{
public:

	TestProgramCharToType() : ITestProgram(Fmt::Format("ToType<{}>(char)", GetTypenameHelper<TTo>::Get())) {}

	virtual ~TestProgramCharToType() {}

	virtual void Run(TestHandler & testHandler) override
	{
		String const unitTestTitle;

		SharedPtr<UnitTestCharToType<TTo>> pUnitTestCharToType = MakeShared<UnitTestCharToType<TTo>>(unitTestTitle);

		testHandler.Assert<TTo, char>(pUnitTestCharToType, '7', 8);

		char parameters[] = { '0', '5', '2', '3', '4', '1', '6', '7', '9', '8' };
		const TTo expectedValues[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		testHandler.Assert<TTo, char>(pUnitTestCharToType, parameters, expectedValues, { 0, 9 });

		char ascii0 = '0';
		TTo zero = 0;
		testHandler.Assert<TTo, char>(pUnitTestCharToType, &ascii0, &zero);

		testHandler.Assert<TTo, char>(pUnitTestCharToType, UnitTestCharToType<TTo>::GetParameters(), UnitTestCharToType<TTo>::GetExpected(), { 0, 9 });
	}
};

// ---------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------

void UiAddTextArtMenu(UiMenu & uiMenu)
{
	SharedPtr<UiMenu> pTextArtMenu = MakeShared<UiMenu>("Text art");

	pTextArtMenu->AddOption("Display text art 1", [](UiIo const& uiIo) { uiIo << '\n' << GetTextArt(); });
	pTextArtMenu->AddOption("Display text art 2", [](UiIo const& uiIo) { uiIo << '\n' << GetTextArt2(); });
	pTextArtMenu->AddOption("Display text art 3", [](UiIo const& uiIo) { uiIo << '\n' << GetTextArt3(); });
	pTextArtMenu->AddOption("Display text art 4", [](UiIo const& uiIo) { uiIo << '\n' << GetTextArt4(); });
	pTextArtMenu->AddOption("Display text art 5", [](UiIo const& uiIo) { uiIo << '\n' << GetTextArt5(); });
	pTextArtMenu->AddOption("Display text art 6", [](UiIo const& uiIo) { uiIo << '\n' << GetTextArt6(); });
	pTextArtMenu->AddOption("Display text art 7", [](UiIo const& uiIo) { uiIo << '\n' << GetTextArt7(); });
	pTextArtMenu->AddOption("Display text art 8", [](UiIo const& uiIo) { uiIo << '\n' << GetTextArt8(); });
	pTextArtMenu->AddOption("Display text art 9", [](UiIo const& uiIo) { uiIo << '\n' << GetTextArt9(); });
	pTextArtMenu->AddOption("Display text art 10", [](UiIo const& uiIo) { uiIo << '\n' << GetTextArt10(); });
	pTextArtMenu->AddOption("Display text art 11", [](UiIo const& uiIo) { uiIo << '\n' << GetTextArt11(); });
	pTextArtMenu->AddOption("Display text art 12", [](UiIo const& uiIo) { uiIo << '\n' << GetTextArt12(); });
	pTextArtMenu->AddOption("Display text art 13", [](UiIo const& uiIo) { uiIo << '\n' << GetTextArt13(); });
	pTextArtMenu->AddOption("Display text art 14", [](UiIo const& uiIo) { uiIo << '\n' << GetTextArt14(); });
	pTextArtMenu->AddOption("Display text art 15", [](UiIo const& uiIo) { uiIo << '\n' << GetTextArt15(); });
	pTextArtMenu->AddOption("Display text art 16", [](UiIo const& uiIo) { uiIo << '\n' << GetTextArt16(); });
	pTextArtMenu->AddOption("Display text art 17", [](UiIo const& uiIo) { uiIo << '\n' << GetTextArt17(); });

	uiMenu.AddOption(pTextArtMenu);
}

// --------------------------------------------------------------------------------------------------------------------------------

void UiAddOptions(UiMenu & uiMenu)
{
	UiAddTextArtMenu(uiMenu);
}

// --------------------------------------------------------------------------------------------------------------------------------

void AddTests(TestHandler & testHandler)
{
	SharedPtr<TestProgramCharToType<int>> pTestProgramCharToInt = MakeShared<TestProgramCharToType<int>>();

	testHandler.Register(pTestProgramCharToInt);

	//const int dummyExpectedValue = 0;
	//testHandler.Assert<int, char>(pUnitTestCharToInt, &ascii0, &dummyExpectedValue,
	//	std::numeric_limits<int>::min(), std::numeric_limits<int>::max());

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

	//String formattedString = Fmt::Format("{}", 0);
	//std::cerr << formattedString << std::endl;
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
