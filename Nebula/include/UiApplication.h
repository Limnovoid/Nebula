#ifndef NEBULA_UI_APPLICATION_H
#define NEBULA_UI_APPLICATION_H

#include "UiMenu.h"
#include "TestHandler.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

class UiApplication
{
public:
	struct Settings
	{
		String					m_applicationName;
		TestHandler::Settings	m_testHandlerSettings;
	};

	UiApplication(Settings const& settings);

	void Run();

	UiMenu & GetRootMenu();

	TestHandler & GetTestHandler();

private:
	UiMenu					m_rootMenu;
	UiIo					m_io;
	TestHandler				m_testHandler;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline UiMenu & UiApplication::GetRootMenu()
{
	return m_rootMenu;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline TestHandler & UiApplication::GetTestHandler()
{
	return m_testHandler;
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_UI_APPLICATION_H
