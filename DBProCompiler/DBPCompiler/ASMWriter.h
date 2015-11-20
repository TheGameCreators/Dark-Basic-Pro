// ASMWriter.h: interface for the CASMWriter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASMWRITER_H__A3BE66B0_1587_46D3_AC0A_E4F78C0A3561__INCLUDED_)
#define AFX_ASMWRITER_H__A3BE66B0_1587_46D3_AC0A_E4F78C0A3561__INCLUDED_

// Common Includes
#include "windows.h"
#include "macros.h"

// Custom Includes
#include "Statement.h"
#include "EXEBlock.h"
#include "Str.h"

// ASKTask Defines
#define ASMMAXCOUNT 300
#define ASMTASK_ASSIGN 1
#define ASMTASK_TEST 4
#define ASMTASK_CALL 5
#define ASMTASK_PUSH 6
#define ASMTASK_POPEAX 7
#define ASMTASK_POPEBX 8
#define ASMTASK_UNKNOWN 9
#define ASMTASK_CONDITION 10
#define ASMTASK_CONDJUMPNE 11
#define ASMTASK_CONDJUMPE 12
#define ASMTASK_CONDGREATER 13
#define ASMTASK_CONDLESS 14
#define ASMTASK_JUMP 15
#define ASMTASK_JUMPSUBROUTINE 16
#define ASMTASK_RETURN 17
#define ASMTASK_ASSIGNTOEAX 18
#define ASMTASK_CONDITIONDATA 19
#define ASMTASK_ADDESP 20
#define ASMTASK_SUBESP 21
#define ASMTASK_PUSHEBP 22
#define ASMTASK_POPEBP 23
#define ASMTASK_MOVEBPESP 24
#define ASMTASK_MOVESPEBP 25
#define ASMTASK_STOREESP 26
#define ASMTASK_RESTOREESP 27
#define ASMTASK_PUSHREGISTERS 28
#define ASMTASK_POPREGISTERS 29
#define ASMTASK_CLEARSTACK 30
#define ASMTASK_DEBUGSTATEMENTHOOK 31
#define ASMTASK_DEBUGJUMPHOOK 32
#define ASMTASK_DEBUGRETURNHOOK 33
#define ASMTASK_RUNTIMEERRORHOOK 34
#define ASMTASK_BREAKPOINTRESUME 37
#define ASMTASK_JUMPMEM 38
#define ASMTASK_PUSHESP 39
#define ASMTASK_PURERETURN 40
#define ASMTASK_PUSHADDRESS 41
#define ASMTASK_PUSHUDT 42

#define ASMTASK_POWER			101
#define ASMTASK_MUL				102
#define ASMTASK_DIV				103
#define ASMTASK_ADD				104
#define ASMTASK_SUB				105
#define ASMTASK_MOD				106

#define ASMTASK_EQUAL			111
#define ASMTASK_GREATER			112
#define ASMTASK_LESS			113
#define ASMTASK_NOTEQUAL		114
#define ASMTASK_GREATEREQUAL	115
#define ASMTASK_LESSEQUAL		116

#define ASMTASK_SHL				131
#define ASMTASK_SHR				132
#define ASMTASK_AND				133
#define ASMTASK_OR				134
#define ASMTASK_NOT				135
#define ASMTASK_XOR				136
#define ASMTASK_BITNOT			137

#define ASMTASK_SETNORETURNIFESPLEAK 501
#define ASMTASK_CALCARRAYOFFSET		502
#define ASMTASK_PUSHINTERNALARRAYINDEX 503

#define ASMTASK_INCVAR		1001
#define ASMTASK_DECVAR		1002


// Defines (1-4/5-8 11-14/15-18 numerical layout important - see DetermineASM)
#define ASM_MOVEAXMEM1 2
#define ASM_MOVEAXMEM2 3
#define ASM_MOVEAXMEM4 4
#define ASM_MOVMEMEAX1 5
#define ASM_MOVMEMEAX2 6
#define ASM_MOVMEMEAX4 7

#define ASM_MOVECXOFFEAX1 8
#define ASM_MOVECXOFFEAX2 9
#define ASM_MOVECXOFFEAX4 10

#define ASM_RELMOVEAXIMM 11
#define ASM_RELMOVEAXMEM1 12
#define ASM_RELMOVEAXMEM2 13
#define ASM_RELMOVEAXMEM4 14
#define ASM_RELMOVMEMEAX1 15
#define ASM_RELMOVMEMEAX2 16
#define ASM_RELMOVMEMEAX4 17

#define ASM_RELMOVEAXEDX1 18
#define ASM_RELMOVEAXEDX2 19
#define ASM_RELMOVEAXEDX4 20

#define ASM_MOVEAXIMM1 21
#define ASM_MOVEAXIMM2 22
#define ASM_MOVEAXIMM4 23

#define ASM_MOVEDXIMM4 24

#define ASM_MOVEDXEAX4 26

#define ASM_RELMOVEAXREDX1 27
#define ASM_RELMOVEAXREDX2 28
#define ASM_RELMOVEAXREDX4 29

#define ASM_ADDEAX1 31
#define ASM_ADDEAX2 32
#define ASM_ADDEAX4 33
#define ASM_ADDEAXEBX1 34
#define ASM_ADDEAXEBX2 35
#define ASM_ADDEAXEBX4 36

#define ASM_ADDEAXECX4 37

#define ASM_MOVEAXECXOFF1 38
#define ASM_MOVEAXECXOFF2 39
#define ASM_MOVEAXECXOFF4 40

#define ASM_MOVMEMST08 41
#define ASM_MOVST0MEM8 42
#define ASM_MOVECXIMM4 43

#define ASM_MOVST0ECXOFF8 44
#define ASM_MOVECXOFFST08 45

#define ASM_MOVMEMIMM1 46
#define ASM_MOVMEMIMM2 47
#define ASM_MOVMEMIMM4 48

#define ASM_PUSHEAX 51
#define ASM_PUSHEDX 52
#define ASM_PUSHEBX 68

#define ASM_PUSHRELEAX1 53
#define ASM_PUSHRELEAX2 54
#define ASM_PUSHRELEAX4 55

#define ASM_POPEAX 57
#define ASM_POPEBX 58
#define ASM_CALLEAX 59
#define ASM_CALLMEM 60
#define ASM_RET 61

#define ASM_ADDESP 62
#define ASM_SUBESP 63
#define ASM_PUSHEBP 64
#define ASM_POPEBP 65
#define ASM_MOVEBPESP 66
#define ASM_MOVESPEBP 67

#define ASM_UNKNOWN 71

#define ASM_MOVEBPIMM1 72
#define ASM_MOVEBPIMM2 73
#define ASM_MOVEBPIMM4 74

#define ASM_CMPEAX1 77
#define ASM_CMPEAX2 78
#define ASM_CMPEAX4 79

#define ASM_JMP 81
#define ASM_JNE 82
#define ASM_JE 83


#define ASM_MOVEAXSIB4 85
#define ASM_MOVECXEAX4 86

#define ASM_MOVEAXEBP1 87
#define ASM_MOVEAXEBP2 88
#define ASM_MOVEAXEBP4 89

#define ASM_MOVEAXESP 90

#define ASM_MOVEBPEAX1 91
#define ASM_MOVEBPEAX2 92
#define ASM_MOVEBPEAX4 93

#define ASM_MOVEBPST08 94
#define ASM_MOVST0EBP8 95

#define ASM_PUSHEBP4 96

#define ASM_MOVMEMESP4 97
#define ASM_MOVESPMEM4 98

#define ASM_MOVEAXOFFECX1 99
#define ASM_MOVEAXOFFECX2 100
#define ASM_MOVEAXOFFECX4 101

#define ASM_MOVECXEAXOFF1 102
#define ASM_MOVECXEAXOFF2 103
#define ASM_MOVECXEAXOFF4 104

#define ASM_MOVECXEDX4 106
#define ASM_MOVEDXECX4 107

#define ASM_MOVEAXST08 108
#define ASM_MOVST0EAX8 109

#define ASM_PUSHAD 110
#define ASM_POPAD 111

#define ASM_LOOP 112
#define ASM_MOVSIB4IMM4 113
#define ASM_MOVSIB4IMM1 114

#define ASM_MOVEAXECX1 115
#define ASM_MOVEAXECX2 116
#define ASM_MOVEAXECX4 117

#define ASM_PUSHIMM4 118

#define ASM_MOVEBXMEM4 119

#define ASM_INCMEM1 120
#define ASM_INCMEM2 121
#define ASM_INCMEM4 122
#define ASM_DECMEM1 123
#define ASM_DECMEM2 124
#define ASM_DECMEM4 125

#define ASM_CALLABS 126
#define ASM_PUSHESP 127
#define ASM_CALLEBX 128

#define ASM_SUBESPEAX 129
#define ASM_JMPREL 130
#define ASM_JMPEBX 131

#define ASM_MOVEBXEAXOFF1 135
#define ASM_MOVEBXEAXOFF2 136
#define ASM_MOVEBXEAXOFF4 137

#define ASM_MOVEDXEAXOFF1 138
#define ASM_MOVEDXEAXOFF2 139
#define ASM_MOVEDXEAXOFF4 140

#define ASM_CMPGREEDXEBX 141
#define ASM_JGE 142
#define ASM_JLE 143

#define ASM_MOVEAXECXREL1 144
#define ASM_MOVEAXECXREL2 145
#define ASM_MOVEAXECXREL4 146

#define ASM_MOVEAXEAXREL1 147
#define ASM_MOVEAXEAXREL2 148
#define ASM_MOVEAXEAXREL4 149

#define ASM_MOVEBXEAX1 150
#define ASM_MOVEBXEAX2 151
#define ASM_MOVEBXEAX4 152

#define ASM_SUBEAX1 153
#define ASM_SUBEAX2 154
#define ASM_SUBEAX4 155
#define ASM_SUBEAXEBX1 156
#define ASM_SUBEAXEBX2 157
#define ASM_SUBEAXEBX4 158

#define ASM_DIVEAXEBX1 159
#define ASM_DIVEAXEBX2 160
#define ASM_DIVEAXEBX4 161

#define ASM_MULEAXEBX1 162
#define ASM_MULEAXEBX2 163
#define ASM_MULEAXEBX4 164

#define ASM_MOVEBXIMM1 165
#define ASM_MOVEBXIMM2 166
#define ASM_MOVEBXIMM4 167

#define ASM_CDQ 168

#define ASM_CMPEDXEBX1 169
#define ASM_CMPEDXEBX2 170
#define ASM_CMPEDXEBX4 171

#define ASM_SETE 172
#define ASM_SETNE 173
#define ASM_SETG 174
#define ASM_SETGE 175
#define ASM_SETL 176
#define ASM_SETLE 177

#define ASM_CMPEBX1 178
#define ASM_CMPEBX2 179
#define ASM_CMPEBX4 180

#define ASM_ANDEAX1 181
#define ASM_ANDEAX2 182
#define ASM_ANDEAX4 183
#define ASM_ANDEAXEBX1 184
#define ASM_ANDEAXEBX2 185
#define ASM_ANDEAXEBX4 186

#define ASM_OREAX1 187
#define ASM_OREAX2 188
#define ASM_OREAX4 189
#define ASM_OREAXEBX1 190
#define ASM_OREAXEBX2 191
#define ASM_OREAXEBX4 192

#define ASM_NOTEAX1 193
#define ASM_NOTEAX2 194
#define ASM_NOTEAX4 195

#define ASM_MOVEAXEDX1 196
#define ASM_MOVEAXEDX2 197
#define ASM_MOVEAXEDX4 198

#define ASM_XOREAX1 199
#define ASM_XOREAX2 200
#define ASM_XOREAX4 201
#define ASM_XOREAXEBX1 202
#define ASM_XOREAXEBX2 203
#define ASM_XOREAXEBX4 204

#define ASM_SHREAX1 205
#define ASM_SHREAX2 206
#define ASM_SHREAX4 207
#define ASM_SHLEAXCLC1 208
#define ASM_SHLEAXCLC2 209
#define ASM_SHLEAXCLC4 210

#define ASM_SHLEAX1 211
#define ASM_SHLEAX2 212
#define ASM_SHLEAX4 213
#define ASM_SHREAXCLC1 214
#define ASM_SHREAXCLC2 215
#define ASM_SHREAXCLC4 216

#define ASM_MOVECXEBX1 217
#define ASM_MOVECXEBX2 218
#define ASM_MOVECXEBX4 219

#define ASM_CMPEAXEBX4 220

#define ASM_MOVEBXEBP1 221
#define ASM_MOVEBXEBP2 222
#define ASM_MOVEBXEBP4 223
#define ASM_POPEDX 224
#define ASM_POPECX 225
#define ASM_PUSHECX 226

#define ASM_MULECXEDX4 227
#define ASM_ADDEBXEDX4 228
#define ASM_MULEDXEAXOFF4 229

#define ASM_MOVMEMEBX4 230
#define ASM_MOVEAXEBX4 231

#define ASM_PUSHFROMEAX 232
#define ASM_MOVEAXEBP 233

// ParamMode Defines
#define PMODE_NONE 0
#define PMODE_IMM 1
#define PMODE_MEM 2
#define PMODE_EBP 3
#define PMODE_MEMOFF 4
#define PMODE_EBPOFF 5
#define PMODE_MEMARR 6
#define PMODE_EBPARR 7
#define PMODE_STACK 8
#define PMODE_MEMREL 9
#define PMODE_EBPREL 10

class CASMWriter  
{
	public:
		CASMWriter();
		virtual ~CASMWriter();

		void SetDefaultCompileFlags ( bool bArraySafetyFlag );
		void SetArrayCheckFlag(bool bFlag) { m_bArrayCheckFlag = bFlag; }
		bool GetArrayCheckFlag(void) { return m_bArrayCheckFlag; }

		void GenerateASMCodes(void);
		void DefineASM(DWORD dwASMCode, LPSTR pDebugStr, int iPreOp, int iOp1, int iOp2, bool bOpData);

		bool CreateASMHeader(void);
		bool CreateASMMiddle(int iPreOpCode, int iOpCode1, int iOpCode2, LPSTR lpOpData);
		bool CreateASMMiddleCore(int iPreOpCode, int iOpCode1, int iOpCode2, LPSTR lpOpData, LPSTR lpOpData2, bool bSecondOpDataIsIMM, DWORD dwSecondOpDataIMMSize);
		bool CheckAndExpandMCBMemory(void);
		bool CheckAndExpandREFMemory(void);

		DWORD GetCurrentMCPosition(void) { return m_pMachineBlock-m_pProgramStart; }

		bool ReportAnyErrorsToCLI(void);
		bool PrepareEXE(LPSTR pEXEFilename, bool bParsingMainProgram, bool bProceedToUpdate);
		bool UpdateMCB(DWORD dwProgramSize);
		bool UpdateMCBRefData(void);
		bool UpdateDLLData(void);
		bool UpdateCommandData(void);
		bool UpdateStringData(void);
		bool UpdateDataData(void);
		bool UpdateDynamicData(void);

		void UpdateStructurePatternDataRec ( LPSTR pPattern, CDeclaration* pDecMain );
		bool UpdateStructurePatternData ( void );

		LPSTR MakeVarDataForTransfer(DWORD* dwDataSize);
		LPSTR MakeVarValuesForTransfer(DWORD* dwDataSize);

		void TraverseDecForPattern(DWORD dwBaseOffset, short pass, DWORD* dwPatternArrayCounter, DWORD* dwSizeOfUserTypePattern, CDeclaration* pDecMain);
		void FreeMachineBlock(void);
		void FreeAll(void);

		bool HideAnyHiddenCode(LPSTR pData, DWORD dwSize);
		LRESULT SendDataToDebugger(int iType, LPSTR pData, DWORD dwDataSize);
		void GetDataFromDebugger(int iType, LPSTR* pData, DWORD* dwDataSize);

		DWORD GetBytePosOfLastInstruction(void);

		DWORD DetermineASMCall(DWORD dwASMCodeAsAByte, DWORD dwTypeValue);
		DWORD DetermineASMCallForREL(DWORD dwASMCodeAsAByte, DWORD dwTypeValue);
		DWORD DetMode(CStr* pP, DWORD dwPType, DWORD dwPOffset);

		void CalculateArrayOffsetInEBX ( CStr* pStr );
		void WriteASMEAXtoARR(DWORD dwMode, CStr* pP, CStr* pPIndex, DWORD dwPType, DWORD dwPOffset);
		void WriteASMARRtoEAX(DWORD dwMode, CStr* pP, CStr* pPIndex, DWORD dwPType, DWORD dwPOffset);
		void WriteASMXtoEAX(DWORD dwMode, CStr* pP, CStr* pPIndex, DWORD dwPType, DWORD dwPOffset);
		void WriteASMEAXtoX(DWORD dwMode, CStr* pP, CStr* pPIndex, DWORD dwPType, DWORD dwPOffset);

		bool WriteASMCall(DWORD dwLine, LPSTR pDLL, LPSTR pDecoratedName);
		bool WriteASMTaskP1(DWORD dwLine, DWORD dwTask, CResultData* pP1);
		bool WriteASMTaskP2(DWORD dwLine, DWORD dwTask, CResultData* pP1, CResultData* pP2);
		bool WriteASMTaskP3(DWORD dwLine, DWORD dwTask, CResultData* pP1, CResultData* pP2, CResultData* pP3);
		bool WriteASMTaskCoreP1(DWORD dwLine, DWORD dwTask, CStr* pP1, DWORD dwP1Type);
		bool WriteASMTaskCoreP2(DWORD dwLine, DWORD dwTask, CStr* pP1, DWORD dwP1Type, CStr* pP2, DWORD dwP2Type);
		bool WriteASMTaskCore(DWORD dwLine, DWORD dwTask, CStr* pP1, CStr* pP1Off, DWORD dwP1Type, DWORD dwP1Offset, CStr* pP2, CStr* pP2Off, DWORD dwP2Type, DWORD dwP2Offset);
		bool WriteASMTaskCore(DWORD dwLine, DWORD dwTask,	CStr* pP1, CStr* pP1Off, DWORD dwP1Type, DWORD dwP1Offset,
															CStr* pP2, CStr* pP2Off, DWORD dwP2Type, DWORD dwP2Offset,
															CStr* pP3, CStr* pP3Off, DWORD dwP3Type, DWORD dwP3Offset );

		bool WriteASMLine(DWORD dwOp, LPSTR pOpData);
		bool WriteASMLine2(DWORD dwOp, LPSTR pOpData, LPSTR pOpData2);
		bool WriteASMLine1IMM(DWORD dwOp, LPSTR pOpData, DWORD dwSizeIMM);
		bool WriteASMLine2IMM(DWORD dwOp, LPSTR pOpData, LPSTR pOpData2, DWORD dwSizeIMM);
		bool WriteASMComment(LPSTR pTitle, LPSTR pC1, LPSTR pC2, LPSTR pC3);

		bool WriteASMLeapMarkerTop(void);
		bool WriteASMLineLeapToTop(DWORD dwOp);
		bool WriteASMLeapMarkerJumpToTop(void);

		bool WriteASMLeapForwardMarker(void);
		bool WriteASMLineLeap(DWORD dwOp, DWORD di);
		bool WriteASMLeapMarkerJump(DWORD dwOp, DWORD di);
		bool WriteASMLeapMarkerJumpNotEqual(DWORD di);
		bool WriteASMLeapMarkerEnd(DWORD di);
		bool WriteASMCheckBreakPointVar(void);
		bool WriteASMForceEscapeAtCodeBREAK(void);
		void SetBreakPointValue(void);

		DWORD AddCommandToTable(LPSTR pDLLString, LPSTR pCommandString);
		bool AddProtectionToSelectedDLLs(LPSTR pDLLString);

		bool GetCondToggle ( void ) { return m_bOneOffCondToggle; }
		void SetCondToggle ( bool bFlag ) { m_bOneOffCondToggle = bFlag; }

	private:

		// Program Execution Settings
		int						m_iInitialDisplayMode;
//		bool					m_bIsInternalDebugger;
//		PROCESS_INFORMATION		g_InternalDebuggerProcessInfo;
		
		// Compile Flag States
		bool					m_bArrayCheckFlag;
		bool					m_bOneOffCondToggle;

		// ASM Program Block
		DWORD					m_dwMCBlockSize;
		LPSTR					m_pProgramStart;
		LPSTR					m_pMachineBlock;

		// Reference Tracking
		DWORD					m_dwRefBufferSize;
		DWORD					m_dwProgramRefPointer;
		DWORD*					m_pProgramRefs;
		DWORD*					m_pProgramRefLabel;

		// Work Variables
	public:
		DWORD					m_dwLineNumber;

	private:
		CMathOp*				m_pP1MathOp;
		CMathOp*				m_pP2MathOp;
		CMathOp*				m_pP3MathOp;

		// Leap Marker Work Variable
		LPSTR					m_pRecordTopBytePosition;
		DWORD					m_pRecordRefPosition[9];
		LPSTR					m_pRecordBytePosition[9];

	private:

		LPSTR					m_pASMDebugString[ASMMAXCOUNT];
		int						m_iASMPreOp[ASMMAXCOUNT];
		int						m_iASMOp1[ASMMAXCOUNT];
		int						m_iASMOp2[ASMMAXCOUNT];
		bool					m_bASMOpData[ASMMAXCOUNT];
};

#endif // !defined(AFX_ASMWRITER_H__A3BE66B0_1587_46D3_AC0A_E4F78C0A3561__INCLUDED_)
