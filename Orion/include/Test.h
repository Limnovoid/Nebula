#ifndef NEBULA_ORION_TEST_H
#define NEBULA_ORION_TEST_H

#include "TestResult.h"
#include "GetTypenameHelper.h"

namespace Nebula
{

namespace Orion
{

template<typename T>
TestResult AddUiTest()
{
	static const GetTypenameHelper<T> getTypenameHelper;

	std::string message = "AddUiTest definition missing for type '" + getTypenameHelper + "'.";

	return TestResult(false, std::move(message));
}

} // namespace Orion --------------------------------------------------------------------------------------------------------------

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_ORION_TEST_H
