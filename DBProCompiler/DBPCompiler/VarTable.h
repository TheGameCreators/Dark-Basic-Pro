// VarTable.h: interface for the CVarTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VARTABLE_H__4910A987_6F89_44BE_BCB7_3DE3DEDD217B__INCLUDED_)
#define AFX_VARTABLE_H__4910A987_6F89_44BE_BCB7_3DE3DEDD217B__INCLUDED_
#include "ParserHeader.h"

#include "PerfMacros.h"

#ifdef __AARON_VARTABLEPERF__
# include <DB3Dict.h>
#endif

class CVarTable  
{
	public:
		CVarTable();
		CVarTable(LPSTR pStr);
		virtual ~CVarTable();
		void Free(void);

		void Add(CVarTable* pNew);
		void Insert(CVarTable* pNew);
		void AddInOrder(LPSTR pName, CVarTable* pNew);
		CVarTable* Advance(DWORD dwCountdown);
		CVarTable* Subtract(DWORD dwCountdown);
		CVarTable* GetNext(void) { return m_pNext; }
		CVarTable* GetPrev(void) { return m_pPrev; }
		void SetVarDefaults(void);

		bool			AddVariable(LPSTR pName, LPSTR pType, DWORD dwArrFlag, DWORD dwLineNumber, bool bFromActualCodeNotFromTypeDefing, DWORD* pdwAction, bool bIsGlobal);
		CVarTable*		FindVariable(LPSTR pScope, LPSTR pName, DWORD dwArrFlag);
		bool			FindVariableExist(LPSTR pFindVar, DWORD dwArrType);
		bool			FindTypeOfVariable(LPSTR pFindVar, DWORD dwArrType, LPSTR* pReturnType);
		DWORD			MakeDefaultVarTypeValue(LPSTR pDecName);
		LPSTR			MakeDefaultVarType(LPSTR pDecName);
		LPSTR			MakeTypeNameOfTypeValue(DWORD dwTypeValue);
		DWORD			GetBasicTypeValue(LPSTR pTypeString);
		CStructTable*	GetStruct(LPSTR pTypeString);
		char			GetCharOfType(DWORD dwTypeValue);
		DWORD			GetTypeValueOfChar(unsigned char cTypeChar);

		void			SetVarScope(CStr* pScope) { m_pVarScope=pScope; }
		CStr*			GetVarScope(void) { return m_pVarScope; }
		void			SetVarName(CStr* pName) { m_pVarName=pName; }
		CStr*			GetVarName(void) { return m_pVarName; }
		void			SetVarType(CStr* pType) { m_pVarType=pType; }
		CStr*			GetVarType(void) { return m_pVarType; }
		void			SetVarTypeValue(DWORD dwTypeValue) { m_dwVarTypeValue=dwTypeValue; }
		DWORD			GetVarTypeValue(void) { return m_dwVarTypeValue; }
		void			SetVarStruct(CStructTable* pStruct) { m_pVarStruct=pStruct; }
		CStructTable*	GetVarStruct(void) { return m_pVarStruct; }
		void			SetArrFlag(DWORD dwFlag) { m_dwArrFlag=dwFlag; }
		DWORD			GetArrFlag(void) { return m_dwArrFlag; }
		DWORD			GetOffsetValue(void) { return m_dwFinalDBMOffset; }
		void			SetPreScanAddFlag(bool bState) { m_bPreScanAdd=bState; }
		bool			GetPreScanAddFlag(void) { return m_bPreScanAdd; }
		void			SetSpecifiedAsGlobalFlag(bool bState) { m_bSpecifiedAsGLOBAL=bState; }
		bool			GetSpecifiedAsGlobalFlag(void) { return m_bSpecifiedAsGLOBAL; }
		
		void			SetAdditionalDataString(CStr* pStr) { m_pAdditionalDataString=pStr; }
		CStr*			GetAdditionalDataString(void) { return m_pAdditionalDataString; }

		void			SetLineNumber(DWORD dwLine) { m_dwLineNumber=dwLine; }
		DWORD			GetLineNumber(void) { return m_dwLineNumber; }
	
		bool			VerifyVariableStructures(void);
		DWORD			EstablishVarOffsets(DWORD* pdwOffsetValue);

		bool			WriteDBMHeader(void);
		bool			WriteDBM(void);
		bool			WriteDBMFooter(DWORD dwSizeOfVariableBuffer);

	private:

		// Debug Data
		DWORD			m_dwLineNumber;

		// Variable Data
		CStr*			m_pVarScope;
		CStr*			m_pVarName;
		CStr*			m_pVarType;
		DWORD			m_dwVarTypeValue;
		CStructTable*	m_pVarStruct;
		DWORD			m_dwArrFlag;
		DWORD			m_dwFinalDBMOffset;
		bool			m_bPreScanAdd;
		bool			m_bSpecifiedAsGLOBAL;

		// Used to hold array dimension string (for parser use)
		CStr*			m_pAdditionalDataString;

		// Mini-CLI Program Support
		bool			m_bOffsetAssigned;

		// Hierarchy Data
		CVarTable*		m_pNext;
		CVarTable*		m_pPrev;

		// Dictionary
#ifdef __AARON_VARTABLEPERF__
		static db3::TDictionary<CVarTable> g_Table;
#endif
};

#endif // !defined(AFX_VARTABLE_H__4910A987_6F89_44BE_BCB7_3DE3DEDD217B__INCLUDED_)
