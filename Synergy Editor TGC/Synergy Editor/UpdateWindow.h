#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "Resource.h"
#include <vector>

// UpdateTool dialog

class UpdateInfo
{
public:
	UpdateInfo(CString File, CString HelpText, CString GUID)
	{
		m_File = File;
		m_HelpText = HelpText;
		m_GUID = GUID;
	}

	CString m_HelpText;
	CString m_File;
	CString m_GUID;
};

class UpdateTool : public CDialog
{
	DECLARE_DYNAMIC(UpdateTool)

public:
	UpdateTool(CWnd* pParent = NULL);   // standard constructor
	virtual ~UpdateTool();

// Dialog Data
	enum { IDD = IDD_UPDATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();	
	virtual void OnCancel();

	afx_msg void OnItemchanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnItemchangedUpdateList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedInstall();

	void ClearMemory();

	CWinThread *thread;

	//void DoNext();
	LRESULT AddAmount(WPARAM a, LPARAM b);
	LRESULT DoNext(WPARAM a, LPARAM b);
	LRESULT Failed(WPARAM a, LPARAM b);
	void ResetVals(bool updatingval);
	void WriteCompleted();
	void DoLastUpdate();
	static UINT CheckForUpdateThread(LPVOID pParam);

	std::vector<UpdateInfo*> m_Info;	
	std::vector<int> m_Deletes;
	std::vector<CString> m_CompletedGUIDs;

	CFont cNew;

	CEdit cUpdateInfo;
	CListCtrl cUpdateList;
	CStatic cTitle;	
	CButton cInstall;

	int pos;
	int count;
	int lastUpdate;
	bool updating;

	DWORD total;

	DECLARE_MESSAGE_MAP()
public:
	void AddItem(CString Name, CString Type, CString Version, CString Size, CString Help, CString File, CString GUID);
	void AddAmount(DWORD Amount);

	std::vector<CString> m_GUIDs;
	CStatic cProgress;
};
