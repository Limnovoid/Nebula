#ifndef NEBULA_ORION_UI_MENU_H
#define NEBULA_ORION_UI_MENU_H

namespace Nebula
{

namespace Orion
{

class IOption
{
public:
	virtual ~IOption() = default;

	virtual StringView GetPrompt() = 0;
	virtual void Execute() = 0;
};

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class UiOption : public IOption
{
public:
	using Function = std::function<void(void)>;

	UiOption(char const* prompt, Function function) : m_prompt(prompt), m_function(function) {}

	virtual ~UiOption() = default;

	// IOption interface
	virtual StringView GetPrompt() override { return MakeStringView(m_prompt); }
	virtual void Execute() override { m_function(); }

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
	virtual void Execute() override
	{
		bool isExecuting = true;

		AddOption(MakeShared<UiOption>("Return", [&isExecuting]() { isExecuting = false; }));

		while (isExecuting)
		{
			DisplayOptions();

			size_t selection;
			std::cout << "\n# ";
			std::cin >> selection;
			SelectOption(selection);

			std::cout << "\n----------------------------------------------------------------\n";
		}
	}

	void AddOption(SharedPtr<IOption> pOption) { m_options.push_back(pOption); }

	void DisplayOptions()
	{
		std::cout << m_header << " options:" << std::endl;
		size_t i = 1;
		for (SharedPtr<IOption> const option : m_options)
			std::cout << " " << i << ". " << option->GetPrompt() << std::endl;
	}

	void SelectOption(size_t const index)
	{
		if (index <= m_options.size())
			m_options[index - 1]->Execute();
	}

private:
	String								m_header;
	std::vector<SharedPtr<IOption>>		m_options;
};

} // namespace Orion --------------------------------------------------------------------------------------------------------------


} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_ORION_UI_MENU_H
