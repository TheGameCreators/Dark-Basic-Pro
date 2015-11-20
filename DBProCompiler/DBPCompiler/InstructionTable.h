// InstructionTable.h: interface for the CInstructionTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSTRUCTIONTABLE_H__2C54A6BC_B075_484B_9706_9BBCE0C8F37F__INCLUDED_)
#define AFX_INSTRUCTIONTABLE_H__2C54A6BC_B075_484B_9706_9BBCE0C8F37F__INCLUDED_

#define __AARON_INSTRPERF__ 1

// Includes
#include "ParserHeader.h"
#include "InstructionTableEntry.h"
#include "Task.h"

#include "PerfMacros.h"

#ifdef __AARON_INSTRPERF__
# include <DB3Dict.h>
# include <DB3Array.h>
#endif

// Defines (for m_InternalInstructions[])
#define IT_INTERNAL_MAXCOUNT			1000

#define IT_INTERNAL_ALLOC				1
#define IT_INTERNAL_FREE				2
#define IT_INTERNAL_ASSIGNLL			3
#define IT_INTERNAL_ASSIGNFF			4
#define IT_INTERNAL_ASSIGNSS			5
#define IT_INTERNAL_ASSIGNBB			6
#define IT_INTERNAL_ASSIGNYY			7
#define IT_INTERNAL_ASSIGNWW			8
#define IT_INTERNAL_ASSIGNDD			9
#define IT_INTERNAL_ASSIGNOO			10
#define IT_INTERNAL_ASSIGNRR			11
#define IT_INTERNAL_ASSIGNPP			12
#define IT_INTERNAL_RELASSIGNLL			13
#define IT_INTERNAL_RELASSIGNFF			14
#define IT_INTERNAL_RELASSIGNSS			15
#define IT_INTERNAL_RELASSIGNBB			16
#define IT_INTERNAL_RELASSIGNYY			17
#define IT_INTERNAL_RELASSIGNWW			18
#define IT_INTERNAL_RELASSIGNDD			19
#define IT_INTERNAL_RELASSIGNOO			20
#define IT_INTERNAL_RELASSIGNRR			21
#define IT_INTERNAL_STRFREE				22
#define IT_INTERNAL_USERFUNCTIONEXIT	24
#define IT_INTERNAL_ASSIGNUDT			25

// Internal Math Instructions

// Integer Maths
#define IT_INTERNAL_POWERLLL			51
#define IT_INTERNAL_MULLLL				52
#define IT_INTERNAL_DIVLLL				53
#define IT_INTERNAL_ADDLLL				54
#define IT_INTERNAL_SUBLLL				55
#define IT_INTERNAL_MODLLL				56

#define IT_INTERNAL_EQUALLLL			57
#define IT_INTERNAL_GREATERLLL			58
#define IT_INTERNAL_LESSLLL				59
#define IT_INTERNAL_NOTEQUALLLL			60
#define IT_INTERNAL_GREATEREQUALLLL		61
#define IT_INTERNAL_LESSEQUALLLL		62

// Float Maths
#define IT_INTERNAL_POWERFFF			71
#define IT_INTERNAL_MULFFF				72
#define IT_INTERNAL_DIVFFF				73
#define IT_INTERNAL_ADDFFF				74
#define IT_INTERNAL_SUBFFF				75
#define IT_INTERNAL_MODFFF				82
#define IT_INTERNAL_EQUALLFF			76
#define IT_INTERNAL_GREATERLFF			77
#define IT_INTERNAL_LESSLFF				78
#define IT_INTERNAL_NOTEQUALLFF			79
#define IT_INTERNAL_GREATEREQUALLFF		80
#define IT_INTERNAL_LESSEQUALLFF		81

// Double Float Maths
#define IT_INTERNAL_POWEROOO			91
#define IT_INTERNAL_MULOOO				92
#define IT_INTERNAL_DIVOOO				93
#define IT_INTERNAL_ADDOOO				94
#define IT_INTERNAL_SUBOOO				95
#define IT_INTERNAL_EQUALLOO			96
#define IT_INTERNAL_GREATERLOO			97
#define IT_INTERNAL_LESSLOO				98
#define IT_INTERNAL_NOTEQUALLOO			99
#define IT_INTERNAL_GREATEREQUALLOO		100
#define IT_INTERNAL_LESSEQUALLOO		101

// String Maths
#define IT_INTERNAL_POWERSSS			111
#define IT_INTERNAL_MULSSS				112
#define IT_INTERNAL_DIVSSS				113
#define IT_INTERNAL_ADDSSS				114
#define IT_INTERNAL_SUBSSS				115
#define IT_INTERNAL_EQUALLSS			121
#define IT_INTERNAL_GREATERLSS			122
#define IT_INTERNAL_LESSLSS				123
#define IT_INTERNAL_NOTEQUALLSS			124
#define IT_INTERNAL_GREATEREQUALLSS		125
#define IT_INTERNAL_LESSEQUALLSS		126

// Double Integer Maths
#define IT_INTERNAL_POWERRRR			31
#define IT_INTERNAL_MULRRR				32
#define IT_INTERNAL_DIVRRR				33
#define IT_INTERNAL_ADDRRR				34
#define IT_INTERNAL_SUBRRR				35
#define IT_INTERNAL_EQUALLRR			36
#define IT_INTERNAL_GREATERLRR			37
#define IT_INTERNAL_LESSLRR				38
#define IT_INTERNAL_NOTEQUALLRR			39
#define IT_INTERNAL_GREATEREQUALLRR		40
#define IT_INTERNAL_LESSEQUALLRR		41

// Bitwise Maths
#define IT_INTERNAL_SHIFTLLLL			141
#define IT_INTERNAL_SHIFTRLLL			142
#define IT_INTERNAL_BITORLLL			143
#define IT_INTERNAL_BITANDLLL			144
#define IT_INTERNAL_BITXORLLL			145
#define IT_INTERNAL_BITNOTLLL			149

// Comparison Maths
#define IT_INTERNAL_ORLLL				146
#define IT_INTERNAL_ANDLLL				147
#define IT_INTERNAL_NOTLLL				148

// DWORD Pointer Maths
#define IT_INTERNAL_POWERDDD			151
#define IT_INTERNAL_MULDDD				152
#define IT_INTERNAL_DIVDDD				153
#define IT_INTERNAL_ADDDDD				154
#define IT_INTERNAL_SUBDDD				155
#define IT_INTERNAL_MODDDD				156

// lee - 240306 - u6b4 - re-introduced becasue 0xFF < 0x00 = false in DWORD terms
#define IT_INTERNAL_EQUALDDD			156
#define IT_INTERNAL_GREATERDDD			157
#define IT_INTERNAL_LESSDDD				158
#define IT_INTERNAL_NOTEQUALDDD			159
#define IT_INTERNAL_GREATEREQUALDDD		160
#define IT_INTERNAL_LESSEQUALDDD		161

// Boolean Maths
#define IT_INTERNAL_POWERBBB			171
#define IT_INTERNAL_MULBBB				172
#define IT_INTERNAL_DIVBBB				173
#define IT_INTERNAL_ADDBBB				174
#define IT_INTERNAL_SUBBBB				175
#define IT_INTERNAL_MODBBB				176

// BYTE Maths
#define IT_INTERNAL_POWERYYY			181
#define IT_INTERNAL_MULYYY				182
#define IT_INTERNAL_DIVYYY				183
#define IT_INTERNAL_ADDYYY				184
#define IT_INTERNAL_SUBYYY				185
#define IT_INTERNAL_MODYYY				186

// WORD Maths
#define IT_INTERNAL_POWERWWW			191
#define IT_INTERNAL_MULWWW				192
#define IT_INTERNAL_DIVWWW				193
#define IT_INTERNAL_ADDWWW				194
#define IT_INTERNAL_SUBWWW				195
#define IT_INTERNAL_MODWWW				196

// Casting Maths
#define IT_INTERNAL_CASTLTOF			201
#define IT_INTERNAL_CASTLTOB			202
#define IT_INTERNAL_CASTLTOY			203
#define IT_INTERNAL_CASTLTOW			204
#define IT_INTERNAL_CASTLTOD			205
#define IT_INTERNAL_CASTLTOO			206
#define IT_INTERNAL_CASTLTOR			207
#define IT_INTERNAL_CASTFTOL			211
#define IT_INTERNAL_CASTFTOB			212
#define IT_INTERNAL_CASTFTOY			213
#define IT_INTERNAL_CASTFTOW			214
#define IT_INTERNAL_CASTFTOD			215
#define IT_INTERNAL_CASTFTOO			216
#define IT_INTERNAL_CASTFTOR			217
#define IT_INTERNAL_CASTBTOL			221
#define IT_INTERNAL_CASTBTOF			222
#define IT_INTERNAL_CASTBTOY			223
#define IT_INTERNAL_CASTBTOW			224
#define IT_INTERNAL_CASTBTOD			225
#define IT_INTERNAL_CASTBTOO			226
#define IT_INTERNAL_CASTBTOR			227
#define IT_INTERNAL_CASTYTOL			231
#define IT_INTERNAL_CASTYTOF			232
#define IT_INTERNAL_CASTYTOB			233
#define IT_INTERNAL_CASTYTOW			234
#define IT_INTERNAL_CASTYTOD			235
#define IT_INTERNAL_CASTYTOO			236
#define IT_INTERNAL_CASTYTOR			237
#define IT_INTERNAL_CASTWTOL			241
#define IT_INTERNAL_CASTWTOF			242
#define IT_INTERNAL_CASTWTOB			243
#define IT_INTERNAL_CASTWTOY			244
#define IT_INTERNAL_CASTWTOD			245
#define IT_INTERNAL_CASTWTOO			246
#define IT_INTERNAL_CASTWTOR			247
#define IT_INTERNAL_CASTDTOL			251
#define IT_INTERNAL_CASTDTOF			252
#define IT_INTERNAL_CASTDTOB			253
#define IT_INTERNAL_CASTDTOY			254
#define IT_INTERNAL_CASTDTOW			255
#define IT_INTERNAL_CASTDTOO			256
#define IT_INTERNAL_CASTDTOR			257
#define IT_INTERNAL_CASTOTOL			261
#define IT_INTERNAL_CASTOTOF			262
#define IT_INTERNAL_CASTOTOB			263
#define IT_INTERNAL_CASTOTOY			264
#define IT_INTERNAL_CASTOTOW			265
#define IT_INTERNAL_CASTOTOD			266
#define IT_INTERNAL_CASTOTOR			267
#define IT_INTERNAL_CASTRTOL			271
#define IT_INTERNAL_CASTRTOF			272
#define IT_INTERNAL_CASTRTOB			273
#define IT_INTERNAL_CASTRTOY			274
#define IT_INTERNAL_CASTRTOW			275
#define IT_INTERNAL_CASTRTOD			276
#define IT_INTERNAL_CASTRTOO			277

// Internal Command
#define IT_INTERNAL_RETURN				301
#define IT_INTERNAL_END					302
#define IT_INTERNAL_SYNC				303
#define IT_INTERNAL_STARTPROGRAM		304
#define IT_INTERNAL_ENDPROGRAM			305
#define IT_INTERNAL_INCVAR				306
#define IT_INTERNAL_DECVAR				307
#define IT_INTERNAL_PURERETURN			308
#define IT_INTERNAL_ENDERROR			309

// Defines for Build-In-Instruction Tasks
#define BUILD_RET						1
#define BUILD_END						2
#define BUILD_SYNC						3
#define BUILD_STARTPROGRAM				4
#define BUILD_ENDPROGRAMANDQUIT			5
#define BUILD_USERFUNCTIONEXIT			6
#define BUILD_PURERET					7
#define BUILD_COPYUDT					8
#define	BUILD_ENDERROR					9

#define BUILD_POWER						101
#define BUILD_MUL						102
#define BUILD_DIV						103
#define BUILD_ADD						104
#define BUILD_SUB						105
#define BUILD_MOD						106

#define BUILD_SHR						151
#define BUILD_SHL						152
#define BUILD_BITAND					153
#define BUILD_BITOR						154
#define BUILD_BITXOR					155
#define BUILD_BITNOT					156

#define BUILD_AND						161
#define BUILD_OR						162
#define BUILD_NOT						163

#define BUILD_EQUAL						201
#define BUILD_GREATER					202
#define BUILD_LESS						203
#define BUILD_NOTEQUAL					204
#define BUILD_GREATEREQUAL				205
#define BUILD_LESSEQUAL					206

#define BUILD_INC						1001
#define BUILD_DEC						1002
#define BUILD_INCADD					1003
#define BUILD_DECADD					1004


// Class Definition
class CInstructionTable  
{
	public:
		CInstructionTable();
		virtual ~CInstructionTable();

		void		ScanPluginsForCommands(void);
		bool		LoadInstructionDatabase(void);
		bool		DefineHardCodedCommand(void);
		bool		SetInternalInstructionDatabase(void);

		void		SetRef(DWORD dwRefIndex, CInstructionTableEntry* pRef) { m_InternalInstructRef[dwRefIndex]=pRef; }
		CInstructionTableEntry* GetRef(DWORD dwRefIndex) { return m_InternalInstructRef[dwRefIndex]; }
		void		SetIIValue(DWORD dwRefIndex, DWORD dwValue) { m_InternalInstructions[dwRefIndex]=dwValue; }
		DWORD		GetIIValue(DWORD dwRefIndex) { return m_InternalInstructions[dwRefIndex]; }

		bool		AddCommand(LPSTR pName, LPSTR pDLL, LPSTR pDecoratedName, LPSTR pParamTypesString, DWORD resultp, DWORD pmax);
		bool		AddBuildCommand(LPSTR pName, LPSTR pDesc, LPSTR pParamTypesString, DWORD resultp, DWORD pmax, DWORD dwInternal, DWORD dwBuildID);
		bool		AddCommandCore(LPSTR pName, LPSTR pDLL, LPSTR pDecoratedName, LPSTR pParamTypesString, DWORD resultp, DWORD pmax, DWORD dwInternalValueIndex, DWORD dwBuildID);
		bool		AddCommandCore2(LPSTR pName, LPSTR pDLL, LPSTR pDecoratedName, LPSTR pParamTypesString, DWORD resultp, DWORD pmax, DWORD dwInternalValueIndex, DWORD dwBuildID, DWORD dwPlace, bool bPassArrayAsInput, CStr* pParamFullDesc, LPSTR* plpretStr);
		bool		AddUserFunction(LPSTR pName, DWORD resultp, LPSTR pParamTypesString, DWORD pmax, CDeclaration* pDecChain);
		bool		AddUniqueCommand(LPSTR pName, LPSTR pDLL, LPSTR pDecoratedName, LPSTR pParamTypesString, DWORD resultp, DWORD pmax);

		CInstructionTableEntry *GetEntry(int iType, const char *pStringData, bool *pRetFail=nullptr);
		CInstructionTableEntry *ResolveEntry(CInstructionTableEntry *pHead, int iWithAnyReturnValue, bool bCommandWhiteSpace=false);
		bool		FindEntryDirect(int iType, bool bCommandWhiteSpace, LPSTR pStringData, int iWithReturnValue, DWORD* pdwData, DWORD* pdwParamMax, DWORD* pdwLength, CInstructionTableEntry** ppRef);
		bool		FindEntry(int iType, bool bCommandWhiteSpace, CInstructionTableEntry* pEntry, LPSTR pStringData, int iWithReturnValue, DWORD* pdwData, DWORD* pdwParamMax, DWORD* pdwLength, CInstructionTableEntry** ppRef);

		bool		FindInstruction(bool bCommandWhiteSpace, LPSTR pStringData, int iWithReturnValue, DWORD* Data, DWORD* pdwParamMax, DWORD* Length, CInstructionTableEntry** ppRef);
		bool		FindUserFunction(LPSTR pStringData, int iWithReturnValue, DWORD* Data, DWORD* pdwParamMax, DWORD* Length);

		bool		FindInstructionParams(DWORD dwInstructionValue, DWORD dwParamMax, DWORD* pdwData, DWORD* pdwParamMax, CStr** pValidParamTypes, CInstructionTableEntry** pRefEntry);
		bool		FindUserFunctionParams(DWORD dwInstructionValue, DWORD dwParamMax, DWORD* pdwData, DWORD* pdwParamMax, CStr** pValidParamTypes, CInstructionTableEntry** pRefEntry);
		bool		CompareInstructionNames(CInstructionTableEntry* pRefEntryA, CInstructionTableEntry* pRefEntryB);
		bool		FindInstructionWithNameAndParams(LPSTR pFriendName, LPSTR pParams);

		CInstructionTableEntry* FindUserFunction(LPSTR pUserFunctionName);
		CInstructionTableEntry* FindReservedFunction(LPSTR pFunctionName);
		CInstructionTableEntry* FindLastFriendOfName(LPSTR pFriendName);

		void		ScanStart(void);
		void		ScanStep(void);
		void		ScanEnd(void);

		bool		VerifyCertificateForPlugin ( LPSTR pDLLName, LPSTR pProductName );
		LPSTR		ReadRawStringTable ( LPSTR pFilenameEXE, DWORD* pdwDataSize  );
		bool		LoadCommandsFromDLL(LPSTR pCategory, LPSTR pFilename);
		bool		TurnStringIntoCommand(LPSTR pCategory, LPSTR pDLLName, LPSTR pRawCommandString);
		void		AddCommandToHelpTxt(LPSTR pCategory, LPSTR pDLLName, LPSTR pParamStr, DWORD dwReturnParam, DWORD dwParamCount, LPSTR pParamDesc);
		bool		CheckTroubleCommandSyntax(LPSTR lpTXTThisSyntax, LPSTR pCommandName);
	
		DWORD		DetermineInternalCommandCode(DWORD dwMathSymbol, DWORD dwTypeValue);

		bool		EnsureWordIsNotPartOfACommand ( LPSTR pConstantName );

		inline CInstructionTableEntry *GetEntryByIndex(db3::uint index) { return m_EntryArray[index]; }

	private:
		// Types
		typedef db3::TDictionary<CInstructionTableEntry> map_type;
		typedef db3::CLock lock_type;
		typedef db3::CAutolock autolock_type;
		typedef db3::TArray<CInstructionTableEntry *> array_type;

		// Track Current Internal ID (Counter)
		DWORD						m_dwCurrentInternalID;

		// Instruction Maps
#ifdef __AARON_INSTRPERF__
		map_type					m_InstructionMap;
		lock_type					m_InstructionMapLock;

		map_type					m_UserFunctionMap;
		lock_type					m_UserFunctionMapLock;

		array_type					m_EntryArray;
#else
		CInstructionTableEntry*		m_pFirstInstructionEntry;
		CInstructionTableEntry*		m_pFirstUserFunctionEntry;

		lock_type					m_Lock;
#endif

		// Internal Instruction Value Database
		DWORD						m_InternalInstructions[IT_INTERNAL_MAXCOUNT];
		CInstructionTableEntry*		m_InternalInstructRef[IT_INTERNAL_MAXCOUNT];
};

#endif // !defined(AFX_INSTRUCTIONTABLE_H__2C54A6BC_B075_484B_9706_9BBCE0C8F37F__INCLUDED_)
