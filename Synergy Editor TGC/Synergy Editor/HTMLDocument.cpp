#include "StdAfx.h"
#include "HTMLDocument.h"
#include "Help.h"

/////////////////////////////////////////////////////////////////////////////
// CMfcieDoc

IMPLEMENT_DYNCREATE(HTMLDocument, CDocument)

BEGIN_MESSAGE_MAP(HTMLDocument, CDocument)
	//{{AFX_MSG_MAP(CMfcieDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMfcieDoc construction/destruction

HTMLDocument::HTMLDocument(void)
{
}

HTMLDocument::~HTMLDocument(void)
{
}

BOOL HTMLDocument::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

BOOL HTMLDocument::OnOpenDocument(LPCTSTR lpszPathName)
{
	POSITION pos = GetFirstViewPosition ();
	CHelp* pView = DYNAMIC_DOWNCAST (CHelp, GetNextView (pos));
	if (pView != NULL)
	{
		pView->Navigate(lpszPathName);
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMfcieDoc serialization

void HTMLDocument::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMfcieDoc diagnostics

#ifdef _DEBUG
void HTMLDocument::AssertValid() const
{
	CDocument::AssertValid();
}

void HTMLDocument::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMfcieDoc commands
