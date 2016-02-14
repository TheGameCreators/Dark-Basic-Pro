// ExtendedFind.cpp : implementation file
//

#include "stdafx.h"
#include "Synergy Editor.h"
#include "ExtendedFind.h"


// ExtendedFind dialog

IMPLEMENT_DYNAMIC(ExtendedFind, CFindReplaceDialog)

ExtendedFind::ExtendedFind(CWnd* pParent /*=NULL*/)
	:CFindReplaceDialog ()
{}

ExtendedFind::~ExtendedFind()
{
	if (m_pParent != NULL)
	{
		ASSERT_VALID (m_pParent);

		m_pParent->m_pExtendedFind = NULL;
		if (m_pParent->IsDisableMainframeForFindDlg ())
		{
			AfxGetMainWnd()->ModifyStyle(WS_DISABLED,0);
			m_pParent->SetFocus();
		}
	}
}

void ExtendedFind::DoDataExchange(CDataExchange* pDX)
{
	CFindReplaceDialog::DoDataExchange(pDX);
	DDX_Control(pDX, ID_FIND_SELECTED, cSelected);
}


BEGIN_MESSAGE_MAP(ExtendedFind, CFindReplaceDialog)
END_MESSAGE_MAP()


// ExtendedFind message handlers
