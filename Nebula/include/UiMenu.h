#ifndef NEBULA_ORION_UI_MENU_H
#define NEBULA_ORION_UI_MENU_H

#include "UiOption.h"
#include "Format.h"
#include "ToString.h"
#include "ToType.h"

namespace Nebula
{

class UiMenu : public IOption
{
public:
	UiMenu(char const* header, bool isRootMenu = false);

	// IOption interface
	virtual StringView GetPrompt() override;
	virtual void Execute(UiIo const& uiIo) override;

	void AddOption(SharedPtr<IOption> pOption);

	void AddOption(char const* prompt, UiOption::Function function);

	void DisplayOptions(UiIo const& uiIo);

	void DisplayReturnOption(UiIo const& uiIo);

	void SelectOption(UiIo const& uiIo, char const selection);

	void Return();

private:
	using OptionList = std::vector<SharedPtr<IOption>>;

	String					m_prompt;
	StringView				m_header;
	OptionList				m_options;

	bool					m_isExecuting;
	SharedPtr<UiOption>		m_returnOption;
	char					m_returnKey;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline void UiMenu::AddOption(SharedPtr<IOption> pOption)
{
	m_options.push_back(pOption);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void UiMenu::AddOption(char const* prompt, UiOption::Function function)
{
	m_options.push_back(MakeShared<UiOption>(prompt, function));
}

// --------------------------------------------------------------------------------------------------------------------------------

inline StringView UiMenu::GetPrompt()
{
	return MakeStringView(m_prompt);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void UiMenu::Return()
{
	m_isExecuting = false;
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_ORION_UI_MENU_H
