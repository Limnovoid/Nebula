#include "Stable.h"
#include "Nebula.h"
#include "Nova.h"
#include "Neutron.h"

int main(int argc, int** argv)
{
	using namespace Nebula;

	UiApplication::Settings appSettings;
	appSettings.m_applicationName = "Nebula";

	UiApplication uiApplication(appSettings);

	//UiAddOptions(uiApplication.GetRootMenu());
	//AddTests(uiApplication.GetTestHandler());
	//Nova::AddTests(uiApplication.GetTestHandler());
	//Neutron::AddTests(uiApplication.GetTestHandler());

	uiApplication.GetRootMenu().AddOption(TestRegister::GetMenu());

	uiApplication.Run();
}
