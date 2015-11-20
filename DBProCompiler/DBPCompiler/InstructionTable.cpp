// InstructionTable.cpp: implementation of the CInstructionTable class.
//
//////////////////////////////////////////////////////////////////////

// Take protection code from TGC Online folder
#include "..\\TGCOnline\\CertificateKey.h"

// Includes
#include "macros.h"
#include "InstructionTable.h"
#include "VarTable.h"
#include "direct.h"
#include "DBPCompiler.h"
#include "io.h"

// External Class Pointers
extern CVarTable* g_pVarTable;
extern CDBPCompiler* g_pDBPCompiler;

//
// Internal Support Functions
//

bool					m_bScanActive=false;
int						m_findex;
struct _finddata_t		m_filedata[200];
long					m_hInternalFile[200];
int						m_FileReturnValue[200];

void Init(void)
{
	m_findex=0;
	for(int n=0; n<199; n++)
	{
		m_hInternalFile[n]=NULL;
		m_FileReturnValue[n]=-1;
	}
}

void Free(void)
{
	for(int n=0; n<199; n++)
	{
		if(m_hInternalFile[n]) _findclose(m_hInternalFile[n]);
		m_FileReturnValue[n]=-1;
	}
}

void FFindCloseFile(void)
{
	_findclose(m_hInternalFile[m_findex]);
	m_hInternalFile[m_findex]=NULL;
}

void FFindFirstFile(LPSTR pExt)
{
	if(m_hInternalFile[m_findex]) FFindCloseFile();
	m_hInternalFile[m_findex] = _findfirst(pExt, &m_filedata[m_findex]);
	if(m_hInternalFile[m_findex]!=-1L)
	{
		// Success!
		m_FileReturnValue[m_findex]=0;
	}
}

int FGetFileReturnValue(void)
{
	return m_FileReturnValue[m_findex];
}

void FFindNextFile(void)
{
	m_FileReturnValue[m_findex] = _findnext(m_hInternalFile[m_findex], &m_filedata[m_findex]);
}

int FGetActualTypeValue(int flagvalue)
{
	if(flagvalue & _A_SUBDIR)
		return 1;
	else
		return 0;
}

bool FileExist(LPSTR pFilename)
{
	HANDLE hReadFile = CreateFile(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hReadFile!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(hReadFile);
		return true;
	}
	else
		return false;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define ALLOWED_LOWER "abcdefghijklmnopqrstuvwxyz"
#define ALLOWED_UPPER "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define ALLOWED_ALPHA ALLOWED_LOWER ALLOWED_UPPER
#define ALLOWED_DIGIT "0123456789"
#define ALLOWED_ALNUM ALLOWED_ALPHA ALLOWED_DIGIT
#define ALLOWED_IDENT ALLOWED_ALNUM "_"

#define ALLOWED_DBINT ALLOWED_IDENT "+#$% "
#define ALLOWED_DBUSR ALLOWED_IDENT "#$%"

CInstructionTable::CInstructionTable()
#ifdef __AARON_INSTRPERF__
: m_InstructionMap (ALLOWED_DBINT, map_type::ECase::Insensitive, map_type::EOnDestruct::DeleteEntriesAndData),
  m_UserFunctionMap(ALLOWED_DBUSR, map_type::ECase::Insensitive, map_type::EOnDestruct::DeleteEntriesAndData)
#endif
{
	m_dwCurrentInternalID=1000;

#ifndef __AARON_INSTRPERF__
	m_pFirstInstructionEntry=NULL;
	m_pFirstUserFunctionEntry=NULL;
#endif

	// Clear instruction table array
	ZeroMemory(m_InternalInstructions, sizeof(m_InternalInstructions));
	ZeroMemory(m_InternalInstructRef, sizeof(m_InternalInstructRef));
}

CInstructionTable::~CInstructionTable()
{
#ifdef __AARON_INSTRPERF__
#else
	if(m_pFirstInstructionEntry) m_pFirstInstructionEntry->Free();
	if(m_pFirstUserFunctionEntry) m_pFirstUserFunctionEntry->Free();
#endif
}

bool CInstructionTable::DefineHardCodedCommand(void)
{
	// Bitwise
	AddBuildCommand("+mathint", "SHLLLL", "L", 1, 1, IT_INTERNAL_SHIFTLLLL, BUILD_SHL);
	AddBuildCommand("+mathint", "SHRLLL", "L", 1, 1, IT_INTERNAL_SHIFTRLLL, BUILD_SHR);
	AddBuildCommand("+mathint", "BITORLLL", "L", 1, 1, IT_INTERNAL_BITORLLL, BUILD_BITOR);
	AddBuildCommand("+mathint", "BITANDLLL", "L", 1, 1, IT_INTERNAL_BITANDLLL, BUILD_BITAND);
	AddBuildCommand("+mathint", "BITXORLLL", "L", 1, 1, IT_INTERNAL_BITXORLLL, BUILD_BITXOR);
	AddBuildCommand("+mathint", "BITNOTLLL", "L", 1, 1, IT_INTERNAL_BITNOTLLL, BUILD_BITNOT);

	// Logic
	AddBuildCommand("+mathint", "ORLLL", "L", 1, 1, IT_INTERNAL_ORLLL, BUILD_OR);
	AddBuildCommand("+mathint", "ANDLLL", "L", 1, 1, IT_INTERNAL_ANDLLL, BUILD_AND);
	AddBuildCommand("+mathint", "NOTLLL", "L", 1, 1, IT_INTERNAL_NOTLLL, BUILD_NOT);
	AddBuildCommand("+mathint", "CONDLLL", "LL", 1, 2, IT_INTERNAL_EQUALLLL, BUILD_EQUAL);
	AddBuildCommand("+mathint", "CONDLLL", "LL", 1, 2, IT_INTERNAL_NOTEQUALLLL, BUILD_NOTEQUAL);
	AddBuildCommand("+mathint", "CONDLLL", "LL", 1, 2, IT_INTERNAL_GREATERLLL, BUILD_GREATER);
	AddBuildCommand("+mathint", "CONDLLL", "LL", 1, 2, IT_INTERNAL_GREATEREQUALLLL, BUILD_GREATEREQUAL);
	AddBuildCommand("+mathint", "CONDLLL", "LL", 1, 2, IT_INTERNAL_LESSLLL, BUILD_LESS);
	AddBuildCommand("+mathint", "CONDLLL", "LL", 1, 2, IT_INTERNAL_LESSEQUALLLL, BUILD_LESSEQUAL);

	// External Maths
	AddCommandCore("+math", "dbprocore.dll", "?PowerLLL@@YAKHH@Z", "LL", 1, 2, IT_INTERNAL_POWERLLL, 0);
	AddCommandCore("+math", "dbprocore.dll", "?PowerBBB@@YAKKK@Z", "BB", 4, 2, IT_INTERNAL_POWERBBB, 0);
	AddCommandCore("+math", "dbprocore.dll", "?PowerBBB@@YAKKK@Z", "YY", 5, 2, IT_INTERNAL_POWERYYY, 0);
	AddCommandCore("+math", "dbprocore.dll", "?PowerWWW@@YAKKK@Z", "WW", 6, 2, IT_INTERNAL_POWERWWW, 0);
	AddCommandCore("+math", "dbprocore.dll", "?PowerDDD@@YAKKK@Z", "DD", 7, 2, IT_INTERNAL_POWERDDD, 0);

	// INT Math
	AddBuildCommand("+math", "MULLLL", "LL", 1, 2, IT_INTERNAL_MULLLL, BUILD_MUL);
	AddBuildCommand("+math", "DIVLLL", "LL", 1, 2, IT_INTERNAL_DIVLLL, BUILD_DIV);
	AddBuildCommand("+math", "ADDLLL", "LL", 1, 2, IT_INTERNAL_ADDLLL, BUILD_ADD);
	AddBuildCommand("+math", "SUBLLL", "LL", 1, 2, IT_INTERNAL_SUBLLL, BUILD_SUB);
	AddBuildCommand("+math", "MODLLL", "LL", 1, 2, IT_INTERNAL_MODLLL, BUILD_MOD);

	// BOOLEAN Math
	AddBuildCommand("+math", "MULBBB", "BB", 1, 2, IT_INTERNAL_MULBBB, BUILD_MUL);
	AddBuildCommand("+math", "DIVBBB", "BB", 1, 2, IT_INTERNAL_DIVBBB, BUILD_DIV);
	AddBuildCommand("+math", "ADDBBB", "BB", 1, 2, IT_INTERNAL_ADDBBB, BUILD_ADD);
	AddBuildCommand("+math", "SUBBBB", "BB", 1, 2, IT_INTERNAL_SUBBBB, BUILD_SUB);
	AddBuildCommand("+math", "MODBBB", "BB", 1, 2, IT_INTERNAL_MODBBB, BUILD_MOD);

	// BYTE Math
	AddBuildCommand("+math", "MULYYY", "YY", 1, 2, IT_INTERNAL_MULYYY, BUILD_MUL);
	AddBuildCommand("+math", "DIVYYY", "YY", 1, 2, IT_INTERNAL_DIVYYY, BUILD_DIV);
	AddBuildCommand("+math", "ADDYYY", "YY", 1, 2, IT_INTERNAL_ADDYYY, BUILD_ADD);
	AddBuildCommand("+math", "SUBYYY", "YY", 1, 2, IT_INTERNAL_SUBYYY, BUILD_SUB);
	AddBuildCommand("+math", "MODYYY", "YY", 1, 2, IT_INTERNAL_MODYYY, BUILD_MOD);

	// WORD Math
	AddBuildCommand("+math", "MULWWW", "WW", 1, 2, IT_INTERNAL_MULWWW, BUILD_MUL);
	AddBuildCommand("+math", "DIVWWW", "WW", 1, 2, IT_INTERNAL_DIVWWW, BUILD_DIV);
	AddBuildCommand("+math", "ADDWWW", "WW", 1, 2, IT_INTERNAL_ADDWWW, BUILD_ADD);
	AddBuildCommand("+math", "SUBWWW", "WW", 1, 2, IT_INTERNAL_SUBWWW, BUILD_SUB);
	AddBuildCommand("+math", "MODWWW", "WW", 1, 2, IT_INTERNAL_MODWWW, BUILD_MOD);

	// DWORD Math
	AddBuildCommand("+math", "MULDDD", "DD", 1, 2, IT_INTERNAL_MULDDD, BUILD_MUL);
	AddBuildCommand("+math", "DIVDDD", "DD", 1, 2, IT_INTERNAL_DIVDDD, BUILD_DIV);
	AddBuildCommand("+math", "ADDDDD", "DD", 1, 2, IT_INTERNAL_ADDDDD, BUILD_ADD);
	AddBuildCommand("+math", "SUBDDD", "DD", 1, 2, IT_INTERNAL_SUBDDD, BUILD_SUB);
	AddBuildCommand("+math", "MODDDD", "DD", 1, 2, IT_INTERNAL_MODDDD, BUILD_MOD);

	// Quantity INC and DEC
	AddBuildCommand("inc", "INCLL", "LL", 0, 2, IT_INTERNAL_INCVAR, BUILD_INC);
	AddBuildCommand("inc", "INCFF", "FF", 0, 2, IT_INTERNAL_INCVAR, BUILD_INCADD);
	AddBuildCommand("inc", "INCBB", "BB", 0, 2, IT_INTERNAL_INCVAR, BUILD_INC);
	AddBuildCommand("inc", "INCYY", "YY", 0, 2, IT_INTERNAL_INCVAR, BUILD_INC);
	AddBuildCommand("inc", "INCWW", "WW", 0, 2, IT_INTERNAL_INCVAR, BUILD_INC);
	AddBuildCommand("inc", "INCDD", "DD", 0, 2, IT_INTERNAL_INCVAR, BUILD_INC);
	AddBuildCommand("inc", "INCRR", "RR", 0, 2, IT_INTERNAL_INCVAR, BUILD_INCADD);
	AddBuildCommand("inc", "INCOO", "OO", 0, 2, IT_INTERNAL_INCVAR, BUILD_INCADD);
	AddBuildCommand("dec", "DECLL", "LL", 0, 2, IT_INTERNAL_DECVAR, BUILD_DEC);
	AddBuildCommand("dec", "DECFF", "FF", 0, 2, IT_INTERNAL_DECVAR, BUILD_DECADD);
	AddBuildCommand("dec", "DECBB", "BB", 0, 2, IT_INTERNAL_DECVAR, BUILD_DEC);
	AddBuildCommand("dec", "DECYY", "YY", 0, 2, IT_INTERNAL_DECVAR, BUILD_DEC);
	AddBuildCommand("dec", "DECWW", "WW", 0, 2, IT_INTERNAL_DECVAR, BUILD_DEC);
	AddBuildCommand("dec", "DECDD", "DD", 0, 2, IT_INTERNAL_DECVAR, BUILD_DEC);
	AddBuildCommand("dec", "DECRR", "RR", 0, 2, IT_INTERNAL_DECVAR, BUILD_DECADD);
	AddBuildCommand("dec", "DECOO", "OO", 0, 2, IT_INTERNAL_DECVAR, BUILD_DECADD);

	// Singular INC and DEC
	AddBuildCommand("inc", "INCL", "L", 0, 1, IT_INTERNAL_INCVAR, BUILD_INC);
	AddBuildCommand("inc", "INCF", "F", 0, 1, IT_INTERNAL_INCVAR, BUILD_INCADD);
	AddBuildCommand("inc", "INCB", "B", 0, 1, IT_INTERNAL_INCVAR, BUILD_INC);
	AddBuildCommand("inc", "INCY", "Y", 0, 1, IT_INTERNAL_INCVAR, BUILD_INC);
	AddBuildCommand("inc", "INCW", "W", 0, 1, IT_INTERNAL_INCVAR, BUILD_INC);
	AddBuildCommand("inc", "INCD", "D", 0, 1, IT_INTERNAL_INCVAR, BUILD_INC);
	AddBuildCommand("inc", "INCR", "R", 0, 1, IT_INTERNAL_INCVAR, BUILD_INCADD);
	AddBuildCommand("inc", "INCO", "O", 0, 1, IT_INTERNAL_INCVAR, BUILD_INCADD);
	AddBuildCommand("dec", "DECL", "L", 0, 1, IT_INTERNAL_DECVAR, BUILD_DEC);
	AddBuildCommand("dec", "DECF", "F", 0, 1, IT_INTERNAL_DECVAR, BUILD_DECADD);
	AddBuildCommand("dec", "DECB", "B", 0, 1, IT_INTERNAL_DECVAR, BUILD_DEC);
	AddBuildCommand("dec", "DECY", "Y", 0, 1, IT_INTERNAL_DECVAR, BUILD_DEC);
	AddBuildCommand("dec", "DECW", "W", 0, 1, IT_INTERNAL_DECVAR, BUILD_DEC);
	AddBuildCommand("dec", "DECD", "D", 0, 1, IT_INTERNAL_DECVAR, BUILD_DEC);
	AddBuildCommand("dec", "DECR", "R", 0, 1, IT_INTERNAL_DECVAR, BUILD_DECADD);
	AddBuildCommand("dec", "DECO", "O", 0, 1, IT_INTERNAL_DECVAR, BUILD_DECADD);

	// Complete
	return true;
}

bool CInstructionTable::SetInternalInstructionDatabase(void)
{
#ifdef __AARON_INSTRPERF__
# define BLANKCMD " "
#else
# define BLANKCMD ""
#endif
	// Internal DBM that uses Hard Coded Machine Code
	AddBuildCommand("return", "RET", "", 0, 0, IT_INTERNAL_RETURN, BUILD_RET);
	AddBuildCommand(BLANKCMD, "PURERET", "", 0, 0, IT_INTERNAL_PURERETURN, BUILD_PURERET);
	AddBuildCommand("sync", "", "", 0, 0, IT_INTERNAL_SYNC, BUILD_SYNC);
	AddBuildCommand("end", "", "", 0, 0, IT_INTERNAL_END, BUILD_END);
	AddBuildCommand("enderror", "", "", 0, 0, IT_INTERNAL_ENDERROR, BUILD_ENDERROR);
	AddBuildCommand(BLANKCMD, "STARTPROG", "", 0, 0, IT_INTERNAL_STARTPROGRAM, BUILD_STARTPROGRAM);
	AddBuildCommand(BLANKCMD, "ENDPROG", "", 0, 0, IT_INTERNAL_ENDPROGRAM, BUILD_ENDPROGRAMANDQUIT);

	// Optimisations from Hard Coded ASM
	DefineHardCodedCommand();

	// Internal Commands (generated internally)
	AddCommandCore("+exitfunction", "", "", "", 0, 0, IT_INTERNAL_USERFUNCTIONEXIT, BUILD_USERFUNCTIONEXIT);
	AddCommandCore("+allocate", "dbprocore.dll", "?DimDDD@@YAKKKKKKKKKKKK@Z", "DDDDDDDDDD", 7, 11, IT_INTERNAL_ALLOC, 0);
	AddCommandCore("+deallocate", "dbprocore.dll", "?UnDimDD@@YAKK@Z", "D", 7, 1, IT_INTERNAL_FREE, 0);
	AddCommandCore("+assign", "", "MOVLL", "LL", 0, 0, IT_INTERNAL_ASSIGNLL, 0);
	AddCommandCore("+assign", "", "MOVFF", "FF", 0, 0, IT_INTERNAL_ASSIGNFF, 0);
	AddCommandCore("+assign", "dbprocore.dll", "?EquateSS@@YAKKK@Z", "S", 3, 2, IT_INTERNAL_ASSIGNSS, 0);
	AddCommandCore("+free", "dbprocore.dll", "?FreeSS@@YAKK@Z", "S", 3, 1, IT_INTERNAL_STRFREE, 0);
	
	// Internal Assignments Commands
	AddCommandCore("+assign", "", "MOVBB", "BB", 0, 0, IT_INTERNAL_ASSIGNBB, 0);
	AddCommandCore("+assign", "", "MOVYY", "YY", 0, 0, IT_INTERNAL_ASSIGNYY, 0);
	AddCommandCore("+assign", "", "MOVWW", "WW", 0, 0, IT_INTERNAL_ASSIGNWW, 0);
	AddCommandCore("+assign", "", "MOVDD", "DD", 0, 0, IT_INTERNAL_ASSIGNDD, 0);
	AddCommandCore("+assign", "", "MOVOO", "OO", 0, 0, IT_INTERNAL_ASSIGNOO, 0);
	AddCommandCore("+assign", "", "MOVRR", "RR", 0, 0, IT_INTERNAL_ASSIGNRR, 0);
	AddCommandCore("+assign", "", "MOVPP", "PP", 0, 0, IT_INTERNAL_ASSIGNPP, 0);
	AddCommandCore("+relassign", "", "MOVREL_LL", "mL", 0, 0, IT_INTERNAL_RELASSIGNLL, 0);
	AddCommandCore("+relassign", "", "MOVREL_FF", "gF", 0, 0, IT_INTERNAL_RELASSIGNFF, 0);
	AddCommandCore("+relassign", "", "MOVREL_SS", "tS", 0, 0, IT_INTERNAL_RELASSIGNSS, 0);
	AddCommandCore("+relassign", "", "MOVREL_BB", "cB", 0, 0, IT_INTERNAL_RELASSIGNBB, 0);
	AddCommandCore("+relassign", "", "MOVREL_YY", "zY", 0, 0, IT_INTERNAL_RELASSIGNYY, 0);
	AddCommandCore("+relassign", "", "MOVREL_WW", "xW", 0, 0, IT_INTERNAL_RELASSIGNWW, 0);
	AddCommandCore("+relassign", "", "MOVREL_DD", "eD", 0, 0, IT_INTERNAL_RELASSIGNDD, 0);
	AddCommandCore("+relassign", "", "MOVREL_OO", "uO", 0, 0, IT_INTERNAL_RELASSIGNOO, 0);
	AddCommandCore("+relassign", "", "MOVREL_RR", "vR", 0, 0, IT_INTERNAL_RELASSIGNRR, 0);

	AddCommandCore("+udtassign", "", "MOVUDT", "DD", 0, 0, IT_INTERNAL_ASSIGNUDT, BUILD_COPYUDT);

	// Internal Pointer Math Commands (what are the es for)
//	AddCommandCore("+mathptr", "dbprocore.dll", "?PowerDDD@@YAKKK@Z", "eD", 7, 2, IT_INTERNAL_POWERDDD, 0);
//	AddCommandCore("+mathptr", "dbprocore.dll", "?MulDDD@@YAKKK@Z", "eD", 7, 2, IT_INTERNAL_MULDDD, 0);
//	AddCommandCore("+mathptr", "dbprocore.dll", "?DivDDD@@YAKKK@Z", "eD", 7, 2, IT_INTERNAL_DIVDDD, 0);
//	AddCommandCore("+mathptr", "dbprocore.dll", "?AddDDD@@YAKKK@Z", "eD", 7, 2, IT_INTERNAL_ADDDDD, 0);
//	AddCommandCore("+mathptr", "dbprocore.dll", "?SubDDD@@YAKKK@Z", "eD", 7, 2, IT_INTERNAL_SUBDDD, 0);
//	AddCommandCore("+mathptr", "dbprocore.dll", "?EqualDDD@@YAKKK@Z", "eD", 7, 2, IT_INTERNAL_EQUALDDD, 0);
//	AddCommandCore("+mathptr", "dbprocore.dll", "?GreaterDDD@@YAKKK@Z", "eD", 7, 2, IT_INTERNAL_GREATERDDD, 0);
//	AddCommandCore("+mathptr", "dbprocore.dll", "?LessDDD@@YAKKK@Z", "eD", 7, 2, IT_INTERNAL_LESSDDD, 0);
//	AddCommandCore("+mathptr", "dbprocore.dll", "?NotEqualDDD@@YAKKK@Z", "eD", 7, 2, IT_INTERNAL_NOTEQUALDDD, 0);
//	AddCommandCore("+mathptr", "dbprocore.dll", "?GreaterEqualDDD@@YAKKK@Z", "eD", 7, 2, IT_INTERNAL_GREATEREQUALDDD, 0);
//	AddCommandCore("+mathptr", "dbprocore.dll", "?LessEqualDDD@@YAKKK@Z", "eD", 7, 2, IT_INTERNAL_LESSEQUALDDD, 0);
	// lee - 240306 - u6b4 - reintroduced because 0xFF < 0x00 is not true (which INT will result in)
	AddCommandCore("+mathptr", "dbprocore.dll", "?EqualDDD@@YAKKK@Z", "eD", 7, 2, IT_INTERNAL_EQUALDDD, 0);
	AddCommandCore("+mathptr", "dbprocore.dll", "?GreaterDDD@@YAKKK@Z", "eD", 7, 2, IT_INTERNAL_GREATERDDD, 0);
	AddCommandCore("+mathptr", "dbprocore.dll", "?LessDDD@@YAKKK@Z", "eD", 7, 2, IT_INTERNAL_LESSDDD, 0);
	AddCommandCore("+mathptr", "dbprocore.dll", "?NotEqualDDD@@YAKKK@Z", "eD", 7, 2, IT_INTERNAL_NOTEQUALDDD, 0);
	AddCommandCore("+mathptr", "dbprocore.dll", "?GreaterEqualDDD@@YAKKK@Z", "eD", 7, 2, IT_INTERNAL_GREATEREQUALDDD, 0);
	AddCommandCore("+mathptr", "dbprocore.dll", "?LessEqualDDD@@YAKKK@Z", "eD", 7, 2, IT_INTERNAL_LESSEQUALDDD, 0);

	// Internal Math Commands
	AddCommandCore("+mathfloat", "dbprocore.dll", "?PowerFFF@@YAKMM@Z", "FF", 1, 2, IT_INTERNAL_POWERFFF, 0);
	AddCommandCore("+mathfloat", "dbprocore.dll", "?MulFFF@@YAKMM@Z", "FF", 1, 2, IT_INTERNAL_MULFFF, 0);
	AddCommandCore("+mathfloat", "dbprocore.dll", "?DivFFF@@YAKMM@Z", "FF", 1, 2, IT_INTERNAL_DIVFFF, 0);
	AddCommandCore("+mathfloat", "dbprocore.dll", "?AddFFF@@YAKMM@Z", "FF", 1, 2, IT_INTERNAL_ADDFFF, 0);
	AddCommandCore("+mathfloat", "dbprocore.dll", "?SubFFF@@YAKMM@Z", "FF", 1, 2, IT_INTERNAL_SUBFFF, 0);
	AddCommandCore("+mathfloat", "dbprocore.dll", "?ModFFF@@YAKMM@Z", "FF", 1, 2, IT_INTERNAL_MODFFF, 0);

	// Internal Comparison Commands
	AddCommandCore("+mathfloat", "dbprocore.dll", "?EqualLFF@@YAKMM@Z", "FF", 1, 2, IT_INTERNAL_EQUALLFF, 0);
	AddCommandCore("+mathfloat", "dbprocore.dll", "?GreaterLFF@@YAKMM@Z", "FF", 1, 2, IT_INTERNAL_GREATERLFF, 0);
	AddCommandCore("+mathfloat", "dbprocore.dll", "?LessLFF@@YAKMM@Z", "FF", 1, 2, IT_INTERNAL_LESSLFF, 0);
	AddCommandCore("+mathfloat", "dbprocore.dll", "?NotEqualLFF@@YAKMM@Z", "FF", 1, 2, IT_INTERNAL_NOTEQUALLFF, 0);
	AddCommandCore("+mathfloat", "dbprocore.dll", "?GreaterEqualLFF@@YAKMM@Z", "FF", 1, 2, IT_INTERNAL_GREATEREQUALLFF, 0);
	AddCommandCore("+mathfloat", "dbprocore.dll", "?LessEqualLFF@@YAKMM@Z", "FF", 1, 2, IT_INTERNAL_LESSEQUALLFF, 0);

	// Internal Math Commands
//	AddCommandCore("+mathstr", "dbprocore.dll", "?AddSSS@@YAKKKK@Z", "SS", 3, 2, IT_INTERNAL_ADDSSS, 0);
//	AddCommandCore("+mathstr", "dbprocore.dll", "?EqualLSS@@YAKKK@Z", "SS", 3, 2, IT_INTERNAL_EQUALLSS, 0);
//	AddCommandCore("+mathstr", "dbprocore.dll", "?GreaterLSS@@YAKKK@Z", "SS", 3, 2, IT_INTERNAL_GREATERLSS, 0);
//	AddCommandCore("+mathstr", "dbprocore.dll", "?LessLSS@@YAKKK@Z", "SS", 3, 2, IT_INTERNAL_LESSLSS, 0);
//	AddCommandCore("+mathstr", "dbprocore.dll", "?NotEqualLSS@@YAKKK@Z", "SS", 3, 2, IT_INTERNAL_NOTEQUALLSS, 0);
//	AddCommandCore("+mathstr", "dbprocore.dll", "?GreaterEqualLSS@@YAKKK@Z", "SS", 3, 2, IT_INTERNAL_GREATEREQUALLSS, 0);
//	AddCommandCore("+mathstr", "dbprocore.dll", "?LessEqualLSS@@YAKKK@Z", "SS", 3, 2, IT_INTERNAL_LESSEQUALLSS, 0);
	AddCommandCore("+mathstr", "dbprocore.dll", "?AddSSS@@YAKKKK@Z", "SS", 3, 2, IT_INTERNAL_ADDSSS, 0);
	AddCommandCore("+mathstr", "dbprocore.dll", "?EqualLSS@@YAKKK@Z", "SS", 1, 2, IT_INTERNAL_EQUALLSS, 0);
	AddCommandCore("+mathstr", "dbprocore.dll", "?GreaterLSS@@YAKKK@Z", "SS", 1, 2, IT_INTERNAL_GREATERLSS, 0);
	AddCommandCore("+mathstr", "dbprocore.dll", "?LessLSS@@YAKKK@Z", "SS", 1, 2, IT_INTERNAL_LESSLSS, 0);
	AddCommandCore("+mathstr", "dbprocore.dll", "?NotEqualLSS@@YAKKK@Z", "SS", 1, 2, IT_INTERNAL_NOTEQUALLSS, 0);
	AddCommandCore("+mathstr", "dbprocore.dll", "?GreaterEqualLSS@@YAKKK@Z", "SS", 1, 2, IT_INTERNAL_GREATEREQUALLSS, 0);
	AddCommandCore("+mathstr", "dbprocore.dll", "?LessEqualLSS@@YAKKK@Z", "SS", 1, 2, IT_INTERNAL_LESSEQUALLSS, 0);

	// Internal Math Commands
	AddCommandCore("+mathdoublef", "dbprocore.dll", "?PowerOOO@@YANNN@Z", "OO", 8, 2, IT_INTERNAL_POWEROOO, 0);
	AddCommandCore("+mathdoublef", "dbprocore.dll", "?MulOOO@@YANNN@Z", "OO", 8, 2, IT_INTERNAL_MULOOO, 0);
	AddCommandCore("+mathdoublef", "dbprocore.dll", "?DivOOO@@YANNN@Z", "OO", 8, 2, IT_INTERNAL_DIVOOO, 0);
	AddCommandCore("+mathdoublef", "dbprocore.dll", "?AddOOO@@YANNN@Z", "OO", 8, 2, IT_INTERNAL_ADDOOO, 0);
	AddCommandCore("+mathdoublef", "dbprocore.dll", "?SubOOO@@YANNN@Z", "OO", 8, 2, IT_INTERNAL_SUBOOO, 0);

	// Internal Comparison Commands
	AddCommandCore("+mathdoublef", "dbprocore.dll", "?EqualLOO@@YAKNN@Z", "OO", 1, 8, IT_INTERNAL_EQUALLOO, 0);
	AddCommandCore("+mathdoublef", "dbprocore.dll", "?GreaterLOO@@YAKNN@Z", "OO", 1, 8, IT_INTERNAL_GREATERLOO, 0);
	AddCommandCore("+mathdoublef", "dbprocore.dll", "?LessLOO@@YAKNN@Z", "OO", 1, 8, IT_INTERNAL_LESSLOO, 0);
	AddCommandCore("+mathdoublef", "dbprocore.dll", "?NotEqualLOO@@YAKNN@Z", "OO", 1, 8, IT_INTERNAL_NOTEQUALLOO, 0);
	AddCommandCore("+mathdoublef", "dbprocore.dll", "?GreaterEqualLOO@@YAKNN@Z", "OO", 1, 8, IT_INTERNAL_GREATEREQUALLOO, 0);
	AddCommandCore("+mathdoublef", "dbprocore.dll", "?LessEqualLOO@@YAKNN@Z", "OO", 1, 8, IT_INTERNAL_LESSEQUALLOO, 0);

	// Internal Math Commands
	AddCommandCore("+mathdoublei", "dbprocore.dll", "?PowerRRR@@YA_J_J0@Z", "RR", 9, 2, IT_INTERNAL_POWERRRR, 0);
	AddCommandCore("+mathdoublei", "dbprocore.dll", "?MulRRR@@YA_J_J0@Z", "RR", 9, 2, IT_INTERNAL_MULRRR, 0);
	AddCommandCore("+mathdoublei", "dbprocore.dll", "?DivRRR@@YA_J_J0@Z", "RR", 9, 2, IT_INTERNAL_DIVRRR, 0);
	AddCommandCore("+mathdoublei", "dbprocore.dll", "?AddRRR@@YA_J_J0@Z", "RR", 9, 2, IT_INTERNAL_ADDRRR, 0);
	AddCommandCore("+mathdoublei", "dbprocore.dll", "?SubRRR@@YA_J_J0@Z", "RR", 9, 2, IT_INTERNAL_SUBRRR, 0);

	// Internal Comparison Commands
	AddCommandCore("+mathdoublei", "dbprocore.dll", "?EqualLRR@@YAK_J0@Z", "RR", 1, 9, IT_INTERNAL_EQUALLRR, 0);
	AddCommandCore("+mathdoublei", "dbprocore.dll", "?GreaterLRR@@YAK_J0@Z", "RR", 1, 9, IT_INTERNAL_GREATERLRR, 0);
	AddCommandCore("+mathdoublei", "dbprocore.dll", "?LessLRR@@YAK_J0@Z", "RR", 1, 9, IT_INTERNAL_LESSLRR, 0);
	AddCommandCore("+mathdoublei", "dbprocore.dll", "?NotEqualLRR@@YAK_J0@Z", "RR", 1, 9, IT_INTERNAL_NOTEQUALLRR, 0);
	AddCommandCore("+mathdoublei", "dbprocore.dll", "?GreaterEqualLRR@@YAK_J0@Z", "RR", 1, 9, IT_INTERNAL_GREATEREQUALLRR, 0);
	AddCommandCore("+mathdoublei", "dbprocore.dll", "?LessEqualLRR@@YAK_J0@Z", "RR", 1, 9, IT_INTERNAL_LESSEQUALLRR, 0);

	// Cast Instructions
	AddCommandCore("+cast", "dbprocore.dll", "?CastLtoF@@YAKH@Z", "L", 1, 1, IT_INTERNAL_CASTLTOF, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastLtoB@@YAKH@Z", "L", 1, 1, IT_INTERNAL_CASTLTOB, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastLtoB@@YAKH@Z", "L", 1, 1, IT_INTERNAL_CASTLTOY, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastLtoW@@YAKH@Z", "L", 1, 1, IT_INTERNAL_CASTLTOW, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastLtoD@@YAKH@Z", "L", 1, 1, IT_INTERNAL_CASTLTOD, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastLtoO@@YANH@Z", "L", 1, 1, IT_INTERNAL_CASTLTOO, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastLtoR@@YA_JH@Z", "L", 1, 1, IT_INTERNAL_CASTLTOR, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastFtoL@@YAKM@Z", "F", 1, 1, IT_INTERNAL_CASTFTOL, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastFtoB@@YAKM@Z", "F", 1, 1, IT_INTERNAL_CASTFTOB, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastFtoB@@YAKM@Z", "F", 1, 1, IT_INTERNAL_CASTFTOY, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastFtoW@@YAKM@Z", "F", 1, 1, IT_INTERNAL_CASTFTOW, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastFtoD@@YAKM@Z", "F", 1, 1, IT_INTERNAL_CASTFTOD, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastFtoO@@YANM@Z", "F", 1, 1, IT_INTERNAL_CASTFTOO, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastFtoR@@YA_JM@Z", "F", 1, 1, IT_INTERNAL_CASTFTOR, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastBtoL@@YAKE@Z", "B", 1, 1, IT_INTERNAL_CASTBTOL, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastBtoF@@YAKE@Z", "B", 1, 1, IT_INTERNAL_CASTBTOF, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastBtoW@@YAKE@Z", "B", 1, 1, IT_INTERNAL_CASTBTOW, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastBtoD@@YAKE@Z", "B", 1, 1, IT_INTERNAL_CASTBTOD, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastBtoO@@YANE@Z", "B", 1, 1, IT_INTERNAL_CASTBTOO, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastBtoR@@YA_JE@Z", "B", 1, 1, IT_INTERNAL_CASTBTOR, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastBtoL@@YAKE@Z", "Y", 1, 1, IT_INTERNAL_CASTYTOL, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastBtoF@@YAKE@Z", "Y", 1, 1, IT_INTERNAL_CASTYTOF, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastBtoW@@YAKE@Z", "Y", 1, 1, IT_INTERNAL_CASTYTOW, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastBtoD@@YAKE@Z", "Y", 1, 1, IT_INTERNAL_CASTYTOD, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastBtoO@@YANE@Z", "Y", 1, 1, IT_INTERNAL_CASTYTOO, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastBtoR@@YA_JE@Z", "Y", 1, 1, IT_INTERNAL_CASTYTOR, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastWtoL@@YAKG@Z", "W", 1, 1, IT_INTERNAL_CASTWTOL, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastWtoF@@YAKG@Z", "W", 1, 1, IT_INTERNAL_CASTWTOF, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastWtoB@@YAKG@Z", "W", 1, 1, IT_INTERNAL_CASTWTOB, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastWtoB@@YAKG@Z", "W", 1, 1, IT_INTERNAL_CASTWTOY, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastWtoD@@YAKG@Z", "W", 1, 1, IT_INTERNAL_CASTWTOD, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastWtoO@@YANG@Z", "W", 1, 1, IT_INTERNAL_CASTWTOO, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastWtoR@@YA_JG@Z", "W", 1, 1, IT_INTERNAL_CASTWTOR, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastDtoL@@YAKK@Z", "D", 1, 1, IT_INTERNAL_CASTDTOL, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastDtoF@@YAKK@Z", "D", 1, 1, IT_INTERNAL_CASTDTOF, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastDtoB@@YAKK@Z", "D", 1, 1, IT_INTERNAL_CASTDTOB, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastDtoB@@YAKK@Z", "D", 1, 1, IT_INTERNAL_CASTDTOY, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastDtoW@@YAKK@Z", "D", 1, 1, IT_INTERNAL_CASTDTOW, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastDtoO@@YANK@Z", "D", 1, 1, IT_INTERNAL_CASTDTOO, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastDtoR@@YA_JK@Z", "D", 1, 1, IT_INTERNAL_CASTDTOR, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastOtoL@@YAKN@Z", "O", 1, 1, IT_INTERNAL_CASTOTOL, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastOtoF@@YAKN@Z", "O", 1, 1, IT_INTERNAL_CASTOTOF, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastOtoB@@YAKN@Z", "O", 1, 1, IT_INTERNAL_CASTOTOB, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastOtoB@@YAKN@Z", "O", 1, 1, IT_INTERNAL_CASTOTOY, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastOtoW@@YAKN@Z", "O", 1, 1, IT_INTERNAL_CASTOTOW, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastOtoD@@YAKN@Z", "O", 1, 1, IT_INTERNAL_CASTOTOD, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastOtoR@@YA_JN@Z", "O", 1, 1, IT_INTERNAL_CASTOTOR, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastRtoL@@YAK_J@Z", "R", 1, 1, IT_INTERNAL_CASTRTOL, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastRtoF@@YAK_J@Z", "R", 1, 1, IT_INTERNAL_CASTRTOF, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastRtoB@@YAK_J@Z", "R", 1, 1, IT_INTERNAL_CASTRTOB, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastRtoB@@YAK_J@Z", "R", 1, 1, IT_INTERNAL_CASTRTOY, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastRtoW@@YAK_J@Z", "R", 1, 1, IT_INTERNAL_CASTRTOW, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastRtoD@@YAK_J@Z", "R", 1, 1, IT_INTERNAL_CASTRTOD, 0);
	AddCommandCore("+cast", "dbprocore.dll", "?CastRtoO@@YAN_J@Z", "R", 1, 1, IT_INTERNAL_CASTRTOO, 0);

	return true;
}

void CInstructionTable::ScanStart(void)
{
	// Initialise
	Init();

	// Start Scan
	FFindFirstFile("*.dll");

	// begin scan
	m_bScanActive=true;
}

void CInstructionTable::ScanStep(void)
{
	if(FGetFileReturnValue()!=-1)
	{
		// Item Details
		LPSTR pDLLFile = m_filedata[m_findex].name;
		DWORD dwType = m_filedata[m_findex].attrib;
		if(stricmp(pDLLFile,".")!=NULL && stricmp(pDLLFile, "..")!=NULL)
		{
			if(FGetActualTypeValue(dwType)==1)
			{
				// No folders
			}
			else
			{
				// Do File Thing
				LPSTR pName = new char[strlen(pDLLFile)+1];
				strcpy(pName, pDLLFile);
				pName[strlen(pName)-4]=0;
				LoadCommandsFromDLL(pName, pDLLFile);
				SAFE_DELETE(pName);
			}
		}

		// Go to next one
		FFindNextFile();
	}
	else
	{
		// Determine quit state
		if(m_findex==0)
		{
			// Scan Over
			m_bScanActive=false;
		}
	}
}

void CInstructionTable::ScanEnd(void)
{
	// Free usages
	Free();
}

void CInstructionTable::ScanPluginsForCommands(void)
{
	// Store current folder
	char path[_MAX_PATH];
	_getcwd(path, _MAX_PATH);
	g_pDBPCompiler->SetInternalFile(PATH_CURRENTFOLDER, path);

	// leefix - 011004 - set certificate, ready for verification of any licensed DLLs
	_chdir(g_pDBPCompiler->GetInternalFile(PATH_ROOTPATH));
	ReadLocalHWKey();

	// Switch to PLUGINS Folder
	_chdir(g_pDBPCompiler->GetInternalFile(PATH_PLUGINSFOLDER));

	/* When I compiled VS2012 (with VS 10.0 platform), got this error:
	   No user-defined-conversion operator available that can perform this conversion, or the operator cannot be called

	//
	//	TODO: Make each DLL load in parallel
	//	NOTE: This will probably be a good test for the work queue
	//

	// For each DLL found, attempt to add it as a commandDLL
	struct SEnv
	{
		const char *ca;
		const char *nm;
		CInstructionTable *it;
	};
	const SEnv parms[] =
	{
		"core","dbprocore.dll",this,
		"display","DBProSetupDebug.dll",this,
		"text","DBProTextDebug.dll",this,
		"image","DBProImageDebug.dll",this,
		"basic2d","DBProBasic2DDebug.dll",this,
		"sprite","DBProSpritesDebug.dll",this,
		"input","DBProInputDebug.dll",this,
		"system","DBProSystemDebug.dll",this,
		"file","DBProFileDebug.dll",this,
		"ftp","DBProFtpDebug.dll",this,
		"memblocks","DBProMemblocksDebug.dll",this,
		"animation","DBProAnimationDebug.dll",this,
		"bitmap","DBProBitmapDebug.dll",this,
		"multiplayer","DBProMultiplayerDebug.dll",this,
		"camera","DBProCameraDebug.dll",this,
		"light","DBProLightDebug.dll",this,
		"matrix","DBProMatrixDebug.dll",this,
		"basic3d","DBProBasic3DDebug.dll",this,
		"world","DBProWorld3DDebug.dll",this,
		"world","DBProLODTerrainDebug.dll",this,
		"3dmaths","DBProVectorsDebug.dll",this,
		"particles","DBProParticlesDebug.dll",this,
		"music","DBProMusicDebug.dll",this,
		"sound","DBProSoundDebug.dll",this,
		"basic3d","DBProCSGDebug.dll",this,
		"transforms","DBProTransformsDebug.dll",this
	};
	void(*load)(const SEnv *env) = [](const SEnv *env)->void
	{
		env->it->LoadCommandsFromDLL(const_cast<LPSTR>(env->ca), const_cast<LPSTR>(env->nm));
	};

	db3::CSignal sig;
	db3::uint p;
	for(p=0; p<sizeof(parms)/sizeof(parms[0]); p++)
	{
		//g_WorkQueue.Enqueue(load, &parms[p], &sig);
		load(&parms[p]);
	}

	//sig.Sync();
	*/

	// For each DLL found, attempt to add it as a commandDLL
	LoadCommandsFromDLL("core","dbprocore.dll");
	LoadCommandsFromDLL("display","DBProSetupDebug.dll");
	LoadCommandsFromDLL("text","DBProTextDebug.dll");
	LoadCommandsFromDLL("image","DBProImageDebug.dll");
	LoadCommandsFromDLL("basic2d","DBProBasic2DDebug.dll");
	LoadCommandsFromDLL("sprite","DBProSpritesDebug.dll");
	LoadCommandsFromDLL("input","DBProInputDebug.dll");
	LoadCommandsFromDLL("system","DBProSystemDebug.dll");
	LoadCommandsFromDLL("file","DBProFileDebug.dll");
	LoadCommandsFromDLL("ftp","DBProFtpDebug.dll");
	LoadCommandsFromDLL("memblocks","DBProMemblocksDebug.dll");
	LoadCommandsFromDLL("animation","DBProAnimationDebug.dll");
	LoadCommandsFromDLL("bitmap","DBProBitmapDebug.dll");
	LoadCommandsFromDLL("multiplayer","DBProMultiplayerDebug.dll");
	LoadCommandsFromDLL("camera","DBProCameraDebug.dll");
	LoadCommandsFromDLL("light","DBProLightDebug.dll");
	LoadCommandsFromDLL("matrix","DBProMatrixDebug.dll");
	LoadCommandsFromDLL("basic3d","DBProBasic3DDebug.dll");
	LoadCommandsFromDLL("world","DBProWorld3DDebug.dll");
	LoadCommandsFromDLL("world","DBProLODTerrainDebug.dll");
	LoadCommandsFromDLL("3dmaths","DBProVectorsDebug.dll");
	LoadCommandsFromDLL("particles","DBProParticlesDebug.dll");
	LoadCommandsFromDLL("music","DBProMusicDebug.dll");
	LoadCommandsFromDLL("sound","DBProSoundDebug.dll");
	LoadCommandsFromDLL("basic3d","DBProCSGDebug.dll");
	LoadCommandsFromDLL("transforms","DBProTransformsDebug.dll");

	// Switch to PLUGINS-USER Folder
	_chdir(g_pDBPCompiler->GetInternalFile(PATH_PLUGINSUSERFOLDER));
	ScanStart();
	while(m_bScanActive) ScanStep();
	ScanEnd();

	// Switch to PLUGINS-LICENSED Folder
	_chdir(g_pDBPCompiler->GetInternalFile(PATH_PLUGINSLICENSEDFOLDER));
	ScanStart();
	while(m_bScanActive) ScanStep();
	ScanEnd();

	// Switch back to current folder
	_chdir(g_pDBPCompiler->GetInternalFile(PATH_CURRENTFOLDER));
}

bool CInstructionTable::LoadInstructionDatabase(void)
{
	// Scan Available DLLs for Command Names (this will be a plugin traversal)
	ScanPluginsForCommands();

	// Complete
	return true;
}

bool CInstructionTable::AddCommand(LPSTR pName, LPSTR pDLL, LPSTR pDecoratedName, LPSTR pParamTypesString, DWORD resultp, DWORD pmax)
{
	return AddCommandCore2(pName, pDLL, pDecoratedName, pParamTypesString, resultp, pmax, 0, 0, 0, false, NULL, NULL);
}

bool CInstructionTable::AddUniqueCommand(LPSTR pName, LPSTR pDLL, LPSTR pDecoratedName, LPSTR pParamTypesString, DWORD resultp, DWORD pmax)
{
	// Only add if completely unique
	if(FindInstructionWithNameAndParams(pName, pParamTypesString)==false)
		return AddCommandCore2(pName, pDLL, pDecoratedName, pParamTypesString, resultp, pmax, 0, 0, 0, false, NULL, NULL);
	else
		return true;
}

bool CInstructionTable::AddBuildCommand(LPSTR pName, LPSTR pDesc, LPSTR pParamTypesString, DWORD resultp, DWORD pmax, DWORD dwInternalValueIndex, DWORD dwBuildID)
{
	return AddCommandCore2(pName, "", pDesc, pParamTypesString, resultp, pmax, dwInternalValueIndex, dwBuildID, 0, false, NULL, NULL);
}

bool CInstructionTable::AddCommandCore(LPSTR pName, LPSTR pDLL, LPSTR pDecoratedName, LPSTR pParamTypesString, DWORD resultp, DWORD pmax, DWORD dwInternalValueIndex, DWORD dwBuildID)
{
	return AddCommandCore2(pName, pDLL, pDecoratedName, pParamTypesString, resultp, pmax, dwInternalValueIndex, dwBuildID, 0, false, NULL, NULL);
}

bool CInstructionTable::AddCommandCore2(LPSTR pName, LPSTR pDLL, LPSTR pDecoratedName, LPSTR pParamTypesString, DWORD resultp, DWORD pmax, DWORD dwInternalValueIndex, DWORD dwBuildID, DWORD dwPlace, bool bPassArrayAsInput, CStr* pParamFullDesc, LPSTR* plpretStr )
{
	// Make Entry
	CInstructionTableEntry* pEntry = new CInstructionTableEntry;
	CStr* pStr = new CStr(pName);
	CStr* pStrDLL = new CStr(pDLL);
	CStr* pStrDecName = new CStr(pDecoratedName);
	CStr* pStrParamTypes = new CStr(pParamTypesString);

	if (!pEntry || !pStr || !pStrDLL || !pStrDecName || !pStrParamTypes)
	{
		// NOTE: Yes, it's okay to delete a nullptr. They're ignored by delete.
		delete pEntry;
		delete pStr;
		delete pStrDLL;
		delete pStrDecName;
		delete pStrParamTypes;

		return false;
	}

	DWORD dwCurrentID;

	dwCurrentID = static_cast<DWORD>(db3::atomic_inc(reinterpret_cast<db3::u32 *>(&m_dwCurrentInternalID)));

	pEntry->SetData(dwCurrentID, pStr, pStrDLL, pStrDecName, pStrParamTypes, resultp, pmax, dwInternalValueIndex, dwBuildID);
	pEntry->SetSpecialArrayParam(bPassArrayAsInput);

	// Set param full desc if available
	if(pParamFullDesc)
	{
		if(pParamFullDesc->Length()==0)
		{
			SAFE_DELETE(pParamFullDesc);
		}
		else
		{
			// Add to instruction as a value full param desc
			pEntry->SetFullParamDesc(pParamFullDesc);
		}
	}

#ifndef __AARON_INSTRPERF__
	// See if command has friends of same name (as they go together)
	CInstructionTableEntry* pLastFriendEntry = FindLastFriendOfName(pName);

	// Has a Friend - Insert Into Database
	if(pLastFriendEntry)
	{
		// If command is identical to friend, cannot add it
		if(pEntry->GetHardcoreInternalValue()==0)
		{
			if(pEntry->GetReturnParam()==pLastFriendEntry->GetReturnParam())
			{
				if(pEntry->GetDLL()->Length()>0 && pLastFriendEntry->GetDLL()->Length()>0)
				{
					if(pEntry->GetDecoratedName()->Length()>0 && pLastFriendEntry->GetDecoratedName()->Length()>0)
					{
						if(stricmp(pLastFriendEntry->GetName()->GetStr(), pEntry->GetName()->GetStr())==NULL)
						{
							if(stricmp(pLastFriendEntry->GetParamTypes()->GetStr(), pEntry->GetParamTypes()->GetStr())==NULL)
							{
								// if return string valid, fill with conflicting DLL/name
								if ( plpretStr )
								{
									// create and return in ptr
									*plpretStr = new char[512];
									wsprintf ( *plpretStr, "%s", pLastFriendEntry->GetDLL()->GetStr() );
								}
								// identical command, cannot have two the same
								SAFE_DELETE(pEntry);
								return false;
							}
						}
					}
				}
			}
		}

		{
			db3::CAutolock autolock(m_Lock);

			// Add unique command to instructions
			CInstructionTableEntry* pNextAfterLast = pLastFriendEntry->GetNext();
			if(pNextAfterLast)
				pNextAfterLast->Insert(pEntry);
			else
				pLastFriendEntry->Add(pEntry);
		}
	}
	else
	{
		db3::CAutolock autolock(m_Lock);

		// Newy - Add to Database
		if(m_pFirstInstructionEntry==NULL)
			m_pFirstInstructionEntry=pEntry;
		else
			m_pFirstInstructionEntry->Add(pEntry);
	}
#else
	{
		autolock_type autolock(m_InstructionMapLock);

		auto pFirst = m_InstructionMap.Lookup(pName);
		if (!pFirst)
			return false;

		if (pFirst->P != nullptr)
			pFirst->P->Add(pEntry);
		else
			pFirst->P = pEntry;

		//
		//	NOTE: Checks for duplicate commands will be performed after all of the other commands have been loaded. This
		//	      is to ensure that all duplicates are found (improves experience for plugin developers). This also
		//	      reduces the amount of time spent in the lock (which reduces waiting in other threads and therefore
		//	      improves throughput).
		//
	}
#endif

	// NOTE: Storing this doesn't look like it's necessary, but it might as well be done anyway
	// NOTE[20121123]: Storing this is somewhat necessary.
	if (!m_EntryArray.CheckSlot(static_cast<db3::uint>(dwCurrentID)))
		return false;

	m_EntryArray[dwCurrentID] = pEntry;

	// Store reference if internal
	if(dwInternalValueIndex>0)
	{
		SetIIValue(dwInternalValueIndex, dwCurrentID);
		SetRef(dwInternalValueIndex, pEntry);
	}

	// Set Place if any of return param in input param list
	pEntry->SetReturnParamPlace(dwPlace);

	return true;
}

bool CInstructionTable::AddUserFunction(LPSTR pName, DWORD resultp, LPSTR pParamTypesString, DWORD pmax, CDeclaration* pDecChain)
{
	// leefix-040803-Before confirm, check if name is a reserved word or function name
	if ( g_pStatementList->GetProgramStatements()->DetermineIfReservedWord ( pName ) ) return false;
	if ( g_pStatementList->GetProgramStatements()->DetermineIfFunctionName ( pName, false ) ) return false;

	// Increment ID
	DWORD dwCurrentID = static_cast<DWORD>(db3::atomic_inc(reinterpret_cast<db3::u32 *>(&m_dwCurrentInternalID)));

	// Make Entry
	CInstructionTableEntry* pEntry = new CInstructionTableEntry;
	CStr* pStr = new CStr(pName);
	CStr* pStrID = new CStr(1);
	CStr* pStrParamTypes = new CStr(pParamTypesString);

	if (!pEntry || !pStr || !pStrID || !pStrParamTypes)
	{
		delete pEntry;
		delete pStr;
		delete pStrID;
		delete pStrParamTypes;

		return false;
	}

	pStrID->SetNumericText(dwCurrentID);
	pEntry->SetData(dwCurrentID, pStr, NULL, pStrID, pStrParamTypes, resultp, pmax, 0, 0);

	// Ensure all user functions know about their userfunction structure
	if ( pDecChain ) pEntry->SetDecChain(pDecChain);

#ifdef __AARON_INSTRPERF__
	{
		autolock_type autolock(m_UserFunctionMapLock);

		auto entry = m_UserFunctionMap.Lookup(pName);
		if (!entry)
			return false;

		if (entry->P != nullptr)
			entry->P->Add(pEntry);
		else
			entry->P = pEntry;
	}

	if (!m_EntryArray.CheckSlot(static_cast<db3::uint>(dwCurrentID)))
		return false;

	m_EntryArray[dwCurrentID] = pEntry;
#else
	// Add to Database
	if(m_pFirstUserFunctionEntry==NULL)
		m_pFirstUserFunctionEntry=pEntry;
	else
		m_pFirstUserFunctionEntry->Add(pEntry);
#endif

	// success
	return true;
}

#if 0
inline bool IdentifierBreak(char c, bool bAcceptWhitespace)
{
	if (c=='\0' || (c<=' ' && bAcceptWhitespace))
		return true;

	if (c=='(' || c==':')
		return true;

	return false;
}
#endif
inline const char *AdvanceIdentifier(const char *pStringData, bool bAcceptWhitespace)
{
#if 0
	const char *p;

	p = pStringData;
	while(*p != '\0')
	{
		if ((*p<=' ' && bAcceptWhitespace) || *p=='(' || *p==':')
			return p;

		p++;
	}

	return p;
#else
# define DIFFER_ACCEPT 0
# if DIFFER_ACCEPT
	static bool accept_n[256];
	static bool accept_w[256];
	static bool didinit=false;
	bool *accept;
# else
	static bool accept[256];
	static bool didinit=false;
# endif
	const char *p;

	if (!didinit)
	{
		static db3::CLock lock;
		db3::CAutolock autolock(lock);

# if DIFFER_ACCEPT
		memset(accept_n, 0, sizeof(accept_n));
		memset(accept_w, 0, sizeof(accept_w));
# else
		memset(accept, 0, sizeof(accept));
# endif

# if DIFFER_ACCEPT
		for(p="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_#$%"; *p!='\0'; p++)
			accept_n[*p]=accept_w[*p] = true;

		for(p=" "; *p!='\0'; p++)
			accept_w[*p] = true;
# else
		for(p="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_#$%"; *p!='\0'; p++)
			accept[*p] = true;
# endif

		didinit = true;
	}

# if DIFFER_ACCEPT
	accept = bAcceptWhitespace ? &accept_w[0] : &accept_n[0];
# endif

	p = pStringData;
	while(*p != '\0')
	{
		if (!accept[*(db3::u8 *)p])
			return p;

		p++;
	}

	return p;
#endif
}

CInstructionTableEntry *CInstructionTable::GetEntry(int iType, const char *pStringData, bool *pRetFail)
{
	map_type &mt = iType == 0 ? m_InstructionMap : m_UserFunctionMap;

	auto entry = mt.Find(pStringData);
	if (!entry)
	{
		if (pRetFail)
			*pRetFail = true;

		return nullptr;
	}

	if (pRetFail)
		*pRetFail = false;

	return entry->P;
}
CInstructionTableEntry *CInstructionTable::ResolveEntry(CInstructionTableEntry *pHead, int iWithAnyReturnValue, bool bCommandWhiteSpace)
{
	CInstructionTableEntry *pIter;

	for(pIter=pHead; pIter; pIter=pIter->GetNext())
	{
		bool bRetParmOK = false;
		if (bCommandWhiteSpace==true)
		{
			DWORD dwRetParm = pIter->GetReturnParam();

			if (iWithAnyReturnValue==0)
				bRetParmOK = dwRetParm==0 || (dwRetParm>=11 && dwRetParm<=19) || !!pIter->GetSpecialArrayParam();
			else if(iWithAnyReturnValue==1)
				bRetParmOK = dwRetParm>0;
		} else
			bRetParmOK = true;

		if (!bRetParmOK)
			continue;

		return pIter;
	}

	return nullptr;
}
bool CInstructionTable::FindEntryDirect(int iType, bool bCommandWhiteSpace, LPSTR pStringData, int iWithReturnValue, DWORD* pdwData, DWORD* pdwParamMax, DWORD* pdwLength, CInstructionTableEntry** ppRef)
{
	CInstructionTableEntry *pItem, *pRslv;

	pItem = GetEntry(iType, pStringData, nullptr);
	if (!pItem)
		return false;

	pRslv = ResolveEntry(pItem, iWithReturnValue, bCommandWhiteSpace);
	if (!pRslv)
		return false;

	*pdwData = pRslv->GetInternalID();
	*pdwParamMax = pRslv->GetParamMax();
	*pdwLength = pRslv->GetName()->Length();
	if (ppRef)
		*ppRef = pRslv;

	return true;
}

bool CInstructionTable::FindEntry(int iType, bool bCommandWhiteSpace, CInstructionTableEntry* pBaseEntry, LPSTR pStringData, int iWithAnyReturnValue, DWORD* pdwData, DWORD* pdwParamMax, DWORD* pdwLength, CInstructionTableEntry** ppRef)
{
#ifdef __AARON_INSTRPERF__
# if 0
	map_type &mt = iType == 0 ? m_InstructionMap     : m_UserFunctionMap;
	// locking may not be necessary because we're only doing a look up...
	//lock_type &l = iType == 0 ? m_InstructionMapLock : m_UserFunctionMapLock;
	//autolock_type autolock(l);

	CInstructionTableEntry *largestMatch = nullptr, *lastMatch = nullptr;
	const char *check;
	db3::uint len;
	char buf[256];
	//bool possibleMatch;

	check = pStringData;
	while(*check!='\0')
	{
		check = AdvanceIdentifier(check + 1, bCommandWhiteSpace);

		len = static_cast<db3::uint>(check - pStringData);
		strncpy_s(buf, sizeof(buf), pStringData, len);

		for(char *x=strchr(buf, '\0') - 1; *x<=' '; x--)
			*x = '\0';

		auto entry = mt.Find(buf);
		if (!entry) //if this entry isn't here, larger entries won't occur, break from the loop
			break;

		if (entry->P)
			largestMatch = entry->P;

		if ((*check<=' ' && !bCommandWhiteSpace) || *check=='(' || *check==':')
			break;
	}

	lastMatch = largestMatch;

	for(; largestMatch; largestMatch=largestMatch->GetNext())
	{
		bool bRetParmOK = false;
		if (bCommandWhiteSpace==true)
		{
			DWORD dwRetParm = largestMatch->GetReturnParam();

			if (iWithAnyReturnValue==0)
				bRetParmOK = dwRetParm==0 || (dwRetParm>=11 && dwRetParm<=19) || !!largestMatch->GetSpecialArrayParam();
			else if(iWithAnyReturnValue==1)
				bRetParmOK = dwRetParm>0;
		} else
			bRetParmOK = true;

		if (!bRetParmOK)
			continue;

		*pdwData = largestMatch->GetInternalID();
		*pdwParamMax = largestMatch->GetParamMax();
		*pdwLength = largestMatch->GetName()->Length();
		if (ppRef)
			*ppRef = largestMatch;

		return true;
	}

	return false;
# else
	CInstructionTableEntry *pTest=nullptr, *pBest=nullptr, *pRslv=nullptr;
	const char *check;
	db3::uint len;
	char buf[256];
	bool bRetFail;

	bRetFail = false;
	check = pStringData;
	while(*check!='\0')
	{
		check = AdvanceIdentifier(check + 1, bCommandWhiteSpace);

		len = static_cast<db3::uint>(check - pStringData);
		while(len > 0 && pStringData[len - 1]<=' ')
			len--;

		strncpy_s(buf, sizeof(buf), pStringData, len);

		pTest = GetEntry(iType, buf, &bRetFail);
		if (!pTest && bRetFail==true)
			break;

		if (pTest)
			pBest = pTest;

		if ((*check<=' ' && !bCommandWhiteSpace) || *check=='(' || *check==':')
			break;
	}

	if (!pBest)
		return false;

	pRslv = ResolveEntry(pBest, iWithAnyReturnValue, bCommandWhiteSpace);
	if (!pRslv)
		return false;

	*pdwData = pRslv->GetInternalID();
	*pdwParamMax = pRslv->GetParamMax();
	*pdwLength = pRslv->GetName()->Length();
	if (ppRef)
		*ppRef = pRslv;

	return true;
# endif
#else
	// Default result
	bool bResult=false;

	// Biggest instruction goes
	DWORD dwBiggestInstruction=0;
	DWORD dwBiggestInstructionLength=0;

	// Scan entire instruction database
	LPSTR pTry = new char[256];
	CInstructionTableEntry* pEntry = pBaseEntry;
	if(pEntry)
	{
		while(pEntry)
		{
			// Copy next one in to test against
			strcpy(pTry, pEntry->GetName()->GetStr());

			// Check with current parse item
			DWORD length=strlen(pTry);
			bool bFoundAPossible=false;
			if(strnicmp(pStringData, pTry, length)==NULL)
			{
				if(bCommandWhiteSpace==false)
				{
					if( pStringData[length]=='(' || pStringData[length]==0 )
					{
						bFoundAPossible=true;
					}
					else
					{
						// Inform user
						if(iType==1)
						{
							// Scan on - look for more
							bool bNothingElse=true;
							LPSTR pPtr = pStringData+length;
							while(*(pPtr)!=0 && *(pPtr)!=10 && *(pPtr)!=':' && bNothingElse==true)
							{
								if(*pPtr>32) bNothingElse=false;
								pPtr++;
							}

							// all user functions now need brackets
							if(bNothingElse==true)
							{
								DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
								g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+41);
							}
						}
					}
				}
				if((bCommandWhiteSpace==true && (pStringData[length]<=32 || pStringData[length]==':')) )
					bFoundAPossible=true;
			}

			// Match, test further
			if(bFoundAPossible==true)
			{
				bool bRetParamOk=false;
				if(iWithAnyReturnValue==0 && pEntry->GetReturnParam()==0) bRetParamOk=true;
				if(iWithAnyReturnValue==1 && pEntry->GetReturnParam()>0) bRetParamOk=true;
				if(iWithAnyReturnValue==0 && pEntry->GetReturnParam()>=11 && pEntry->GetReturnParam()<=19) bRetParamOk=true;
				if(iWithAnyReturnValue==0 && pEntry->GetSpecialArrayParam()) bRetParamOk=true;
				if(bCommandWhiteSpace==false) bRetParamOk=true;
				if(bRetParamOk==true)
				{
					if(length>dwBiggestInstructionLength)
					{
						dwBiggestInstructionLength=length;
						dwBiggestInstruction=(DWORD)pEntry->GetInternalID();
						*pdwData=dwBiggestInstruction;
						*pdwParamMax=pEntry->GetParamMax();
						*pdwLength=length;
						if(ppRef) *ppRef=pEntry;
						bResult=true;
					}
				}
			}
			pEntry=pEntry->GetNext();
		}
	}
	SAFE_DELETE(pTry);

	return bResult;
#endif
}

bool CInstructionTable::FindInstruction(bool bCommandWhiteSpace, LPSTR pStringData, int iWithAnyReturnValue, DWORD* pdwData, DWORD* pdwParamMax, DWORD* pdwLength, CInstructionTableEntry** ppRef)
{
#ifdef __AARON_INSTRPERF__
	CInstructionTableEntry *m_pFirstInstructionEntry = nullptr;
#endif
	if (!bCommandWhiteSpace)
		return FindEntryDirect(0, bCommandWhiteSpace, pStringData, iWithAnyReturnValue, pdwData, pdwParamMax, pdwLength, ppRef);

	return FindEntry(0, bCommandWhiteSpace, m_pFirstInstructionEntry, pStringData, iWithAnyReturnValue, pdwData, pdwParamMax, pdwLength, ppRef);
}

bool CInstructionTable::FindUserFunction(LPSTR pStringData, int iWithAnyReturnValue, DWORD* pdwData, DWORD* pdwParamMax, DWORD* pdwLength)
{
#ifdef __AARON_INSTRPERF__
	CInstructionTableEntry *m_pFirstUserFunctionEntry = nullptr;
#endif
	return FindEntry(1, false, m_pFirstUserFunctionEntry, pStringData, iWithAnyReturnValue, pdwData, pdwParamMax, pdwLength, NULL);
}

bool CInstructionTable::FindInstructionParams(DWORD dwInstructionValue, DWORD dwParamMax, DWORD* pdwData, DWORD* pdwParamMax, CStr** pValidParamTypes, CInstructionTableEntry** pRefEntry)
{
#ifdef __AARON_INSTRPERF__
	if (static_cast<db3::uint>(dwInstructionValue) >= m_EntryArray.Size())
		return false;

	auto primary = m_EntryArray[dwInstructionValue];
	if (!primary)
		return false;

	*pdwData = primary->GetInternalID();
	*pdwParamMax = primary->GetParamMax();
	*pValidParamTypes = primary->GetParamTypes();
	*pRefEntry = primary;

	return true;
#else
	CInstructionTableEntry* pEntry = m_pFirstInstructionEntry;
	if(pEntry)
	{
		while(pEntry)
		{
			if(pEntry->GetInternalID()==dwInstructionValue)
			{
				// Store first instruction of this name (sorted)
				CInstructionTableEntry* pPrimary = pEntry;

				// Found Instruction - now find params that match
				while(stricmp(pPrimary->GetName()->GetStr(),pEntry->GetName()->GetStr())==NULL)
				{
					// Match Param to exit with valid instruction
					if(pPrimary->GetParamMax()==pEntry->GetParamMax())
					{
						*pdwData=pEntry->GetInternalID();
						*pdwParamMax=pEntry->GetParamMax();
						*pValidParamTypes=pEntry->GetParamTypes();
						*pRefEntry=pEntry;
						return true;
					}
					pEntry=pEntry->GetNext();
				}
			}
			pEntry=pEntry->GetNext();
		}
	}

	// Could not match param soft fail
	return false;
#endif
}

bool CInstructionTable::FindUserFunctionParams(DWORD dwInstructionValue, DWORD dwParamMax, DWORD* pdwData, DWORD* pdwParamMax, CStr** pValidParamTypes, CInstructionTableEntry** pRefEntry)
{
#ifdef __AARON_INSTRPERF__
	if (static_cast<db3::uint>(dwInstructionValue) >= m_EntryArray.Size())
		return false;

	auto primary = m_EntryArray[dwInstructionValue];
	if (!primary)
		return false;

	*pdwData = primary->GetInternalID();
	*pdwParamMax = primary->GetParamMax();
	*pValidParamTypes = primary->GetParamTypes();
	*pRefEntry = primary;

	return true;
#else
	CInstructionTableEntry* pEntry = m_pFirstUserFunctionEntry;
	if(pEntry)
	{
		while(pEntry)
		{
			if(pEntry->GetInternalID()==dwInstructionValue)
			{
				// Store first instruction of this name (sorted)
				CInstructionTableEntry* pPrimary = pEntry;

				// Found Instruction - now find params that match
				while(stricmp(pPrimary->GetName()->GetStr(),pEntry->GetName()->GetStr())==NULL)
				{
					// Match Param to exit with valid instruction
					if(pPrimary->GetParamMax()==pEntry->GetParamMax())
					{
						*pdwData=pEntry->GetInternalID();
						*pdwParamMax=pEntry->GetParamMax();
						*pValidParamTypes=pEntry->GetParamTypes();
						*pRefEntry=pEntry;
						return true;
					}
					pEntry=pEntry->GetNext();
				}
			}
			pEntry=pEntry->GetNext();
		}
	}

	// Could not match param soft fail
	return false;
#endif
}

bool CInstructionTable::CompareInstructionNames(CInstructionTableEntry* pRefEntryA, CInstructionTableEntry* pRefEntryB)
{
	// Compare Entry names, success if match
	if(stricmp(pRefEntryA->GetName()->GetStr(), pRefEntryB->GetName()->GetStr())==NULL)
		return true;

	// Could not match names soft fail
	return false;
}

CInstructionTableEntry* CInstructionTable::FindUserFunction(LPSTR pUserFunctionName)
{
#ifdef __AARON_INSTRPERF__
	auto entry = m_UserFunctionMap.Find(pUserFunctionName);
	if (!entry)
		return nullptr;

	return entry->P;
#else
	// Search for user function
	CInstructionTableEntry* pEntry = m_pFirstUserFunctionEntry;
	while(pEntry)
	{
		if(stricmp(pEntry->GetName()->GetStr(), pUserFunctionName)==NULL)
			return pEntry;

		pEntry=pEntry->GetNext();
	}

	// Could not find
	return NULL;
#endif
}

CInstructionTableEntry* CInstructionTable::FindReservedFunction(LPSTR pFunctionName)
{
#ifdef __AARON_INSTRPERF__
	auto entry = m_InstructionMap.Find(pFunctionName);
	if (!entry)
		return nullptr;

	return entry->P;
#else
	// Search for reserved function
	CInstructionTableEntry* pEntry = m_pFirstInstructionEntry;
	while(pEntry)
	{
		if(stricmp(pEntry->GetName()->GetStr(), pFunctionName)==NULL)
			return pEntry;

		pEntry=pEntry->GetNext();
	}

	// Could not find
	return NULL;
#endif
}

bool CInstructionTable::FindInstructionWithNameAndParams(LPSTR pName, LPSTR pParams)
{
#ifdef __AARON_INSTRPERF__
	auto entry = m_InstructionMap.Find(pName);
	if (!entry)
		return nullptr;

	auto item = entry->P;

	while(item)
	{
		if (stricmp(item->GetParamTypes()->GetStr(), pParams)==NULL)
			return true;

		item = item->GetNext();
	}

	return false;
#else
	// Search for entry with matching name and params
	CInstructionTableEntry* pEntry = m_pFirstInstructionEntry;
	while(pEntry)
	{
		if(stricmp(pEntry->GetName()->GetStr(), pName)==NULL
		&& stricmp(pEntry->GetParamTypes()->GetStr(), pParams)==NULL)
		{
			// Found same instruction
			return true;
		}

		pEntry=pEntry->GetNext();
	}

	// Could not fund soft fail
	return false;
#endif
}

CInstructionTableEntry* CInstructionTable::FindLastFriendOfName(LPSTR pFriendName)
{
#ifdef __AARON_INSTRPERF__
	auto entry = m_InstructionMap.Find(pFriendName);
	if (!entry)
		return nullptr;

	auto item = entry->P;
	if (!item)
		return nullptr;

	while(item->GetNext())
		item = item->GetNext();

	return item;
#else
	// Latest match
	CInstructionTableEntry* pMatch = NULL;

	// Search for last entry with the friend name
	CInstructionTableEntry* pEntry = m_pFirstInstructionEntry;
	while(pEntry)
	{
		if(stricmp(pEntry->GetName()->GetStr(), pFriendName)==NULL) pMatch=pEntry;
		pEntry=pEntry->GetNext();
	}

	// Pass last match if any
	return pMatch;
#endif
}

// DLL Scanning and Database Building

LPSTR CInstructionTable::ReadRawStringTable ( LPSTR pFilenameEXE, DWORD* pdwDataSize )
{
	// raw string table data retutned
	LPSTR pReturnData = NULL;

	// Simply scans the EXE and locates the pattern in the data, and replaces it
	DWORD dwSizeOfEXECode = 0;	
	DWORD dwOverallDataSize = 0;
	HMODULE hEXE = LoadLibraryEx(pFilenameEXE, NULL, LOAD_LIBRARY_AS_DATAFILE);
	if ( hEXE )
	{
		// look for string table data in packed data format
		HRSRC hRes = NULL;
		int iFirstStringIndexFound = -1;
		for ( int iIndex=0; iIndex<255; iIndex++ )
		{
			/* old system did not account for gaps in the string table sequence (styx)
			hRes = FindResource(hEXE, (LPCTSTR)iIndex, RT_STRING);
			if ( hRes )
			{
				dwOverallDataSize += SizeofResource(hEXE, hRes);
				if ( iFirstStringIndexFound==-1 )
					iFirstStringIndexFound = iIndex;
			}
			if ( iFirstStringIndexFound!=-1 && hRes==NULL )
				break;
			*/
			hRes = FindResource(hEXE, (LPCTSTR)iIndex, RT_STRING);
			if ( hRes )
			{
				DWORD dwSizeOStringData = SizeofResource(hEXE, hRes);
				if ( dwSizeOStringData>0 )
				{
					dwOverallDataSize += dwSizeOStringData;
					if ( iFirstStringIndexFound==-1 )
						iFirstStringIndexFound = iIndex;
				}
			}
		}
		if ( iFirstStringIndexFound!=-1 )
		{
			// get first data ptr again
			hRes = FindResource(hEXE, (LPCTSTR)iFirstStringIndexFound, RT_STRING);
			HGLOBAL hGlobal = LoadResource(hEXE, hRes);
			LPVOID lpResReal = LockResource(hGlobal);

			// get string table data
			pReturnData = new char [ dwOverallDataSize ];
			memcpy(pReturnData, (LPSTR)lpResReal, dwOverallDataSize);
		}

		// free usages
		if ( hEXE ) FreeLibrary ( hEXE );
	}

	// erase again if first character is NOT a space(32)
	if ( pReturnData )
		if ( pReturnData [ 0 ]!=32 )
			SAFE_DELETE ( pReturnData );

	// return data size
	if ( pdwDataSize ) *pdwDataSize = dwOverallDataSize;

	// return string
	return pReturnData;
}

bool CInstructionTable::VerifyCertificateForPlugin ( LPSTR pDLLName, LPSTR pProductCode )
{
	// find product
	int iProductIndex = FindIndexOfPluginByDLLName ( pDLLName );

	// trial, 60day and full certificates
	int iTrial = 0;
	int i60Day = 1;
	int iFull = 2;

	// if not a recognised product, fail verify
	if ( iProductIndex==-1 )
	{
		char buf[1024];

		sprintf_s(buf, sizeof(buf), "Licensed Plugin \"%s\" is not recognised by compiler! "
			"Visit www.thegamecreators.com for an update.", pDLLName);
		MessageBox ( NULL, buf, "Plugin Error", MB_OK );
		return false;
	}

	// switch to compiler folder to check certificates
	bool bResult = false;
	char pStoreCurrentFolder [ _MAX_PATH ];
	getcwd ( pStoreCurrentFolder, _MAX_PATH );
	_chdir ( g_pDBPCompiler->GetInternalFile(PATH_ROOTPATH) );

	// ensure product code matches internal product code
	DWORD iThisProductCode = atoi ( pProductCode+17 );
	int iTrialStatus = 0;
	if ( g_pProductCodes [ iProductIndex ] == iThisProductCode )
	{
		// use Certificate Key System
		iTrialStatus = AmIActive ( iProductIndex+iTrial, NULL );
		if ( iTrialStatus==1
		||	 AmIActive ( iProductIndex+i60Day, NULL )==1
		||	 AmIActive ( iProductIndex+iFull,  NULL )==1 )
		{
			// plugin is valid
			bResult = true;
		}
	}

	// report failure to validate
	/* keep silent for now!
	if ( bResult==false )
	{
		if ( iTrialStatus==2 )
			MessageBox ( NULL, "Licensed plugin trial has expired!", "Plugin Error", MB_OK );
		else
			MessageBox ( NULL, "Licensed plugin requires a valid certificate!", "Plugin Error", MB_OK );
	}
	*/

	// restore current directory
	_chdir ( pStoreCurrentFolder );

	// plugin is invalid
	return bResult;
}
//#define __AARON_DEBUG__ 1
#define __AARON_COMMANDS__ 1

#ifdef __AARON_DEBUG__
static void PrintToFile(const char *text, ...)
{
	va_list args;
	FILE *f;

	f = fopen("D:\\Log\\Commands.txt", "a+");
	if (!f)
		return;

	va_start(args, text);
	vfprintf(f, text, args);
	va_end(args);
	fprintf(f, "\n");
	fclose(f);
}
#else
# define PrintToFile(text,...)
#endif
bool CInstructionTable::LoadCommandsFromDLL(LPSTR pCategory, LPSTR pFilename)
{
#ifdef __AARON_COMMANDS__
	// Added for simplicity
	static char libname[512];
	FILE *f;

	sprintf_s(libname, "%s.commands", pFilename);

	f = fopen(libname, "r");
	if (f != nullptr)
	{
		static char tempstr[4096];

		while(fgets(tempstr, sizeof(tempstr), f) != nullptr)
		{
			char *p;

			p = strchr(tempstr, '\n');
			if (p)
				*p = '\0';

			p = &tempstr[0];
			while(*p<=' ' && *p!='\0')
				p++;

			if (*p=='\0')
				break;
				
			PrintToFile( "%s:\"%s\"", libname, p );
			if(TurnStringIntoCommand(pCategory, pFilename, p)==false)
			{
				char err[512];
				wsprintf(err, "Command in '%s' command-table unrecognised (%s)", libname, p);
				g_pErrorReport->AddErrorString(err);
				fclose(f);
				return false;
			}
		}

		fclose(f);
		return true;
	}
#endif

	// First check if DLL is PROTECTED or not
	DWORD dwDataSize = 0;
	HMODULE hModule = NULL;
	DWORD dwCountProtectedCommands=0;
	LPSTR pProtectedData = ReadRawStringTable ( pFilename, &dwDataSize );
	if ( pProtectedData )
	{
		// if a protected plugin, obtain string data in sequential stream
		LPSTR pPtr=pProtectedData;
		LPSTR pPtrLineStart=pProtectedData;
		LPSTR pPtrEnd=pProtectedData+dwDataSize;
		while ( pPtr<pPtrEnd )
		{
			// find end of line
			if ( (unsigned char)*(pPtr)==0 )
			{
				// get line
				char pTempStr [ 300 ];
				strcpy ( pTempStr, pPtrLineStart );
				pPtrLineStart=pPtr+1;

				// first entry is protected plugin header
				if ( dwCountProtectedCommands == 0 )
				{
					// protected plugin must be verified before we include commands
					LPSTR pProductCode = pTempStr;
					if ( VerifyCertificateForPlugin ( pFilename, pProductCode )==false )
					{
						// failed to verify - simply ignore plugin silently
						char pDirINI [ _MAX_PATH ];
						getcwd ( pDirINI, _MAX_PATH);
						strcat ( pDirINI, "\\report.ini");
						
						// make silent INI report
						WritePrivateProfileString("VERIFICATION REPORT", pFilename, "NOT VALID", pDirINI);

						// free usages
						SAFE_DELETE ( pProtectedData );
						return true;
					}

					// and advance to first character of data (string table starts)
					while ( (unsigned char)*(pPtr+0)==0 && pPtr+2<pPtrEnd )
						pPtr++;

					// add to count
					dwCountProtectedCommands=1;
					pPtrLineStart=pPtr;
				}
				else
				{
					// add each as found
					PrintToFile( "[protected]%s:\"%s\"", pFilename, pTempStr );
					if(TurnStringIntoCommand(pCategory, pFilename, pTempStr)==false)
					{
						char err[512];
						wsprintf(err, "Command in '%s' command-table unrecognised (%s)", pFilename, pTempStr);
						g_pErrorReport->AddErrorString(err);
						return false;
					}

					// add to count
					dwCountProtectedCommands++;

					// two nulls or more, advance to end or next string char
					if ( (unsigned char)*(pPtr+1)==0 )
					{
						// and advance to first character of data (string table starts)
						while ( (unsigned char)*(pPtr+0)==0 && pPtr+2<pPtrEnd )
							pPtr++;

						// continue or end
						if ( pPtr+2<pPtrEnd )
							pPtrLineStart=pPtr;
						else
							break;
					}
				}
			}

			// next character
			pPtr++;
		}

		// Free usages
		SAFE_DELETE ( pProtectedData );
	}
	else
	{
		// if not a protected plugin, open for direct string reading
		hModule = LoadLibraryEx ( pFilename, NULL, LOAD_LIBRARY_AS_DATAFILE );
		if ( hModule  )
		{
			// Load DLL Decorated Names via unprotected resource
			DWORD dwTry=0;
			DWORD dwMax=0;
			DWORD dwLength = 255;
			LPSTR pTempStr = new char[dwLength+1];
			while(dwTry<1000)
			{
				int iStrQty = LoadString(hModule, 1+dwTry, pTempStr, 2);
				if(iStrQty!=0) dwMax=1+dwTry;
				dwTry++;
			}

			// For each string..
			for(DWORD n=0; n<dwMax; n++)
			{
				// Get String
				int iChar = LoadString(hModule, 1+n, pTempStr, dwLength);

				// Parse it for info
				if(iChar>0)
				{
					PrintToFile( "%s:\"%s\"", pFilename, pTempStr );
					if(TurnStringIntoCommand(pCategory, pFilename, pTempStr)==false)
					{
						char err[512];
						wsprintf(err, "Command in '%s' command-table unrecognised (%s)", pFilename, pTempStr);
						g_pErrorReport->AddErrorString(err);
						SAFE_DELETE(pTempStr);
						FreeLibrary(hModule);
						return false;
					}
				}
			}
			SAFE_DELETE(pTempStr);
		}

		// free module
		if ( hModule ) FreeLibrary ( hModule );
	}

	// Complete
	return true;
}

bool CInstructionTable::TurnStringIntoCommand(LPSTR pCategory, LPSTR pDLLName, LPSTR pRawCommandString)
{
	// Parse Sections of string
	CStr* pStr = new CStr(pRawCommandString);
	DWORD dwParamPos = pStr->FindFirstChar('%');
	CStr* pStrPart2 = new CStr(pRawCommandString+dwParamPos+1);
	DWORD dwDecPos = pStrPart2->FindFirstChar('%') + dwParamPos + 2;
	SAFE_DELETE(pStrPart2);

	// Ensure we have TWO of them
	if(dwParamPos>0)
	{
		CStr* pCheckForTwo = new CStr(pStr->GetStr()+dwParamPos+1);
		DWORD dwSecondPos = pCheckForTwo->FindFirstChar('%');
		SAFE_DELETE(pCheckForTwo);
		if(dwSecondPos==0)
		{
			SAFE_DELETE(pStr);
			return false;
		}
	}
	else
	{
		SAFE_DELETE(pStr);
		return false;
	}

	// Validate String Parts
	if(dwParamPos>0 && dwDecPos>0 && dwDecPos>dwParamPos)
	{
		// Command Name Part
		CStr* pName = new CStr(pRawCommandString);
		pName->SetChar(dwParamPos, 0);

		// Command Params Part
		CStr* pParam = new CStr(pRawCommandString+dwParamPos+1);
		pParam->SetChar(dwDecPos-dwParamPos-2, 0);
		unsigned char cFirstParamChar=0;
		if(pName->GetChar(pName->Length()-1)=='[')
		{
			// Command Is Expression
			pName->SetChar(pName->Length()-1,0);
			cFirstParamChar = pParam->GetChar(0);
			pParam->SetText(pParam->GetStr()+1);
		}

		// If * character inside param description
		DWORD dwStarPos = 0;
		if(pParam->Length()>0)
		{
			// param to its left is a return variable
			dwStarPos = pParam->FindFirstChar('*');
			if(dwStarPos>0)
			{
				cFirstParamChar = pParam->GetChar(dwStarPos-1);
			}
		}

		bool bPassArrayPtrAsInput=false;
		DWORD dwReturnParam=0;
		if(cFirstParamChar>0)
		{
			switch(cFirstParamChar)
			{
				case 'L' : dwReturnParam=1;	break;
				case 'F' : dwReturnParam=2;	break;
				case 'S' : dwReturnParam=3;	break;
				case 'B' : dwReturnParam=4;	break;
				case 'Y' : dwReturnParam=5;	break;
				case 'W' : dwReturnParam=6;	break;
				case 'D' : dwReturnParam=7;	break;
				case 'O' : dwReturnParam=8;	break;
				case 'R' : dwReturnParam=9;	break;
				case 'H' : dwReturnParam=7; bPassArrayPtrAsInput=true; break;
				case 'X' : dwReturnParam=501;break;//any-type (no casting)
				default: dwReturnParam=1;	break;
			}
		}

		// If param is zero, no params required
		if(pParam->CheckChar(0,'0')) pParam->SetText("");

		// If star part of param string, remove it and make return-var a ptr..
		if(dwStarPos>0)
		{
			// Shuffle to overwrite * character
			for(DWORD n=dwStarPos; n<pParam->Length(); n++)
				pParam->SetChar(n, pParam->GetChar(n+1));

			if(bPassArrayPtrAsInput)
			{
				// Keep pass in and return out as DWORDs for correct array-ptr handling
//				pParam->SetChar(0, 'D'); //LEEFIX-300902-Keep Array Handling on the H
				dwReturnParam=7;
			}
			else
			{
				dwReturnParam=dwReturnParam+10;
			}
		}

		// Decorated Name Part
		CStr* pDecorated = new CStr(pRawCommandString+dwDecPos);

		// Get Param Desc string out of decorated name
		CStr* pParamDesc = new CStr("");
		pParamDesc->SetText(pDecorated->GetStr());
		DWORD dwPDPos = pDecorated->FindFirstChar('%');
		if(dwPDPos>0)
		{
			pDecorated->SetChar(dwPDPos,0);
			pParamDesc->SetText(pDecorated->GetStr()+dwPDPos+1);
		}
		else
			pParamDesc->SetText("");

		// released locally
		CStr* pParamDescForHelp = new CStr("");
		pParamDescForHelp->SetText(pParamDesc->GetStr());

		// Add Command To Database
		if(strcmp(pDecorated->GetStr(), "??")!=NULL)
		{
			LPSTR lpConflictingDLLName = NULL;
			if(AddCommandCore2(pName->GetStr(), pDLLName, pDecorated->GetStr(), pParam->GetStr(), dwReturnParam, pParam->Length(), 0, 0, dwStarPos, bPassArrayPtrAsInput, pParamDesc, &lpConflictingDLLName)==false)
			{
				// Could not add command as it was identically duplicated!
				char err[512];
//				wsprintf(err, "Command in DLL command-table duplicated (%s:%s)", pDLLName, pName->GetStr());
				wsprintf(err, "Duplicate %s in %s and %s!", pName->GetStr(), pDLLName, lpConflictingDLLName);
				g_pErrorReport->AddErrorString(err);
				SAFE_DELETE(lpConflictingDLLName);
				SAFE_DELETE(pParamDescForHelp);
				SAFE_DELETE(pName);
				SAFE_DELETE(pStr);
				return false;
			}
			pParamDesc=NULL;//used inside
		}

		// If active, add Command To HelpText Folder
		if(g_pDBPCompiler->GetGenerateHelpTxtMode())
		{
			if(dwStarPos>0) dwReturnParam=0; //so visible sign of return value in help!
			AddCommandToHelpTxt(pCategory, pName->GetStr(), pParam->GetStr(), dwReturnParam, pParam->Length(), pParamDescForHelp->GetStr());
		}

		// Free usages
		SAFE_DELETE(pName);
		SAFE_DELETE(pParam);
		SAFE_DELETE(pDecorated);
		SAFE_DELETE(pParamDescForHelp);
		SAFE_DELETE(pParamDesc);
		SAFE_DELETE(pStr);
	}
	else
	{
		SAFE_DELETE(pStr);
		return false;
	}

	// Complete
	return true;
}

void CInstructionTable::AddCommandToHelpTxt(LPSTR pCategory, LPSTR pCommandName, LPSTR pParamStr, DWORD dwReturnParam, DWORD dwParamCount, LPSTR pParamDesc)
{
	// Usage strings
	char lpFilename[_MAX_PATH];
	char lpSubMenuFilename[_MAX_PATH];
	char lpMenuFilename[_MAX_PATH];
	char lpTXTTitle[_MAX_PATH];
	char lpTXTSyntax[_MAX_PATH];
	char lpTXTThisSyntax[_MAX_PATH];
	char lpTXTCommand[_MAX_PATH];
	char lpTXTThisCommand[_MAX_PATH];

	// Make folder and enter it
	mkdir("helptxt");
	chdir("helptxt");
	char lpIndexFilename[_MAX_PATH];
	_getcwd(lpIndexFilename, _MAX_PATH);
	strcat(lpIndexFilename, "\\index.txt");
	mkdir("commands");
	chdir("commands");
	mkdir(pCategory);
	chdir(pCategory);

	// PREPARE HELP PAGE CONTENTS TXT

	// Filename
	wsprintf(lpFilename, "..\\%s\\%s.txt", pCategory, pCommandName);

	// Title of Command Page
	wsprintf(lpTXTTitle, "%s", pCommandName);

	// Check if a trouble command
	strcpy(lpTXTThisSyntax, "");
	if(CheckTroubleCommandSyntax(lpTXTThisSyntax, pCommandName))
	{
		// Syntax Of Standard Command
		if(dwReturnParam>0 && dwReturnParam<=10)
		{
			if(dwReturnParam>0 && pParamStr[0]!='*')
			{
				strcat(lpTXTThisSyntax, "Return ");
				switch(dwReturnParam)
				{
					case 1 : strcat(lpTXTThisSyntax, "Integer");			break;
					case 2 : strcat(lpTXTThisSyntax, "Float");				break;
					case 3 : strcat(lpTXTThisSyntax, "String");				break;
					case 4 : strcat(lpTXTThisSyntax, "Flag");				break;
					case 5 : strcat(lpTXTThisSyntax, "BYTE");				break;
					case 6 : strcat(lpTXTThisSyntax, "WORD");				break;
					case 7 : strcat(lpTXTThisSyntax, "DWORD");				break;
					case 8 : strcat(lpTXTThisSyntax, "Double Float");		break;
					case 9 : strcat(lpTXTThisSyntax, "Double Integer");		break;
				}
				strcat(lpTXTThisSyntax, "=");
			}
			strcat(lpTXTThisSyntax, pCommandName);
			if(dwReturnParam>0)
				strcat(lpTXTThisSyntax, "(");
			else
				strcat(lpTXTThisSyntax, " ");
		}
		else
		{
			// No return value command
			strcat(lpTXTThisSyntax, pCommandName);
			strcat(lpTXTThisSyntax, " ");
		}

		if(dwParamCount>0)
		{
			if(strcmp(pParamDesc, "")==NULL)
			{
				for(DWORD p=0; p<=dwParamCount; p++)
				{
					if(pParamStr[p]>0)
					{
						switch(pParamStr[p])
						{
							case 'L' : strcat(lpTXTThisSyntax, "Integer");			break;
							case 'F' : strcat(lpTXTThisSyntax, "Float");			break;
							case 'S' : strcat(lpTXTThisSyntax, "String");			break;
							case 'B' : strcat(lpTXTThisSyntax, "Flag");				break;
							case 'Y' : strcat(lpTXTThisSyntax, "BYTE");				break;
							case 'W' : strcat(lpTXTThisSyntax, "WORD");				break;
							case 'D' : strcat(lpTXTThisSyntax, "DWORD");			break;
							case 'O' : strcat(lpTXTThisSyntax, "Double Float");		break;
							case 'R' : strcat(lpTXTThisSyntax, "Double Integer");	break;
						}
						strcat(lpTXTThisSyntax, " Value");
						if(p<dwParamCount-1) strcat(lpTXTThisSyntax, ", ");
					}
				}
			}
			else
			{
				// Param Desc stored in DLL
				strcat(lpTXTThisSyntax, pParamDesc);
			}
		}
		if(dwReturnParam>0) strcat(lpTXTThisSyntax, ")");
	}

	// WRITE HELP TXT FILE

	// Create help txt file for command
	WritePrivateProfileString("COMMAND", "TITLE", lpTXTTitle, lpFilename);

	// Add Unique Command To Unique Syntax Line
	DWORD dwField=1;
	char lpField[256];
	while(dwField<998)
	{
		wsprintf(lpField, "SYNTAX%d", dwField);
		GetPrivateProfileString("COMMAND", lpField, "", lpTXTSyntax, 256, lpFilename);
		if(strcmp(lpTXTSyntax,"")==NULL)
		{
			// Not there, so add it
			WritePrivateProfileString("COMMAND", lpField, lpTXTThisSyntax, lpFilename);
			break;
		}
		if(strcmp(lpTXTSyntax,lpTXTThisSyntax)==NULL)
		{
			// Already got it
			break;
		}
		dwField++;
	}

	// Add parameter types to command-data
	wsprintf(lpField, "PARAM");
	char lpThisParamStr [ 256 ];
	GetPrivateProfileString("COMMAND", lpField, "", lpThisParamStr, 256, lpFilename);
	char lpNewParamStr [ 256 ];
	strcpy ( lpNewParamStr, "" );
	if ( dwReturnParam > 0 ) strcat ( lpNewParamStr, "(" );
	switch(dwReturnParam)
	{
		case 1 : strcat(lpNewParamStr, "L");		break;
		case 2 : strcat(lpNewParamStr, "F");		break;
		case 3 : strcat(lpNewParamStr, "S");		break;
		case 4 : strcat(lpNewParamStr, "B");		break;
		case 5 : strcat(lpNewParamStr, "Y");		break;
		case 6 : strcat(lpNewParamStr, "W");		break;
		case 7 : strcat(lpNewParamStr, "D");		break;
		case 8 : strcat(lpNewParamStr, "O");		break;
		case 9 : strcat(lpNewParamStr, "R");		break;
	}
	if ( pParamStr ) strcat ( lpNewParamStr, pParamStr );
	if ( strlen(lpNewParamStr) > strlen(lpThisParamStr) )
		WritePrivateProfileString("COMMAND", lpField, lpNewParamStr, lpFilename);

	// Add Unique Command To The IndexTXT (for the index.htm)
	DWORD dwIndexField=1;
	char lpIndexField[_MAX_PATH];
	char lpIndexFieldData[_MAX_PATH];
	char lpFilenameToCommand[_MAX_PATH];
	wsprintf(lpFilenameToCommand, "commands\\%s\\%s.htm", pCategory, pCommandName);
	while(1)
	{
		wsprintf(lpIndexField, "ENTRY%d", dwIndexField);
		GetPrivateProfileString("INDEX", lpIndexField, "", lpIndexFieldData, _MAX_PATH, lpIndexFilename);
		if(strcmp(lpIndexFieldData,"")==NULL)
		{
			WritePrivateProfileString("INDEX", lpIndexField, pCommandName, lpIndexFilename);
			wsprintf(lpIndexField, "FILE%d", dwIndexField);
			WritePrivateProfileString("INDEX", lpIndexField, lpFilenameToCommand, lpIndexFilename);
			break;
		}
		if(strcmp(lpIndexFieldData, pCommandName)==NULL)
		{
			// Already got it
			break;
		}
		dwIndexField++;
	}

	// Leave category folder
	chdir("..");

	// Update category sub-menu file
	dwField=1;
	bool bAlreadyExists=false;
	strcpy(lpTXTThisCommand, pCommandName);
	wsprintf(lpSubMenuFilename, "..\\commands\\%s.txt", pCategory);
	while(dwField<2000)
	{
		wsprintf(lpField, "MENU%d", dwField);
		GetPrivateProfileString("SUBMENU", lpField, "", lpTXTCommand, 256, lpSubMenuFilename);
		if(strcmp(lpTXTCommand,lpTXTThisCommand)==NULL)
		{
			// Already got it
			bAlreadyExists=true;
			break;
		}
		if(lpTXTSyntax[0]=='@')
		{
			// End of list
			dwField++;
			break;
		}
		dwField++;
	}
	if(bAlreadyExists==false)
	{
		// New commands temporarily between 2000-3000 for reordering manually.
		while(dwField<3000)
		{
			char pAnything[256];
			wsprintf(lpField, "MENU%d", dwField);
			GetPrivateProfileString("SUBMENU", lpField, "", pAnything, 256, lpSubMenuFilename);
			if(strcmp(pAnything,"")==NULL)
			{
				// Empty - Can put it here
				wsprintf(lpField, "MENU%d", dwField);
				WritePrivateProfileString("SUBMENU", lpField, lpTXTThisCommand, lpSubMenuFilename);
				break;
			}
			dwField++;
		}
	}

	// Leave commands folder
	chdir("..");

	// Update Commands Menu
	dwField=1;
	strcpy(lpTXTThisCommand, pCategory);
	wsprintf(lpMenuFilename, "..\\helptxt\\commands.txt", pCategory);
	while(1)
	{
		wsprintf(lpField, "MENU%d", dwField);
		GetPrivateProfileString("COMMANDSMENU", lpField, "", lpTXTCommand, 256, lpMenuFilename);
		if(strcmp(lpTXTCommand,"")==NULL)
		{
			WritePrivateProfileString("COMMANDSMENU", lpField, lpTXTThisCommand, lpMenuFilename);
			break;
		}
		if(strcmp(lpTXTCommand,lpTXTThisCommand)==NULL)
		{
			// Already got it
			break;
		}
		dwField++;
	}

	// Leave helptxt folder
	chdir("..");
}

bool CInstructionTable::CheckTroubleCommandSyntax(LPSTR lpTXTThisSyntax, LPSTR pCommandName)
{
	bool bNotAnyTrouble=true;

	if(stricmp(pCommandName,"print")==NULL)
	{
		strcpy(lpTXTThisSyntax, "PRINT Print Statements");
		bNotAnyTrouble=false;
	}
	if(stricmp(pCommandName,"input")==NULL)
	{
		strcpy(lpTXTThisSyntax, "INPUT Print Statements, Input Variable");
		bNotAnyTrouble=false;
	}

	return bNotAnyTrouble;
}

DWORD CInstructionTable::DetermineInternalCommandCode(DWORD dwMathSymbol, DWORD dwTypeValue)
{
	switch(dwTypeValue)
	{
		case 1 : // INTEGER
		case 101 : // INTEGER POINTER
		switch(dwMathSymbol)
		{
			case 1 :	return IT_INTERNAL_POWERLLL;
			case 2 :	return IT_INTERNAL_MULLLL;
			case 3 :	return IT_INTERNAL_DIVLLL;
			case 4 :	return IT_INTERNAL_ADDLLL;
			case 5 :	return IT_INTERNAL_SUBLLL;
			case 6 :	return IT_INTERNAL_MODLLL;
			case 27 :	return IT_INTERNAL_EQUALLLL;
			case 25 :	return IT_INTERNAL_GREATERLLL;
			case 26 :	return IT_INTERNAL_LESSLLL;
			case 22 :	return IT_INTERNAL_NOTEQUALLLL;
			case 23 :	return IT_INTERNAL_GREATEREQUALLLL;
			case 24 :	return IT_INTERNAL_LESSEQUALLLL;
		}
		break;

		case 2 : // FLOAT
		case 102 : // FLOAT POINTER
		switch(dwMathSymbol)
		{
			case 1 :	return IT_INTERNAL_POWERFFF;
			case 2 :	return IT_INTERNAL_MULFFF;
			case 3 :	return IT_INTERNAL_DIVFFF;
			case 4 :	return IT_INTERNAL_ADDFFF;
			case 5 :	return IT_INTERNAL_SUBFFF;
			case 6 :	return IT_INTERNAL_MODFFF;
			case 27 :	return IT_INTERNAL_EQUALLFF;
			case 25 :	return IT_INTERNAL_GREATERLFF;
			case 26 :	return IT_INTERNAL_LESSLFF;
			case 22 :	return IT_INTERNAL_NOTEQUALLFF;
			case 23 :	return IT_INTERNAL_GREATEREQUALLFF;
			case 24 :	return IT_INTERNAL_LESSEQUALLFF;
		}
		break;

		case 3 : // STRING
		case 103 : // STRING POINTER
		switch(dwMathSymbol)
		{
//			case 1 :	return IT_INTERNAL_POWERSSS;
//			case 2 :	return IT_INTERNAL_MULSSS;
//			case 3 :	return IT_INTERNAL_DIVSSS;
//			case 5 :	return IT_INTERNAL_SUBSSS;
			case 4 :	return IT_INTERNAL_ADDSSS;
			case 27 :	return IT_INTERNAL_EQUALLSS;
			case 25 :	return IT_INTERNAL_GREATERLSS;
			case 26 :	return IT_INTERNAL_LESSLSS;
			case 22 :	return IT_INTERNAL_NOTEQUALLSS;
			case 23 :	return IT_INTERNAL_GREATEREQUALLSS;
			case 24 :	return IT_INTERNAL_LESSEQUALLSS;
		}
		break;

		case 4 : // BOOL
		case 104 : // BOOL POINTER
		switch(dwMathSymbol)
		{
			case 1 :	return IT_INTERNAL_POWERBBB;
			case 2 :	return IT_INTERNAL_MULBBB;
			case 3 :	return IT_INTERNAL_DIVBBB;
			case 4 :	return IT_INTERNAL_ADDBBB;
			case 5 :	return IT_INTERNAL_SUBBBB;
			case 6 :	return IT_INTERNAL_MODBBB;
			case 27 :	return IT_INTERNAL_EQUALLLL;
			case 25 :	return IT_INTERNAL_GREATERLLL;
			case 26 :	return IT_INTERNAL_LESSLLL;
			case 22 :	return IT_INTERNAL_NOTEQUALLLL;
			case 23 :	return IT_INTERNAL_GREATEREQUALLLL;
			case 24 :	return IT_INTERNAL_LESSEQUALLLL;
		}
		break;

		case 5 : // BYTE
		case 105 : // BYTE POINTER
		switch(dwMathSymbol)
		{
			case 1 :	return IT_INTERNAL_POWERYYY;
			case 2 :	return IT_INTERNAL_MULYYY;
			case 3 :	return IT_INTERNAL_DIVYYY;
			case 4 :	return IT_INTERNAL_ADDYYY;
			case 5 :	return IT_INTERNAL_SUBYYY;
			case 6 :	return IT_INTERNAL_MODYYY;
			case 27 :	return IT_INTERNAL_EQUALLLL;
			case 25 :	return IT_INTERNAL_GREATERLLL;
			case 26 :	return IT_INTERNAL_LESSLLL;
			case 22 :	return IT_INTERNAL_NOTEQUALLLL;
			case 23 :	return IT_INTERNAL_GREATEREQUALLLL;
			case 24 :	return IT_INTERNAL_LESSEQUALLLL;
		}
		break;

		case 6 : // WORD
		case 106 : // WORD POINTER
		switch(dwMathSymbol)
		{
			case 1 :	return IT_INTERNAL_POWERWWW;
			case 2 :	return IT_INTERNAL_MULWWW;
			case 3 :	return IT_INTERNAL_DIVWWW;
			case 4 :	return IT_INTERNAL_ADDWWW;
			case 5 :	return IT_INTERNAL_SUBWWW;
			case 6 :	return IT_INTERNAL_MODWWW;
			case 27 :	return IT_INTERNAL_EQUALLLL;
			case 25 :	return IT_INTERNAL_GREATERLLL;
			case 26 :	return IT_INTERNAL_LESSLLL;
			case 22 :	return IT_INTERNAL_NOTEQUALLLL;
			case 23 :	return IT_INTERNAL_GREATEREQUALLLL;
			case 24 :	return IT_INTERNAL_LESSEQUALLLL;
		}
		break;

		case 7 : // DWORD
		case 107 : // DWORD POINTER
		case 1001 : // USERDEFINED STRUCTURE VAR
		case 1101 : // USERDEFINED STRUCTURE PTR
		switch(dwMathSymbol)
		{
			case 1 :	return IT_INTERNAL_POWERDDD;
			case 2 :	return IT_INTERNAL_MULDDD;
			case 3 :	return IT_INTERNAL_DIVDDD;
			case 4 :	return IT_INTERNAL_ADDDDD;
			case 5 :	return IT_INTERNAL_SUBDDD;
			case 6 :	return IT_INTERNAL_MODDDD; // leeadd - 300305 - forgot this?!?
// lee - 240306 - u6b4 - definate differences in INT compare and DWORD compare (ie 0xFF < 0x00 = false)
//			case 27 :	return IT_INTERNAL_EQUALLLL;
//			case 25 :	return IT_INTERNAL_GREATERLLL;
//			case 26 :	return IT_INTERNAL_LESSLLL;
//			case 22 :	return IT_INTERNAL_NOTEQUALLLL;
//			case 23 :	return IT_INTERNAL_GREATEREQUALLLL;
//			case 24 :	return IT_INTERNAL_LESSEQUALLLL;
			case 27 :	return IT_INTERNAL_EQUALDDD;
			case 25 :	return IT_INTERNAL_GREATERDDD;
			case 26 :	return IT_INTERNAL_LESSDDD;
			case 22 :	return IT_INTERNAL_NOTEQUALDDD;
			case 23 :	return IT_INTERNAL_GREATEREQUALDDD;
			case 24 :	return IT_INTERNAL_LESSEQUALDDD;
		}
		break;

		case 8 : // DOUBLE FLOAT
		case 108 : // DOUBLE FLOAT POINTER
		switch(dwMathSymbol)
		{
			case 1 :	return IT_INTERNAL_POWEROOO;
			case 2 :	return IT_INTERNAL_MULOOO;
			case 3 :	return IT_INTERNAL_DIVOOO;
			case 4 :	return IT_INTERNAL_ADDOOO;
			case 5 :	return IT_INTERNAL_SUBOOO;
			case 27 :	return IT_INTERNAL_EQUALLOO;
			case 25 :	return IT_INTERNAL_GREATERLOO;
			case 26 :	return IT_INTERNAL_LESSLOO;
			case 22 :	return IT_INTERNAL_NOTEQUALLOO;
			case 23 :	return IT_INTERNAL_GREATEREQUALLOO;
			case 24 :	return IT_INTERNAL_LESSEQUALLOO;
		}
		break;

		case 9 : // DOUBLE INTEGER
		case 109 : // DOUBLE INTEGER POINTER
		switch(dwMathSymbol)
		{
			case 1 :	return IT_INTERNAL_POWERRRR;
			case 2 :	return IT_INTERNAL_MULRRR;
			case 3 :	return IT_INTERNAL_DIVRRR;
			case 4 :	return IT_INTERNAL_ADDRRR;
			case 5 :	return IT_INTERNAL_SUBRRR;
			case 27 :	return IT_INTERNAL_EQUALLRR;
			case 25 :	return IT_INTERNAL_GREATERLRR;
			case 26 :	return IT_INTERNAL_LESSLRR;
			case 22 :	return IT_INTERNAL_NOTEQUALLRR;
			case 23 :	return IT_INTERNAL_GREATEREQUALLRR;
			case 24 :	return IT_INTERNAL_LESSEQUALLRR;
		}
		break;
	}

	// Not Type Based
	switch(dwMathSymbol)
	{
		case 11 :	return IT_INTERNAL_SHIFTRLLL;
		case 12 :	return IT_INTERNAL_SHIFTLLLL;
		case 31 :	return IT_INTERNAL_BITANDLLL;
		case 32 :	return IT_INTERNAL_BITORLLL;
		case 33 :	return IT_INTERNAL_BITXORLLL;
		case 34 :	return IT_INTERNAL_BITNOTLLL;

		case 41 :	return IT_INTERNAL_ANDLLL;
		case 42 :	return IT_INTERNAL_ORLLL;
		case 43 :	return IT_INTERNAL_NOTLLL;

		case 101 :	return 0;//IT_INTERNAL_CASTLTOL;
		case 102 :	return IT_INTERNAL_CASTLTOF;
		case 103 :	return 0;//IT_INTERNAL_CASTLTOS;
		case 104 :	return IT_INTERNAL_CASTLTOB;
		case 105 :	return IT_INTERNAL_CASTLTOY;
		case 106 :	return IT_INTERNAL_CASTLTOW;
		case 107 :	return IT_INTERNAL_CASTLTOD;
		case 108 :	return IT_INTERNAL_CASTLTOO;
		case 109 :	return IT_INTERNAL_CASTLTOR;

		case 111 :	return IT_INTERNAL_CASTFTOL;
		case 112 :	return 0;//IT_INTERNAL_CASTFTOF;
		case 114 :	return IT_INTERNAL_CASTFTOB;
		case 115 :	return IT_INTERNAL_CASTFTOY;
		case 116 :	return IT_INTERNAL_CASTFTOW;
		case 117 :	return IT_INTERNAL_CASTFTOD;
		case 118 :	return IT_INTERNAL_CASTFTOO;
		case 119 :	return IT_INTERNAL_CASTFTOR;

		case 131 :	return IT_INTERNAL_CASTBTOL;
		case 132 :	return IT_INTERNAL_CASTBTOF;
		case 134 :	return 0;//IT_INTERNAL_CASTBTOB;
		case 135 :	return 0;//IT_INTERNAL_CASTBTOY;
		case 136 :	return IT_INTERNAL_CASTBTOW;
		case 137 :	return IT_INTERNAL_CASTBTOD;
		case 138 :	return IT_INTERNAL_CASTBTOO;
		case 139 :	return IT_INTERNAL_CASTBTOR;

		case 141 :	return IT_INTERNAL_CASTYTOL;
		case 142 :	return IT_INTERNAL_CASTYTOF;
		case 144 :	return 0;//IT_INTERNAL_CASTYTOB;
		case 145 :	return 0;//IT_INTERNAL_CASTYTOY;
		case 146 :	return IT_INTERNAL_CASTYTOW;
		case 147 :	return IT_INTERNAL_CASTYTOD;
		case 148 :	return IT_INTERNAL_CASTYTOO;
		case 149 :	return IT_INTERNAL_CASTYTOR;

		case 151 :	return IT_INTERNAL_CASTWTOL;
		case 152 :	return IT_INTERNAL_CASTWTOF;
		case 154 :	return IT_INTERNAL_CASTWTOB;
		case 155 :	return IT_INTERNAL_CASTWTOY;
		case 156 :	return 0;//IT_INTERNAL_CASTWTOW;
		case 157 :	return IT_INTERNAL_CASTWTOD;
		case 158 :	return IT_INTERNAL_CASTWTOO;
		case 159 :	return IT_INTERNAL_CASTWTOR;

		case 161 :	return IT_INTERNAL_CASTDTOL;
		case 162 :	return IT_INTERNAL_CASTDTOF;
		case 164 :	return IT_INTERNAL_CASTDTOB;
		case 165 :	return IT_INTERNAL_CASTDTOY;
		case 166 :	return IT_INTERNAL_CASTDTOW;
		case 167 :	return 0;//IT_INTERNAL_CASTDTOD;
		case 168 :	return IT_INTERNAL_CASTDTOO;
		case 169 :	return IT_INTERNAL_CASTDTOR;

		case 171 :	return IT_INTERNAL_CASTOTOL;
		case 172 :	return IT_INTERNAL_CASTOTOF;
		case 174 :	return IT_INTERNAL_CASTOTOB;
		case 175 :	return IT_INTERNAL_CASTOTOY;
		case 176 :	return IT_INTERNAL_CASTOTOW;
		case 177 :	return IT_INTERNAL_CASTOTOD;
		case 178 :	return 0;//IT_INTERNAL_CASTOTOO;
		case 179 :	return IT_INTERNAL_CASTOTOR;

		case 181 :	return IT_INTERNAL_CASTRTOL;
		case 182 :	return IT_INTERNAL_CASTRTOF;
		case 184 :	return IT_INTERNAL_CASTRTOB;
		case 185 :	return IT_INTERNAL_CASTRTOY;
		case 186 :	return IT_INTERNAL_CASTRTOW;
		case 187 :	return IT_INTERNAL_CASTRTOD;
		case 188 :	return IT_INTERNAL_CASTRTOO;
		case 189 :	return 0;//IT_INTERNAL_CASTRTOR;
	}
	return 0;
}

bool CInstructionTable::EnsureWordIsNotPartOfACommand ( LPSTR pConstantName )
{
#ifdef __AARON_INSTRPERF__
	//
	//	TODO: Implement this.
	//
	return false;
#else
	// go through entire command database and make sure the submitted word does NOT occur
	if ( m_pFirstInstructionEntry && pConstantName )
	{
		LPSTR pLastCommand = NULL;
		LPSTR pPtr, pPtrBegin, pPtrEnd;
		DWORD dwCompareSize = strlen( pConstantName );
		CInstructionTableEntry* pEntry = m_pFirstInstructionEntry;
		while(pEntry)
		{
			// get command name
			LPSTR pCommandName = pEntry->GetName()->GetStr();

			// quick rejects
			if ( pCommandName )
			{
				if ( pCommandName[0]==0 )
					goto _next;
				if ( pCommandName[0]=='+' )
					goto _next;
			}

			// skip if this same as last
			if ( pLastCommand )
				if ( strcmp ( pLastCommand, pCommandName )==NULL )
					goto _next;

			// compare command string with passed in constant-name
			pPtr = pCommandName;
			pPtrEnd = pPtr + strlen(pCommandName);
			pPtrBegin = pPtr;
			while ( pPtr<=pPtrEnd )
			{
				// space or end of string
				if ( *pPtr==32 || *pPtr==0 )
				{
					// this one word (leefix - 260604 - u54-must be greater than one letter - deal with JOYSTICK FIRE A( by detecting bracket)
					DWORD dwOneWordLength = pPtr - pPtrBegin;
					if ( dwOneWordLength > 1 )
					{
						// leefix - 260604 - u54 - largest word rules
						DWORD dwThisCompareSize = dwCompareSize;
						if ( dwThisCompareSize<dwOneWordLength ) dwThisCompareSize=dwOneWordLength;
						if ( strnicmp ( pPtrBegin, pConstantName, dwThisCompareSize )==NULL )
						{
							// this word matches the one word from the command, leave!
							return true;
						}
					}

					// new word, begin
					pPtrBegin = pPtr+1;
				}

				// next char
				pPtr++;
			}
			pLastCommand = pCommandName;

			// get next command
			_next: pEntry=pEntry->GetNext();
		}
	}

	// complete
	return false;
#endif
}
