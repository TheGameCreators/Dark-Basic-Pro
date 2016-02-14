#pragma once

class Doc : public CDocument
{
protected: // create from serialization only
	Doc();
	DECLARE_DYNCREATE(Doc)

	CString m_OldName;

	// The time the file was last written to, used to check that we are editing the last one
	CTime m_LastWrite; 

	bool saved;

	// User has said no to reload file
	bool m_IgnoreChangeCheck;
	
	void UpdateTitle(CString name);	

// Attributes
public:

// Operations
public:
	CString GetText();
	void HighlightLine(int line);	
	bool IsABreakpoint(int line);
	void CheckFileStatus();
// Overrides
public:
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnNewDocument();
	virtual void SetModifiedFlag(BOOL bModified);

// Implementation
public:
	virtual ~Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


