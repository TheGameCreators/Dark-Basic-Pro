#include "StdAfx.h"
#include "SemanticParserManager.h"
#include "Variables.h"
#include "EditorControl.h"
#include "SemanticParser.h"
#include "MainFrm.h"

View* SemanticParserManager::pView;
int SemanticParserManager::oldLength;
CWinThread* SemanticParserManager::m_pThread;

SemanticParserManager::SemanticParserManager()
{
	m_pThread = NULL;
}

void SemanticParserManager::Init(MainFrame* pMainWnd)
{
	pView = NULL;
	oldLength = -1;
	SetTimer(pMainWnd->GetSafeHwnd(), Variables::SemanticParser, 10000, 0);
}

void SemanticParserManager::SetView(View* newView) // A new view is being activated
{
	pView = newView;
	oldLength = -1;
}

void SemanticParserManager::CloseView(View* newView) // A view has closed, so stop it from being used
{
	if(pView == newView)
	{
		pView = NULL;
	}
}

void SemanticParserManager::Start()
{
	if(pView == NULL)
	{
		return;
	}

	if(!SemanticParser::m_Running.Lock(1))
	{
		return;
	}

	ASSERT_VALID(pView->m_pEdit);
	int newLength = pView->m_pEdit->GetText().GetLength();
	if(newLength > 0 && newLength != oldLength)
	{
		oldLength = newLength;
		SemanticData* dt = new SemanticData();
		dt->Text = pView->GetText();
		dt->Filename = pView->GetDocument()->GetPathName();
		dt->mode = 0;
		m_pThread = AfxBeginThread(SemanticParser::ParseThread, dt, THREAD_PRIORITY_LOWEST);
	}
	else
	{
		SemanticParser::m_Running.Unlock();
	}
}

void SemanticParserManager::ThreadPause()
{
	if(m_pThread == NULL)
	{
		return;
	}
	DWORD dwStatus;
	if(GetExitCodeThread(m_pThread->m_hThread, &dwStatus))
	{
		if (dwStatus == STILL_ACTIVE)
		{
			m_pThread->SuspendThread();
		}
	}
}

void SemanticParserManager::ThreadResume()
{
	if(m_pThread == NULL)
	{
		return;
	}
	DWORD dwStatus;
	if(GetExitCodeThread(m_pThread->m_hThread, &dwStatus))
	{
		if (dwStatus == STILL_ACTIVE)
		{
			m_pThread->ResumeThread();
		}
	}
}
