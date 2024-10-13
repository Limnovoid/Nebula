#include "UiOption.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

UiOption::UiOption(StringView prompt, Function function, bool requiresConfirmation) :
	m_prompt(prompt),
	m_function(function),
	m_requiresConfirmation(requiresConfirmation)
{
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
