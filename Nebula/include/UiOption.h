#ifndef NEBULA_UI_OPTION_H
#define NEBULA_UI_OPTION_H

#include "IOption.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

class UiOption : public IOption
{
public:
	using Function = std::function<void(UiIo const&)>;

	UiOption(StringView prompt, Function function, bool const requiresConfirmation = false);

	virtual ~UiOption() = default;

	// IOption interface
	virtual StringView GetPrompt() const override;
	virtual void Execute(UiIo const& uiIo) override;

private:
	String const		m_prompt;
	Function const		m_function;
	bool const			m_requiresConfirmation;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline StringView UiOption::GetPrompt() const
{
	return MakeStringView(m_prompt);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void UiOption::Execute(UiIo const& uiIo)
{
	if (!m_requiresConfirmation || (RESULT_CODE_SUCCESS == uiIo.GetConfirmation(Fmt::Format("{} - Are you sure", m_prompt.c_str()))))
		m_function(uiIo);
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_UI_OPTION_H
