#include "stdafx.h"
#include "Synergy Editor.h"
#include "UpdateWindow.h"
#include "UpdateFtpCore.h"
#include "Utilities.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// UpdateTool dialog
IMPLEMENT_DYNAMIC(UpdateTool, CDialog)

UpdateTool::UpdateTool(CWnd* pParent /*=NULL*/) : CDialog(UpdateTool::IDD, pParent) 
{
	ResetVals(false);
}

BOOL UpdateTool::OnInitDialog()
{
   CDialog::OnInitDialog();

	LOGFONT lfNew;	
	ZeroMemory (&lfNew, sizeof(LOGFONT));
	lfNew.lfHeight = 22;
	lfNew.lfWeight = FW_BOLD;
	
	cNew.CreateFontIndirect(&lfNew);

	cTitle.SetFont(&cNew, TRUE);

	cUpdateList.InsertColumn(0, _T("Install"), LVCFMT_CENTER, 50);
	cUpdateList.InsertColumn(1, _T("Name"), LVCFMT_LEFT, 193);
	cUpdateList.InsertColumn(2, _T("Type"), LVCFMT_LEFT, 60);
	cUpdateList.InsertColumn(3, _T("Version"), LVCFMT_LEFT, 50);
	cUpdateList.InsertColumn(4, _T("Size"), LVCFMT_LEFT, 50);
	cUpdateList.SetExtendedStyle(cUpdateList.GetExtendedStyle() | LVS_EX_CHECKBOXES | LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT);
	
	cInstall.EnableWindow(FALSE);

	cProgress.ShowWindow(SW_HIDE);

	CString guidFile = Utilities::ExtractPath(Utilities::GetApplicationPath()) + _T("Updates.txt");
	if(Utilities::CheckFileExists(guidFile))
	{
		CStdioFile read;
		BOOL res = read.Open(guidFile, CFile::modeRead);	
		if(res)
		{		
			CString guid;
			while(read.ReadString(guid))
			{
				m_GUIDs.push_back(guid);
			}
		}
	}	

	ShowWindow(SW_SHOW); 
	CenterWindow();

	CUpdateFtpCore::InitSession(this);
	
	thread = AfxBeginThread(CheckForUpdateThread, this);

	return TRUE;
}

UINT UpdateTool::CheckForUpdateThread(LPVOID pParam)
{
	int res = CUpdateFtpCore::CheckForUpdate(); 
	if(res == 1)
	{		
		return 0; // Cancelled
	}

	UpdateTool *wnd = (UpdateTool*)pParam;

	if(wnd->cUpdateList.GetItemCount() == 0)
	{
		wnd->cTitle.SetWindowTextW(_T("Software is up-to-date"));
		AfxMessageBox(_T("Your software is up-to-date"), MB_ICONINFORMATION | MB_OK);
	}
	else if(res == 2)
	{
		wnd->cTitle.SetWindowTextW(_T("Connection Error"));
		AfxMessageBox(_T("Could not connect to update server"), MB_ICONERROR | MB_OK);
	}
	else
	{
		wnd->cTitle.SetWindowTextW(_T("Updates are available"));
	}

	return 0; // Finished
}

void UpdateTool::AddItem(CString Name, CString Type, CString Version, CString Size, CString Help, CString File, CString GUID)
{
	int pos = cUpdateList.GetItemCount();
	cUpdateList.InsertItem(pos, _T(""));

	cUpdateList.SetItemText(pos, 1, Name);
	cUpdateList.SetItemText(pos, 2, Type);
	cUpdateList.SetItemText(pos, 3, Version);
	cUpdateList.SetItemText(pos, 4, Size);

	m_Info.push_back(new UpdateInfo(File, Help, GUID));
}

// UpdateTool message handlers
void UpdateTool::OnLvnItemchangedUpdateList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	UINT pos = pNMLV->iItem;
	if(pos < m_Info.size())
	{
		cUpdateInfo.SetWindowTextW(m_Info.at(pNMLV->iItem)->m_HelpText);
	}

	BOOL enable = false;
	for(int i=0; i < cUpdateList.GetItemCount(); i++)
	{
		if(ListView_GetCheckState(cUpdateList.GetSafeHwnd(), i))
		{			
			enable = true;
			break;
		}
	}
	cInstall.EnableWindow(!updating && enable);

	*pResult = 0;
}

void UpdateTool::OnItemchanging(NMHDR* pNMHDR, LRESULT* pResult)
{
	// return FALSE (or 0) to allow change, TRUE to prevent
	*pResult = updating;
}

void UpdateTool::OnCancel()
{
	if(updating)
	{
		if(AfxMessageBox(_T("An update is in process, are you sure you wish to cancel?"), MB_YESNO | MB_ICONQUESTION) == IDNO)
		{
			return;
		}
		WriteCompleted();
	}
	CUpdateFtpCore::Cancel();
	WaitForSingleObject(thread->m_hThread, -1);
	CDialog::OnCancel();
}

void UpdateTool::WriteCompleted()
{
	CString guidFile = Utilities::ExtractPath(Utilities::GetApplicationPath()) + _T("Updates.txt");
	CStdioFile write;
	BOOL res;
	if(!Utilities::CheckFileExists(guidFile))
	{
		res = write.Open(guidFile, CFile::modeReadWrite | CFile::modeCreate);
	}
	else
	{
		res = write.Open(guidFile, CFile::modeReadWrite);
	}
	if(res)
	{		
		write.SeekToEnd();
		for(UINT j=0; j < m_CompletedGUIDs.size(); j++)
		{
			write.WriteString(m_CompletedGUIDs.at(j) + _T("\n"));
		}
	}
}

LRESULT UpdateTool::AddAmount(WPARAM a, LPARAM b)
{
	total += (DWORD)a;
	CString text;
	// leechange - 011008 - more resolution for most downloads
	//text.Format(_T("Downloaded %d bytes"), total);
	text.Format(_T("Downloaded %dKB"), total/1024);
	cProgress.SetWindowText(text);
	cProgress.Invalidate();
	return TRUE;
}

void UpdateTool::DoLastUpdate()
{
	if(lastUpdate > -1)
	{
		m_CompletedGUIDs.push_back(m_Info.at(lastUpdate)->m_GUID);
		m_Deletes.push_back(lastUpdate);
	}
}

LRESULT UpdateTool::DoNext(WPARAM a, LPARAM b)
{
	DoLastUpdate(); // Will fail first time, but needed here to confirm last download completed	
	for(int i=pos; i < cUpdateList.GetItemCount(); i++)
	{
		if(ListView_GetCheckState(cUpdateList.GetSafeHwnd(), i))
		{		
			lastUpdate = i;
			//CHttpUpdater::DownloadSelected(m_Info.at(i)->m_File);	
			CUpdateFtpCore::DownloadSelected(m_Info.at(i)->m_File);
			pos = i+1;			
			count++;
			return TRUE;
		}
	}

	//CHttpUpdater::EndSession(); // Finished now

	WriteCompleted();
	
	if(count == m_Info.size())
	{
		AfxMessageBox(_T("All updates were successfully installed, please restart all instances of this program for the changes to take effect."), MB_ICONINFORMATION | MB_OK);
		CDialog::OnOK();
	}
	else
	{
		for(int p = (((int)m_Deletes.size())-1); p >= 0; p--)
		{
			cUpdateList.DeleteItem(m_Deletes.at(p));
		}		
		m_Deletes.clear();

		cProgress.ShowWindow(SW_HIDE);
		cInstall.EnableWindow(true);
		ResetVals(false);
		AfxMessageBox(_T("The chosen updates were successfully installed, please restart all instances of this program for the changes to take effect. The files you did not choose to update are available to download now or at a later date."), MB_ICONINFORMATION | MB_OK);
	}
	
	return TRUE;
}

// Resets the variables used in each update session
void UpdateTool::ResetVals(bool updatingval)
{
	updating = updatingval;
	lastUpdate = -1;
	total = 0;
	count = 0;
	pos = 0;
}

void UpdateTool::OnBnClickedInstall()
{
	cInstall.EnableWindow(false);
	ResetVals(true);
	cProgress.ShowWindow(SW_SHOW);
	DoNext(0,0);	
}

LRESULT UpdateTool::Failed(WPARAM a, LPARAM b)
{
	cProgress.ShowWindow(SW_HIDE);
	cInstall.EnableWindow(true);
	ResetVals(false);

	if((int)a == 1)
	{
		AfxMessageBox(_T("The update process failed. Please check your internet settings and try again later"), MB_ICONERROR | MB_OK);
	}

	return TRUE;
}

UpdateTool::~UpdateTool() 
{
	ClearMemory();
}

void UpdateTool::ClearMemory()
{
	for(UINT i=0; i < m_Info.size(); i++)
	{
		delete m_Info.at(i);
	}
	m_Info.clear();
	m_Deletes.clear();
	m_CompletedGUIDs.clear();
}

void UpdateTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_UPDATE_INFO, cUpdateInfo);
	DDX_Control(pDX, IDC_UPDATE_LIST, cUpdateList);
	DDX_Control(pDX, IDC_TITLE, cTitle);
	DDX_Control(pDX, ID_INSTALL, cInstall);
	DDX_Control(pDX, IDC_PROGRESS, cProgress);
}

BEGIN_MESSAGE_MAP(UpdateTool, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_UPDATE_LIST, OnLvnItemchangedUpdateList)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_UPDATE_LIST, OnItemchanging)
	ON_BN_CLICKED(ID_INSTALL, OnBnClickedInstall)

	ON_MESSAGE(WM_USER + 1, DoNext)
	ON_MESSAGE(WM_USER + 2, AddAmount)
	ON_MESSAGE(WM_USER + 3, Failed)
END_MESSAGE_MAP()
