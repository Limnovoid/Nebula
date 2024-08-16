#include "UiApplication.h"

namespace Nebula
{
	
namespace Orion
{

UiApplication::UiApplication(char const* name) :
	m_rootMenu(name),
	m_io(std::cout, std::cin)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void UiApplication::Run()
{
	m_rootMenu.Execute(m_io);
}

} // namespace Orion --------------------------------------------------------------------------------------------------------------

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
