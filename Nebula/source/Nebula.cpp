#include "Nebula.h"

// Include all project headers for library building.
#include "Bitset.h"
#include "ConstString.h"
#include "Exception.h"
#include "File.h"
#include "Format.h"
#include "GetTypenameHelper.h"
#include "IOption.h"
#include "ITestProgram.h"
#include "IUnitTest.h"
#include "Macros.h"
#include "Maths.h"
#include "Metadata.h"
#include "NebulaString.h"
#include "NebulaTypes.h"
#include "Random.h"
#include "Result.h"
#include "Stable.h"
#include "TestHandler.h"
#include "TextArt.h"
#include "ToString.h"
#include "ToType.h"
#include "Typename.h"
#include "UiApplication.h"
#include "UiIo.h"
#include "UiMenu.h"
#include "UiOption.h"
#include "UnitTest.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

template<typename TTo> requires IsInt<TTo>
class TestProgramCharToType : public ITestProgram
{
public:
	TestProgramCharToType() : ITestProgram(Fmt::Format("ToType<{}>(char)", GetTypenameHelper<TTo>::Get())) {}

	virtual ~TestProgramCharToType() {}

protected:
	virtual void RunImpl(TestHandler & testHandler) override
	{
		SharedPtr<UnitTestCharToType<TTo>> pUnitTestCharToType = MakeShared<UnitTestCharToType<TTo>>();

		testHandler.Assert<TTo, char>(pUnitTestCharToType, '7', 8);

		char parameters[] = { '0', '5', '2', '3', '4', '1', '6', '7', '9', '8' };
		const TTo expectedValues[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		testHandler.Assert<TTo, char>(pUnitTestCharToType, parameters, expectedValues, { 0, 9 });

		char ascii0 = '0';
		TTo zero = 0;
		testHandler.Assert<TTo, char>(pUnitTestCharToType, &ascii0, &zero);

		testHandler.Assert<TTo, char>(pUnitTestCharToType, UnitTestCharToType<TTo>::GetParameters(), UnitTestCharToType<TTo>::GetExpected(), { 0, 9 });

		// TODO
		// asserts with lambda and function pointer ?
		// some way for TestHandler to extract the function name for logging ?
	}
};

// ---------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------

class TestProgramExceptions : public ITestProgram
{
public:
	TestProgramExceptions() : ITestProgram("Exceptions") {}

	virtual ~TestProgramExceptions() {}

protected:
	virtual void RunImpl(TestHandler & testHandler) override
	{
		try
		{
			throw Exception(RESULT_CODE_SUCCESS, "This is an Exception test.");
		}
		catch (Exception const& e)
		{
			std::cerr << e.ToString() << std::endl;
		}

		try
		{
			throw AssertionException();
		}
		catch (AssertionException const& e)
		{
			std::cerr << e.ToString() << std::endl;
		}
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
	pTextArtMenu->AddOption("Display text art 18", [](UiIo const& uiIo) { uiIo << '\n' << GetTextArt18(); });

	uiMenu.AddOption(pTextArtMenu);
}

// --------------------------------------------------------------------------------------------------------------------------------

void UiAddOptions(UiMenu & uiMenu)
{
	UiAddTextArtMenu(uiMenu);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TArg, typename TRet>
class TestVoidArg
{
public:
	using Func = std::function<TRet(TArg)>;

	Func	m_func;
};

template<typename T> requires IsFormattable<T>
String GetFormatted(T const& t)
{
	return Fmt::Format("Formatted = {}", t);
}

template<typename T>
String GetFormatted(T const& t)
{
	return Fmt::Format("Formatted = N/A");
}

void AddTests(TestHandler & testHandler)
{
	testHandler.Register(MakeShared<TestProgramCharToType<int>>());
	testHandler.Register(MakeShared<TestProgramExceptions>());

	constexpr ConstString constString = "cstr1";
	std::cout << constString << std::endl;

	constexpr char c1 = constString[0];
	constexpr char c2 = constString[1];
	std::cout << "c1 = " << c1 << std::endl;
	std::cout << "c2 = " << c2 << std::endl;

	std::cout << "int -> " << Typename<int>::Get() << std::endl;
	std::cout << "float -> " << Typename<float>::Get() << std::endl;
	std::cout << "double -> " << Typename<double>::Get() << std::endl;
	std::cout << "long double -> " << Typename<long double>::Get() << std::endl;

	constexpr ConstString constCat1 = ConstCat("str1", " + ", "str2");
	std::cout << "constCat1 = \"" << constCat1 << '"' << std::endl;

	constexpr ConstString constString1 = ConstString("str1") + ConstString(" + ") + ConstString("str2");
	constexpr ConstString constString2 = ConstString("str1") + " + " + "str2";
	constexpr ConstString constString3 = "str1" + ConstString(" + ") + "str2";

	std::cout << "CS + CS = \"" << constString1 << '"' << std::endl;
	std::cout << "CS + SL = \"" << constString2 << '"' << std::endl;
	std::cout << "SL + CS = \"" << constString3 << '"' << std::endl;

	//std::cout << '"' << ConstCat("literal", "+", ConstString("constString")) << '"' << std::endl;

	std::cout << std::endl;

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
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
