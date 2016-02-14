#include "StdAfx.h"
#include "WelcomeDialog.h"
#include "Resource.h"
#include "Utilities.h"
#include "Settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// WelcomeDialog
IMPLEMENT_DYNAMIC(WelcomeDialog, CBCGPPropertySheet)

WelcomeDialog::~WelcomeDialog()
{
	for(UINT i=0; i < m_Pages.size(); i++)
	{
		delete m_Pages.at(i);
	}
	m_Pages.clear();
}

BOOL WelcomeDialog::OnInitDialog()
{
	CBCGPPropertySheet::OnInitDialog();

	CRect rect, tabrect;
	int width;

	//Get button sizes and positions
	GetDlgItem(IDCANCEL)->GetWindowRect(rect);
	GetTabControl()->GetWindowRect(tabrect);
	ScreenToClient(rect); 
	ScreenToClient(tabrect);

	if(m_bModeless)
	{
		RECT rc;
		GetWindowRect (&rc);
		// Increase the height of the CPropertySheet by 30
		rc.bottom += 30;
		// Resize the CPropertySheet
		MoveWindow (rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top);
		// Convert to client coordinates
		ScreenToClient (&rc);
		// m_Button is of type CButton and is a member of CMySheet
		m_Button.Create (_T("Close"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, rect, this, IDOK);
		m_Button.SetFont(GetFont());
	}

	//New button -> width, height and Y-coordiate of IDOK
	// -> X-coordinate of tab control
	width = 200;
	rect.left = tabrect.left; 
	rect.right = tabrect.left + width;

	//Create new "Add" button and set standard font
	m_ButtonAgain.Create(_T("Do not show again"), BS_CHECKBOX|WS_CHILD|WS_VISIBLE|WS_TABSTOP, rect, this, IDC_WELCOME_SHOW);
	m_ButtonAgain.SetFont(GetFont());

	((CButton*)GetDlgItem(IDC_WELCOME_SHOW))->SetCheck(Settings::ShowWelcomeScreen ? BST_UNCHECKED : BST_CHECKED);

	return TRUE;
}

WelcomeDialog::WelcomeDialog(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CBCGPPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	this->m_psh.dwFlags |= PSH_NOAPPLYNOW;

	SetLook (CBCGPPropertySheet::PropSheetLook_Tabs);

	CString intro(Utilities::ExtractPath(Utilities::GetApplicationPath()) + _T("Videos\\Introduction"));

	if(	Utilities::CheckPathExists(intro))
	{
		WelcomePages* page = new WelcomePages();
		page->SetPath(intro);
		m_Pages.push_back(page);
		AddPage (page);	
	}

	CString dir = Utilities::ExtractPath(Utilities::GetApplicationPath()) + _T("Videos\\*.*");

	 CFileFind finder;

   // start working for files
   BOOL bWorking = finder.FindFile(dir);

   while (bWorking)
   {
      bWorking = finder.FindNextFile();

      // skip . and .. files; otherwise, we'd recur infinitely!
      if (finder.IsDots())
         continue;

      // if it's a directory, recursively search it
      if (finder.IsDirectory() && finder.GetFilePath() != intro)
      {
	 	WelcomePages* page = new WelcomePages();
		page->SetPath(finder.GetFilePath());
		m_Pages.push_back(page);
		AddPage (page);	
      }
   }

   finder.Close();
}

void WelcomeDialog::OnCheck()
{	
	((CButton*)GetDlgItem(IDC_WELCOME_SHOW))->SetCheck(!((CButton*)GetDlgItem(IDC_WELCOME_SHOW))->GetCheck());
	Settings::ShowWelcomeScreen = (((CButton*)GetDlgItem(IDC_WELCOME_SHOW))->GetCheck() == BST_UNCHECKED);
	Settings::SaveSettings();
}

BEGIN_MESSAGE_MAP(WelcomeDialog, CBCGPPropertySheet)
	//{{AFX_MSG_MAP(WelcomeDialog)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		ON_BN_CLICKED(IDC_WELCOME_SHOW, OnCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// WelcomeDialog message handlers
