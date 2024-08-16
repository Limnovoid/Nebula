#ifndef NEBULA_ORION_UI_APPLICATION_H
#define NEBULA_ORION_UI_APPLICATION_H

#include "UiMenu.h"

namespace Nebula
{

namespace Orion
{

class UiApplication
{
public:
	UiApplication(char const* name);

	void Run();

	UiMenu & GetRootMenu();

private:
	UiMenu		m_rootMenu;
	UiIo		m_io;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline UiMenu & UiApplication::GetRootMenu()
{
	return m_rootMenu;
}

} // namespace Orion --------------------------------------------------------------------------------------------------------------

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_ORION_UI_APPLICATION_H
