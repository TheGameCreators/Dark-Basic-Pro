#pragma once

// CImagePrieviewStatic
class CImagePreviewStatic :	public CStatic
{
	DECLARE_DYNAMIC(CImagePreviewStatic)
public:
					CImagePreviewStatic();
	virtual			~CImagePreviewStatic();

	void			SetFilename(CString szFilename, bool stretch);
	CPoint			GetSize();

protected:
	CString			sFilename;
	Image			*m_img;
	Graphics		*m_graphics;
	bool			stretch;
	bool			deleted;

	CPoint			m_BitmapSize;

	void OnPaint() ;
	int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
};


