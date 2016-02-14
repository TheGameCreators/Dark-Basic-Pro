#pragma once

#include "View.h"

class MainFrame;

class SemanticParserManager
{
public:
	static void Init(MainFrame* pMainWnd);
	static void Start();
	static void SetView(View* newView);
	static void CloseView(View* newView);

	static void ThreadPause();
	static void ThreadResume();

private:
	SemanticParserManager();
	static View* pView;	
	static int oldLength;
	static CWinThread* m_pThread;
};
