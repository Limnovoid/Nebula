#include "Stable.h"
#include "Nebula.h"
#include "Nova.h"
#include "Neutron.h"
#include "UiApplication.h"

#include "Exception.h"

int main(int argc, int** argv)
{
	using namespace Nebula;

	UiApplication::Settings appSettings;
	appSettings.m_applicationName = "Nebula";
	appSettings.m_testHandlerSettings.m_sharedLogFilePath = "NebulaTests.log";

	UiApplication uiApplication(appSettings);

	UiAddOptions(uiApplication.GetRootMenu());
	AddTests(uiApplication.GetTestHandler());
	Nova::AddTests(uiApplication.GetTestHandler());

	uiApplication.Run();
}
