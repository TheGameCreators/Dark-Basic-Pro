#include "StdAfx.h"
#include "OptionsDlg.h"
#include "Resource.h"
#include "Settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg
IMPLEMENT_DYNAMIC(COptionsDlg, CBCGPPropertySheet)

COptionsDlg::~COptionsDlg()
{
	Settings::SaveSettings();
}

COptionsDlg::COptionsDlg(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CBCGPPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	SetLook (CBCGPPropertySheet::PropSheetLook_List, 125);
	// EnablePageHeader (max (40, globalData.GetTextHeight () * 2));

	AddPage(&m_Environment);
	AddPage(&m_Text);
	AddPage(&m_Fonts);	
	AddPage(&m_Defaults);	
	AddPage(&m_Semantic);
	AddPage(&m_Startup);
	AddPage(&m_Help);
}

BOOL COptionsDlg::OnInitDialog()
{
	CBCGPPropertySheet::OnInitDialog();

	CRect rect, tabrect;
	int width;

	//Get button sizes and positions
	GetDlgItem(IDCANCEL)->GetWindowRect(rect);
	GetTabControl()->GetWindowRect(tabrect);
	ScreenToClient(rect); 
	ScreenToClient(tabrect);

	//New button -> width, height and Y-coordiate of IDOK
	// -> X-coordinate of tab control
	width = 300;
	rect.left = 10; 
	rect.right = rect.left + width;
	rect.top = rect.top + 5;

	//Create new "Add" button and set standard font
	m_cRestartRequired.Create(_T("* Restart required for changes to take effect"), WS_CHILD|WS_VISIBLE, rect, this);
	m_cRestartRequired.SetFont(GetFont());

	return TRUE;
}

//void COptionsDlg::OnDrawPageHeader (CDC* pDC, int nPage, CRect rectHeader)
//{
//	CBCGPDrawManager dm (*pDC);
//
//	COLORREF clrFill = globalData.clrBarFace;
//
//	dm.FillGradient (rectHeader, 
//		pDC->GetPixel (rectHeader.left, rectHeader.bottom),
//		clrFill);
//
//	rectHeader.bottom -= 10;
//
//	CString strText;
//
//	switch (nPage)
//	{
//		case 0:
//			strText = _T("Change the most popular options");
//			break;
//		case 1:
//			strText = _T("Customize the features that are present when editing code");
//			break;
//		case 2:
//			strText = _T("Customize the fonts and colours used to display the different code styles");
//			break;
//		case 3:
//			strText = _T("Add, remove and order the precompilers used at compile time");
//			break;
//		case 4:
//			strText = _T("Set the project properties used by default when a new project is created");
//			break;
//		case 5:
//			strText = _T("Change the behaviour of the inbuilt backup engine");
//			break;
//		case 6:
//			strText = _T("Customize the behaviour of the semantic parser");
//			break;
//		case 7:
//			strText = _T("Set the aplication startup behaviour");
//			break;
//		case 8:
//			strText = _T("Add, remove and order the include files added by default to new projects");
//			break;
//		case 9:
//			strText = _T("Change how help files are presented");
//			break;
//	}
//
//	CRect rectText = rectHeader;
//	rectText.left = 10;
//	rectText.right -= 20;
//
//	CFont* pOldFont = pDC->SelectObject (&globalData.fontBold);
//	pDC->SetBkMode (TRANSPARENT);
//	pDC->SetTextColor (globalData.clrBarText);
//
//	UINT uiFlags = DT_SINGLELINE | DT_VCENTER;
//
//	CRect rectTextCalc = rectText;
//	pDC->DrawText (strText, rectTextCalc, uiFlags | DT_CALCRECT);
//
//	if (rectTextCalc.right > rectText.right)
//	{
//		rectText.DeflateRect (0, 10);
//		uiFlags = DT_WORDBREAK;
//	}
//
//	pDC->DrawText (strText, rectText, uiFlags);
//
//	pDC->SelectObject (pOldFont);
//}

BEGIN_MESSAGE_MAP(COptionsDlg, CBCGPPropertySheet)
	//{{AFX_MSG_MAP(COptionsDlg)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg message handlers
