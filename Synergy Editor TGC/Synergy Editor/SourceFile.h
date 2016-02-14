#pragma once

class SourceFile
{
public:
	CString filename;
	int line;

	SourceFile(CString filename, int line);
	~SourceFile();
};
