#include "Orion.h"

#include "GetTypenameHelper.h"
#include "Metadata.h"
#include "TestResult.h"
#include "UiMenu.h"
#include "TextArt.h"

namespace Nebula
{

namespace Orion
{

void UiAddTextArtMenu(UiMenu & uiMenu)
{
	SharedPtr<UiMenu> pTextArtMenu = MakeShared<UiMenu>("Text art");
	
	pTextArtMenu->AddOption("Display text art 1", [](UiIo const& uiIo) { uiIo << '\n' << GetTextArt(); });
	pTextArtMenu->AddOption("Display text art 2", [](UiIo const& uiIo) { uiIo << '\n' << GetTextArt2(); });

	uiMenu.AddOption(pTextArtMenu);
}

// --------------------------------------------------------------------------------------------------------------------------------

void UiAddOptions(UiMenu & uiMenu)
{
	SharedPtr<UiMenu> pOrionMenu = MakeShared<UiMenu>("Orion");

	UiAddTextArtMenu(*pOrionMenu);

	uiMenu.AddOption(pOrionMenu);
}

} // namespace Orion --------------------------------------------------------------------------------------------------------------

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
