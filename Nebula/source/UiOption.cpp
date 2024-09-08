#include "UiOption.h"

namespace Nebula
{

UiOption::UiOption(StringView prompt, Function function) :
	m_prompt(prompt),
	m_function(function)
{
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
