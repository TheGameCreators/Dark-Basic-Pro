#pragma once

class Keyword
{
public:
	CString keyword;
	CString help;
	CString url;
	int length;

	Keyword(CString keyword, CString help, CString url);
};
