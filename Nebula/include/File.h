#ifndef NEBULA_FILE_H
#define NEBULA_FILE_H

#include "Result.h"
#include "Bitset.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

class File
{
public:
	class OpenMode : public Bitset<5>
	{
		using Bitset = Bitset<5>;

	public:
		static const uint8_t READ		= std::ios::in;
		static const uint8_t WRITE		= std::ios::out;
		static const uint8_t APPEND		= std::ios::app;
		static const uint8_t TRUNCATE	= std::ios::trunc;
		static const uint8_t BINARY		= std::ios::binary;

		OpenMode(uint8_t bitmap);
	};

	File();
	File(StringView filePath);

	/// <summary> Set the filePath. Does not open the file. </summary>
	void SetPath(StringView filePath);

	/// <summary> Open the file at the given location. </summary>
	/// <returns>
	/// <list type="bullet">
	/// <item> <term> RESULT_CODE_SUCCESS </term> <description> The file was opened at the given location. </description> </item>
	/// <item> <term> RESULT_CODE_ALREADY_OPEN </term> <description> The file is already open at the given location. </description> </item>
	/// <item> <term> RESULT_CODE_FAILURE </term> <description> Unknown error. </description> </item>
	/// </list>
	/// </returns>
	Result Open(StringView filePath, OpenMode const mode);

	/// <summary> Open the file at the stored filePath's location. </summary>
	/// <returns>
	/// <list type="bullet">
	/// <item> <term> RESULT_CODE_SUCCESS </term> <description> File was opened at the stored filePath's location. </description> </item>
	/// <item> <term> RESULT_CODE_NOT_INITIALIZED </term> <description> There is no stored filePath. </description> </item>
	/// <item> <term> RESULT_CODE_ALREADY_OPEN </term> <description> The file is already open at the stored filePath's location. </description> </item>
	/// <item> <term> RESULT_CODE_FAILURE </term> <description> Unknown error. </description> </item>
	/// </list>
	/// </returns>
	Result Open(OpenMode const mode);

	/// <summary> Close the file. </summary>
	/// <returns>
	/// <list type="bullet">
	/// <item> <term> RESULT_CODE_SUCCESS </term> <description> File was closed. </description> </item>
	/// <item> <term> RESULT_CODE_NOT_OPEN </term> <description> The file was not open. </description> </item>
	/// <item> <term> RESULT_CODE_FAILURE </term> <description> Unknown error. </description> </item>
	/// </list>
	/// </returns>
	Result Close();

	/// <returns>True if the file is open at the stored filePath's location, false otherwise.</returns>
	bool IsOpen() const;

	/// <summary> Write to the file. </summary>
	/// <returns>
	/// <list type="bullet">
	/// <item> <term> RESULT_CODE_SUCCESS </term> <description> The message was written to the file. </description> </item>
	/// <item> <term> RESULT_CODE_NOT_OPEN </term> <description> The file was not open. </description> </item>
	/// <item> <term> RESULT_CODE_FAILURE </term> <description> Unknown error. </description> </item>
	/// </list>
	/// </returns>
	Result Write(StringView string);

private:
	std::ofstream				m_ofstream;

	//std::filesystem::path		m_filePath;
	String						m_filePath;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline void File::SetPath(StringView filePath)
{
	m_filePath = filePath;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline bool File::IsOpen() const
{
	return m_ofstream.is_open();
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Result File::Write(StringView string)
{
	if (!IsOpen())
		return RESULT_CODE_NOT_OPEN;

	m_ofstream.write(string.data(), string.length());

	return m_ofstream.good() ? RESULT_CODE_SUCCESS : RESULT_CODE_FAILURE;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

inline File::OpenMode::OpenMode(uint8_t bitmap) :
	Bitset(bitmap)
{
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_FILE_H
