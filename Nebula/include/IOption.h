#ifndef NEBULA_IOPTION_H
#define NEBULA_IOPTION_H

#include "NebulaString.h"
#include "UiIo.h"

namespace Nebula
{

class IOption
{
public:
	virtual ~IOption() = default;

	virtual StringView GetPrompt() = 0;
	virtual void Execute(UiIo const&) = 0;
};

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_IOPTION_H
