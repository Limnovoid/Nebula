#include "File.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

File::File()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

File::File(StringView filePath)
	: m_filePath(filePath)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

Result File::Open(OpenMode const mode)
{
	if (m_filePath.empty())
		return RESULT_CODE_NOT_INITIALIZED;

	if (m_ofstream.is_open())
		return RESULT_CODE_ALREADY_OPEN;

	m_ofstream.open(m_filePath, mode.To<std::ios_base::openmode>());

	return m_ofstream.good() ? RESULT_CODE_SUCCESS : RESULT_CODE_FAILURE;
}

// --------------------------------------------------------------------------------------------------------------------------------

Result File::Open(StringView filePath, OpenMode const mode)
{
	SetPath(filePath);

	return Open(mode);
}

// --------------------------------------------------------------------------------------------------------------------------------

Result File::Close()
{
	if (!m_ofstream.is_open())
		return RESULT_CODE_NOT_OPEN;
	
	m_ofstream.close();

	return m_ofstream.good() ? RESULT_CODE_SUCCESS : RESULT_CODE_FAILURE;
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
