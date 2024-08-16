#ifndef NEBULA_ORION_UI_MENU_H
#define NEBULA_ORION_UI_MENU_H

#include "ToType.h"

namespace Nebula
{

namespace Orion
{

class UiIo
{
public:
	UiIo(std::ostream & outputStream, std::istream & inputStream) :
		m_outputStream(outputStream),
		m_inputStream(inputStream)
	{
	}

	template<typename T>
	void Print(T const& t) const { m_outputStream << t; }

	template<typename T>
	void Get(T & t) const { m_inputStream >> t; }

private:
	std::ostream &		m_outputStream;
	std::istream &		m_inputStream;
};

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline UiIo const& operator<<(UiIo const& uiIo, T const& t)
{
	uiIo.Print(t);

	return uiIo;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline UiIo const& operator>>(UiIo const& uiIo, T & t)
{
	uiIo.Get(t);

	return uiIo;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class IOption
{
public:
	virtual ~IOption() = default;

	virtual StringView GetPrompt() = 0;
	virtual void Execute(UiIo const&) = 0;
};

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class UiOption : public IOption
{
public:
	using Function = std::function<void(UiIo const&)>;

	UiOption(char const* prompt, Function function) : m_prompt(prompt), m_function(function) {}

	virtual ~UiOption() = default;

	// IOption interface
	virtual StringView GetPrompt() override { return MakeStringView(m_prompt); }
	virtual void Execute(UiIo const& uiIo) override { m_function(uiIo); }

private:
	String		m_prompt;
	Function	m_function;
};

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class UiMenu : public IOption
{
public:
	UiMenu(char const* header) : m_header(header) {}

	// IOption interface
	virtual StringView GetPrompt() override { return MakeStringView(m_header); }
	virtual void Execute(UiIo const& uiIo) override
	{
		bool isExecuting = true;

		AddOption(MakeShared<UiOption>("Return", [&isExecuting](UiIo const&) { isExecuting = false; }));

		while (isExecuting)
		{
			DisplayOptions(uiIo);

			uiIo << "\n# ";

			char selection;
			uiIo >> selection;

			SelectOption(uiIo, selection);

			uiIo << "\n----------------------------------------------------------------\n";
		}
	}

	void AddOption(SharedPtr<IOption> pOption)
	{
		m_options.push_back(pOption);
	}

	void AddOption(char const* prompt, UiOption::Function function)
	{
		m_options.push_back(MakeShared<UiOption>(prompt, function)); 
	}

	void DisplayOptions(UiIo const& uiIo)
	{
		uiIo << m_header << " options:\n";
		for (size_t i = 0; i < m_options.size(); ++i)
			uiIo << " " << (1 + i) << ". " << m_options[i]->GetPrompt() << '\n';
	}

	void SelectOption(UiIo const& uiIo, char const selection)
	{
		size_t index = ToType<size_t>(selection);

		if (index <= m_options.size())
			m_options[index - 1]->Execute(uiIo);
	}

private:
	String								m_header;
	std::vector<SharedPtr<IOption>>		m_options;
};

} // namespace Orion --------------------------------------------------------------------------------------------------------------


} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_ORION_UI_MENU_H
