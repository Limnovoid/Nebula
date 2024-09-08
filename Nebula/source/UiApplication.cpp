#include "UiApplication.h"

#include "TextArt.h"

namespace Nebula
{
	
UiApplication::UiApplication(StringView name) :
	m_rootMenu(name, true),
	m_io(std::cout, std::cin),
	m_testHandler(m_io)
{
	m_rootMenu.AddOption(m_testHandler.GetMenu());
}

// --------------------------------------------------------------------------------------------------------------------------------

void UiApplication::Run()
{
	m_io << GetTextArt13();

	m_rootMenu.Execute(m_io);
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
