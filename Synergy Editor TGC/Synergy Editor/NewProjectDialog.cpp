// NewProjectDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Synergy Editor.h"
#include "NewProjectDialog.h"
#include "Utilities.h"
#include "Settings.h"

// NewProjectDialog dialog

IMPLEMENT_DYNAMIC(NewProjectDialog, CDialog)

NewProjectDialog::NewProjectDialog(CWnd* pParent /*=NULL*/)
	: CDialog(NewProjectDialog::IDD, pParent)
{
	stickyIncludes = false;
}

NewProjectDialog::~NewProjectDialog()
{}

BOOL NewProjectDialog::PreTranslateMessage(MSG* pMsg)
{
     m_ToolTip.RelayEvent(pMsg);

     return CDialog::PreTranslateMessage(pMsg);
}

BOOL NewProjectDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	cTemplates.InsertString(-1, _T("Empty Project"));

	templates = Utilities::ExtractPath(Utilities::GetApplicationPath()) + _T("Templates\\");
	if(Utilities::CheckPathExists(templates))
	{
		TCHAR temp[MAX_PATH];
		DWORD tempLength = MAX_PATH;
		GetCurrentDirectory(tempLength, temp);
		CString currentDir(temp);
		SetCurrentDirectory(templates);

		CFileFind finder;
		BOOL bWorking = finder.FindFile(_T("*.dba"));
		while(bWorking)
		{
			bWorking = finder.FindNextFile();
			CString filename(finder.GetFileName());

			cTemplates.AddString(filename);
		}

		SetCurrentDirectory(temp);
	}

	cTemplates.SetCurSel(0);

	cLocation.SetWindowTextW(Settings::DBPLocation + _T("Projects\\"));

	cCreateDirectory.SetCheck(BST_CHECKED);

	cOK.EnableWindow(FALSE);

	if(Settings::StickyIncludes != _T(""))
	{
		cStickyIncludes.SetCheck(BST_CHECKED);
	}
	else 
	{
		cStickyIncludes.EnableWindow(FALSE);
	}	

	//Create the ToolTip control
	if( !m_ToolTip.Create(this))
	{
	   TRACE0("Unable to create the ToolTip!");
	}
	else
	{
	  m_ToolTip.AddTool( &cTemplates, _T("Choose a template to base your project on"));
	  m_ToolTip.AddTool( &cName, _T("Enter the name of your project"));
	  m_ToolTip.AddTool( &cLocation, _T("Enter or choose the location of your project"));
	  m_ToolTip.AddTool( &cCreateDirectory, _T("Click to allow DarkBASIC Professional Editor to create a subdirectory with your projects name"));
	  m_ToolTip.AddTool( &cSkip, _T("Click to skip this stage and create a new blank project"));
	  m_ToolTip.AddTool( &cOK, _T("Click to create a new project based on your entries"));
	  m_ToolTip.AddTool( &cCancel, _T("Click to cancel this process, and return without creating a project"));
	  m_ToolTip.AddTool( &cStickyIncludes, _T("Click to automatically add any Sticky Includes to this project"));
	  m_ToolTip.AddTool( &cBrowseFolder, _T("Click to browse for a folder to save your project"));

	  m_ToolTip.Activate(TRUE);
	}

	return TRUE;
}

void NewProjectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TEMPLATES, cTemplates);
	DDX_Control(pDX, IDC_PROJECT_NAME, cName);
	DDX_Control(pDX, IDC_BROWSE_PROJECT, cBrowseFolder);
	DDX_Control(pDX, IDC_PROJECT_LOCATION, cLocation);
	DDX_Control(pDX, IDC_BTN_SKIP, cSkip);
	DDX_Control(pDX, IDOK, cOK);
	DDX_Control(pDX, IDCANCEL, cCancel);
	DDX_Control(pDX, IDC_CHECK1, cCreateDirectory);
	DDX_Control(pDX, IDC_ADD_STICKY_INCLUDES, cStickyIncludes);
}

void NewProjectDialog::OnOK()
{
	cName.GetWindowTextW(projectName);
	cLocation.GetWindowTextW(projectPath);

	if(projectPath.Right(1) != _T("\\"))
	{
		projectPath += _T("\\");
	}

	if(Utilities::CheckPathExists(projectPath) && !Utilities::CheckAccess(projectPath, Utilities::O_RDWR))
	{
		AfxMessageBox(_T("The selected path cannot be used because DarkBASIC Professional Editor does not have permission to access this directory. You should either:\n1). Ensure your project is in a path with write permissions\n2). Run DarkBASIC Professional Editor with administrative permissions"), MB_ICONERROR | MB_OK);
		return;
	}

	if(cCreateDirectory.GetCheck()  == BST_CHECKED)
	{
		projectPath += (projectName + _T("\\"));
		if(!Utilities::CheckPathExists(projectPath))
		{
			if(!CreateDirectory(projectPath, NULL))
			{
				if(AfxMessageBox(_T("Could not create directory, do you wish to continue?\n\nIf you are using Windows Vista/7 then either:\n1). Ensure your project is in a path with write permissions\n2). Run DarkBASIC Professional Editor with administrative permissions"), MB_ICONERROR | MB_YESNO) == IDNO)
				{
					return;
				}
			}
		}
	}

	if(!Utilities::CheckPathExists(projectPath))
	{
		AfxMessageBox(_T("The directory specified does not exist"), MB_ICONERROR | MB_OK);
		return;
	}
	
	cTemplates.GetText(cTemplates.GetCurSel(), templateName);
	if(templateName != _T("Empty Project"))
	{
		templateName = templates + templateName;
	}

	stickyIncludes = (cStickyIncludes.GetCheck() == BST_CHECKED);

	CDialog::OnOK();
}

void NewProjectDialog::OnCancel()
{
	projectName = _T("Cancel");
	CDialog::OnCancel();
}

BEGIN_MESSAGE_MAP(NewProjectDialog, CDialog)
	ON_BN_CLICKED(IDC_BTN_SKIP, &NewProjectDialog::OnBnClickedBtnSkip)
	ON_BN_CLICKED(IDC_BROWSE_PROJECT, &NewProjectDialog::OnBnClickedBtnBrowse)
	ON_EN_CHANGE(IDC_PROJECT_NAME, &NewProjectDialog::OnEnChangeProjectName)
	ON_EN_CHANGE(IDC_PROJECT_LOCATION, &NewProjectDialog::OnEnChangeProjectLocation)
	ON_LBN_SELCHANGE(IDC_LIST_TEMPLATES, &NewProjectDialog::OnLbnSelchangeListTemplates)
END_MESSAGE_MAP()


// NewProjectDialog message handlers

void NewProjectDialog::OnBnClickedBtnSkip()
{
	CDialog::OnCancel();
}

void NewProjectDialog::OnBnClickedBtnBrowse()
{
	CString path;
	if(!Utilities::ShowBrowseWindow(path, _T("Please select a folder and press 'OK'."), Settings::DBPLocation + _T("Projects\\")))
	{
		return;
	}		
	SetWindowTextW(path);
}

void NewProjectDialog::CheckCanOK()
{
	cOK.EnableWindow(FALSE);

	int nIndex = cTemplates.GetCurSel();
	if(nIndex == -1) 
	{
		return;
	}

	cName.GetWindowTextW(projectName);
	if(projectName == _T(""))
	{
		return;
	}

	cName.GetWindowTextW(projectPath);
	if(projectPath == _T(""))
	{
		return;
	}

	cOK.EnableWindow(TRUE);
}

void NewProjectDialog::OnEnChangeProjectName()
{
	CheckCanOK();
}

void NewProjectDialog::OnEnChangeProjectLocation()
{
	CheckCanOK();
}

void NewProjectDialog::OnLbnSelchangeListTemplates()
{
	CheckCanOK();
}
