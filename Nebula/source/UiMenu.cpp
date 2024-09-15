#include "UiMenu.h"

#include "Format.h"
#include "ToString.h"
#include "ToType.h"

namespace Nebula
{

UiMenu::UiMenu(StringView header, bool isRootMenu) :
	m_prompt(header),
	m_returnKey('Q'),
	m_isExecuting(false)
{
	m_prompt += "...";
	m_header = MakeStringView(m_prompt); // Initialize after the above line in case the append causes a reallocation (invalidating the string view).

	m_returnOption = MakeShared<UiOption>((isRootMenu ? "Exit" : "Return"), [this](UiIo const& uiIo) { Return(); }, isRootMenu);
}

// --------------------------------------------------------------------------------------------------------------------------------

void UiMenu::Execute(UiIo const& uiIo)
{
	m_isExecuting = true;

	while (m_isExecuting)
	{
		uiIo << "\n----------------------------------------------------------------------------------------------------------------\n";

		DisplayOptions(uiIo);
		DisplayReturnOption(uiIo);

		uiIo.Newline();

		String selection;
		uiIo.Get(selection, "Choose");

		SelectOption(uiIo, selection);
	}
}

// --------------------------------------------------------------------------------------------------------------------------------

void UiMenu::DisplayOptions(UiIo const& uiIo)
{
	uiIo << "Menu: " << m_header << "\n\n";
	for (size_t i = 0; i < m_options.size(); ++i)
		uiIo << Fmt::Format("{0:>4}", (1 + i)) << ". " << m_options[i]->GetPrompt() << '\n';
}

// --------------------------------------------------------------------------------------------------------------------------------

void UiMenu::DisplayReturnOption(UiIo const& uiIo)
{
	uiIo << '\n';
	uiIo << Fmt::Format("{0:>4}", 'Q') << ". " << m_returnOption->GetPrompt() << '\n';
}

// --------------------------------------------------------------------------------------------------------------------------------

void UiMenu::SelectOption(UiIo const& uiIo, StringView selection)
{
	if ((1 == selection.size()) && (String::ToUpper(selection.front()) == m_returnKey))
	{
		m_returnOption->Execute(uiIo);
	}
	else
	{
		size_t index;
		if ((RESULT_CODE_SUCCESS == ToType<size_t>(selection, index)) && (index <= m_options.size()))
			m_options[index - 1]->Execute(uiIo);
	}
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
