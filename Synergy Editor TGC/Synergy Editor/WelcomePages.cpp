// WelcomePages.cpp : implementation file

#include "stdafx.h"
#include "Synergy Editor.h"
#include "WelcomePages.h"
#include "Utilities.h"

// WelcomePages dialog
IMPLEMENT_DYNAMIC(WelcomePages, CBCGPPropertyPage)

WelcomePages::WelcomePages(CWnd* pParent)
	: CBCGPPropertyPage(WelcomePages::IDD)
{
	buttonNum = 1;
}

void WelcomePages::SetPath(CString path)
{
	m_Path = path + _T("\\");

	title = path.Mid(path.ReverseFind('\\') + 1);

	int length = title.GetLength() + 1;
	wchar_t* string = new wchar_t[length];
	wcscpy_s( string, length, CT2CW( (LPCTSTR) title ));

	this->m_psp.dwFlags |= PSP_USETITLE;
	this->m_psp.pszTitle = string;
}

WelcomePages::~WelcomePages()
{
	delete [] this->m_psp.pszTitle;
}

BOOL WelcomePages::OnInitDialog()
{
	CDialog::OnInitDialog();

	CWnd *pOK = GetParent()->GetDlgItem(IDOK);
	if (pOK)
		pOK->ShowWindow(SW_HIDE);
	CWnd *pCancel = GetParent()->GetDlgItem(IDCANCEL);
	if (pCancel)
		pCancel->SetWindowTextW(_T("Close"));

	CString file(m_Path + title + _T(".txt"));
	if(Utilities::CheckFileExists(file)){
		CStdioFile read;
		BOOL res = read.Open(file, CFile::modeRead);	
		if(res)
		{
			CString title, avi, image;
			read.ReadString(title);
			cText.SetWindowTextW(title);
			while(read.ReadString(title))
			{
				read.ReadString(avi);
				read.ReadString(image);
				CreateButton(title, avi, image); 
			}
			read.Close();
		}
	}

//CString path(m_Path);
//path.Append(_T("*.*"));
//CFileFind finder;
//  // start working for files
//  BOOL bWorking = finder.FindFile(path);
//while (bWorking)
//{
//	bWorking = finder.FindNextFile();
//	// skip . and .. files; otherwise, we'd recur infinitely!
//	if (finder.IsDots())
//		continue;

//	// if it's a directory, recursively search it
//	if (!finder.IsDirectory())
//	{
//		CString path = finder.GetFilePath();
//		if(path.Right(3) == _T("avi"))
//		{
//			CreateButton(path);
//		}
//	}
//}
//  finder.Close();

	for(int i=buttonNum; i <= 10; i++)
	{
		CBCGPButton* button;
		switch(i)
		{
			case 1:
				button = &cButton1;
			break;
			case 2:
				button = &cButton2;
			break;
			case 3:
				button = &cButton3;
			break;
			case 4:
				button = &cButton4;
			break;
			case 5:
				button = &cButton5;
			break;
			case 6:
				button = &cButton6;
			break;
			case 7:
				button = &cButton7;
			break;
			case 8:
				button = &cButton8;
			break;
			case 9:
				button = &cButton9;
			break;
			case 10:
				button = &cButton10;
			break;
		}
		button->ShowWindow(SW_HIDE);
	}

	return TRUE;
}

void WelcomePages::CreateButton(CString title, CString filename, CString bitmap)
{
	if(buttonNum < 11)
	{
		CBCGPButton* button;
		switch(buttonNum)
		{
			case 1:
				button = &cButton1;
			break;
			case 2:
				button = &cButton2;
			break;
			case 3:
				button = &cButton3;
			break;
			case 4:
				button = &cButton4;
			break;
			case 5:
				button = &cButton5;
			break;
			case 6:
				button = &cButton6;
			break;
			case 7:
				button = &cButton7;
			break;
			case 8:
				button = &cButton8;
			break;
			case 9:
				button = &cButton9;
			break;
			case 10:
				button = &cButton10;
			break;
		}
		avis[buttonNum-1] = filename;
		button->m_bRighImage = FALSE;
		button->m_bTopImage = FALSE;

		if(title.Replace(_T("�"), _T("\n")) > 0)
		{
			title = _T("\n") + title;
		}
		button->SetWindowTextW(title);

		bitmap = m_Path + bitmap;
		HBITMAP picture = (HBITMAP) ::LoadImage(AfxGetInstanceHandle(), bitmap, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		if(picture)
		{
			button->SetImage(picture);
		}
		else
		{
			button->SetImage(IDB_PYRAMID);
		}
	}	
	buttonNum++;
}

void WelcomePages::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AVI_1, cButton1);
	DDX_Control(pDX, IDC_AVI_2, cButton2);
	DDX_Control(pDX, IDC_AVI_3, cButton3);
	DDX_Control(pDX, IDC_AVI_4, cButton4);
	DDX_Control(pDX, IDC_AVI_5, cButton5);
	DDX_Control(pDX, IDC_AVI_6, cButton6);
	DDX_Control(pDX, IDC_AVI_7, cButton7);
	DDX_Control(pDX, IDC_AVI_8, cButton8);
	DDX_Control(pDX, IDC_AVI_9, cButton9);
	DDX_Control(pDX, IDC_AVI_10, cButton10);
	DDX_Control(pDX, IDC_WELCOME_TEXT, cText);
}


BEGIN_MESSAGE_MAP(WelcomePages, CDialog)
	ON_BN_CLICKED(IDC_AVI_1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_AVI_2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_AVI_3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_AVI_4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_AVI_5, OnBnClickedButton5)
	ON_BN_CLICKED(IDC_AVI_6, OnBnClickedButton6)
	ON_BN_CLICKED(IDC_AVI_7, OnBnClickedButton7)
	ON_BN_CLICKED(IDC_AVI_8, OnBnClickedButton8)
	ON_BN_CLICKED(IDC_AVI_9, OnBnClickedButton9)
	ON_BN_CLICKED(IDC_AVI_10, OnBnClickedButton10)
END_MESSAGE_MAP()


// WelcomePages message handlers

void WelcomePages::OnBnClickedButton1()
{
	Execute(1);
}

void WelcomePages::OnBnClickedButton2()
{
	Execute(2);
}

void WelcomePages::OnBnClickedButton3()
{
	Execute(3);
}

void WelcomePages::OnBnClickedButton4()
{
	Execute(4);
}

void WelcomePages::OnBnClickedButton5()
{
	Execute(5);
}

void WelcomePages::OnBnClickedButton6()
{
	Execute(6);
}

void WelcomePages::OnBnClickedButton7()
{
	Execute(7);
}

void WelcomePages::OnBnClickedButton8()
{
	Execute(8);
}

void WelcomePages::OnBnClickedButton9()
{
	Execute(9);
}

void WelcomePages::OnBnClickedButton10()
{
	Execute(10);
}

void WelcomePages::Execute(int id)
{
	CString filename(m_Path + avis[id-1]);
	ShellExecuteW(NULL, NULL, filename, NULL, NULL, SW_NORMAL);
}
