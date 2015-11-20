// StructTable.h: interface for the CStructTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRUCTTABLE_H__0EDF6884_E537_492E_806D_71DD644FE9B4__INCLUDED_)
#define AFX_STRUCTTABLE_H__0EDF6884_E537_492E_806D_71DD644FE9B4__INCLUDED_
#include "ParserHeader.h"

#include "PerfMacros.h"

#ifdef __AARON_STRUCPERF__
# include "DB3Dict.h"
#endif

class CStructTable  
{
	public:
		CStructTable();
		virtual ~CStructTable();
		void Free(void);

		void Add(CStructTable* pNew);
		CStructTable* GetNext(void) { return m_pNext; }

		void			SetStructDefaults(void);

		bool			SetStruct(DWORD dwValue, LPSTR pStructName, unsigned char cStructChar, DWORD dwSize);
		bool			AddStruct(DWORD dwValue, LPSTR pStructName, unsigned char cStructChar, DWORD dwSize);
		bool			AddStructUserType(DWORD dwMode, LPSTR pStructName, unsigned char cStructChar, CDeclaration* pDecChain, CStatement* pTypeBlock, DWORD dwStructTypeMode);
		bool			AddStructUserType(DWORD dwMode, LPSTR pStructName, unsigned char cStructChar, CDeclaration* pDecChain, CStatement* pTypeBlock, DWORD dwStructTypeMode, bool* bReportError );
		bool			AddStructUserType(DWORD dwMode, LPSTR pStructName, unsigned char cStructChar, CDeclaration* pDecChain, CStatement* pTypeBlock, DWORD dwStructTypeMode, bool* pbReportError, DWORD dwParamInUserFunction );

		void			SetTypeMode(DWORD dwMode) { m_dwTypeMode=dwMode; }
		void			SetTypeValue(DWORD dwValue) { m_dwTypeValue=dwValue; }
		void			SetTypeName(CStr* pName) { m_pTypeName=pName; }
		void			SetTypeChar(unsigned char cChar) { m_cTypeChar=cChar; }
		void			SetTypeSize(DWORD dwSize) { m_dwSize=dwSize; }
		void			SetDecChain(CDeclaration* pDec) { m_pDecChain=pDec; }
		void			SetTypeBlock(CStatement* pBlock) { m_pDecBlock=pBlock; }
		void			SetParamInUserFunction(DWORD dwCount) { m_dwParamInUserFunction=dwCount; }

		DWORD			GetTypeMode(void) { return m_dwTypeMode; }
		DWORD			GetTypeValue(void) { return m_dwTypeValue; }
		CStr*			GetTypeName(void) { return m_pTypeName; }
		unsigned char	GetTypeChar(void) { return m_cTypeChar; }
		DWORD			GetTypeSize(void) { return m_dwSize; }
		CStatement*		GetBlock(void) { return m_pDecBlock; }
		CDeclaration*	GetDecChain(void) { return m_pDecChain; }
		DWORD			GetParamInUserFunction(void) { return m_dwParamInUserFunction; }

		bool			CalculateAllSizes(void);
		bool			CalculateSize(void);
		CStructTable*	DoesTypeEvenExist(LPSTR pName);
		DWORD			GetSizeOfType(LPSTR pName);
		CDeclaration*	FindDecInType(LPSTR pTypename, LPSTR pFieldname);
		CDeclaration*	FindFieldInType(LPSTR pTypename, LPSTR pFieldname, LPSTR* pReturnType, DWORD* pdwArrFlag, DWORD* pdwOffset);
		bool			FindOffsetFromField(LPSTR pTypename, LPSTR pFieldname, DWORD* pReturnOffset, DWORD* dwSizeData);
		int				FindIndex(LPSTR pTypename);

		bool			WriteDBMHeader(void);
		bool			WriteDBM(void);

	private:

		// Structure Type Data
		DWORD			m_dwTypeMode;
		DWORD			m_dwTypeValue;
		CStr*			m_pTypeName;
		unsigned char	m_cTypeChar;
		DWORD			m_dwSize;
		DWORD			m_dwParamInUserFunction;

		// Chaining Pointer
		CDeclaration*	m_pDecChain;

		// Stores Link to Type Object
		CStatement*		m_pDecBlock;

		// Hierarchy Data
		CStructTable*	m_pNext;

#ifdef __AARON_STRUCPERF__
		static db3::TDictionary<CStructTable> g_Table;
#endif
};

#endif // !defined(AFX_STRUCTTABLE_H__0EDF6884_E537_492E_806D_71DD644FE9B4__INCLUDED_)
