// DBMWriter.h: interface for the CDBMWriter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBMWRITER_H__C1FF6E3E_45BA_478C_88E2_D2CB3C061575__INCLUDED_)
#define AFX_DBMWRITER_H__C1FF6E3E_45BA_478C_88E2_D2CB3C061575__INCLUDED_

#include "windows.h"
#include "Str.h"

class CDBMWriter  
{
	public:
		CDBMWriter();
		virtual ~CDBMWriter();

		bool			OutputDBM(const char *pDBMStr, size_t length);
		bool			OutputDBM(CStr* pDBMStr);
		DWORD			EatCarriageReturn(void);
		bool			CheckAndExpandDBMMemory(DWORD dwLengthOfNewAddData);
		bool			WriteProgramAsEXEOrDEBUG(LPSTR lpEXEFilename, bool bParsingMainProgram);

		void			SetNewCodeFlag(bool bFlag) { m_bNewCodeToParse=bFlag; }
		bool			GetNewCodeFlag(void) { return m_bNewCodeToParse; }

	public:

		void			SetDBMDataPointer(LPSTR pData) { m_pDBMDataPointer=pData; }
		LPSTR			GetDBMDataPointer(void) { return m_pDBMDataPointer; }

	private:

		LPSTR			m_pDBMData;
		LPSTR			m_pDBMDataPointer;
		DWORD			m_dwDBMDataSize;
		bool			m_bNewCodeToParse;
};

#endif // !defined(AFX_DBMWRITER_H__C1FF6E3E_45BA_478C_88E2_D2CB3C061575__INCLUDED_)
