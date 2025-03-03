#ifndef NEBULA_UI_MENU_H
#define NEBULA_UI_MENU_H

#include "UiOption.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

class UiMenu : public IOption
{
public:
	using OptionList = std::vector<SharedPtr<IOption>>;

	UiMenu(StringView header, bool isRootMenu = false);

	// IOption interface
	virtual StringView GetPrompt() const override;
	virtual void Execute(UiIo const& uiIo) override;

	void AddOption(SharedPtr<IOption> pOption);

	void AddOption(char const* prompt, UiOption::Function function);

	void DisplayOptions(UiIo const& uiIo);

	void DisplayReturnOption(UiIo const& uiIo);

	void SelectOption(UiIo const& uiIo, StringView selection);

	void Return();

	OptionList const& GetOptions() const;

	StringView GetHeader() const;

private:

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

inline StringView UiMenu::GetPrompt() const
{
	return MakeStringView(m_prompt);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void UiMenu::Return()
{
	m_isExecuting = false;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline UiMenu::OptionList const& UiMenu::GetOptions() const
{
	return m_options;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline StringView UiMenu::GetHeader() const
{
	return m_header;
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_UI_MENU_H
