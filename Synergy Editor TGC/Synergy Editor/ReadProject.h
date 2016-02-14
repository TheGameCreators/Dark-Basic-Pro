#pragma once
#include "MainFrm.h"

class ReadProject
{
public:
	static void Read(CString);

private:
	static MainFrame* pMainWnd;

	static void handleStr(CString name, CString def, CString& field);
	static void handleBool(CString name, CString def, bool& field);
	static void readFiles();
	static void readMedia();
	static void readToDo();
	static void readCursors();
	static void readComments();
};
