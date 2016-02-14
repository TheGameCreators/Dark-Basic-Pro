#pragma once

#include "Thread.h"

// CAdBanner

class CAdBanner : public CButton, public Thread
{
	DECLARE_DYNAMIC(CAdBanner)

public:
	CAdBanner();
	virtual ~CAdBanner();

	void SetLoading( bool loading ) { m_bLoading = loading; Invalidate(); }

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd( CDC *pDC );

	unsigned Run( );

	CBitmap m_bmpBanner;
	bool m_bBitmapLoaded;
	bool m_bLoading;
};


