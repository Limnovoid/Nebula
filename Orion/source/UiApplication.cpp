#include "UiApplication.h"

namespace Nebula
{
	
namespace Orion
{

UiApplication::UiApplication(char const* name) :
	m_rootMenu(name)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void UiApplication::Run()
{
	m_rootMenu.Execute();
}

} // namespace Orion --------------------------------------------------------------------------------------------------------------

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
