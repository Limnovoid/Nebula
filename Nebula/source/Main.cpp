#include "Stable.h"

#include "TestResult.h"
#include "Block.h"
#include "TextArt.h"
#include "UiMenu.h"

int main(int argc, int** argv)
{
	using namespace Nebula;

	std::string message1 = "test message 1";
	std::string message2 = "test message 2";

	Orion::TestResult testResult1(false, message1);
	Orion::TestResult testResult2(false, std::move(message2));

	assert(message2.empty());

	testResult1.SetMessage(message1);
	testResult2.SetMessage(std::move(message1));

	assert(message1.empty());

	/*class UiOption
	{
	public:
		UiOption(char const* prompt, std::function<void(void)> action) : m_prompt(prompt), m_action(action) {}

		std::string					m_prompt;
		std::function<void(void)>	m_action;
	};

	class UiMenu
	{
	public:
		UiMenu(char const* header) : m_header(header) {}

		void AddOption(UiOption && option) { m_options.push_back(option); }

		void DisplayOptions()
		{
			std::cout << m_header << " options:" << std::endl;
			size_t i = 1;
			for (UiOption const& option : m_options)
				std::cout << " " << i << ". " << option.m_prompt << std::endl;
		}

		void SelectOption(size_t const index)
		{
			if (index <= m_options.size())
				m_options[index - 1].m_action();
		}

	private:
		std::string				m_header;
		std::vector<UiOption>	m_options;
	};

	bool isRunning = true;

	UiMenu uiMenu("Root");

	uiMenu.AddOption(UiOption("Exit", [&isRunning]() { isRunning = false; }));

	while (isRunning)
	{
		uiMenu.DisplayOptions();

		size_t selection;
		std::cout << "\n# ";
		std::cin >> selection;
		uiMenu.SelectOption(selection);

		std::cout << "\n----------------------------------------------------------------\n";
	}*/

	Orion::UiMenu uiMenu("Root");

	uiMenu.Execute();
}
