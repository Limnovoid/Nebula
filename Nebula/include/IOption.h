#ifndef NEBULA_I_OPTION_H
#define NEBULA_I_OPTION_H

#include "NebulaString.h"
#include "UiIo.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

class IOption
{
public:
	virtual ~IOption() = default;

	virtual StringView GetPrompt() const = 0;
	virtual void Execute(UiIo const&) = 0;
};

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_I_OPTION_H
