// ExtendedReplace.cpp : implementation file
//

#include "stdafx.h"
#include "Synergy Editor.h"
#include "ExtendedReplace.h"


// ExtendedReplace dialog

IMPLEMENT_DYNAMIC(ExtendedReplace, CFindReplaceDialog)

ExtendedReplace::ExtendedReplace(CWnd* pParent /*=NULL*/)
	: CFindReplaceDialog()
{

}

ExtendedReplace::~ExtendedReplace()
{
}

void ExtendedReplace::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, ID_FIND_SELECTED, cSelected);
}


BEGIN_MESSAGE_MAP(ExtendedReplace, CFindReplaceDialog)
END_MESSAGE_MAP()


// ExtendedReplace message handlers
