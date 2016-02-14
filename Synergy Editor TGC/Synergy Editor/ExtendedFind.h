#pragma once

#include "Resource.h"
#include "afxwin.h"

// ExtendedFind dialog

class View;

class ExtendedFind : public CFindReplaceDialog
{
	DECLARE_DYNAMIC(ExtendedFind)

public:
	ExtendedFind(CWnd* pParent = NULL);   // standard constructor
	virtual ~ExtendedFind();

// Dialog Data
	enum { IDD = IDD_FIND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CButton cSelected;
	View* m_pParent;
};
