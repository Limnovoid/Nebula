#include "UiMenu.h"

namespace Nebula
{

UiMenu::UiMenu(char const* header, bool isRootMenu) :
	m_prompt(header),
	m_header(m_prompt),
	m_returnKey('Q')
{
	m_prompt += "...";

	m_returnOption = MakeShared<UiOption>((isRootMenu ? "Exit" : "Return"), [this](UiIo const&) { Return(); });
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

		uiIo << "\n# ";

		char selection;
		uiIo >> selection;

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

void UiMenu::SelectOption(UiIo const& uiIo, char const selection)
{
	if (ToUpper(selection) == m_returnKey)
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
