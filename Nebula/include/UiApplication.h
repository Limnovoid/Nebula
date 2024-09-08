#ifndef NEBULA_ORION_UI_APPLICATION_H
#define NEBULA_ORION_UI_APPLICATION_H

#include "UiMenu.h"
#include "Test.h"

namespace Nebula
{

class UiApplication
{
public:
	UiApplication(StringView name);

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

#endif//NEBULA_ORION_UI_APPLICATION_H
