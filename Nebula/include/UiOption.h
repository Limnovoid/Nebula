#ifndef NEBULA_UI_OPTION_H
#define NEBULA_UI_OPTION_H

#include "IOption.h"

namespace Nebula
{

class UiOption : public IOption
{
public:
	using Function = std::function<void(UiIo const&)>;

	UiOption(char const* prompt, Function function);

	virtual ~UiOption() = default;

	// IOption interface
	virtual StringView GetPrompt() override;
	virtual void Execute(UiIo const& uiIo) override;

private:
	String		m_prompt;
	Function	m_function;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline StringView UiOption::GetPrompt()
{
	return MakeStringView(m_prompt);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void UiOption::Execute(UiIo const& uiIo)
{
	m_function(uiIo);
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_UI_OPTION_H
