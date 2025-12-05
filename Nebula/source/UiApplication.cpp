#include "UiApplication.h"

#include "TextArt.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

UiApplication::UiApplication(Settings const& settings) :
	m_rootMenu(settings.m_applicationName, true),
	m_io(std::cout, std::cin)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void UiApplication::Run()
{
	m_io << GetTextArt20();

	m_rootMenu.Execute(m_io);
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
