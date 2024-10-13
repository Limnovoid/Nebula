#include "UiApplication.h"

#include "TextArt.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{
	
UiApplication::UiApplication(Settings const& settings) :
	m_rootMenu(settings.m_applicationName, true),
	m_io(std::cout, std::cin),
	m_testHandler(settings.m_testHandlerSettings)
{
	m_rootMenu.AddOption(m_testHandler.GetMenu());
}

// --------------------------------------------------------------------------------------------------------------------------------

void UiApplication::Run()
{
	m_io << GetTextArt18();

	m_rootMenu.Execute(m_io);
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
