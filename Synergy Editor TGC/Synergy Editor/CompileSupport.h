#pragma once

class CompileSupport
{
public:
	static void Compile(bool run);
	static void CompileHelp(bool run);

	static bool m_IsCompiling;
	static CString m_strHelpFile;
private:
	static UINT CompileThread(LPVOID pParam);
	static bool m_run;
};
