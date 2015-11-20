// InstructionTableEntry.h: interface for the CInstructionTableEntry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSTRUCTIONTABLEENTRY_H__4CC0B572_0A5F_418D_8595_59DC544431D6__INCLUDED_)
#define AFX_INSTRUCTIONTABLEENTRY_H__4CC0B572_0A5F_418D_8595_59DC544431D6__INCLUDED_

#include "windows.h"
#include "Str.h"
#include "Task.h"

class CDeclaration;

class CInstructionTableEntry  
{

	public:
		CInstructionTableEntry();
		virtual ~CInstructionTableEntry();
		void Free(void);

		void						Add(CInstructionTableEntry *pNew);
		void						Insert(CInstructionTableEntry *pNew);
		CInstructionTableEntry*		GetNext(void) { return m_pNext; }
		void						SetData(DWORD id, CStr* pStr, CStr* pDLL, CStr* pDecoratedName, CStr* pParamTypes, DWORD returnparam, DWORD param, DWORD dwInternalId, DWORD dwBuildID);
		void						SetReturnParamType(DWORD returnparam) { m_dwReturnParam=returnparam; }
		void						SetReturnParamPlace(DWORD place) { m_dwReturnPlace=place; }
		void						SetSpecialArrayParam(bool bState) { m_bSpecialArrayParam=bState; }
		void						SetFullParamDesc(CStr* pStr) { m_pParamDesc=pStr; }

		void						SetDecChain(CDeclaration* pRef) { m_pDecChain=pRef; }
		CDeclaration*				GetDecChain(void) { return m_pDecChain; }

		DWORD						GetInternalID(void) { return m_dwInternalID; }
		CStr*						GetName(void) { return m_pName; }
		CStr*						GetDLL(void) { return m_pDLL; }
		CStr*						GetDecoratedName(void) { return m_pDecoratedName; }
		CStr*						GetParamTypes(void) { return m_pParamTypes; }
		DWORD						GetReturnParam(void) { return m_dwReturnParam; }
		DWORD						GetParamMax(void) { return m_dwParamMax; }
		DWORD						GetHardcoreInternalValue(void) { return m_dwHardcoreInternalValue; }
		DWORD						GetBuildID(void) { return m_dwBuildID; }
		DWORD						GetReturnParamPlace(void) { return m_dwReturnPlace; }
		bool						GetSpecialArrayParam(void) { return m_bSpecialArrayParam; }
		CStr*						GetFullParamDesc(void) { return m_pParamDesc; }

	private:
		// Instruction Entry Data
		DWORD						m_dwInternalID;
		DWORD						m_dwReturnParam;
		DWORD						m_dwParamMax;
		CStr*						m_pName;
		CStr*						m_pDLL;
		CStr*						m_pDecoratedName;
		CStr*						m_pParamTypes;
		CStr*						m_pParamDesc;
		DWORD						m_dwHardcoreInternalValue;
		DWORD						m_dwBuildID;
		DWORD						m_dwReturnPlace;
		bool						m_bSpecialArrayParam;
		CDeclaration*				m_pDecChain;

		// Hierarchy Data
		CInstructionTableEntry*		m_pPrev;
		CInstructionTableEntry*		m_pNext;
};

#endif // !defined(AFX_INSTRUCTIONTABLEENTRY_H__4CC0B572_0A5F_418D_8595_59DC544431D6__INCLUDED_)
