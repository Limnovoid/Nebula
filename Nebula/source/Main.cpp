#include "Stable.h"

#include "Nebula.h"
#include "Orion.h"
#include "UiApplication.h"

int main(int argc, int** argv)
{
	using namespace Nebula;

	UiApplication uiApplication("Nebula");

	UiAddOptions(uiApplication.GetRootMenu());
	AddTests(uiApplication.GetTestHandler());

	uiApplication.Run();
}
