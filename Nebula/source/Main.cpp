#include "Stable.h"

#include "TestResult.h"
#include "Block.h"
#include "TextArt.h"
#include "UiApplication.h"
#include "Orion.h"

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

	Orion::UiApplication uiApplication("Nebula");

	Orion::UiAddOptions(uiApplication.GetRootMenu());

	uiApplication.Run();
}
