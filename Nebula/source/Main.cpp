#include "Stable.h"

#include "TestResult.h"
#include "Block.h"
#include "TextArt.h"
#include "UiApplication.h"
#include "Orion.h"
#include "Maths.h"

int main(int argc, int** argv)
{
	using namespace Nebula;

	//std::string message1 = "test message 1";
	//std::string message2 = "test message 2";
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

	//int res = ToType<int>('c');
	//unsigned res2 = ToType<unsigned>('c');

	//String str1000 = ToString<unsigned>(1000);
	//String strMax = ToString<unsigned>(std::numeric_limits<unsigned>::max());

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
		assert(myIntVector[50 + i] == i);

	//constexpr size_t maxDigitsSizeT = Maths::NumDigits(std::numeric_limits<double>::max());

	Orion::UiApplication uiApplication("Nebula");
	
	Orion::UiAddOptions(uiApplication.GetRootMenu());
	
	uiApplication.Run();
}
