#pragma once
#include <vector>
#include "SemanticParser.h"

class OldItems
{
public:
	CString		m_OldItem;
	bool		m_Claimed;
	HTREEITEM	m_TreeItem;

	OldItems(CString stringItem, HTREEITEM treeItem)
	{
		m_OldItem =		stringItem;
		m_Claimed =		false;
		m_TreeItem =	treeItem;
	}
};

class ClassView : public CBCGPDockingControlBar
{
public:
	ClassView(void);

private:
	CTreeCtrl	m_wndTree;
	CImageList  m_TreeImages;
	HTREEITEM	hRoot1, hFunctions, hErrors, hTypes, hLabels, hVariables, hConstants;

	bool m_bFirstUpdate;

	void OnChangeVisualStyle ();
	void Clear();
	void ClearExisting();
	void OnClick( NMHDR * pNotifyStruct, LRESULT * result );
	void OnContextMenu(CWnd* pWnd, CPoint point); // Supress

	CString treeTitle, functionsTitle, errorsTitle, typesTitle, labelsTitle, variablesTitle, constantsTitle;

	LRESULT OnAddText(WPARAM a, LPARAM b);
	LRESULT OnClear(WPARAM a, LPARAM b);
	LRESULT OnErrorMessage(WPARAM a, LPARAM b);

	// Iterators
	std::vector<SemanticMessage*>::iterator mySEVectorIterator; // Errors
	std::vector<SemanticFunction*>::iterator mySFVectorIterator; // Functions
	std::vector<SemanticType*>::iterator mySTVectorIterator; // Types
	std::vector<SemanticLabel*>::iterator mySLVectorIterator; // Labels
	std::vector<SemanticVariable*>::iterator mySVVectorIterator; // Variables
	std::vector<SemanticVariable*>::iterator mySCVectorIterator; // Constants

	// Previous values
	void ClearPrevious();
	std::vector<OldItems*> vErrors; // Errors
	std::vector<OldItems*> vFunctions; // Functions
	std::vector<OldItems*> vTypes; // Types
	std::vector<OldItems*> vLabels; // Labels
	std::vector<OldItems*> vVariables; // Variables
	std::vector<OldItems*> vConstants; // Constants

	//std::vector<OldItems*> vErrors2; // Errors
	//std::vector<OldItems*> vFunctions2; // Functions
	//std::vector<OldItems*> vTypes2; // Types
	//std::vector<OldItems*> vLabels2; // Labels
	//std::vector<OldItems*> vVariables2; // Variables
	//std::vector<OldItems*> vConstants2; // Constants

// Generated message map functions
protected:
	//{{AFX_MSG(CWorkSpaceBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
