#include "UiApplication.h"

#include "TextArt.h"

namespace Nebula
{
	
UiApplication::UiApplication(char const* name) :
	m_rootMenu(name, true),
	m_io(std::cout, std::cin)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void UiApplication::Run()
{
	m_io << GetTextArt13();

	m_rootMenu.Execute(m_io);
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
