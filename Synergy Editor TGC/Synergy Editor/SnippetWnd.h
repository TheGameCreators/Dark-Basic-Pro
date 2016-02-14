#pragma once
#include "afxwin.h"

class SnippetWnd :
	public CMiniFrameWnd
{
	DECLARE_DYNCREATE(SnippetWnd)
public:
	SnippetWnd(bool surround);
	~SnippetWnd(void);

	virtual BOOL Create(
   LPCTSTR lpClassName,
   LPCTSTR lpWindowName,
   DWORD dwStyle,
   const RECT& rect,
   CWnd* pParentWnd,
   UINT nID = 0	);

	virtual void OnDestroy();
	virtual void PostNcDestroy();
	virtual BCGNcHitTestType OnNcHitTest(CPoint /*point*/) ;


protected:
	DECLARE_MESSAGE_MAP()
	void OnLbnKillfocusList1();
	void OnLbnDblclkList1();
	afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);


	CListBox *m_pLstBoxData;
	bool m_Surround;
	CWnd *m_pParentWnd;
	CFont m_lbFont;
};
