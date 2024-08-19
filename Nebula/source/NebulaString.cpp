#include "NebulaString.h"

namespace Nebula
{

String ToString(bool const value)
{
	return (value ? "true" : "false");
}

// --------------------------------------------------------------------------------------------------------------------------------

bool IsWhitespace(char const c)
{
	switch (c)
	{
	case ' ':
	case '\t':
		return true;
	}

	return false;
}

// --------------------------------------------------------------------------------------------------------------------------------

StringView GetFirstWord(StringView string)
{
	size_t wordStartOffset = 0;

	while ((wordStartOffset < string.size()) && (IsWhitespace(string[wordStartOffset])))
		++wordStartOffset;

	if (string.size() == wordStartOffset)
		return StringView();

	size_t wordEndOffset = wordStartOffset + 1;

	while ((wordEndOffset < string.size()) && (!IsWhitespace(string[wordEndOffset])))
		++wordEndOffset;

	return string.substr(wordStartOffset, (wordEndOffset - wordStartOffset));
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------