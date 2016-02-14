#pragma once

#include "ImagePreviewStatic.h"

class ImageChild :
	public CWnd
{
// Construction

// Attributes

// Operations
public:
	void SetPicture(CString m_Filename);

// Overrides
protected:
	CImagePreviewStatic m_Bitmap;

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);

// Implementation

// Generated message map functions
protected:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
};
