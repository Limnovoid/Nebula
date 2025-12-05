#ifndef NEBULA_H
#define NEBULA_H

// Include all project headers...
#include "Bitset.h"
#include "ConstString.h"
#include "Exception.h"
#include "ExplicitArithmetic.h"
#include "File.h"
#include "Format.h"
#include "GetTypenameHelper.h"
#include "IOption.h"
#include "ITestScript.h"
#include "IUnitTest.h"
#include "Macros.h"
#include "Maths.h"
#include "Metadata.h"
#include "NebulaString.h"
#include "NebulaTypes.h"
#include "NeedsInitializationHelper.h"
#include "Random.h"
#include "Result.h"
#include "Stable.h"
#include "TestHandler.h"
#include "TestHandler2.h"
#include "TextArt.h"
#include "ToString.h"
#include "ToType.h"
#include "Typename.h"
#include "UiApplication.h"
#include "UiIo.h"
#include "UiMenu.h"
#include "UiOption.h"
#include "UnitTest.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

class UiMenu;
class TestHandler;

// ---------------------------------------------------------------------------------------------------------------------------------

void UiAddOptions(UiMenu &);
void AddTests(TestHandler &);

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_H
