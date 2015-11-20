// StatementChain.h: interface for the CStatementChain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATEMENTCHAIN_H__F922139D_BB85_46FE_865C_E017740374F8__INCLUDED_)
#define AFX_STATEMENTCHAIN_H__F922139D_BB85_46FE_865C_E017740374F8__INCLUDED_
#include "ParserHeader.h"

class CStatementChain  
{
	public:
		CStatementChain();
		virtual ~CStatementChain();
		CStatementChain* GetNext(void) { return m_pNext; }
		void Add(CStatementChain* pChain);

		void SetStatementBlock(CStatement* pChain) { m_FirstStatmentBlock=pChain; }
		CStatement* GetStatementBlock(void) { return m_FirstStatmentBlock; }

	private:

		// Statement Block Data
		CStatement*				m_FirstStatmentBlock;


		// Hierarchy Data
		CStatementChain*		m_pNext;
};

#endif // !defined(AFX_STATEMENTCHAIN_H__F922139D_BB85_46FE_865C_E017740374F8__INCLUDED_)
