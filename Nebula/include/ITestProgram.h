#ifndef NEBULA_I_TEST_PROGRAM_H
#define NEBULA_I_TEST_PROGRAM_H

namespace Nebula
{

class TestHandler;

// --------------------------------------------------------------------------------------------------------------------------------

class ITestProgram
{
public:
	ITestProgram(StringView title);

	virtual ~ITestProgram();

	void Run(TestHandler & testHandler);

	StringView GetTitle();

private:
	virtual void RunImpl(TestHandler & testHandler) = 0;

	String		m_title;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline StringView ITestProgram::GetTitle()
{
	return MakeStringView(m_title);
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_I_TEST_PROGRAM_H
