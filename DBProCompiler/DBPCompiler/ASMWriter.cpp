// ASMWriter.cpp: implementation of the CASMWriter class.
//
//////////////////////////////////////////////////////////////////////

// Includes
#include "ParserHeader.h"
#include "FileBuilder.h"
#include "ASMWriter.h"
#include "DataTable.h"
#include "LabelTable.h"
#include "VarTable.h"
#include "StructTable.h"
#include "DebugInfo.h"
#include "Errors.h"
#include "DBPCompiler.h"

#include <DB3Time.h>

// External Class Pointer
extern CEXEBlock* g_pEXE;
extern CDBPCompiler* g_pDBPCompiler;
extern CDataTable* g_pDataTable;
extern CDataTable* g_pStringTable;
extern CDataTable* g_pDLLTable;
extern CDataTable* g_pCommandTable;
extern CLabelTable* g_pLabelTable;
extern CVarTable* g_pVarTable;
extern CStructTable* g_pStructTable;
extern CDebugInfo g_DebugInfo;
extern CError* g_pErrorReport;
extern CASMWriter* g_pASMWriter;

// External Global Vars
extern DWORD g_dwEscapeValueMem;
extern DWORD g_dwBreakOutPosition;
extern LPSTR g_pVarSpaceAddressInUse;
extern DWORD g_dwVarSpaceSizeInUse;
extern GDI_RetVoidParamVoidPFN g_CORE_SyncRefresh;
extern bool g_bIsInternalDebugger;
extern PROCESS_INFORMATION g_InternalDebuggerProcessInfo;
extern bool g_bExternaliseDLLS;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CASMWriter::CASMWriter()
{
	// ASM Program Block
	m_dwMCBlockSize=0;
	m_pProgramStart=NULL;
	m_pMachineBlock=NULL;

	// Reference Tracking
	m_dwRefBufferSize=0;
	m_dwProgramRefPointer=0;
	m_pProgramRefs=NULL;
	m_pProgramRefLabel=NULL;
	m_bOneOffCondToggle=false;

	// Work Variables
	m_dwLineNumber=0;

	// Reset ASM Code Database
	for(DWORD i=0; i<ASMMAXCOUNT; i++)
	{
		m_pASMDebugString[i]=NULL;
		m_iASMPreOp[i]=0;
		m_iASMOp1[i]=0;
		m_iASMOp2[i]=0;
		m_bASMOpData[i]=false;
	}

	for(DWORD di=0; di<9; di++)
	{
		m_pRecordRefPosition[di] = 0;
		m_pRecordBytePosition[di] = 0;
	}

	// First task to create default ASM Codes
	GenerateASMCodes();

	// Clear Debug Mode Runner Vars
	g_bIsInternalDebugger=false;
	memset(&g_InternalDebuggerProcessInfo, 0, sizeof(PROCESS_INFORMATION));
}

CASMWriter::~CASMWriter()
{
	// Free all usages
	FreeAll();
}

void CASMWriter::SetDefaultCompileFlags ( bool bArraySafetyFlag )
{
	// Compile Flags by default
	SetArrayCheckFlag(bArraySafetyFlag);
}

void CASMWriter::GenerateASMCodes(void)
{
	// Default ASM Codes for ASMWriting
	DefineASM(ASM_MOVEAXIMM1,	"MOV EAX IMM1",		-1,		0xB0+0,	-1,		true);
	DefineASM(ASM_MOVEAXIMM2,	"MOV EAX IMM2",		0x66,	0xB8+0,	-1,		true);
	DefineASM(ASM_MOVEAXIMM4,	"MOV EAX IMM4",		-1,		0xB8+0,	-1,		true);

	DefineASM(ASM_MOVEBXIMM1,	"MOV EBX IMM1",		-1,		0xB0+3,	-1,		true);
	DefineASM(ASM_MOVEBXIMM2,	"MOV EBX IMM2",		0x66,	0xB8+3,	-1,		true);
	DefineASM(ASM_MOVEBXIMM4,	"MOV EBX IMM4",		-1,		0xB8+3,	-1,		true);
	
	DefineASM(ASM_MOVEDXIMM4,	"MOV EDX IMM4",		-1,		0xB8+2,	-1,		true);

	DefineASM(ASM_MOVEAXMEM1,	"MOV EAX MEM1",		-1,		0xA0,	-1,		true);
	DefineASM(ASM_MOVEAXMEM2,	"MOV EAX MEM2",		0x66,	0xA1,	-1,		true);
	DefineASM(ASM_MOVEAXMEM4,	"MOV EAX MEM4",		-1,		0xA1,	-1,		true);
	DefineASM(ASM_MOVMEMEAX1,	"MOV MEM1 EAX",		-1,		0xA2,	-1,		true);
	DefineASM(ASM_MOVMEMEAX2,	"MOV MEM2 EAX",		0x66,	0xA3,	-1,		true);
	DefineASM(ASM_MOVMEMEAX4,	"MOV MEM4 EAX",		-1,		0xA3,	-1,		true);

	DefineASM(ASM_MOVECXOFFEAX1,"MOV [ECX+A] EAX1",	-1,		0x88,	0x81,	true);
	DefineASM(ASM_MOVECXOFFEAX2,"MOV [ECX+A] EAX2",	0x66,	0x89,	0x81,	true);
	DefineASM(ASM_MOVECXOFFEAX4,"MOV [ECX+A] EAX4",	-1,		0x89,	0x81,	true);

	DefineASM(ASM_MOVEAXOFFECX1,"MOV [EAX+A] ECX1",	-1,		0x88,	0x88,	true);
	DefineASM(ASM_MOVEAXOFFECX2,"MOV [EAX+A] ECX2",	0x66,	0x89,	0x88,	true);
	DefineASM(ASM_MOVEAXOFFECX4,"MOV [EAX+A] ECX4",	-1,		0x89,	0x88,	true);
	
	DefineASM(ASM_MOVEAXECXREL1,"MOV [EAX] ECX1",	-1,		0x88,	0x08,	false);
	DefineASM(ASM_MOVEAXECXREL2,"MOV [EAX] ECX2",	0x66,	0x89,	0x08,	false);
	DefineASM(ASM_MOVEAXECXREL4,"MOV [EAX] ECX4",	-1,		0x89,	0x08,	false);
	
	DefineASM(ASM_MOVEAXEAXREL1,"MOV EAX [EAX1]",	-1,		0x8A,	0x00,	false);
	DefineASM(ASM_MOVEAXEAXREL2,"MOV EAX [EAX2]",	0x66,	0x8B,	0x00,	false);
	DefineASM(ASM_MOVEAXEAXREL4,"MOV EAX [EAX4]",	-1,		0x8B,	0x00,	false);
	
	DefineASM(ASM_MOVECXEAXOFF1,"MOV ECX1 [EAX+A]",	-1,		0x8A,	0x88,	true);
	DefineASM(ASM_MOVECXEAXOFF2,"MOV ECX2 [EAX+A]",	0x66,	0x8B,	0x88,	true);
	DefineASM(ASM_MOVECXEAXOFF4,"MOV ECX4 [EAX+A]",	-1,		0x8B,	0x88,	true);
	
	DefineASM(ASM_MOVEAXECXOFF1,"MOV EAX1 [ECX+A]",	-1,		0x8A,	0x81,	true);
	DefineASM(ASM_MOVEAXECXOFF2,"MOV EAX2 [ECX+A]",	0x66,	0x8B,	0x81,	true);
	DefineASM(ASM_MOVEAXECXOFF4,"MOV EAX4 [ECX+A]",	-1,		0x8B,	0x81,	true);
	
	DefineASM(ASM_MOVEDXEAXOFF1,"MOV EDX1 [EAX+A]",	-1,		0x8A,	0x90,	true);
	DefineASM(ASM_MOVEDXEAXOFF2,"MOV EDX2 [EAX+A]",	0x66,	0x8B,	0x90,	true);
	DefineASM(ASM_MOVEDXEAXOFF4,"MOV EDX4 [EAX+A]",	-1,		0x8B,	0x90,	true);

	DefineASM(ASM_MULEDXEAXOFF4,"MUL EDX4 [EAX+A]",	0x0F,	0xAF,	0x90,	true);

	DefineASM(ASM_MOVEBXEAXOFF1,"MOV EBX1 [EAX+A]",	-1,		0x8A,	0x98,	true);
	DefineASM(ASM_MOVEBXEAXOFF2,"MOV EBX2 [EAX+A]",	0x66,	0x8B,	0x98,	true);
	DefineASM(ASM_MOVEBXEAXOFF4,"MOV EBX4 [EAX+A]",	-1,		0x8B,	0x98,	true);

	DefineASM(ASM_MOVEAXEBP1,	"MOV EAX1 [EBP+A]",	-1,		0x8A,	0x85,	true);
	DefineASM(ASM_MOVEAXEBP2,	"MOV EAX2 [EBP+A]",	0x66,	0x8B,	0x85,	true);
	DefineASM(ASM_MOVEAXEBP4,	"MOV EAX4 [EBP+A]",	-1,		0x8B,	0x85,	true);
	
	DefineASM(ASM_MOVEBXEBP1,	"MOV EBX1 [EBP+A]",	-1,		0x8A,	0x9D,	true);
	DefineASM(ASM_MOVEBXEBP2,	"MOV EBX2 [EBP+A]",	0x66,	0x8B,	0x9D,	true);
	DefineASM(ASM_MOVEBXEBP4,	"MOV EBX4 [EBP+A]",	-1,		0x8B,	0x9D,	true);
	
	DefineASM(ASM_MOVEBPEAX1,	"MOV [EBP+A] EAX1",	-1,		0x88,	0x85,	true);
	DefineASM(ASM_MOVEBPEAX2,	"MOV [EBP+A] EAX2",	0x66,	0x89,	0x85,	true);
	DefineASM(ASM_MOVEBPEAX4,	"MOV [EBP+A] EAX4",	-1,		0x89,	0x85,	true);
	
	DefineASM(ASM_MOVEDXEAX4,	"MOV EDX EAX",		-1,		0x8B,	0xD0,	false);
	DefineASM(ASM_MOVECXIMM4,	"MOV ECX IMM4",		-1,		0xB8+1, -1,		true);
	DefineASM(ASM_MOVECXEAX4,	"MOV ECX EAX",		-1,		0x8B,	0xC8,	false);
	DefineASM(ASM_MOVECXEDX4,	"MOV ECX EDX",		-1,		0x8B,	0xCA,	false);
	DefineASM(ASM_MOVEDXECX4,	"MOV EDX ECX",		-1,		0x8B,	0xD1,	false);

	DefineASM(ASM_MOVEAXEBX4,	"MOV EAX EBX",		-1,		0x8B,	0xC3,	false);

	DefineASM(ASM_MOVECXEBX1,	"MOV ECX EBX1",		-1,		0x8A,	0xCB,	false);
	DefineASM(ASM_MOVECXEBX2,	"MOV ECX EBX2",		-1,		0x8B,	0xCB,	false);
	DefineASM(ASM_MOVECXEBX4,	"MOV ECX EBX4",		-1,		0x8B,	0xCB,	false);
	
	DefineASM(ASM_MOVEAXECX1,	"MOV EAX ECX1",		-1,		0x8A,	0xC1,	true);
	DefineASM(ASM_MOVEAXECX2,	"MOV EAX ECX2",		0x66,	0x8B,	0xC1,	true);
	DefineASM(ASM_MOVEAXECX4,	"MOV EAX ECX4",		-1,		0x8B,	0xC1,	true);

	DefineASM(ASM_MOVEAXEDX1,	"MOV EAX EDX1",		-1,		0x8A,	0xC2,	true);
	DefineASM(ASM_MOVEAXEDX2,	"MOV EAX EDX2",		0x66,	0x8B,	0xC2,	true);
	DefineASM(ASM_MOVEAXEDX4,	"MOV EAX EDX4",		-1,		0x8B,	0xC2,	true);

	DefineASM(ASM_MOVEBXEAX1,	"MOV EBX EAX1",		-1,		0x8A,	0xD8,	true);
	DefineASM(ASM_MOVEBXEAX2,	"MOV EBX EAX2",		0x66,	0x8B,	0xD8,	true);
	DefineASM(ASM_MOVEBXEAX4,	"MOV EBX EAX4",		-1,		0x8B,	0xD8,	true);

	DefineASM(ASM_ADDEAXEBX1,	"ADD EAX EBX1",		-1,		0x00,	0xD8,	false);
	DefineASM(ASM_ADDEAXEBX2,	"ADD EAX EBX2",		0x66,	0x01,	0xD8,	false);
	DefineASM(ASM_ADDEAXEBX4,	"ADD EAX EBX4",		-1,		0x01,	0xD8,	false);

	DefineASM(ASM_ADDEAXECX4,	"ADD EAX ECX4",		-1,		0x01,	0xC8,	false);

	DefineASM(ASM_ADDESP,		"ADD ESP",			-1,		0x81,	0xC4,	true);
	DefineASM(ASM_SUBESP,		"SUB ESP",			-1,		0x81,	0xEC,	true);
	DefineASM(ASM_SUBESPEAX,	"SUB ESP EAX",		-1,		0x29,	0xC4,	false);

	DefineASM(ASM_PUSHEBP,		"PUSH EBP",			-1,		0x55,	-1,		true);
	DefineASM(ASM_POPEBP,		"POP EBP",			-1,		0x5D,	-1,		true);
	
	DefineASM(ASM_MOVEBPESP,	"MOV EBP ESP",		-1,		0x89,	0xE5,	true);
	DefineASM(ASM_MOVESPEBP,	"MOV ESP EBP",		-1,		0x89,	0xEC,	true);

	DefineASM(ASM_MOVEAXESP,	"MOV EAX ESP",		-1,		0x89,	0xE0,	false);
	DefineASM(ASM_MOVEAXEBP,	"MOV EAX EBP",		-1,		0x89,	0xE8,	false);

	DefineASM(ASM_MOVMEMIMM1,	"MOV MEM IMM1",		-1,		0xC6,	0x05,	true);
	DefineASM(ASM_MOVMEMIMM2,	"MOV MEM IMM2",		0x66,	0xC7,	0x05,	true);
	DefineASM(ASM_MOVMEMIMM4,	"MOV MEM IMM4",		-1,		0xC7,	0x05,	true);

	DefineASM(ASM_MOVEBPIMM1,	"MOV [EBP+A] IMM1",	-1,		0xC6,	0x85,	true);
	DefineASM(ASM_MOVEBPIMM2,	"MOV [EBP+A] IMM2",	0x66,	0xC7,	0x85,	true);
	DefineASM(ASM_MOVEBPIMM4,	"MOV [EBP+A] IMM4",	-1,		0xC7,	0x85,	true);
	
	DefineASM(ASM_RELMOVEAXEDX1,"REL MOV [AX1] DX1",-1,		0x88,	0x10,	true);
	DefineASM(ASM_RELMOVEAXEDX2,"REL MOV [AX2] DX2",0x66,	0x89,	0x10,	true);
	DefineASM(ASM_RELMOVEAXEDX4,"REL MOV [EAX] EDX",-1,		0x89,	0x10,	true);

	DefineASM(ASM_RELMOVEAXREDX1,"REL MOV AX1 [DX1]",-1,	0x8A,	0x02,	true);
	DefineASM(ASM_RELMOVEAXREDX2,"REL MOV AX2 [DX2]",0x66,	0x8B,	0x02,	true);
	DefineASM(ASM_RELMOVEAXREDX4,"REL MOV EAX [EDX]",-1,	0x8B,	0x02,	true);

	DefineASM(ASM_MOVMEMST08,	"FSTP [MEM] ST08",	-1,		0xDD,	0x1D,	true);
	DefineASM(ASM_MOVST0MEM8,	"FLD ST08 [MEM]",	-1,		0xDD,	0x05,	true);
	DefineASM(ASM_MOVEBPST08,	"FSTP [EBP+A] ST08",-1,		0xDD,	0x9D,	true);
	DefineASM(ASM_MOVST0EBP8,	"FLD ST08 [EBP+A]",	-1,		0xDD,	0x85,	true);
	DefineASM(ASM_MOVEAXST08,	"FSTP [EAX+A] ST08",-1,		0xDD,	0x98,	true);
	DefineASM(ASM_MOVST0EAX8,	"FLD ST08 [EAX+A]",	-1,		0xDD,	0x80,	true);
	DefineASM(ASM_MOVECXOFFST08,"FSTP [ECX+A] ST08",-1,		0xDD,	0x99,	true);
	DefineASM(ASM_MOVST0ECXOFF8,"FLD ST08 [ECX+A]",	-1,		0xDD,	0x81,	true);
	
	DefineASM(ASM_PUSHEAX,		"PUSH EAX",			-1,		0x50+0,	-1,		true);
	DefineASM(ASM_PUSHEDX,		"PUSH EDX",			-1,		0x50+2,	-1,		true);
	DefineASM(ASM_PUSHEBX,		"PUSH EBX",			-1,		0x50+3,	-1,		true);
	DefineASM(ASM_PUSHESP,		"PUSH ESP",			-1,		0x50+4,	-1,		true);
	DefineASM(ASM_PUSHECX,		"PUSH ECX",			-1,		0x50+1,	-1,		true);

	DefineASM(ASM_PUSHRELEAX1,	"PUSH REL AX1",		-1,		0xFF,	0x30,	true);
	DefineASM(ASM_PUSHRELEAX2,	"PUSH REL AX2",		-1,		0xFF,	0x30,	true);
	DefineASM(ASM_PUSHRELEAX4,	"PUSH REL EAX",		-1,		0xFF,	0x30,	true);

	DefineASM(ASM_PUSHEBP4,		"PUSH [EBP+A]",		-1,		0xFF,	0xB5,	true);
	DefineASM(ASM_PUSHFROMEAX,	"PUSH [EAX]",		-1,		0xFF,	0x30,	false);

	DefineASM(ASM_CALLEAX,		"CALL EAX",			-1,		0xFF,	0xD0,	false);
	DefineASM(ASM_CALLEBX,		"CALL EBX",			-1,		0xFF,	0xD3,	false);
	DefineASM(ASM_CALLMEM,		"CALL MEM",			-1,		0xE8,	-1,		true);
	DefineASM(ASM_CALLABS,		"CALL REL",			-1,		0xE8,	-1,		true);
	
	DefineASM(ASM_POPEAX,		"POP EAX",			-1,		0x58,	-1,		false);
	DefineASM(ASM_POPEBX,		"POP EBX",			-1,		0x5B,	-1,		false);
	DefineASM(ASM_RET,			"RET",				-1,		0xC3,	-1,		false);
	DefineASM(ASM_POPEDX,		"POP EDX",			-1,		0x5A,	-1,		false);
	DefineASM(ASM_POPECX,		"POP ECX",			-1,		0x59,	-1,		false);

	DefineASM(ASM_UNKNOWN,		"???",				-1,		-1,		-1,		false);

	DefineASM(ASM_CMPEAX1,		"CMP EAX1",			-1,		0x3C,	-1,		true);
	DefineASM(ASM_CMPEAX2,		"CMP EAX2",			0x66,	0x3D,	-1,		true);
	DefineASM(ASM_CMPEAX4,		"CMP EAX4",			-1,		0x3D,	-1,		true);

	DefineASM(ASM_CMPEBX1,		"CMP EBX1",			-1,		0x80,	0xFB,	true);
	DefineASM(ASM_CMPEBX2,		"CMP EBX2",			0x66,	0x81,	0xFB,	true);
	DefineASM(ASM_CMPEBX4,		"CMP EBX4",			-1,		0x81,	0xFB,	true);
	
	DefineASM(ASM_CMPGREEDXEBX,	"CMP EDX EBX4",		-1,		0x3B,	0xDA,	false);

	DefineASM(ASM_CMPEDXEBX1,	"CMP EDX EBX1",		-1,		0x3A,	0xDA,	false);
	DefineASM(ASM_CMPEDXEBX2,	"CMP EDX EBX2",		0x66,	0x3B,	0xDA,	false);
	DefineASM(ASM_CMPEDXEBX4,	"CMP EDX EBX4",		-1,		0x3B,	0xDA,	false);

	DefineASM(ASM_CMPEAXEBX4,	"CMP EAX EBX4",		-1,		0x3B,	0xD8,	false);

	DefineASM(ASM_SETE,			"SETE EAX",			0x0F,	0x94,	0xC0,	false);
	DefineASM(ASM_SETNE,		"SETNE EAX",		0x0F,	0x95,	0xC0,	false);
	DefineASM(ASM_SETG,			"SETG EAX",			0x0F,	0x9F,	0xC0,	false);
	DefineASM(ASM_SETGE,		"SETGE EAX",		0x0F,	0x9D,	0xC0,	false);
	DefineASM(ASM_SETL,			"SETL EAX",			0x0F,	0x9C,	0xC0,	false);
	DefineASM(ASM_SETLE,		"SETLE EAX",		0x0F,	0x9E,	0xC0,	false);

	DefineASM(ASM_JMP,			"JMP",				-1,		0xE9,	-1,		true);
	DefineASM(ASM_JNE,			"JNE",				-1,		0x0F,	0x85,	true);
	DefineASM(ASM_JE,			"JE",				-1,		0x0F,	0x84,	true);
	DefineASM(ASM_JMPREL,		"JMP REL",			-1,		0xFF,	0x25,	true);
	DefineASM(ASM_JMPEBX,		"JMP EBX",			-1,		0xFF,	0xE3,	true);

	DefineASM(ASM_JGE,			"JGE",				-1,		0x0F,	0x8D,	true);
	DefineASM(ASM_JLE,			"JLE",				-1,		0x0F,	0x8E,	true);

	DefineASM(ASM_MOVMEMESP4,	"MOV MEM4 ESP",		-1,		0x89,	0x25,	true);
	DefineASM(ASM_MOVESPMEM4,	"MOV ESP MEM4",		-1,		0x8B,	0x25,	true);

	DefineASM(ASM_MOVEBXMEM4,	"MOV EBX MEM4",		-1,		0x8B,	0x1D,	true);
	DefineASM(ASM_MOVMEMEBX4,	"MOV MEM EBX4",		-1,		0x89,	0x1D,	true);

	DefineASM(ASM_MOVEAXSIB4,	"MOV EAX SIB[EAX:EBX*4]",	0x8B,	0x04,	0x98,	false);

	DefineASM(ASM_MOVSIB4IMM1,	"MOV SIB[EAX:ECX*1],IMM1",	0xC6,	0x04,	0x08,	true);
	DefineASM(ASM_MOVSIB4IMM4,	"MOV SIB[EAX:ECX*4],IMM4",	0xC7,	0x04,	0x88,	true);

	DefineASM(ASM_PUSHAD,		"PUSH REGISTERS",	-1,		0x60,	-1,		false);
	DefineASM(ASM_POPAD,		"POP REGISTERS",	-1,		0x61,	-1,		false);

	DefineASM(ASM_LOOP,			"LOOP ECX",			-1,		0xE2,	-1,		true);
	
	DefineASM(ASM_PUSHIMM4,		"PUSH IMM4",		-1,		0x68,	-1,		true);

	DefineASM(ASM_INCMEM1,		"INC MEM1",			-1,		0xFE,	0x05,	true);
	DefineASM(ASM_INCMEM2,		"INC MEM2",			0x66,	0xFF,	0x05,	true);
	DefineASM(ASM_INCMEM4,		"INC MEM4",			-1,		0xFF,	0x05,	true);
	DefineASM(ASM_DECMEM1,		"DEC MEM1",			-1,		0xFE,	0x0D,	true);
	DefineASM(ASM_DECMEM2,		"DEC MEM2",			0x66,	0xFF,	0x0D,	true);
	DefineASM(ASM_DECMEM4,		"DEC MEM4",			-1,		0xFF,	0x0D,	true);

	DefineASM(ASM_ADDEAX1,		"ADD EAX IMM1",		-1,		0x04,	-1,		true);
	DefineASM(ASM_ADDEAX2,		"ADD EAX IMM2",		0x66,	0x05,	-1,		true);
	DefineASM(ASM_ADDEAX4,		"ADD EAX IMM4",		-1,		0x05,	-1,		true);
	
	DefineASM(ASM_SUBEAX1,		"SUB EAX IMM1",		-1,		0x2C,	-1,		true);
	DefineASM(ASM_SUBEAX2,		"SUB EAX IMM2",		0x66,	0x2D,	-1,		true);
	DefineASM(ASM_SUBEAX4,		"SUB EAX IMM4",		-1,		0x2D,	-1,		true);
	DefineASM(ASM_SUBEAXEBX1,	"SUB EAX EBX1",		-1,		0x28,	0xD8,	false);
	DefineASM(ASM_SUBEAXEBX2,	"SUB EAX EBX2",		0x66,	0x29,	0xD8,	false);
	DefineASM(ASM_SUBEAXEBX4,	"SUB EAX EBX4",		-1,		0x29,	0xD8,	false);

	DefineASM(ASM_MULEAXEBX1,	"IMUL EAX EBX1",	-1,		0xF6,	0xEB,	false);
	DefineASM(ASM_MULEAXEBX2,	"IMUL EAX EBX2",	0x66,	0xF7,	0xEB,	false);
	DefineASM(ASM_MULEAXEBX4,	"IMUL EAX EBX4",	-1,		0xF7,	0xEB,	false);

	DefineASM(ASM_CDQ,			"CDQ",				-1,		0x99,	-1,		false);

	DefineASM(ASM_DIVEAXEBX1,	"IDIV EAX EBX1",	-1,		0xF6,	0xFB,	false);
	DefineASM(ASM_DIVEAXEBX2,	"IDIV EAX EBX2",	0x66,	0xF7,	0xFB,	false);
	DefineASM(ASM_DIVEAXEBX4,	"IDIV EAX EBX4",	-1,		0xF7,	0xFB,	false);

	DefineASM(ASM_ANDEAX1,		"AND EAX IMM1",		-1,		0x24,	-1,		true);
	DefineASM(ASM_ANDEAX2,		"AND EAX IMM2",		0x66,	0x25,	-1,		true);
	DefineASM(ASM_ANDEAX4,		"AND EAX IMM4",		-1,		0x25,	-1,		true);
	DefineASM(ASM_ANDEAXEBX1,	"AND EAX EBX1",		-1,		0x20,	0xD8,	false);
	DefineASM(ASM_ANDEAXEBX2,	"AND EAX EBX2",		0x66,	0x21,	0xD8,	false);
	DefineASM(ASM_ANDEAXEBX4,	"AND EAX EBX4",		-1,		0x21,	0xD8,	false);

	DefineASM(ASM_OREAX1,		"OR EAX IMM1",		-1,		0x0C,	-1,		true);
	DefineASM(ASM_OREAX2,		"OR EAX IMM2",		0x66,	0x0D,	-1,		true);
	DefineASM(ASM_OREAX4,		"OR EAX IMM4",		-1,		0x0D,	-1,		true);
	DefineASM(ASM_OREAXEBX1,	"OR EAX EBX1",		-1,		0x08,	0xD8,	false);
	DefineASM(ASM_OREAXEBX2,	"OR EAX EBX2",		0x66,	0x09,	0xD8,	false);
	DefineASM(ASM_OREAXEBX4,	"OR EAX EBX4",		-1,		0x09,	0xD8,	false);

	DefineASM(ASM_NOTEAX1,		"NOT EAX1",			-1,		0xF6,	0xD0,	false);
	DefineASM(ASM_NOTEAX2,		"NOT EAX2",			0x66,	0xF7,	0xD0,	false);
	DefineASM(ASM_NOTEAX4,		"NOT EAX4",			-1,		0xF7,	0xD0,	false);

	DefineASM(ASM_XOREAX1,		"XOR EAX IMM1",		-1,		0x34,	-1,		true);
	DefineASM(ASM_XOREAX2,		"XOR EAX IMM2",		0x66,	0x35,	-1,		true);
	DefineASM(ASM_XOREAX4,		"XOR EAX IMM4",		-1,		0x35,	-1,		true);
	DefineASM(ASM_XOREAXEBX1,	"XOR EAX EBX1",		-1,		0x30,	0xD8,	false);
	DefineASM(ASM_XOREAXEBX2,	"XOR EAX EBX2",		0x66,	0x31,	0xD8,	false);
	DefineASM(ASM_XOREAXEBX4,	"XOR EAX EBX4",		-1,		0x31,	0xD8,	false);
	
	DefineASM(ASM_SHLEAX1,		"SHL EAX1 IMM1",	-1,		0xC0,	0xE0,	true);
	DefineASM(ASM_SHLEAX2,		"SHL EAX2 IMM2",	0x66,	0xC1,	0xE0,	true);
	DefineASM(ASM_SHLEAX4,		"SHL EAX4 IMM4",	-1,		0xC1,	0xE0,	true);
	DefineASM(ASM_SHLEAXCLC1,	"SHL EAX1 CL",		-1,		0xD2,	0xE0,	false);
	DefineASM(ASM_SHLEAXCLC2,	"SHL EAX2 CL",		0x66,	0xD3,	0xE0,	false);
	DefineASM(ASM_SHLEAXCLC4,	"SHL EAX4 CL",		-1,		0xD3,	0xE0,	false);

	DefineASM(ASM_SHREAX1,		"SHR EAX1 IMM1",	-1,		0xC0,	0xE8,	true);
	DefineASM(ASM_SHREAX2,		"SHR EAX2 IMM2",	0x66,	0xC1,	0xE8,	true);
	DefineASM(ASM_SHREAX4,		"SHR EAX4 IMM4",	-1,		0xC1,	0xE8,	true);
	DefineASM(ASM_SHREAXCLC1,	"SHR EAX1 CL",		-1,		0xD2,	0xE8,	false);
	DefineASM(ASM_SHREAXCLC2,	"SHR EAX2 CL",		0x66,	0xD3,	0xE8,	false);
	DefineASM(ASM_SHREAXCLC4,	"SHR EAX4 CL",		-1,		0xD3,	0xE8,	false);

	DefineASM(ASM_MULECXEDX4,	"IMUL ECX EDX4",	0x0F,	0xAF,	0x0A,	false);
	DefineASM(ASM_ADDEBXEDX4,	"ADD EBX EDX4",		-1,		0x03,	0xDA,	false);
}

void CASMWriter::DefineASM(DWORD dwASMCode, LPSTR pDebugStr, int iPreOp, int iOp1, int iOp2, bool bOpData)
{
	// Store Debug String for ASM Code
	m_pASMDebugString[dwASMCode]=new char[strlen(pDebugStr)+1];
	strcpy(m_pASMDebugString[dwASMCode], pDebugStr);

	// Store OpCodes for ASM Code
	m_iASMPreOp[dwASMCode]=iPreOp;
	m_iASMOp1[dwASMCode]=iOp1;
	m_iASMOp2[dwASMCode]=iOp2;

	// Store OpData Flag for ASM Code
	m_bASMOpData[dwASMCode]=bOpData;
}

bool CASMWriter::CreateASMHeader(void)
{
	// Create Empty MC Block
	m_dwMCBlockSize=1024;
	m_pProgramStart = (LPSTR)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, m_dwMCBlockSize);

	// Fill With RET Codes
	for(DWORD n=0; n<m_dwMCBlockSize; n++) *((LPSTR)m_pProgramStart+n) = (unsigned char)0xC3;

	// Prepare RefData
	m_dwRefBufferSize=1024;
	m_dwProgramRefPointer=0;
	m_pProgramRefs = new DWORD[m_dwRefBufferSize];
	m_pProgramRefLabel = new DWORD[m_dwRefBufferSize];
	ZeroMemory(m_pProgramRefs, sizeof(DWORD)*m_dwRefBufferSize);
	ZeroMemory(m_pProgramRefLabel, sizeof(DWORD)*m_dwRefBufferSize);

	// Write Program Into MC Block
	m_pMachineBlock = m_pProgramStart;

	// In Debug Mode, hooks are always present
	if(g_DebugInfo.DebugModeOn())
	{
		// Add To DLL&Command Table
		AddCommandToTable("[EXE", "!DHookS");
		AddCommandToTable("[EXE", "!DHookJ");
		AddCommandToTable("[EXE", "!DHookR");
	}

	return true;
}

bool CASMWriter::CreateASMMiddle(int iPreOpCode, int iOpCode1, int iOpCode2, LPSTR lpOpData)
{
	return CreateASMMiddleCore(iPreOpCode, iOpCode1, iOpCode2, lpOpData, NULL, false, 0);
}

bool CASMWriter::CreateASMMiddleCore(int iPreOpCode, int iOpCode1, int iOpCode2, LPSTR lpOpData, LPSTR lpOpData2, bool bSecondOpDataIsIMM, DWORD dwSecondOpDataIMMSize)
{
	// Check and expand if MCB too small
	CheckAndExpandMCBMemory();

	// Write OpCode(s)
	if(iPreOpCode!=-1)
	{
		*(m_pMachineBlock) = iPreOpCode;
		(m_pMachineBlock)++;
	}
	if(iOpCode1!=-1)
	{
		*(m_pMachineBlock) = iOpCode1;
		(m_pMachineBlock)++;
	}
	if(iOpCode2!=-1)
	{
		*(m_pMachineBlock) = iOpCode2;
		(m_pMachineBlock)++;
	}

	// Write Optional OpData 1 and 2
	for(DWORD n=0; n<2; n++)
	{
		LPSTR pData=lpOpData;
		if(n==1) pData=lpOpData2;
		if(pData)
		{
			if(strcmp(pData, "")!=NULL)
			{
				// Ensure reference array always large enough for new reference
				CheckAndExpandREFMemory();

				// REF or IMM
				if(bSecondOpDataIsIMM==true && n==1)
				{
					// WRITE IMM INTO MC
					DWORD dwDataAsDWORD = (DWORD)_atoi64(pData);
					switch(dwSecondOpDataIMMSize)
					{
						case 0 :	*(DWORD*)(m_pMachineBlock) = dwDataAsDWORD;
									(m_pMachineBlock)+=1;
									break;
						case 1 :	*(DWORD*)(m_pMachineBlock) = dwDataAsDWORD;
									(m_pMachineBlock)+=2;
									break;
						case 2 :	*(DWORD*)(m_pMachineBlock) = dwDataAsDWORD;
									(m_pMachineBlock)+=4;
									break;
					}
				}
				else
				{
					// Work out where in program the replacement should take place
					DWORD MCBBytePos = m_pMachineBlock-m_pProgramStart;

					// Record Reference Position at index
					*(m_pProgramRefs+m_dwProgramRefPointer)=MCBBytePos;

					// Record Reference Label at index
					char* pStr = new char[strlen(pData)+1];
					strcpy(pStr, pData);
					CStr* pCleanStr = new CStr(pStr);
					pCleanStr->EatEdgeSpacesandTabs(NULL);
					strcpy(pStr, pCleanStr->GetStr());
					*(m_pProgramRefLabel+m_dwProgramRefPointer)=(DWORD)pStr;
					SAFE_DELETE(pCleanStr);

					// Advance Ref Index
					m_dwProgramRefPointer++;

					// WRITE BLANK(XXXX) INTO MB
					*(DWORD*)(m_pMachineBlock) = (DWORD)0xFFFFFFFF;
					(m_pMachineBlock)+=4;
				}
			}
		}
		if(lpOpData2==NULL) break;
	}

	// Complete
	return true;
}

bool CASMWriter::CheckAndExpandMCBMemory(void)
{
	// If within 100 bytes of end, expand memory
	LPSTR pMCBDataBarrier=(m_pProgramStart+m_dwMCBlockSize)-(100);
	if(m_pMachineBlock>pMCBDataBarrier)
	{
		// Work out offset of pointer
		DWORD dwOffset = m_pMachineBlock-m_pProgramStart;
		DWORD dwByteOffset = m_pRecordTopBytePosition-m_pProgramStart;

		DWORD dwLeapRelDiff[9];
		for(DWORD di=0; di<9; di++)
			dwLeapRelDiff[di] = m_pRecordBytePosition[di]-m_pProgramStart;

		// Create New Larger memory (another 100K)
		DWORD dwNewSize = m_dwMCBlockSize+(102400);
		LPSTR pNewMem = (LPSTR)GlobalAlloc(GMEM_FIXED, dwNewSize);

		// Fill With RET Codes
		for(DWORD n=0; n<dwNewSize; n++) *((LPSTR)pNewMem+n) = (unsigned char)0xC3;

		// Copy current data to new memory
		memcpy(pNewMem, m_pProgramStart, m_dwMCBlockSize);

		// Erase old
		SAFE_FREE(m_pProgramStart);

		// Rereference to new memory
		m_dwMCBlockSize=dwNewSize;
		m_pProgramStart=pNewMem;
		m_pMachineBlock=pNewMem+dwOffset;

		// If in middle of leap, ensure update
		for(di=0; di<9; di++) m_pRecordBytePosition[di]=pNewMem+dwLeapRelDiff[di];
		m_pRecordTopBytePosition=m_pProgramStart+dwByteOffset;

		// Mem was expanded
		return true;
	}

	// Did not expand
	return false;
}

bool CASMWriter::CheckAndExpandREFMemory(void)
{
	// If within 100 bytes of end, expand memory
	DWORD* pREFDataBarrier=(m_pProgramRefs+m_dwRefBufferSize)-(100);
	if(m_pProgramRefs+m_dwProgramRefPointer>pREFDataBarrier)
	{
		// Work out offset of pointer
		DWORD dwOffset = m_dwProgramRefPointer;

		// Create New Larger memory (another 1K)
		DWORD dwNewSize = m_dwRefBufferSize+1024;
		DWORD* pNewMem = new DWORD[dwNewSize];
		DWORD* pNewMemLabel = new DWORD[dwNewSize];

		// Clear memory
		ZeroMemory(pNewMem, dwNewSize*sizeof(DWORD));
		ZeroMemory(pNewMemLabel, dwNewSize*sizeof(DWORD));

		// Copy current data to new memory
		memcpy(pNewMem, m_pProgramRefs, m_dwRefBufferSize*sizeof(DWORD));
		memcpy(pNewMemLabel, m_pProgramRefLabel, m_dwRefBufferSize*sizeof(DWORD));

		// Erase old
		SAFE_DELETE(m_pProgramRefs);
		SAFE_DELETE(m_pProgramRefLabel);

		// Rereference to new memory
		m_dwRefBufferSize=dwNewSize;
		m_pProgramRefs=pNewMem;
		m_pProgramRefLabel=pNewMemLabel;
		m_dwProgramRefPointer=dwOffset;

		// Mem was expanded
		return true;
	}

	// Did not expand
	return false;
}

void VarValueSenderHook(void)
{
	// Send Message To Debugger
	DWORD dwSize=0;
	LPSTR pData=g_pASMWriter->MakeVarValuesForTransfer(&dwSize);
	g_pASMWriter->SendDataToDebugger(21, pData, dwSize);
	SAFE_DELETE(pData);
}

LRESULT DebugHookStatementFunctionCall(DWORD dwProg, DWORD dwLine, DWORD dwStart, DWORD dwEnd)
{
	// Send Message To Debugger
	DWORD* pData = new DWORD[4];
	pData[0] = dwProg;
	pData[1] = dwLine;
	pData[2] = dwStart;
	pData[3] = dwEnd;

	// Report Progress In Program
	LRESULT lResult = g_pASMWriter->SendDataToDebugger(11, (LPSTR)pData, sizeof(DWORD)*4);

	// The Return Value Controls the Debugger Flow
	switch(lResult)
	{
		case 0 : // Continues To Next Statement
			break;
		case 1 : // Stays At Current Statement
			break;
		case 2 : // Switches back to FullSpeed
			g_dwEscapeValueMem=0;
			lResult=0;//EAX needs to be zero
			break;
		case 11 : // Parse CLI Program
			g_dwBreakOutPosition=1;
			break;
	}

	// If compiler-loaded-debugger is closed, quit program
	if(g_bIsInternalDebugger==true)
	{
		DWORD uExitCode=0;
		GetExitCodeProcess(g_InternalDebuggerProcessInfo.hProcess, &uExitCode);
		if(uExitCode!=STILL_ACTIVE)
		{
			// Close program
			g_dwEscapeValueMem=2;
			PostQuitMessage(0);
		}
	}

	// Update Var Data
	VarValueSenderHook();

	return lResult;
}

void DebugHookJumpFunctionCall(DWORD dwProg, DWORD dwLine, DWORD dwStart, DWORD dwEnd)
{
	// Send Message To Debugger
	DWORD* pData = new DWORD[4];
	pData[0] = dwProg;
	pData[1] = dwLine;
	pData[2] = dwStart;
	pData[3] = dwEnd;
	g_pASMWriter->SendDataToDebugger(12, (LPSTR)pData, sizeof(DWORD)*4);
}

void DebugHookReturnFunctionCall(void)
{
	// Send Message To Debugger
	DWORD dwData;
	g_pASMWriter->SendDataToDebugger(13, (LPSTR)&dwData, 4);
}

bool CASMWriter::ReportAnyErrorsToCLI(void)
{
	DWORD dwRTError=g_pEXE->m_dwRuntimeErrorDWORD;
	DWORD dwRTErrorLine=g_pEXE->m_dwRuntimeErrorLineDWORD;
	if(dwRTError>0)
	{
		// Report error
		LPSTR lpReturnError = new char[1024];
		LPSTR pRuntimeErrorString = NULL;
		if(g_pEXE->m_pRuntimeErrorStringsArray) pRuntimeErrorString = (LPSTR)g_pEXE->m_pRuntimeErrorStringsArray[dwRTError];
		if(dwRTErrorLine>0)
			wsprintf(lpReturnError, "Runtime Error %d [%s] at line %d", dwRTError, pRuntimeErrorString, dwRTErrorLine);
		else
			wsprintf(lpReturnError, "Runtime Error %d [%s]", dwRTError, pRuntimeErrorString);
			
		SendDataToDebugger(31, lpReturnError, strlen(lpReturnError));
		delete lpReturnError;
		lpReturnError=NULL;

		// Clear error
		g_pEXE->m_dwRuntimeErrorDWORD=0;
		g_pEXE->m_dwRuntimeErrorLineDWORD=0;
	}

	// Complete
	return true;
}

bool CASMWriter::HideAnyHiddenCode(LPSTR pData, DWORD dwSize)
{
	LPSTR pPtr = pData;
	LPSTR pPtrEnd = pData + dwSize;
	bool bReplaceOn=false;
	while(pPtr<pPtrEnd)
	{
		// Check ahead
		if(strnicmp(pPtr, "HIDESTART", 9)==NULL)	{ bReplaceOn=true;	pPtr+=9; }
		if(strnicmp(pPtr, "HIDEEND", 7)==NULL)		{ bReplaceOn=false;	pPtr+=7; }

		// Replace
		if(*pPtr>32 && bReplaceOn==true) *pPtr='X';

		// Next char
		pPtr++;
	}

	// Complete
	return true;
}

bool CASMWriter::PrepareEXE(LPSTR pEXEFilename, bool bParsingMainProgram, bool bGotNewCode)
{
	db3::CProfile<> prof("CASMWriter::PrepareEXE()");

	// Toggled if init/run fails..
	bool bResult=true;

	// The end of the machine code (if any)
	if(bGotNewCode==true)
	{
		// Apply new machine code to EXEData
		LPSTR pProgramEnd = m_pMachineBlock;
		DWORD dwProgramSizeBytes = pProgramEnd-m_pProgramStart;

		// Machine Code Block (MCB)
		if(UpdateMCB(dwProgramSizeBytes)==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'PrepareEXE' : 1");
			return false;
		}

		// MCB Reference Data
		if(UpdateMCBRefData()==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'PrepareEXE' : 2");
			return false;
		}

		// DLL Data
		if(UpdateDLLData()==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'PrepareEXE' : 3");
			return false;
		}

		// Command Data
		if(UpdateCommandData()==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'PrepareEXE' : 4");
			return false;
		}

		// Strings Data
		if(UpdateStringData()==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'PrepareEXE' : 5");
			return false;
		}

		// Data Data
		if(UpdateDataData()==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'PrepareEXE' : 6");
			return false;
		}

		// Dynamic Variables Data
		if(UpdateDynamicData()==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'PrepareEXE' : 7");
			return false;
		}

		// U71 - Structures Data (for getting struct pattern at runtime)
		if(UpdateStructurePatternData()==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'PrepareEXE' : 8");
			return false;
		}
	}

	// New Var and Data Space Sizes
	g_pEXE->m_dwVariableSpaceSize = g_pStatementList->GetVarOffsetCounter();
	g_pEXE->m_dwDataSpaceSize = g_pStatementList->GetDataIndexCounter()*10;

	// Full or Debug Mode
	if(g_DebugInfo.DebugModeOn())
	{
		// Find Debugger to send to
		HWND hWnd = FindWindow(NULL,"DBProDebugger");
		if(hWnd==NULL && g_bIsInternalDebugger==true)
		{
			// Internal Debugger has been termined, so terminate App too...
			g_dwEscapeValueMem=2;
		}
		else
		{
			if(hWnd==NULL)
			{
				// If EXE for debug is absolute, using long range debug, so switch to correct dir
				if(pEXEFilename[1]==':')
				{
					// leefix - 240603 - u54 - this means we are running %TEMP\TEMP.EXE
					// and so we need to restore to the actual project exe directory (as we are in TEMP at the mo)
					// BUT when debug runs, the above exe does not point to project folder
					// so rely on the code from media path below
//					CStr* pPathOnly = new CStr("");
//					pPathOnly->SetText(pEXEFilename);
//					pPathOnly->TrimToPathOnly();
//					chdir(pPathOnly->GetStr());
//					SAFE_DELETE(pPathOnly);

// leefix - 240604 - u54 - removed as it seemed to do nothing, and even corrupted m_pEXEFilename
//					// use temp exe in temp folder (absolute path to temp exe)
//					strcpy(pEXEFilename, g_pDBPCompiler->GetInternalFile(PATH_TEMPFOLDER));
//					strcat(pEXEFilename, "\\temp.exe");
				}
			
				// Switch to Compiler Folder First
				char pStoreDir[_MAX_PATH];
				getcwd(pStoreDir, _MAX_PATH);

				// Change Directory now
				chdir(g_pDBPCompiler->GetInternalFile(PATH_ROOTPATH));

				// Start Debugger Window
				STARTUPINFO si;
				ZeroMemory(&si, sizeof(STARTUPINFO));
				si.cb=sizeof(STARTUPINFO);
				ZeroMemory(&g_InternalDebuggerProcessInfo, sizeof(PROCESS_INFORMATION));
				if(CreateProcess(	NULL, g_pDBPCompiler->GetInternalFile(PATH_DEBUGGERFILE),
									NULL, NULL, false,
									NORMAL_PRIORITY_CLASS,
									NULL, NULL,	&si, &g_InternalDebuggerProcessInfo))
				{
					// Wait until Debugger fully loaded
					WaitForInputIdle(g_InternalDebuggerProcessInfo.hProcess, 5000);
					g_bIsInternalDebugger=true;
				}

				// Restore previous directory
				chdir(pStoreDir);

// leefix - 210504 - seemed this could mess up EXE current directory 
// leefix - 240604 - restored as running debug mode removes CWD info!
				// Media Root Folder is project folder so switch to it
				LPSTR pMediaRoot=g_pDBPCompiler->GetProjectField("media root path");
				if(pMediaRoot)
					if(strcmp(pMediaRoot,"")!=NULL)
						chdir(pMediaRoot);
			}

			// Transfer Compiler Info to Debugger
			LPSTR pData = g_DebugInfo.GetProgramPtr();
			HideAnyHiddenCode(pData, g_DebugInfo.GetProgramSize());
			SendDataToDebugger(1, pData, g_DebugInfo.GetProgramSize());
			DWORD dwDataSize=0;
			LPSTR pVarGlobalData=MakeVarDataForTransfer(&dwDataSize);
			SendDataToDebugger(2, pVarGlobalData, dwDataSize);
			SAFE_DELETE(pVarGlobalData);
			LPSTR pNameData = g_pDBPCompiler->GetProgramName();
			SendDataToDebugger(3, pNameData, strlen(pNameData));

			// Report Any Runtime Errors from previous run..
			ReportAnyErrorsToCLI();

			// Prepare Pointers to Debug Hook Functions
			bResult=true;
			LPVOID pDHookS = (LPVOID)DebugHookStatementFunctionCall;
			LPVOID pDHookJ = (LPVOID)DebugHookJumpFunctionCall;
			LPVOID pDHookR = (LPVOID)DebugHookReturnFunctionCall;
			LPSTR pReturnError = NULL;

			// Critical start info
			strcpy(g_pEXE->m_pUnpackFolderName, g_pDBPCompiler->GetInternalFile(PATH_PLUGINSFOLDER));
			g_pEXE->m_dwEncryptionKey=0;
			g_pEXE->StartInfo(g_pEXE->m_pUnpackFolderName, g_pEXE->m_dwEncryptionKey);

			// Main Program Run or MiniProgram Run
			if(bParsingMainProgram)
			{
				// MAIN PROGRAM RUN (MAIN-RUN)

				// Init The EXE in Debug Mode
				bResult=g_pEXE->InitDebug(g_DebugInfo.GetInstance(), pDHookS, pDHookJ, pDHookR, bResult, &pReturnError, NULL, false);

				// Store Variable Space Address In Global (for use by VarHook above)
				g_pVarSpaceAddressInUse = g_pEXE->m_pVariableSpace;
				g_dwVarSpaceSizeInUse = g_pEXE->m_dwVariableSpaceSize;

				// Run The EXE
				bResult=g_pEXE->Run(bResult);
			}
			else
			{
				// MINI PROGRAM RUN (MINI-RUN)

				// Mini-Init (only inits new stuff)
				bResult=g_pEXE->InitMini(pDHookS, pDHookJ, pDHookR, bResult, &pReturnError);

				// Aquire New Variable Allocation
				g_pVarSpaceAddressInUse = g_pEXE->m_pVariableSpace;
				g_dwVarSpaceSizeInUse = g_pEXE->m_dwVariableSpaceSize;

				// Only run CLItext if it produced valid code
				if(bGotNewCode==true)
				{
					// Store breakpoint position from mast run break
					DWORD dwStoreBreakPointBeforeRunCLICode = g_dwBreakOutPosition;

					// Ensure CLI run is full speed
					g_dwEscapeValueMem = 0;

					// Run The EXE for CLICode (remove breakpoint influence for min-prog)
					g_dwBreakOutPosition = 0;
					bResult=g_pEXE->RunFrom(bResult, g_pEXE->m_dwStartOfMiniMC);

					// Restore breakpoint position from mast run break
					g_dwBreakOutPosition = dwStoreBreakPointBeforeRunCLICode;
				}

				// Update BreakPoint with new memblock ptr for return jump (if any)
				if(g_dwBreakOutPosition>0)
				{
					g_dwBreakOutPosition = g_dwBreakOutPosition + (DWORD)g_pEXE->m_pMachineCodeBlock;
				}

				// Update screen (using SYNC) as CLI may not do this..
				g_CORE_SyncRefresh(); g_CORE_SyncRefresh();

				// Ensure CLI runs is debug mode!
				g_dwEscapeValueMem = 1;

				// Report Any Runtime Errors from CLI run..
				ReportAnyErrorsToCLI();

				// Run The Main EXE Again
				bResult=g_pEXE->Run(bResult);
			}

			// If special escape value set, broke from program whilst data still on stacl (cannot return to program)
			if(g_dwEscapeValueMem==3)
			{
				// ESP in and ESP out was not equal - means broke from function..
				g_dwEscapeValueMem=1;
				g_pEXE->m_dwRuntimeErrorDWORD=1;
				g_pEXE->m_dwRuntimeErrorLineDWORD=0;
			}

			// Free Debugger When Done (when quit program)
			if(g_bIsInternalDebugger==true)
			{
				if(g_dwEscapeValueMem==2)
				{
					DWORD uExitCode=0;
					GetExitCodeProcess(g_InternalDebuggerProcessInfo.hProcess, &uExitCode);
					TerminateProcess(g_InternalDebuggerProcessInfo.hProcess,  (UINT)uExitCode );
				}
			}

			// Free Usages
			SAFE_DELETE(pReturnError);
		}
	}
	else
	{
		// Progress Reporting Tool
		g_pErrorReport->ProgressReport("Linker now at line ",g_pErrorReport->GetPerc(0));

		// Save EXE Block
		g_pEXE->Save(g_pDBPCompiler->GetInternalFile(PATH_TEMPEXBFILE));

		// Plugins Root Folder (same as project folder)
		char pEffectsRoot[_MAX_PATH];
		char pPluginsRoot[_MAX_PATH];
		strcpy(pEffectsRoot, g_pDBPCompiler->m_pCompilerPathOnly->GetStr());
		strcat(pEffectsRoot, "effects\\");
		strcpy(pPluginsRoot, g_pDBPCompiler->m_pCompilerPathOnly->GetStr());
		strcat(pPluginsRoot, "plugins\\");

		// Extra Plugins Folders
		char pUserPluginsRoot[_MAX_PATH];
		char pLicensedPluginsRoot[_MAX_PATH];
		strcpy(pUserPluginsRoot, g_pDBPCompiler->m_pCompilerPathOnly->GetStr());
		strcat(pUserPluginsRoot, "plugins-user\\");
		strcpy(pLicensedPluginsRoot, g_pDBPCompiler->m_pCompilerPathOnly->GetStr());
		strcat(pLicensedPluginsRoot, "plugins-licensed\\");

		// Media Root Folder
		LPSTR pMediaRoot=g_pDBPCompiler->GetProjectField("media root path");
		if(pMediaRoot==NULL)
		{
			// No Media Root Folder so use current directory)
			pMediaRoot = new char[_MAX_PATH];
			getcwd(pMediaRoot,_MAX_PATH);
		}
		DWORD dwCutOutRoot=strlen(pMediaRoot);

		// Create EXECUTABLE Here
		CFileBuilder CFBuilder;

		// Pass All Filenames for System Files & Media
		CFBuilder.NewFileTable();

		// Virtual EXB for Program Code
		CFBuilder.AddFile(g_pDBPCompiler->GetInternalFile(PATH_TEMPEXBFILE), "_virtual.dat");
	
		// Add DLLs to Build
		bool bFXFilesRequired = false;
		if ( g_bExternaliseDLLS==false )
		{
			// only if not entirely excluding DLLs
			for(DWORD dll=0; dll<g_pEXE->m_dwNumberOfDLLs; dll++)
			{
				// DLL To Add To Table
				LPSTR pDLLName = (LPSTR)g_pEXE->m_pDLLFilenameArray[dll];

				// Add Actual File to Table
				char pAbsPathToDLL[_MAX_PATH];
				strcpy(pAbsPathToDLL, pPluginsRoot);
				strcat(pAbsPathToDLL, pDLLName);

				// Primarily the plugin, else the user plugin folder
				if(g_pDBPCompiler->FileExists(pAbsPathToDLL))
				{
					CFBuilder.AddFile(pAbsPathToDLL, pDLLName);
				}
				else
				{
					// Get DLL from user folder (if exists - some DLLs can be removed without fault, ie Conv3DS)
					strcpy(pAbsPathToDLL, pUserPluginsRoot);
					strcat(pAbsPathToDLL, pDLLName);
					if(g_pDBPCompiler->FileExists(pAbsPathToDLL))
					{
						CFBuilder.AddFile(pAbsPathToDLL, pDLLName);
					}
					else
					{
						// leefix - 120104 - DLL can be in licensed folder also
						strcpy(pAbsPathToDLL, pLicensedPluginsRoot);
						strcat(pAbsPathToDLL, pDLLName);
						if(g_pDBPCompiler->FileExists(pAbsPathToDLL))
						{
							CFBuilder.AddFile(pAbsPathToDLL, pDLLName);
						}
					}
				}

				// If DLL is Basic3D, need FX files too
				if ( stricmp ( pDLLName, "dbprobasic3ddebug.dll" )==NULL ) bFXFilesRequired = true;
			}
		}

		// Check if Icon Specified256 colour icon specified
		LPSTR pIncludesReplacementIconFile = g_pDBPCompiler->GetProjectField("icon1");
		if(pIncludesReplacementIconFile)
			if(strcmp(pIncludesReplacementIconFile, "")==NULL)
				SAFE_DELETE(pIncludesReplacementIconFile);

		// All MEDIA required (from project file)
		if(g_pDBPCompiler->m_bInternalMediaState==true)
		{
			DWORD media=1;
			while(media<65535)
			{
				// Get Media File
				char mediafieldname[256];
				wsprintf(mediafieldname, "media%d", media);
				LPSTR pMediaFileName = g_pDBPCompiler->GetProjectField(mediafieldname);
				if(pMediaFileName)
				{
					if(strcmp(pMediaFileName,"")==NULL)
					{
						SAFE_DELETE(pMediaFileName);
						break;
					}
				}
				if(pMediaFileName==NULL) break;

				// if pMediaFileName is absolute
				bool bUseMedia=true;
				if(pMediaFileName[1]==':')
				{
					if(strnicmp(pMediaFileName, pMediaRoot, strlen(pMediaRoot))==NULL)
					{
						// remove media path from it
						strrev(pMediaFileName);
						pMediaFileName[strlen(pMediaRoot)]=0;
						strrev(pMediaFileName);
					}
					else
					{
						// media must be in media root - otherwise ignore
						SAFE_DELETE(pMediaFileName);
						bUseMedia=false;
					}
				}
				if(bUseMedia)
				{
					// Check if media has wildcards (more than one file)
					bool bHadWildcards=false;
					for(DWORD d=0; d<strlen(pMediaFileName); d++)
						if(pMediaFileName[d]=='*')
							bHadWildcards=true;

					// What is the relative path from the project root onwards
					char pMediaPath[_MAX_PATH];
					char pAbsPathToMedia[_MAX_PATH];
					if(bHadWildcards==false)
					{
						// Add Actual File to Table
						strcpy(pMediaPath, "media\\");
						strcat(pMediaPath, pMediaFileName);
						strcpy(pAbsPathToMedia, pMediaRoot);
						strcat(pAbsPathToMedia, pMediaFileName);
						CFBuilder.AddFile(pAbsPathToMedia, pMediaPath);
					}
					else
					{
						// Add Wildcard Files to Table
						CFBuilder.AddWildcardFiles(pMediaRoot, pMediaFileName);
					}

					// Only overwrite if icon.ico (priority override icon)
					if(stricmp(pMediaFileName, "icon.ico")==NULL)
					{
						// Found better ICO file
						SAFE_DELETE(pIncludesReplacementIconFile);
						pIncludesReplacementIconFile = pMediaFileName;
					}
					else
					{
						SAFE_DELETE(pMediaFileName);
					}
				}

				// Next Media index
				media++;
			}
		}

		// Icon file used for window/taskbar
		if(pIncludesReplacementIconFile)
		{
			// Media includes an icon
			bool bUseMedia=true;
			char pAbsPathToMediaIcon[_MAX_PATH];
			if(pIncludesReplacementIconFile[1]==':')
			{
				if(strnicmp(pIncludesReplacementIconFile, pMediaRoot, strlen(pMediaRoot))==NULL)
				{
					// remove media path from it
					strcpy(pAbsPathToMediaIcon, pIncludesReplacementIconFile);
				}
				else
				{
					// media must be in media root - otherwise ignore
					SAFE_DELETE(pIncludesReplacementIconFile);
					bUseMedia=false;
				}
			}
			else
			{
				strcpy(pAbsPathToMediaIcon, pMediaRoot);
				strcat(pAbsPathToMediaIcon, pIncludesReplacementIconFile);
			}
			if(bUseMedia)
			{
				// Make an ICO file from BMP
				char pWorkIcon[_MAX_PATH];
				DWORD dwLength=strlen(pAbsPathToMediaIcon);
				if(strnicmp(pAbsPathToMediaIcon+dwLength-4, ".bmp", 4)==NULL)
				{
					// BMP to ICO
					strcpy(pWorkIcon, pPluginsRoot);
					CFBuilder.MakeICOFromBMP(pAbsPathToMediaIcon, pWorkIcon);
					strcat(pWorkIcon, "workicon.ico");
				}
				else
				{
					// Must be ICO
					strcpy(pWorkIcon, pAbsPathToMediaIcon);
				}

				// Add Actual File to Table
				CFBuilder.AddFile(pWorkIcon, "icon.ico");
				SAFE_DELETE(pIncludesReplacementIconFile);
			}
		}
		else
		{
			// Add Actual File to Table
			char pAbsPathToIcon[_MAX_PATH];
			strcpy(pAbsPathToIcon, pPluginsRoot);
			strcat(pAbsPathToIcon, "icon.ico");
			CFBuilder.AddFile(pAbsPathToIcon, "icon.ico");
		}

		// Cursor files used for application window
		for ( DWORD cindex=0; cindex<32; cindex++)
		{
			char pFieldName[_MAX_PATH];
			char pFinalFileName[_MAX_PATH];
			if ( cindex==0 )
			{
				strcpy(pFieldName,"cursorarrow");
				strcpy(pFinalFileName,"arrow.cur");
			}
			if ( cindex==1 )
			{
				strcpy(pFieldName,"cursorwait");
				strcpy(pFinalFileName,"hourglass.cur");
			}
			if ( cindex>=2 )
			{
				wsprintf(pFieldName, "pointer%d", cindex);
				wsprintf(pFinalFileName, "pointer%d.cur", cindex);
			}

			LPSTR pCursorFilename = g_pDBPCompiler->GetProjectField(pFieldName);
			if(pCursorFilename)
			{
				if(strcmp(pCursorFilename, "")>0)
				{
					// Media includes an icon
					bool bUseMedia=true;
					char pAbsPathToMediaCur[_MAX_PATH];
					if(pCursorFilename[1]==':')
					{
						if(strnicmp(pCursorFilename, pMediaRoot, strlen(pMediaRoot))==NULL)
						{
							// remove media path from it
							strcpy(pAbsPathToMediaCur, pCursorFilename);
						}
						else
						{
							// media must be in media root - otherwise ignore
							SAFE_DELETE(pCursorFilename);
							bUseMedia=false;
						}
					}
					else
					{
						strcpy(pAbsPathToMediaCur, pMediaRoot);
						strcat(pAbsPathToMediaCur, pCursorFilename);
					}
					if(bUseMedia)
					{
						// Make an CUR file from BMP
						char pWorkCursor[_MAX_PATH];
						DWORD dwLength=strlen(pAbsPathToMediaCur);
						if(strnicmp(pAbsPathToMediaCur+dwLength-4, ".bmp", 4)==NULL)
						{
							// BMP to CUR
							wsprintf(pWorkCursor, "%sworkcursor%d.cur", pPluginsRoot, cindex);
							CFBuilder.MakeCURFromBMP(pAbsPathToMediaCur, pWorkCursor);
						}
						else
						{
							// Must be CUR
							strcpy(pWorkCursor, pAbsPathToMediaCur);
						}

						// Add Actual File to Table
						CFBuilder.AddFile(pWorkCursor, pFinalFileName);
					}
				}
			}
			SAFE_DELETE(pCursorFilename);
		}

		// Add FX Files when appropriate
		if ( bFXFilesRequired==true )
		{
			char pFXName[_MAX_PATH];
			char pFXPathAndName[_MAX_PATH];
			for ( int i=0; i<6; i++)
			{
				if ( i==0 ) strcpy ( pFXName, "bump.fx" );
				if ( i==1 ) strcpy ( pFXName, "cartoon.fx" );
				if ( i==2 ) strcpy ( pFXName, "rainbow.fx" );
				if ( i==3 ) strcpy ( pFXName, "stencilshadow.fx" );
				if ( i==4 ) strcpy ( pFXName, "stencilshadowbone.fx" );
				if ( i==5 ) strcpy ( pFXName, "quad.fx" );
				strcpy ( pFXPathAndName, pEffectsRoot );
				strcat ( pFXPathAndName, pFXName );
				CFBuilder.AddFile(pFXPathAndName, pFXName);
			}
		}

		// If EXEAlone (for safecast type stuff, also cannot encrypt files)
		if(g_pDBPCompiler->GetEXEAloneState()==true)
		{
			// Cannot add unique key to exe stub, so it disables encryption
			g_pDBPCompiler->m_bEncryptionState=false;
		}

		// If Compression Set
		LPSTR pCompressDLL=NULL;
		if(g_pDBPCompiler->m_bCompressPCKState)
			pCompressDLL = g_pDBPCompiler->GetInternalFile(PATH_COMPRESSDLLFILE);

		// Progress Reporting Tool
		g_pErrorReport->ProgressReport("Linker now at line ",g_pErrorReport->GetPerc(10));

		// Make The EXE
		CFBuilder.MakeEXE(pEXEFilename, g_pDBPCompiler->m_bEncryptionState, pCompressDLL);

		// Progress Reporting Tool
		g_pErrorReport->ProgressReport("Linker now at line ",g_pErrorReport->GetPerc(50));

		// Ensure EXE created okay
		if(g_pDBPCompiler->FileExists(pEXEFilename))
		{
			// Produce Absolute Paths to replacement-files resources
			char pAbsResourceFile[_MAX_PATH];
			CFBuilder.NewFileTable();

			// Get stringdatas (max 24 chars as appendages takes up to 32 chars)
			LPSTR pOrigAppName = g_pDBPCompiler->GetProjectField("app title");
			LPSTR pAppName = new char[33];
			memset ( pAppName, 0, 33 );
			if(pOrigAppName)
			{
				DWORD dwSourceSize = 0;
				if ( pOrigAppName ) dwSourceSize = strlen(pOrigAppName);
				if ( dwSourceSize > 24 ) dwSourceSize=24;
				memcpy(pAppName, pOrigAppName, dwSourceSize);
			}
			else
				strcpy(pAppName, "DBPro Application");

			pAppName[24]=0;

			LPSTR pExeName = new char[33];
			memset ( pExeName, 0, 33 );
			if(pEXEFilename)
			{
				DWORD dwSourceSize = strlen(pEXEFilename);
				if ( dwSourceSize > 24 ) dwSourceSize=24;
				memcpy(pExeName, pEXEFilename, dwSourceSize);
			}
			else
				strcpy(pExeName, "executable.exe");

			pExeName[24]=0;
			SAFE_DELETE(pOrigAppName);

			// [0-9] version info (strings not files)
			for(int iIndex=0; iIndex<=9; iIndex++)
			{
				// Get ver info
				LPSTR pStr = NULL;
				if(iIndex==0) pStr = g_pDBPCompiler->GetProjectField("VerComments");
				if(iIndex==1) pStr = g_pDBPCompiler->GetProjectField("VerCompany");
				if(iIndex==2) pStr = g_pDBPCompiler->GetProjectField("VerFileDesc");
				if(iIndex==3) pStr = g_pDBPCompiler->GetProjectField("VerFileNumber");
				if(iIndex==4) pStr = g_pDBPCompiler->GetProjectField("VerInternal");
				if(iIndex==5) pStr = g_pDBPCompiler->GetProjectField("VerCopyright");
				if(iIndex==6) pStr = g_pDBPCompiler->GetProjectField("VerTrademark");
				if(iIndex==7) pStr = g_pDBPCompiler->GetProjectField("VerFilename");
				if(iIndex==8) pStr = g_pDBPCompiler->GetProjectField("VerProduct");
				if(iIndex==9) pStr = g_pDBPCompiler->GetProjectField("VerProductNumber");

				// Add to filetable [0-9]
				if(iIndex==3 || iIndex==9)
				{
					LPSTR pFillStr = new char[11];
					memset(pFillStr, 0, 11);
					if(pStr==NULL)
					{
						if(iIndex==3) strcpy(pFillStr, "V1.0");
						if(iIndex==9) strcpy(pFillStr, "V1.0");
					}
					else
					{
						DWORD dwSourceSize = 0;
						if ( pStr ) dwSourceSize = strlen(pStr);
						if ( dwSourceSize > 10 ) dwSourceSize=10;
						memcpy(pFillStr, pStr, dwSourceSize);
						pFillStr[10]=0;
					}
					CFBuilder.AddFile(pFillStr,"");
					SAFE_DELETE(pFillStr);
				}
				else
				{
					LPSTR pFillStr = new char[33];
					memset(pFillStr, 0, 33);
					if(pStr==NULL)
					{
						// Copy to fill string
						if(iIndex==0) strcpy(pFillStr, pAppName);
						if(iIndex==1)
						{
							strcpy(pFillStr, pAppName);
							strcat(pFillStr, " Ltd");
						}
						if(iIndex==2) strcpy(pFillStr, pAppName);
						if(iIndex==4) strcpy(pFillStr, pAppName);
						if(iIndex==5)
						{
							strcpy(pFillStr, "(C) ");
							strcat(pFillStr, pAppName);
							strcat(pFillStr, " Ltd");
						}
						if(iIndex==6)
						{
							strcpy(pFillStr, "");
						}
						if(iIndex==7)
						{
							strcpy(pFillStr, pExeName);
						}
						if(iIndex==8)
						{
							strcpy(pFillStr, pAppName);
						}
					}
					else
					{
						DWORD dwSourceSize = 0;
						if ( pStr ) dwSourceSize = strlen(pStr);
						if ( dwSourceSize > 32 ) dwSourceSize=32;
						memcpy(pFillStr, pStr, dwSourceSize);
						pFillStr[32]=0;
					}
					CFBuilder.AddFile(pFillStr,"");
					SAFE_DELETE(pFillStr);
				}

				// Free usages
				SAFE_DELETE(pStr);
			}

			// Free usages
			SAFE_DELETE(pAppName);
			SAFE_DELETE(pExeName);

			// [10] 32x32 icon
			strcpy(pAbsResourceFile, "");
			LPSTR pIconFilename = g_pDBPCompiler->GetProjectField("icon1");
			if(pIconFilename==NULL) pIconFilename = g_pDBPCompiler->GetProjectField("icon1");
			if(pIconFilename)
			{
				if(pIconFilename[1]!=':') strcpy(pAbsResourceFile, pMediaRoot);
				strcat(pAbsResourceFile, pIconFilename);
			}
			else
			{
				strcpy(pAbsResourceFile, pPluginsRoot);
				strcat(pAbsResourceFile, "icon.ico");
			}
			CFBuilder.AddFile(pAbsResourceFile,"");
			SAFE_DELETE(pIconFilename);

			// [11] 16x16 icon
			strcpy(pAbsResourceFile, "");
			pIconFilename = g_pDBPCompiler->GetProjectField("icon2");
			if(pIconFilename==NULL) pIconFilename = g_pDBPCompiler->GetProjectField("icon1");
			if(pIconFilename)
			{
				if(pIconFilename[1]!=':') strcpy(pAbsResourceFile, pMediaRoot);
				strcat(pAbsResourceFile, pIconFilename);
			}
			else
			{
				strcpy(pAbsResourceFile, pPluginsRoot);
				strcat(pAbsResourceFile, "icon.ico");
			}
			CFBuilder.AddFile(pAbsResourceFile,"");
			SAFE_DELETE(pIconFilename);

			// Progress Reporting Tool
			g_pErrorReport->ProgressReport("Linker now at line ",g_pErrorReport->GetPerc(60));

			// Change The EXE with new resources
			CFBuilder.ChangeEXE(pEXEFilename, pPluginsRoot);

			// Exe is alone or packed with PCK file
			bool bKeepEXEAlone=g_pDBPCompiler->GetEXEAloneState();

			// Kind Of Executable Determines if(installer or not)
			DWORD KindOfExecutable=0;
			if(g_pDBPCompiler->GetEXEInstallerState())
			{
				// Installer is a single EXE that unpacks EXErunner+EXEmedia to CWD
				bKeepEXEAlone=false;
				KindOfExecutable=1;
			}

			// Progress Reporting Tool
			g_pErrorReport->ProgressReport("Linker now at line ",g_pErrorReport->GetPerc(100));

			// Add PCK file onto end of executable if required
			if(bKeepEXEAlone==false)
			{
				CFBuilder.AddPCKToEXE(pEXEFilename, KindOfExecutable);
			}
		}
		else
		{
			// EXE Not Created
			FreeMachineBlock();

			// Report failure
			g_pErrorReport->AddErrorString("Failed to 'PrepareEXE' : EXE Filename not exist");

			// return as failure
			return false;
		}

		// Free dynamic media string
		SAFE_DELETE(pMediaRoot);
	}

	// Free Usages
	FreeMachineBlock();

	// Complete
	return bResult;
}

bool CASMWriter::UpdateMCB(DWORD dwProgramSizeBytes)
{
	db3::CProfile<> prof("CASMWriter::UpdateMCB");

	if(g_pEXE->m_pMachineCodeBlock==NULL)
	{
		// Create Array(s)
		DWORD dwNewSize = dwProgramSizeBytes;
		LPSTR pNewArray = new char[dwProgramSizeBytes];
		memcpy(pNewArray, m_pProgramStart, dwNewSize);

		// Update pointers
		g_pEXE->m_dwSizeOfMCB = dwNewSize;
		g_pEXE->m_pMachineCodeBlock = (DWORD*)pNewArray;
	}
	else
	{
		// Mark Beginning of Mini-Code M/C (so refdata points to correct bytes)
		g_pEXE->m_dwStartOfMiniMC = g_pEXE->m_dwSizeOfMCB;

		// Add To Array(s)
		DWORD dwOldSize = g_pEXE->m_dwSizeOfMCB;
		LPSTR pOldArray = (LPSTR)g_pEXE->m_pMachineCodeBlock;
		DWORD dwNewSize = dwOldSize + dwProgramSizeBytes;
		LPSTR pNewArray = new char[dwNewSize];

		// Fill New Array with Old+New, then delete Old
		memcpy(pNewArray, pOldArray, dwOldSize);
		memcpy(pNewArray+dwOldSize, m_pProgramStart, dwProgramSizeBytes);
		SAFE_DELETE(pOldArray);

		// Update pointers
		g_pEXE->m_dwSizeOfMCB = dwNewSize;
		g_pEXE->m_pMachineCodeBlock = (DWORD*)pNewArray;
	}

	// Complete
	return true;
}

bool CASMWriter::UpdateMCBRefData(void)
{
	db3::CProfile<> prof("CASMWriter::UpdateMCBRefData");

	// Tokenise Process Range
	DWORD dwStartAt, dwFinishAt;

	// Create or Add Arrays
	if(g_pEXE->m_pRefArray==NULL)
	{
		// Create Array(s)
		DWORD dwNewSize = m_dwProgramRefPointer;
		LPSTR pNewArray1 = (LPSTR)g_pEXE->CreateArray(dwNewSize);
		LPSTR pNewArray2 = (LPSTR)g_pEXE->CreateArray(dwNewSize);
		LPSTR pNewArray3 = (LPSTR)g_pEXE->CreateArray(dwNewSize);

		// Update pointers
		g_pEXE->m_dwNumberOfReferences = dwNewSize;
		g_pEXE->m_pRefArray = (DWORD*)pNewArray1;
		g_pEXE->m_pRefTypeArray = (DWORD*)pNewArray2;
		g_pEXE->m_pRefIndexArray = (DWORD*)pNewArray3;

		// Tokenise all initial data
		dwStartAt = 0;
		dwFinishAt = g_pEXE->m_dwNumberOfReferences;
	}
	else
	{
		// Add To Array(s)
		DWORD dwOldSize = g_pEXE->m_dwNumberOfReferences;
		LPSTR pOldArray1 = (LPSTR)g_pEXE->m_pRefArray;
		LPSTR pOldArray2 = (LPSTR)g_pEXE->m_pRefTypeArray;
		LPSTR pOldArray3 = (LPSTR)g_pEXE->m_pRefIndexArray;
		DWORD dwNewSize = dwOldSize + m_dwProgramRefPointer;
		LPSTR pNewArray1 = (LPSTR)g_pEXE->CreateArray(dwNewSize);
		LPSTR pNewArray2 = (LPSTR)g_pEXE->CreateArray(dwNewSize);
		LPSTR pNewArray3 = (LPSTR)g_pEXE->CreateArray(dwNewSize);

		// Fill New Array with Old+New, then delete Old
		memcpy(pNewArray1, pOldArray1, dwOldSize*sizeof(DWORD));
		memcpy(pNewArray2, pOldArray2, dwOldSize*sizeof(DWORD));
		memcpy(pNewArray3, pOldArray3, dwOldSize*sizeof(DWORD));
		SAFE_DELETE(pOldArray1);
		SAFE_DELETE(pOldArray2);
		SAFE_DELETE(pOldArray3);

		// Update pointers
		g_pEXE->m_dwNumberOfReferences = dwNewSize;
		g_pEXE->m_pRefArray = (DWORD*)pNewArray1;
		g_pEXE->m_pRefTypeArray = (DWORD*)pNewArray2;
		g_pEXE->m_pRefIndexArray = (DWORD*)pNewArray3;

		// Only need to tokenise new entries
		dwStartAt = dwOldSize;
		dwFinishAt = dwNewSize;
	}

	// Tokenise RefLabels --> RefType & RefIndex 
	int iTokeniseCount = dwFinishAt-dwStartAt;
	for(int ref=0; ref<iTokeniseCount; ref++)
	{
		// Calculate EXERefData Index
		DWORD dwEXERefIndex = dwStartAt+ref;

		// Tokenise entry
		DWORD dwArrFlag=0;
		DWORD dwUseMemOffset=0;
		unsigned char iRefType=0;
		LPSTR pStr = (LPSTR)*(m_pProgramRefLabel+ref);
		if(iRefType==0 && pStr[0]=='[') iRefType=1;
		if(iRefType==0 && pStr[0]=='$' && pStr[1]=='$') iRefType=2;
		if(iRefType==0 && pStr[0]=='@' && pStr[1]=='&') { iRefType=3; dwArrFlag=1; }
		if(iRefType==0 && pStr[0]=='+') { iRefType=3; pStr++; dwUseMemOffset=4; }
		if(iRefType==0 && pStr[0]=='@') iRefType=3;
		if(iRefType==0 && pStr[0]=='$' && pStr[1]=='l' && pStr[2]=='a' && pStr[3]=='b' && pStr[4]=='e' && pStr[5]=='l') iRefType=5;
		if(iRefType==0 && pStr[0]=='$' && pStr[1]=='d' && pStr[2]=='a' && pStr[3]=='b' && pStr[4]=='e' && pStr[5]=='l') iRefType=6;
		if(iRefType==0) iRefType=4;
		if(iRefType>0)
		{
			// Write Ref Header (Type)
			g_pEXE->m_pRefTypeArray[dwEXERefIndex]=iRefType;

			// Write Ref Header (Index)
			DWORD dwIndex=0;
			if(iRefType==1)
			{
				int iCommandIndex=atoi(pStr+1)-1;	// [X=index to command
				if(iCommandIndex!=-1)
					dwIndex=iCommandIndex;
			}
			if(iRefType==2)
			{
				int iStringIndex=atoi(pStr+2)-1;	// $$X=index to string
				if(iStringIndex!=-1)
					dwIndex=iStringIndex;
			}
			if(iRefType==3)
			{
				CVarTable* pVar = g_pVarTable->FindVariable(NULL, pStr+1, dwArrFlag);
				if(pVar)
				{
					DWORD dwOffset = pVar->GetOffsetValue() + dwUseMemOffset;
					dwIndex=dwOffset;				// XXX offset to variable space only
				}
				else
				{
					// Could not find 'array' variable, must not exist
					if(dwArrFlag==1)
					{
						g_pErrorReport->SetError(0, ERR_SYNTAX+6, pStr+2);
						return false;
					}
				}
			}
			if(iRefType==4)
			{
				// lee - 2403060 u6b4 - not sure how this was not caught (-1 would not be 0xFFFFFFFF)
				// dwIndex=atoi(pStr);					// XXX immediate value only
				dwIndex = (DWORD)_atoi64(pStr);
			}
			if(iRefType==5)
			{
				CStr* pRealLabel = new CStr(pStr);
				DWORD dwCrapPos = pRealLabel->FindFirstChar('@');
				if(dwCrapPos>0) pRealLabel->EatChar(dwCrapPos);
				CLabelTable* pLabel = g_pLabelTable->FindLabel(pRealLabel->GetStr());
				if(pLabel==NULL)
				{
					g_pErrorReport->AddErrorString("Failed to 'CreateASMFooter::FindLabel'");
					SAFE_DELETE(pRealLabel);
					return false;
				}
				else
				{
					// XXX offset from program start to jump position
					dwIndex = pLabel->GetBytePosition();
					if(stricmp(pLabel->GetName()->GetStr(), "$labelend")!=NULL)
					{
						// Ensure label is correctly jumped to
						dwIndex = dwIndex + g_pEXE->m_dwStartOfMiniMC;		
					}
				}
				SAFE_DELETE(pRealLabel);
			}
			if(iRefType==6)
			{
				CStr* pRealLabel = new CStr(pStr);
				pRealLabel->SetChar(1,'l');
				DWORD dwCrapPos = pRealLabel->FindFirstChar('@');
				if(dwCrapPos>0) pRealLabel->EatChar(dwCrapPos);
				CLabelTable* pLabel = g_pLabelTable->FindLabel(pRealLabel->GetStr());
				if(pLabel==NULL)
				{
					g_pErrorReport->AddErrorString("Failed to 'CreateASMFooter::FindLabel'");
					return false;
				}
				else
				{
					// XXX offset from datastatement start to data line
					dwIndex=pLabel->GetDataIndex();
				}
				SAFE_DELETE(pRealLabel);
			}
			g_pEXE->m_pRefIndexArray[dwEXERefIndex] = dwIndex;
			g_pEXE->m_pRefArray[dwEXERefIndex] = ( *(m_pProgramRefs+ref) + g_pEXE->m_dwStartOfMiniMC );
		}
	}

	// Complete
	return true;
}

bool CASMWriter::UpdateDLLData(void)
{
	db3::CProfile<> prof("CASMWriter::UpdateDLLData");

	DWORD dwDLLIndex = 0;
	if(g_pEXE->m_pDLLIndexArray==NULL)
	{
		// Create Array(s)
		DWORD dwNewSize = g_pStatementList->GetDLLIndexCounter();// + 1; //for EXE nonDLL
		LPSTR pNewArray1 = (LPSTR)g_pEXE->CreateArray(dwNewSize);
		LPSTR pNewArray2 = (LPSTR)g_pEXE->CreateArray(dwNewSize);
		LPSTR pNewArray3 = (LPSTR)g_pEXE->CreateArray(dwNewSize);

		// Update pointers
		g_pEXE->m_dwNumberOfDLLs = dwNewSize;
		g_pEXE->m_pDLLIndexArray = (DWORD*)pNewArray1;
		g_pEXE->m_pDLLFilenameArray = (DWORD*)pNewArray2;
		g_pEXE->m_pDLLLoadedAlreadyArray = (DWORD*)pNewArray3;
	}
	else
	{
		// Count New DLLs
		DWORD dwNewDLLs=0;
		CDataTable* pStringEntry = g_pDLLTable->GetNext();
		while(pStringEntry)
		{
			if(pStringEntry->GetAddedToEXEData()==false) dwNewDLLs++;
			pStringEntry=pStringEntry->GetNext();
		}

		// Add To Array(s)
		DWORD dwOldSize = g_pEXE->m_dwNumberOfDLLs;
		LPSTR pOldArray1 = (LPSTR)g_pEXE->m_pDLLIndexArray;
		LPSTR pOldArray2 = (LPSTR)g_pEXE->m_pDLLFilenameArray;
		LPSTR pOldArray3 = (LPSTR)g_pEXE->m_pDLLLoadedAlreadyArray;
		DWORD dwNewSize = dwOldSize + dwNewDLLs;
		if(dwNewSize>dwOldSize)
		{
			// Only DLLs can wind up with less in the table than before!
			LPSTR pNewArray1 = (LPSTR)g_pEXE->CreateArray(dwNewSize);
			LPSTR pNewArray2 = (LPSTR)g_pEXE->CreateArray(dwNewSize);
			LPSTR pNewArray3 = (LPSTR)g_pEXE->CreateArray(dwNewSize);

			// Fill New Array with Old+New, then delete Old
			memcpy(pNewArray1, pOldArray1, dwOldSize*sizeof(DWORD));
			memcpy(pNewArray2, pOldArray2, dwOldSize*sizeof(DWORD));
			memcpy(pNewArray3, pOldArray3, dwOldSize*sizeof(DWORD));
			SAFE_DELETE(pOldArray1);
			SAFE_DELETE(pOldArray2);
			SAFE_DELETE(pOldArray3);

			// Update pointers
			g_pEXE->m_dwNumberOfDLLs = dwNewSize;
			g_pEXE->m_pDLLIndexArray = (DWORD*)pNewArray1;
			g_pEXE->m_pDLLFilenameArray = (DWORD*)pNewArray2;
			g_pEXE->m_pDLLLoadedAlreadyArray = (DWORD*)pNewArray3;
		}

		// Start Adding From this index
		dwDLLIndex=dwOldSize;
	}

	// Always make sure dbprocore.dll is first
	for(DWORD pass=0; pass<2; pass++)
	{
		CDataTable* pStringEntry = g_pDLLTable->GetNext();
		while(pStringEntry)
		{
			if(pStringEntry->GetAddedToEXEData()==false)
			{
				// String golding DLL Name
				LPSTR pStringData = pStringEntry->GetString()->GetStr();

				// Ensure it is added in right order
				bool bAddThisString=false;
				if(pass==0 && stricmp("dbprocore.dll", pStringData)==NULL) bAddThisString=true;
				if(pass==1 && stricmp("dbprocore.dll", pStringData)!=NULL) bAddThisString=true;
				if(bAddThisString)
				{
					// Add DLL to DLLEXEData
					pStringEntry->SetAddedToEXEData(true);

					// Ensure it is uniqur in list
					bool bNotGot=true;
					for(DWORD n=0; n<g_pEXE->m_dwNumberOfDLLs; n++)
					{
						LPSTR pCompareWith=(LPSTR)(g_pEXE->m_pDLLFilenameArray[n]);
						if(pCompareWith)
							if(stricmp(pCompareWith, pStringData)==NULL) bNotGot=false;
					}

					// If not got, add to list
					if(bNotGot)
					{
						// Create Dynamic String
						char* pDynamicString = new char[strlen(pStringData)+1];
						strcpy(pDynamicString, pStringData);

						// EXEData from Table
						g_pEXE->m_pDLLIndexArray[dwDLLIndex]=pStringEntry->GetIndex();
						g_pEXE->m_pDLLFilenameArray[dwDLLIndex]=(DWORD)pDynamicString;
						dwDLLIndex++;
					}
				}
			}
			pStringEntry=pStringEntry->GetNext();
		}
	}

	// Complete
	return true;
}

bool CASMWriter::UpdateCommandData(void)
{
	db3::CProfile<> prof("CASMWriter::UpdateCommandData");

	if(g_pEXE->m_pCommandDLLIdArray==NULL)
	{
		// Create Array(s)
		DWORD dwNewSize = g_pStatementList->GetCommandIndexCounter();
		LPSTR pNewArray1 = (LPSTR)g_pEXE->CreateArray(dwNewSize);
		LPSTR pNewArray2 = (LPSTR)g_pEXE->CreateArray(dwNewSize);

		// Update pointers
		g_pEXE->m_dwNumberOfCommands = dwNewSize;
		g_pEXE->m_pCommandDLLIdArray = (DWORD*)pNewArray1;
		g_pEXE->m_pCommandDLLCallArray = (DWORD*)pNewArray2;
	}
	else
	{
		// Add To Array(s)
		DWORD dwOldSize = g_pEXE->m_dwNumberOfCommands;
		LPSTR pOldArray1 = (LPSTR)g_pEXE->m_pCommandDLLIdArray;
		LPSTR pOldArray2 = (LPSTR)g_pEXE->m_pCommandDLLCallArray;
		DWORD dwNewSize = g_pStatementList->GetCommandIndexCounter();
		LPSTR pNewArray1 = (LPSTR)g_pEXE->CreateArray(dwNewSize);
		LPSTR pNewArray2 = (LPSTR)g_pEXE->CreateArray(dwNewSize);

		// Fill New Array with Old+New, then delete Old
		memcpy(pNewArray1, pOldArray1, dwOldSize*sizeof(DWORD));
		memcpy(pNewArray2, pOldArray2, dwOldSize*sizeof(DWORD));
		SAFE_DELETE(pOldArray1);
		SAFE_DELETE(pOldArray2);

		// Update pointers
		g_pEXE->m_dwNumberOfCommands = dwNewSize;
		g_pEXE->m_pCommandDLLIdArray = (DWORD*)pNewArray1;
		g_pEXE->m_pCommandDLLCallArray = (DWORD*)pNewArray2;
	}

	// Assumes commands are added in sequential order
	CDataTable* pStringEntry = g_pCommandTable->GetNext();
	for(DWORD c=0; c<g_pEXE->m_dwNumberOfCommands; c++)
	{
		if(pStringEntry->GetAddedToEXEData()==false)
		{
			// Add to EXEData
			pStringEntry->SetAddedToEXEData(true);

			// Cut String between DLL value and command string
			LPSTR pLeft = "???";
			LPSTR pRight = "???";
			if(pStringEntry)
			{
				CStr* pStr = pStringEntry->GetString();
				DWORD dwPos = pStr->FindFirstChar(',');
				pLeft = pStr->GetLeftOfPosition(dwPos);
				pRight = pStr->GetRightOfPosition(dwPos+1);
			}

			// Create Dynamic String
			char* pDynamicString = new char[strlen(pRight)+1];
			strcpy(pDynamicString, pRight);

			// EXEData from Table
			g_pEXE->m_pCommandDLLIdArray[c]=atoi(pLeft);
			g_pEXE->m_pCommandDLLCallArray[c]=(DWORD)pDynamicString;

			// Free usage
			if(pStringEntry)
			{
				SAFE_DELETE(pLeft);
				SAFE_DELETE(pRight);
			}
		}

		// Next String
		if(pStringEntry) pStringEntry = pStringEntry->GetNext();
	}

	// Complete
	return true;
}

bool CASMWriter::UpdateStringData(void)
{
	db3::CProfile<> prof("CASMWriter::UpdateStringData");

	if(g_pEXE->m_pStringsArray==NULL)
	{
		// Create Array(s)
		DWORD dwNewSize = g_pStatementList->GetStringIndexCounter();
		LPSTR pNewArray = (LPSTR)g_pEXE->CreateArray(dwNewSize);

		// Update pointers
		g_pEXE->m_dwNumberOfStrings = dwNewSize;
		g_pEXE->m_pStringsArray = (DWORD*)pNewArray;
	}
	else
	{
		// Add To Array(s)
		DWORD dwOldSize = g_pEXE->m_dwNumberOfStrings;
		LPSTR pOldArray = (LPSTR)g_pEXE->m_pStringsArray;
		DWORD dwNewSize = g_pStatementList->GetStringIndexCounter();
		LPSTR pNewArray = (LPSTR)g_pEXE->CreateArray(dwNewSize);

		// Fill New Array with Old+New, then delete Old
		memcpy(pNewArray, pOldArray, dwOldSize*sizeof(DWORD));
		SAFE_DELETE(pOldArray);

		// Update pointers
		g_pEXE->m_dwNumberOfStrings = dwNewSize;
		g_pEXE->m_pStringsArray = (DWORD*)pNewArray;
	}

	// Strings Data (assumed added in sequential order)
	CDataTable* pStringEntry = g_pStringTable->GetNext();
	for(DWORD s=0; s<g_pEXE->m_dwNumberOfStrings; s++)
	{
		if(pStringEntry->GetAddedToEXEData()==false)
		{
			// Add to EXEData
			pStringEntry->SetAddedToEXEData(true);

			// Get Data
			LPSTR pStringData=NULL;
			if(pStringEntry)
			{
				// Ensure no speech marks wrap literal string data
				CStr* pNoSpeechMarks = new CStr(pStringEntry->GetString()->GetStr());
				pNoSpeechMarks->EatSpeechMarks();
				pStringData = pNoSpeechMarks->GetStr();
			}
			else
				pStringData = "???";

			// Create Dynamic String
			char* pDynamicString = new char[strlen(pStringData)+1];
			strcpy(pDynamicString, pStringData);

			// EXEData from Table
			g_pEXE->m_pStringsArray[s]=(DWORD)pDynamicString;
		}

		// Next String
		if(pStringEntry) pStringEntry = pStringEntry->GetNext();
	}

	// Complete
	return true;
}

bool CASMWriter::UpdateDataData(void)
{
	db3::CProfile<> prof("CASMWriter::UpdateDataData");

	if(g_pEXE->m_pDataArray==NULL)
	{
		// Create Array(s)
		DWORD dwNewSize = g_pStatementList->GetDataIndexCounter();
		LPSTR pNewArray1 = (LPSTR)new char[dwNewSize*10];
		LPSTR pNewArray2 = (LPSTR)g_pEXE->CreateArray(dwNewSize);

		// Update pointers
		g_pEXE->m_dwNumberOfDataItems = dwNewSize;
		g_pEXE->m_pDataArray = pNewArray1;
		g_pEXE->m_pDataStringsArray = (DWORD*)pNewArray2;
	}
	else
	{
		// Add To Array(s)
		DWORD dwOldSize = g_pEXE->m_dwNumberOfDataItems;
		LPSTR pOldArray1 = (LPSTR)g_pEXE->m_pDataArray;
		LPSTR pOldArray2 = (LPSTR)g_pEXE->m_pDataStringsArray;
		DWORD dwNewSize = g_pStatementList->GetDataIndexCounter();
		LPSTR pNewArray1 = (LPSTR)new char[dwNewSize*10];
		LPSTR pNewArray2 = (LPSTR)g_pEXE->CreateArray(dwNewSize);

		// Fill New Array with Old+New, then delete Old
		memcpy(pNewArray1, pOldArray1, dwOldSize*10);
		memcpy(pNewArray2, pOldArray2, dwOldSize*sizeof(DWORD));
		SAFE_DELETE(pOldArray1);
		SAFE_DELETE(pOldArray2);

		// Update pointers
		g_pEXE->m_dwNumberOfDataItems = dwNewSize;
		g_pEXE->m_pDataArray = pNewArray1;
		g_pEXE->m_pDataStringsArray = (DWORD*)pNewArray2;
	}

	// Data Data (assumed added in sequential order)
	CDataTable* pDataEntry = g_pDataTable->GetNext();
	for(DWORD d=0; d<g_pEXE->m_dwNumberOfDataItems*10; d+=10)
	{
		if(pDataEntry->GetAddedToEXEData()==false)
		{
			// Add to EXEData
			pDataEntry->SetAddedToEXEData(true);

			// Get Type of Data Item
			DWORD dwType = pDataEntry->GetType();
			g_pEXE->m_pDataArray[d+0] = (unsigned char)dwType;

			// Byte 2 is reserved
			g_pEXE->m_pDataArray[d+1] = 0;

			// Set Data Item
			if(dwType==1)
			{
				// EXEData from Table
				*(double*)&g_pEXE->m_pDataArray[d+2] = (double)pDataEntry->GetNumeric();
			}
			if(dwType==2)
			{
				// Data Item as String
				LPSTR pDataItem=NULL;
				if(pDataEntry)
					pDataItem = pDataEntry->GetString()->GetStr();
				else
					pDataItem = "???";

				// Create Dynamic String
				char* pDynamicString = new char[strlen(pDataItem)+1];
				strcpy(pDynamicString, pDataItem);

				// EXEData from Table
				DWORD dwStrIndex=d/10;
				*(DWORD*)&g_pEXE->m_pDataArray[d+2] = (DWORD)dwStrIndex;
				g_pEXE->m_pDataStringsArray[dwStrIndex]=(DWORD)pDynamicString;
			}
		}

		// Next String
		if(pDataEntry) pDataEntry = pDataEntry->GetNext();
	}

	// Complete
	return true;
}

bool CASMWriter::UpdateDynamicData(void)
{
	db3::CProfile<> prof("CASMWriter::UpdateDynamicData");

	// Complete Dynamic Array Refresh
	SAFE_DELETE(g_pEXE->m_pDynamicVarsArray);
	SAFE_DELETE(g_pEXE->m_pDynamicVarsArrayType);

	// Dynamic Variables (Strings and Arrays, etc)
	DWORD dwDynamicVarsCounter=0;
	for(short pass=0; pass<=1; pass++)
	{
		// Second pass creates array
		if(pass==1)
		{
			g_pEXE->m_pDynamicVarsArray = g_pEXE->CreateArray(g_pEXE->m_dwDynamicVarsQuantity);
			g_pEXE->m_pDynamicVarsArrayType = g_pEXE->CreateArray(g_pEXE->m_dwDynamicVarsQuantity);
		}

		// Scan all variables for dyanmic ones
		dwDynamicVarsCounter=0;
		CVarTable* pVarEntry = g_pVarTable;
		while(pVarEntry)
		{
			// Only main program vars
			if(strcmp(pVarEntry->GetVarScope()->GetStr(),"")==NULL)
			{
				if(pVarEntry->GetArrFlag()==1 || pVarEntry->GetVarTypeValue()==3)
				{
					if(pass==1)
					{
						g_pEXE->m_pDynamicVarsArray[dwDynamicVarsCounter]=pVarEntry->GetOffsetValue();
						g_pEXE->m_pDynamicVarsArrayType[dwDynamicVarsCounter]=pVarEntry->GetArrFlag();
					}
					dwDynamicVarsCounter++;
				}
			}
			pVarEntry=pVarEntry->GetNext();
		}

		// Now I know how many are dynamic..second pass to come
		if(pass==0)
		{
			g_pEXE->m_dwDynamicVarsQuantity = dwDynamicVarsCounter;
		}
	}
	
	// Complete
	return true;
}


void CASMWriter::UpdateStructurePatternDataRec ( LPSTR pPattern, CDeclaration* pDecMain )
{
	while(pDecMain)
	{
		// If find string item, add offset to array
		LPSTR pTypeLetter = "-";
		LPSTR pFullString = pDecMain->GetType()->GetStr();
		if ( stricmp ( "integer", pFullString )==NULL )			pTypeLetter = "L";
		if ( stricmp ( "float", pFullString )==NULL )			pTypeLetter = "F";
		if ( stricmp ( "string", pFullString )==NULL )			pTypeLetter = "S";
		if ( stricmp ( "boolean", pFullString )==NULL )			pTypeLetter = "B";
		if ( stricmp ( "byte", pFullString )==NULL )			pTypeLetter = "Y";
		if ( stricmp ( "word", pFullString )==NULL )			pTypeLetter = "W";
		if ( stricmp ( "dword", pFullString )==NULL )			pTypeLetter = "D";
		if ( stricmp ( "double float", pFullString )==NULL )	pTypeLetter = "O";
		if ( stricmp ( "double integer", pFullString )==NULL )	pTypeLetter = "R";

		// If another type, traverse that too
		CStructTable* pStruct = g_pStructTable->DoesTypeEvenExist(pDecMain->GetType()->GetStr());
		if(pStruct)
		{
			CDeclaration* pDeeperDec = pStruct->GetDecChain();
			if(pDeeperDec)
			{
				UpdateStructurePatternDataRec ( pPattern, pDeeperDec );
				pTypeLetter = "";
			}
		}

		// commit data type to pattern string
		strcat ( pPattern, pTypeLetter );

		// Next dec item
		pDecMain = pDecMain->GetNext();
	}
}

bool CASMWriter::UpdateStructurePatternData(void)
{
	db3::CProfile<> prof("CASMWriter::UpdateStructurePatternData");

	// go through all structures and create a full datatype pattern for each
	g_pEXE->m_dwUsertypeStringPatternQuantity = 0;
	SAFE_DELETE(g_pEXE->m_pUsertypeStringPatternArray);

	// first pass count structures, second pass create and fill arrays
	for(short pass=0; pass<=1; pass++)
	{
		// Second pass creates array
		if(pass==1)
		{
			if ( g_pEXE->m_dwUsertypeStringPatternQuantity > 0 )
			{
				g_pEXE->m_pUsertypeStringPatternArray = new char[g_pEXE->m_dwUsertypeStringPatternQuantity];
				strcpy ( (LPSTR)g_pEXE->m_pUsertypeStringPatternArray, "" );
			}
		}

		// Scan all variables for dyanmic ones
		DWORD dwCounter=0;
		CStructTable* pEntry = g_pStructTable;
		while(pEntry)
		{
			if(pEntry->GetDecChain())
			{
				char pPattern[512];
				LPSTR pTypeName = pEntry->GetTypeName()->GetStr();
				strcpy ( pPattern, pTypeName );
				strcat ( pPattern, ":" );
				char num[32];
				wsprintf ( num, "%d", g_pStructTable->FindIndex(pTypeName) );
				strcat ( pPattern, num );
				strcat ( pPattern, ":" );
				UpdateStructurePatternDataRec ( pPattern, pEntry->GetDecChain() );
				strcat ( pPattern, ":" );
				if(pass==1) strcat ( (LPSTR)g_pEXE->m_pUsertypeStringPatternArray, pPattern );
				dwCounter+=strlen(pPattern)+1;
			}
			pEntry=pEntry->GetNext();
		}

		// Now I know how many
		if(pass==0) g_pEXE->m_dwUsertypeStringPatternQuantity = dwCounter;
	}
	
	// Complete
	return true;
}

LPSTR CASMWriter::MakeVarDataForTransfer(DWORD *pdwDataSize)
{
	LPSTR pData = NULL;
	DWORD dwSizeOfData = 0;
	if(g_pVarTable)
	{
		// Gather Var Data
		dwSizeOfData = 4;
		DWORD dwNumberOfVariables = 0;
		CVarTable* pCurrent = g_pVarTable;
		while(pCurrent)
		{
			if(pCurrent->GetVarName()->GetChar(0)!='$' && pCurrent->GetVarScope()->Length()==0)
			{
				dwSizeOfData+=12;
				dwSizeOfData+=pCurrent->GetVarName()->Length();
				dwNumberOfVariables++;
			}
			pCurrent=pCurrent->GetNext();
		}

		// Create Data Space
		pData = new char[dwSizeOfData];

		// Fill With VarData
		LPSTR pPtr = pData;
		pCurrent = g_pVarTable;
		*((DWORD*)pPtr) = dwNumberOfVariables; pPtr+=4;
		while(pCurrent)
		{
			if(pCurrent->GetVarName()->GetChar(0)!='$' && pCurrent->GetVarScope()->Length()==0)
			{
				*((DWORD*)pPtr) = pCurrent->GetVarTypeValue(); pPtr+=4;
				*((DWORD*)pPtr) = pCurrent->GetOffsetValue(); pPtr+=4;
				DWORD dwLengthOfString=pCurrent->GetVarName()->Length();
				*((DWORD*)pPtr) = dwLengthOfString; pPtr+=4;
				for(DWORD t=0; t<dwLengthOfString; t++)
					*(pPtr++) = pCurrent->GetVarName()->GetChar(t);
			}

			// Next variable
			pCurrent=pCurrent->GetNext();
		}
	}

	// Return Data Address
	*pdwDataSize=dwSizeOfData;
	return pData;
}

LPSTR CASMWriter::MakeVarValuesForTransfer(DWORD *pdwDataSize)
{
	LPSTR pData = NULL;
	DWORD dwSizeOfData = 0;
	if(g_pVarTable)
	{
		// Gather Var Data
		dwSizeOfData = 4+g_dwVarSpaceSizeInUse;
		CVarTable* pCurrent = g_pVarTable;
		while(pCurrent)
		{
			if(pCurrent->GetVarName()->GetChar(0)!='$' && pCurrent->GetVarScope()->Length()==0)
			{
				if(pCurrent->GetVarTypeValue()==3)
				{
					// Per String header size
					dwSizeOfData+=9;

					// String Size
					DWORD dwLengthOfString=0;
					DWORD dwOffset=pCurrent->GetOffsetValue();
					LPSTR pStringInMemory=(LPSTR)*(DWORD*)(g_pVarSpaceAddressInUse+dwOffset);
					if(pStringInMemory) dwLengthOfString=strlen(pStringInMemory);
					dwSizeOfData+=dwLengthOfString;
				}
			}
			pCurrent=pCurrent->GetNext();
		}
		dwSizeOfData+=1;

		// Create Data Space
		pData = new char[dwSizeOfData];
		LPSTR pPtr = pData;

		// First DWORD is Size Of VarSpace Block
		*((DWORD*)pPtr) = g_dwVarSpaceSizeInUse; pPtr+=4;

		// Snapshot of varspace memory
		memcpy(pPtr, g_pVarSpaceAddressInUse, g_dwVarSpaceSizeInUse);
		pPtr+=g_dwVarSpaceSizeInUse;

		// For any strings, add chardata to data
		pCurrent = g_pVarTable;
		while(pCurrent)
		{
			if(pCurrent->GetVarName()->GetChar(0)!='$' && pCurrent->GetVarScope()->Length()==0)
			{
				if(pCurrent->GetVarTypeValue()==3)
				{
					// Flag to say another string
					*((LPSTR)pPtr) = 1; pPtr+=1;

					// Store offset to string
					DWORD dwOffset=pCurrent->GetOffsetValue();
					*((DWORD*)pPtr) = dwOffset; pPtr+=4;

					// Locate string if any in memory at offset position
					LPSTR pStringInMemory=(LPSTR)*(DWORD*)(g_pVarSpaceAddressInUse+dwOffset);

					// Store length and contents of string in memory
					DWORD dwLengthOfString=0;
					if(pStringInMemory) dwLengthOfString=strlen(pStringInMemory);
					*((DWORD*)pPtr) = dwLengthOfString; pPtr+=4;
					for(DWORD t=0; t<dwLengthOfString; t++)
						*(pPtr++) = pStringInMemory[t];
				}
			}
			pCurrent=pCurrent->GetNext();
		}
		*((LPSTR)pPtr) = 0;
		pPtr+=1;
	}

	// Return Data Address
	*pdwDataSize=dwSizeOfData;
	return pData;
}

void CASMWriter::TraverseDecForPattern(DWORD dwBaseOffset, short pass, DWORD* dwPatternArrayCounter, DWORD* dwSizeOfUserTypePattern, CDeclaration* pDecMain)
{
	/* old code - not used - pre 2007
	while(pDecMain)
	{
		// If find string item, add offset to array
		DWORD dwOffset=dwBaseOffset + pDecMain->GetOffset();
		if(pDecMain->GetType()->CheckChars(0,5,"STRING"))
		{
			// Store string offset in pattern
			if(pass==1) g_pEXE->m_pUsertypeStringPatternArray[*dwPatternArrayCounter] = dwOffset;
			(*dwPatternArrayCounter)++;

			// Increase size of this pattern
			(*dwSizeOfUserTypePattern)++;
		}

		// If another type, traverse that too
		CStructTable* pStruct = g_pStructTable->DoesTypeEvenExist(pDecMain->GetType()->GetStr());
		if(pStruct)
		{
			CDeclaration* pDeeperDec = pStruct->GetDecChain();
			if(pDeeperDec)
			{
				TraverseDecForPattern(dwOffset, pass, dwPatternArrayCounter, dwSizeOfUserTypePattern, pDeeperDec);
			}
		}

		// Next dec item
		pDecMain = pDecMain->GetNext();
	}
	*/
}

LRESULT CASMWriter::SendDataToDebugger(int iType, LPSTR pData, DWORD dwDataSize)
{
	LRESULT lResult=0;

	// Create Virtual File for Transfer
	HANDLE hFileMap = CreateFileMapping((HANDLE)0xFFFFFFFF,NULL,PAGE_READWRITE,0,dwDataSize+4,"DBPRODEBUGGERMESSAGE");
	if(hFileMap)
	{
		LPVOID lpVoid = MapViewOfFile(hFileMap,FILE_MAP_WRITE,0,0,dwDataSize+4);
		if(lpVoid)
		{
			// Copy to Virtual File
			*(DWORD*)lpVoid = dwDataSize;
			memcpy((LPSTR)lpVoid+4, pData, dwDataSize);

			// Find Debugger to send to
			HWND hWnd = FindWindow(NULL,"DBProDebugger");
			if(hWnd)
			{
				// Found - transmit
				lResult = SendMessage(hWnd, WM_USER+10, iType, 0);
			}

			// Release virtual file
			UnmapViewOfFile(lpVoid);
		}
		CloseHandle(hFileMap);
	}

	// May have result
	return lResult;
}

void CASMWriter::GetDataFromDebugger(int iType, LPSTR* pData, DWORD* dwDataSize)
{
	// Wait for text to arrive by message (3 second timeout)
	DWORD dwTime=timeGetTime();
	while(g_DebugInfo.MessageArrived()==false)
		if(timeGetTime()>dwTime+3000)
			break;

	// Act on any data received
	switch(iType)
	{
		case 51 : // CLI Text Message
			if(g_DebugInfo.GetCLIText())
			{
				*pData = new char[g_DebugInfo.GetCLISize()+2];
				ZeroMemory(*pData, g_DebugInfo.GetCLISize()+2);
				strcpy(*pData, g_DebugInfo.GetCLIText());
				*dwDataSize=strlen(*pData)+1;
			}
			else
			{
				*pData = new char[5];
				ZeroMemory(*pData, 5);
				*dwDataSize=4;
			}
			break;

		default: // Empty Text Data
			*pData = new char[5];
			ZeroMemory(*pData, 5);
			*dwDataSize=4;
			break;
	}

	// Message Processed
	g_DebugInfo.SetMessageArrived(false);
}

void CASMWriter::FreeMachineBlock(void)
{
	// Clear Writer Data
	SAFE_FREE(m_pProgramStart);
	SAFE_DELETE(m_pProgramRefs);
	if(m_pProgramRefLabel)
	{
		for(DWORD n=0; n<m_dwRefBufferSize; n++)
		{
			if(*(m_pProgramRefLabel+n)!=NULL)
			{
				delete (char*)*(m_pProgramRefLabel+n);
				*(m_pProgramRefLabel+n)=NULL;
			}
		}
		SAFE_DELETE(m_pProgramRefLabel);
	}
}

void CASMWriter::FreeAll(void)
{
	// Clear ASM Code Database
	for(DWORD i=0; i<ASMMAXCOUNT; i++)
	{
		SAFE_DELETE(m_pASMDebugString[i]);
	}
}

DWORD CASMWriter::GetBytePosOfLastInstruction(void)
{
	DWORD MCBBytePos = m_pMachineBlock-m_pProgramStart;
	return MCBBytePos;
}

DWORD CASMWriter::DetermineASMCall(DWORD dwASMCodeAsAByte, DWORD dwTypeValue)
{
	// First Determine SizeCode From Type
	DWORD dwAddressSizeCode=0;
	switch(dwTypeValue)
	{
		case 4 :	dwAddressSizeCode=0;	break;	// BYTE
		case 5 :	dwAddressSizeCode=0;	break;	// BYTE
		case 6 :	dwAddressSizeCode=1;	break;	// WORD
		case 8 :	dwAddressSizeCode=3;	break;	// DWORDx2
		case 9 :	dwAddressSizeCode=3;	break;	// DWORDx2
		default :	dwAddressSizeCode=2;	break;	// DWORD
	}

	// Then Determine Call From Size
	return dwASMCodeAsAByte+dwAddressSizeCode;
}

DWORD CASMWriter::DetermineASMCallForREL(DWORD dwASMCodeAsAByte, DWORD dwTypeValue)
{
	DWORD dwAddressSizeCode=0;
	switch(dwTypeValue)
	{
		case 104 :	dwAddressSizeCode=0;	break;	// RELATIVE ADDRESS TO A BYTE
		case 105 :	dwAddressSizeCode=0;	break;	// RELATIVE ADDRESS TO A BYTE
		case 106 :	dwAddressSizeCode=1;	break;	// RELATIVE ADDRESS TO A WORD
		case 101 :	
		case 102 :	
		case 103 :	
		case 107 :	dwAddressSizeCode=2;	break;	// RELATIVE ADDRESS TO A DWORD
		case 108 :	dwAddressSizeCode=3;	break;	// RELATIVE ADDRESS TO A DWORDx2
		case 109 :	dwAddressSizeCode=3;	break;	// RELATIVE ADDRESS TO A DWORDx2
		default:	dwAddressSizeCode=2;	break;	// RELATIVE ADDRESS TO A DWORD
	}
	return dwASMCodeAsAByte+dwAddressSizeCode;
}

bool CASMWriter::WriteASMCall(DWORD dwLine, LPSTR pDLL, LPSTR pDecoratedName)
{
	CStr CommandString("");
	CommandString.SetText("[");
	CommandString.AddText(pDLL);
	CommandString.AddText(",");
	CommandString.AddText(pDecoratedName);
	return g_pASMWriter->WriteASMTaskCoreP1(dwLine, ASMTASK_CALL, &CommandString, 1);
}

DWORD CASMWriter::DetMode(CStr* pP, DWORD dwPType, DWORD dwPOffset)
{
	if(pP)
	{
		if(pP->GetChar(0)=='@')
		{
			if(pP->GetChar(1)==':')
			{
				if(dwPType==1001)
					return PMODE_EBP;
				else
				{
					if((dwPType>100 && dwPType<=199) || dwPType==1101)
						return PMODE_EBPARR;
					else
					{
						if(dwPType>200 && dwPType<=299)
							return PMODE_EBPREL;
						else
						{
							if(dwPOffset>0)
								return PMODE_EBPOFF;
							else
								return PMODE_EBP;
						}
					}
				}
			}
			else
			{
				if(dwPType==1001)
					return PMODE_MEM;
				else
				{
					if((dwPType>100 && dwPType<=199) || dwPType==1101)
						return PMODE_MEMARR;
					else
					{
						if(dwPType>200 && dwPType<=299)
							return PMODE_MEMREL;
						else
						{
							if(dwPOffset>0)
								return PMODE_MEMOFF;
							else
								return PMODE_MEM;
						}
					}
				}
			}
		}
		else
			return PMODE_IMM;
	}
	return PMODE_NONE;
}

void CASMWriter::CalculateArrayOffsetInEBX ( CStr* pPIndex )
{
	// Locate Array Element (EBX)
	if(pPIndex)
	{
		// If empty, it must use internal index (list system)
		if(pPIndex->Length()==0)
		{
			// Internal unified list index
			WriteASMLine(ASM_MOVEBXEAXOFF4, "-4");
		}
		else
		{
			/* moved to mathop to calc-array-offset
			// Always have first dimension D1
			WriteASMLine(ASM_POPEBX, "");

			// Loop through subsequent dimensions D2-D9
			int iCount=0;
			int iCountMax = (DWORD)(pNumSubscriptsOnStack->GetValue())-1;
			while(iCount<iCountMax)
			{
				CStr* pValue = new CStr("");
				int iHeaderOffset = (-56)+(iCount*4);
				pValue->SetNumericText(iHeaderOffset);
				WriteASMLine(ASM_POPEDX, "");
				WriteASMLine(ASM_MULEDXEAXOFF4, pValue->GetStr());
				WriteASMLine(ASM_ADDEBXEDX4, "");
				SAFE_DELETE(pValue);
				iCount++;
			}
			*/
			if(pPIndex->GetChar(0)=='@')
			{
				if(pPIndex->GetChar(1)==':')
				{
					DWORD dwCorrectASMCode=DetermineASMCall(ASM_MOVEBXEBP1,1);
					WriteASMLine(dwCorrectASMCode, (pPIndex->GetStr()+2));	
				}
				else
				{
					WriteASMLine(ASM_MOVEBXMEM4, pPIndex->GetStr());
				}
			}
			else
			{
				// Normal specified index
				WriteASMLine(ASM_MOVEBXIMM4, pPIndex->GetStr());
			}
		}

		// If Array Check Active
		if(GetArrayCheckFlag())
		{
			// Perform array bounds (for user subscripts) check and leap over
			WriteASMLine(ASM_PUSHEDX, "");//added 300305 - to stop EDX overwritten as it can store DOUBLE compoent!
			WriteASMLine(ASM_MOVEDXEAXOFF4, "-16");
			WriteASMLine(ASM_CMPGREEDXEBX, "");
			WriteASMLine(ASM_POPEDX, "");//added 300305 - to stop EDX overwritten as it can store DOUBLE compoent!
			WriteASMLeapMarkerJump(ASM_JGE, 2);
			WriteASMLine(ASM_CMPEBX4, "-1");
			WriteASMLeapMarkerJump(ASM_JLE, 3);
		}
	}
	else
		WriteASMLine(ASM_MOVEBXIMM4, "0");
}

void CASMWriter::WriteASMARRtoEAX(DWORD dwMode, CStr* pP, CStr* pOffset, DWORD dwPType, DWORD dwPOffset)
{
	// Temp vars
	DWORD dwCorrectASMCode1=0;
	DWORD dwCorrectASMCode2=0;

	// Create offset strings
	CStr* pOffset1Str = new CStr("");
	pOffset1Str->SetNumericText( dwPOffset );
	CStr* pOffset2Str = new CStr("");
	pOffset2Str->SetNumericText( dwPOffset + 4 );

	// If Array Check Active
	if(GetArrayCheckFlag())
	{
		// Make Sure Array Exists
		WriteASMLine(ASM_CMPEAX4, "0");

		// Leap Marker OpCode
		WriteASMLeapMarkerJump(ASM_JE, 1);
	}

	// Locate Array Element
	CalculateArrayOffsetInEBX(pOffset);

	WriteASMLine(ASM_MOVEAXSIB4, "");

	// Read From Array
	switch(dwPType-100)
	{
		case 1001:	// UDT ARRAY - Place EAX at Location of UDT Array Element
					// EAX already pointing to array data index, just need to add dataoffset (.field part)
					WriteASMLine(ASM_ADDEAX4, pOffset1Str->GetStr());
					
// leefix - 170403 - odd code, writes to ECX twice - replaced with above
//					dwCorrectASMCode1=DetermineASMCallForREL(ASM_MOVECXEAXOFF1,dwPType);
//					WriteASMLine(dwCorrectASMCode1, pOffset1Str->GetStr());
//					WriteASMLine(ASM_MOVECXEAX4, "");
					break;

		case 8:		// MEMARR to ST08
					WriteASMLine(ASM_MOVST0EAX8, pOffset1Str->GetStr());
					break;

		case 9:		// MEMARR to EAX/EDX
					WriteASMLine(ASM_MOVECXEAXOFF4, pOffset2Str->GetStr());
					WriteASMLine(ASM_MOVEDXECX4, "");
					WriteASMLine(ASM_MOVECXEAXOFF4, pOffset1Str->GetStr());
					WriteASMLine(ASM_MOVEAXECX4, "");
					break;

		default:	// MEMARR to EAX
					dwCorrectASMCode1=DetermineASMCallForREL(ASM_MOVECXEAXOFF1,dwPType);
					dwCorrectASMCode2=DetermineASMCallForREL(ASM_MOVEAXECX1,dwPType);
					WriteASMLine(dwCorrectASMCode1, pOffset1Str->GetStr());
					WriteASMLine(dwCorrectASMCode2, "");
					break;
	}

	// If Array Check Active
	if(GetArrayCheckFlag())
	{
		// leeadd - 300305 - Leap size depends on (runtimeerror hook activity)
		int iLeapSize = 10; //10 is from this code below (exc. rutimeerrorhook)
		if(g_DebugInfo.DebugModeOn())
			iLeapSize+=51;
		else
			iLeapSize+=26;

		// leap over error handling code for arrays (10 bytes)
		char leapstring[32];
		itoa ( iLeapSize, leapstring, 10 );
		WriteASMLine(ASM_JMP, leapstring);

		// Complete Leap Marker (so we jump here)
		WriteASMLeapMarkerEnd(1);
		WriteASMLeapMarkerEnd(2);
		WriteASMLeapMarkerEnd(3);

		// the array error (1=not there/2+3-out of bounds), else continue after this (+10)
		WriteASMLine2(ASM_MOVMEMIMM4, "@$_ERR_", "118");

		// leefix - 300305 - ensure array errors reported immediately
		WriteASMTaskCoreP2(m_dwLineNumber, ASMTASK_RUNTIMEERRORHOOK, NULL, 0, NULL, 0);
	}

	// Free offset strings
	SAFE_DELETE(pOffset1Str);
	SAFE_DELETE(pOffset2Str);
}

void CASMWriter::WriteASMXtoEAX(DWORD dwMode, CStr* pP, CStr* pPIndex, DWORD dwPType, DWORD dwPOffset)
{
	DWORD dwCorrectASMCode = 0;
	CStr* pDoubleStr = NULL;
	int iOffset = 0;
	DWORD dwDWORDRep=0;
	DWORD dwExtraDWORD=0;
	CStr* pDWORD1Str=NULL;
	CStr* pDWORD2Str=NULL;
	CStr* pOffset1Str=NULL;
	CStr* pOffset2Str=NULL;
	CStr* pTemp1Str=NULL;
	CStr* pTemp2Str=NULL;
	DWORD dwIMMSize=0;

	switch(dwMode)
	{
		case PMODE_IMM:	// IMM to EAX

			dwExtraDWORD=0;
			dwDWORDRep = pP->GetDWORDRepresentation(dwPType, &dwExtraDWORD);
			pDWORD1Str = new CStr("");
			pDWORD2Str = new CStr("");
			pDWORD1Str->SetDWORDNumericText(dwDWORDRep);
			pDWORD2Str->SetDWORDNumericText(dwExtraDWORD);
			switch(dwPType)
			{
				case 8:		// IMM to ST08
							pTemp1Str = new CStr("@$_TEMPA_");
							pTemp2Str = new CStr("@$_TEMPB_");
							WriteASMLine2IMM(ASM_MOVMEMIMM4, pTemp1Str->GetStr(), pDWORD1Str->GetStr(),2);
							WriteASMLine2IMM(ASM_MOVMEMIMM4, pTemp2Str->GetStr(), pDWORD2Str->GetStr(),2);
							WriteASMLine(ASM_MOVST0MEM8, pTemp1Str->GetStr());
							SAFE_DELETE(pTemp1Str);
							SAFE_DELETE(pTemp2Str);
							break;

				case 9:		// IMM to EAX/EDX
							WriteASMLine(ASM_MOVEDXIMM4, pDWORD2Str->GetStr());
							WriteASMLine(ASM_MOVEAXIMM4, pDWORD1Str->GetStr());
							break;

				case 3:		// IMMSTR to EAX
							WriteASMLine(ASM_MOVEAXIMM4, pP->GetStr());
							break;

				case 20:	// DATA LABEL POINTER to EAX
							WriteASMLine(ASM_MOVEAXIMM4, pP->GetStr());
							break;

				default:	// IMM to EAX
							dwCorrectASMCode=DetermineASMCall(ASM_MOVEAXIMM1,dwPType);
							dwIMMSize=dwCorrectASMCode-ASM_MOVEAXIMM1;
							WriteASMLine2IMM(dwCorrectASMCode, NULL, pDWORD1Str->GetStr(), dwIMMSize);
							break;
			}
			SAFE_DELETE(pDWORD1Str);
			SAFE_DELETE(pDWORD2Str);
			break;
			
		case PMODE_MEM:	// MEM to EAX

			pDoubleStr = new CStr("+");
			pDoubleStr->AddText(pP->GetStr());
			switch(dwPType)
			{
				case 8:		// MEM to ST08
							WriteASMLine(ASM_MOVST0MEM8, pP->GetStr());
							break;

				case 9:		// MEM to EAX/EDX
							WriteASMLine(ASM_MOVEAXMEM4, pDoubleStr->GetStr());
							WriteASMLine(ASM_MOVEDXEAX4, "");
							WriteASMLine(ASM_MOVEAXMEM4, pP->GetStr());
							break;

				default:	// MEM to EAX
							dwCorrectASMCode=DetermineASMCall(ASM_MOVEAXMEM1,dwPType);
							WriteASMLine(dwCorrectASMCode, pP->GetStr());
							break;
			}
			SAFE_DELETE(pDoubleStr);
			break;

		case PMODE_MEMOFF:	// MEMOFF to EAX

			pOffset1Str = new CStr("");
			pOffset2Str = new CStr("");
			pOffset1Str->SetDWORDNumericText(dwPOffset);
			pOffset2Str->SetDWORDNumericText(dwPOffset+4);
			WriteASMLine(ASM_MOVECXIMM4, pP->GetStr());
			switch(dwPType)
			{
				case 8:		// ST08 to MEM
							WriteASMLine(ASM_MOVST0ECXOFF8, pOffset1Str->GetStr());
							break;

				case 9:		// EAX/EDX to MEM
							WriteASMLine(ASM_MOVEAXECXOFF4, pOffset2Str->GetStr());
							WriteASMLine(ASM_MOVEDXEAX4, "");
							WriteASMLine(ASM_MOVEAXECXOFF4, pOffset1Str->GetStr());
							break;
	
				default:	// EAX to MEM
							dwCorrectASMCode=DetermineASMCall(ASM_MOVEAXECXOFF1,dwPType);
							WriteASMLine(dwCorrectASMCode, pOffset1Str->GetStr());
							break;
			}
			SAFE_DELETE(pOffset1Str);
			SAFE_DELETE(pOffset2Str);
			break;

		case PMODE_EBP:	// EBP to EAX

			pDoubleStr = new CStr((pP->GetStr()+2));
			iOffset=(int)pDoubleStr->GetValue();
			//if(iOffset<0) iOffset+=4;
			iOffset+=4;//leefix-230603-double must +4!
			pDoubleStr->SetNumericText(iOffset);
			switch(dwPType)
			{
				case 8:		// EBP to ST08
							WriteASMLine(ASM_MOVST0EBP8, (pP->GetStr()+2));
							break;

				case 9:		// EBP to EAX/EDX
							WriteASMLine(ASM_MOVEAXEBP4, pDoubleStr->GetStr());
							WriteASMLine(ASM_MOVEDXEAX4, "");
							WriteASMLine(ASM_MOVEAXEBP4, (pP->GetStr()+2));
							break;

				default:	// EBP to EAX
							dwCorrectASMCode=DetermineASMCall(ASM_MOVEAXEBP1,dwPType);
							WriteASMLine(dwCorrectASMCode, (pP->GetStr()+2));	
							break;
			}
			SAFE_DELETE(pDoubleStr);
			break;

		case PMODE_EBPOFF:	// EBP-OFFSET to EAX

			pOffset2Str = new CStr((pP->GetStr()+2));
			iOffset=(int)pOffset2Str->GetValue();
			pOffset1Str = new CStr("");
			pOffset1Str->SetNumericText( iOffset + dwPOffset );
			//if(iOffset<0) iOffset+=4;
			iOffset+=4;//leefix-230603-double must +4!
			pOffset2Str->SetNumericText( iOffset + dwPOffset );
			switch(dwPType)
			{
				case 8:		// EBP to ST08
							WriteASMLine(ASM_MOVST0EBP8, pOffset1Str->GetStr());
							break;

				case 9:		// EBP to EAX/EDX
							WriteASMLine(ASM_MOVEAXEBP4, pOffset2Str->GetStr());
							WriteASMLine(ASM_MOVEDXEAX4, "");
							WriteASMLine(ASM_MOVEAXEBP4, pOffset1Str->GetStr());
							break;

				default:	// EBP to EAX
							dwCorrectASMCode=DetermineASMCall(ASM_MOVEAXEBP1,dwPType);
							WriteASMLine(dwCorrectASMCode, pOffset1Str->GetStr());	
							break;
			}
			SAFE_DELETE(pOffset1Str);
			SAFE_DELETE(pOffset2Str);
			break;

		case PMODE_MEMARR:	// MEMARR to EAX

			// Find Array location
			WriteASMLine(ASM_MOVEAXMEM4, pP->GetStr());

			// Copy Array Data to EAX
			WriteASMARRtoEAX(dwMode, pP, pPIndex, dwPType, dwPOffset);

			break;

		case PMODE_EBPARR:	// EBPARR to EAX

			// Find Array location
			WriteASMLine(ASM_MOVEAXEBP4, (pP->GetStr()+2));

			// Copy Array Data to EAX
			WriteASMARRtoEAX(dwMode, pP, pPIndex, dwPType, dwPOffset);

			break;

		case PMODE_MEMREL:	// [MEM] to EAX

			pDoubleStr = new CStr("+");
			pDoubleStr->AddText(pP->GetStr());
			switch(dwPType)
			{
				case 8:		// MEM to ST08
							break;

				case 9:		// MEM to EAX/EDX
							break;

				default:	// MEM to EAX
							dwCorrectASMCode=DetermineASMCall(ASM_MOVEAXMEM1,dwPType);
							WriteASMLine(dwCorrectASMCode, pP->GetStr());
							// leefix - 280305 - passing the memaddr as the offset is very odd,
							// changed it to use the actual data ofset passed in, possible typo
							dwCorrectASMCode=DetermineASMCall(ASM_MOVEAXEAXREL1,dwPType);
							WriteASMLine(dwCorrectASMCode, "");
							break;
			}
			SAFE_DELETE(pDoubleStr);
			break;

		case PMODE_EBPREL:	// [MEM] to EAX (that is, value pointed by EBP+x to EAX)

			switch(dwPType)
			{
				case 8:		// MEM to ST08
							break;

				case 9:		// MEM to EAX/EDX
							break;

				default:	// MEM to EAX
							dwCorrectASMCode=DetermineASMCall(ASM_MOVEAXEBP1,dwPType);
							WriteASMLine(dwCorrectASMCode, (pP->GetStr()+2));
							dwCorrectASMCode=DetermineASMCall(ASM_MOVEAXEAXREL1,dwPType);
// 280305 - as above fix	WriteASMLine(dwCorrectASMCode, pP->GetStr());
							WriteASMLine(dwCorrectASMCode, "");
							break;
			}
			break;
	}
}

void CASMWriter::WriteASMEAXtoARR(DWORD dwMode, CStr* pP, CStr* pOffset, DWORD dwPType, DWORD dwPOffset)
{
	// Temp vars
	DWORD dwCorrectASMCode=0;

	// Calculate Offset Strings
	CStr* pOffset1Str = new CStr("");
	pOffset1Str->SetNumericText( dwPOffset );
	CStr* pOffset2Str = new CStr("");
	pOffset2Str->SetNumericText( dwPOffset+4 );

	// If Array Check Active
	if(GetArrayCheckFlag())
	{
		// Make Sure Array Exists
		WriteASMLine(ASM_CMPEAX4, "0");

		// Leap Marker OpCode
		WriteASMLeapMarkerJump(ASM_JE, 1);
	}

	// Locate Array Element (EBX)
	CalculateArrayOffsetInEBX(pOffset);

	// Perform extraction of data to EAX
	WriteASMLine(ASM_MOVEAXSIB4, "");

	// Write To Element
	switch(dwPType-100)
	{
		case 1001:	// UDT ARRAY
//					dwCorrectASMCode=DetermineASMCallForREL(ASM_MOVEAXOFFECX1,dwPType);
//					WriteASMLine(dwCorrectASMCode, pOffset1Str->GetStr());
					WriteASMLine(ASM_ADDEAXECX4, "");
					break;

		case 8:		// ST08 to MEMARR
					WriteASMLine(ASM_MOVEAXST08, pOffset1Str->GetStr());
					break;

		case 9:		// EAX/EDX to MEMARR
					WriteASMLine(ASM_MOVEAXOFFECX4, pOffset1Str->GetStr());
					WriteASMLine(ASM_MOVECXEDX4, "");
					WriteASMLine(ASM_MOVEAXOFFECX4, pOffset2Str->GetStr());
					break;

		default:	// EAX to MEMARR
					dwCorrectASMCode=DetermineASMCallForREL(ASM_MOVEAXOFFECX1,dwPType);
					WriteASMLine(dwCorrectASMCode, pOffset1Str->GetStr());
					break;
	}

	// If Array Check Active
	if(GetArrayCheckFlag())
	{
		// leeadd - 300305 - Leap size depends on (runtimeerror hook activity)
		int iLeapSize = 10; //10 is from this code below (exc. rutimeerrorhook)
		if(g_DebugInfo.DebugModeOn())
			iLeapSize+=51;
		else
			iLeapSize+=26;

		// leap over error handling code for arrays (10 bytes)
		char leapstring[32];
		itoa ( iLeapSize, leapstring, 10 );
		WriteASMLine(ASM_JMP, leapstring);

		// Complete Leap Marker (so we jump here)
		WriteASMLeapMarkerEnd(1);
		WriteASMLeapMarkerEnd(2);
		WriteASMLeapMarkerEnd(3);

		// the array error (1=not there/2+3-out of bounds), else continue after this (+10)
		WriteASMLine2(ASM_MOVMEMIMM4, "@$_ERR_", "118");

		// leefix - 300305 - ensure array errors reported immediately
		WriteASMTaskCoreP2(m_dwLineNumber, ASMTASK_RUNTIMEERRORHOOK, NULL, 0, NULL, 0);
	}

	// Free offset strings
	SAFE_DELETE(pOffset1Str);
	SAFE_DELETE(pOffset2Str);
}

void CASMWriter::WriteASMEAXtoX(DWORD dwMode, CStr* pP, CStr* pPIndex, DWORD dwPType, DWORD dwPOffset)
{
	DWORD dwCorrectASMCode=0;
	CStr* pDoubleStr=NULL;
	CStr* pOffset1Str=NULL;
	CStr* pOffset2Str=NULL;
	CStr* pTemp1Str=NULL;
	CStr* pTemp2Str=NULL;
	int iOffset=0;

	switch(dwMode)
	{
		case PMODE_MEM:		// EAX to MEM
			pDoubleStr = new CStr("+");
			pDoubleStr->AddText(pP->GetStr());
			switch(dwPType)
			{
				case 8:		// ST08 to MEM
							WriteASMLine(ASM_MOVMEMST08, pP->GetStr());
							break;

				case 9:		// EAX/EDX to MEM
							WriteASMLine(ASM_MOVMEMEAX4, pP->GetStr());
							WriteASMLine(ASM_MOVEAXEDX4, "");
							WriteASMLine(ASM_MOVMEMEAX4, pDoubleStr->GetStr());
							break;
	
				default:	// EAX to MEM
							dwCorrectASMCode=DetermineASMCall(ASM_MOVMEMEAX1,dwPType);
							WriteASMLine(dwCorrectASMCode, pP->GetStr());
							break;
			}
			SAFE_DELETE(pDoubleStr);
			break;

		case PMODE_MEMOFF:	// EAX to MEM-OFFSET
			pOffset1Str = new CStr("");
			pOffset2Str = new CStr("");
			pOffset1Str->SetDWORDNumericText(dwPOffset);
			pOffset2Str->SetDWORDNumericText(dwPOffset+4);
			WriteASMLine(ASM_MOVECXIMM4, pP->GetStr());
			switch(dwPType)
			{
				case 8:		// ST08 to MEMFF
							WriteASMLine(ASM_MOVECXOFFST08, pOffset1Str->GetStr());
							break;

				case 9:		// EAX/EDX to MEMFF
							WriteASMLine(ASM_MOVECXOFFEAX4, pOffset1Str->GetStr());
							WriteASMLine(ASM_MOVEAXEDX4, "");
							WriteASMLine(ASM_MOVECXOFFEAX4, pOffset2Str->GetStr());
							break;
	
				default:	// EAX to MEMFF
							dwCorrectASMCode=DetermineASMCall(ASM_MOVECXOFFEAX1,dwPType);
							WriteASMLine(dwCorrectASMCode, pOffset1Str->GetStr());
							break;
			}
			SAFE_DELETE(pOffset1Str);
			SAFE_DELETE(pOffset2Str);
			break;

		case PMODE_EBP:		// EAX to EBP

			pDoubleStr = new CStr((pP->GetStr()+2));
			iOffset=(int)pDoubleStr->GetValue();
			//if(iOffset<0) iOffset+=4;
			iOffset+=4;//leefix-230603-double must +4!
			pDoubleStr->SetNumericText(iOffset);
			switch(dwPType)
			{
				case 8:		// ST08 to EBP
							WriteASMLine(ASM_MOVEBPST08, (pP->GetStr()+2));
							break;

				case 9:		// EAX/EDX to EBP
							WriteASMLine(ASM_MOVEBPEAX4, (pP->GetStr()+2));
							WriteASMLine(ASM_MOVEAXEDX4, "");
							WriteASMLine(ASM_MOVEBPEAX4, pDoubleStr->GetStr());
							break;

				default:	// EAX to EBP
							dwCorrectASMCode=DetermineASMCall(ASM_MOVEBPEAX1,dwPType);
							WriteASMLine(dwCorrectASMCode, (pP->GetStr()+2));
							break;
			}
			SAFE_DELETE(pDoubleStr);
			break;

		case PMODE_EBPOFF:	// EAX to EBP-OFFSET

			pOffset2Str = new CStr((pP->GetStr()+2));
			iOffset=(int)pOffset2Str->GetValue();
			pOffset1Str = new CStr("");
			pOffset1Str->SetNumericText( iOffset + dwPOffset );
			//if(iOffset<0) iOffset+=4;
			iOffset+=4;//leefix-230603-double must +4!
			pOffset2Str->SetNumericText( iOffset + dwPOffset );
			switch(dwPType)
			{
				case 8:		// ST08 to EBP
							WriteASMLine(ASM_MOVEBPST08, pOffset1Str->GetStr());
							break;

				case 9:		// EAX/EDX to EBP
							WriteASMLine(ASM_MOVEBPEAX4, pOffset1Str->GetStr());
							WriteASMLine(ASM_MOVEAXEDX4, "");
							WriteASMLine(ASM_MOVEBPEAX4, pOffset2Str->GetStr());
							break;

				default:	// EAX to EBP
							dwCorrectASMCode=DetermineASMCall(ASM_MOVEBPEAX1,dwPType);
							WriteASMLine(dwCorrectASMCode, pOffset1Str->GetStr());
							break;
			}
			SAFE_DELETE(pOffset1Str);
			SAFE_DELETE(pOffset2Str);
			break;

		case PMODE_MEMARR:	// EAX to MEMARR

			// Store EAX safely in ECX
			WriteASMLine(ASM_MOVECXEAX4, "");

			// Find Location of Array
			WriteASMLine(ASM_MOVEAXMEM4, pP->GetStr());

			// Store EAX in Array
			WriteASMEAXtoARR(dwMode, pP, pPIndex, dwPType, dwPOffset);

			break;

		case PMODE_EBPARR:	// EAX to EBPARR

			// Store EAX safely in ECX
			WriteASMLine(ASM_MOVECXEAX4, "");

			// Find Location of Array
			WriteASMLine(ASM_MOVEAXEBP4, (pP->GetStr()+2));

			// Store EAX in Array
			WriteASMEAXtoARR(dwMode, pP, pPIndex, dwPType, dwPOffset);

			break;

		case PMODE_STACK:	// EAX to STACK

			switch(dwPType)
			{
				case 8:		
				case 108:	// ST08 to STACK
							pTemp1Str = new CStr("@$_TEMPA_");
							pTemp2Str = new CStr("@$_TEMPB_");
							WriteASMLine(ASM_MOVMEMST08, pTemp1Str->GetStr());
							WriteASMLine(ASM_MOVEAXMEM4, pTemp2Str->GetStr());
							WriteASMLine(ASM_PUSHEAX, "");
							WriteASMLine(ASM_MOVEAXMEM4, pTemp1Str->GetStr());
							WriteASMLine(ASM_PUSHEAX, "");
							SAFE_DELETE(pTemp1Str);
							SAFE_DELETE(pTemp2Str);
							break;

				case 9:		
				case 109:	// EAX/EDX to STACK
							WriteASMLine(ASM_PUSHEDX, "");
							WriteASMLine(ASM_PUSHEAX, "");
							break;

				default:	// EAX to STACK
							WriteASMLine(ASM_PUSHEAX, "");
							break;
			}
			break;

		case PMODE_MEMREL:	// EAX to [MEM]

			// Store EAX (as using EAX to get seg-memaddress)
			WriteASMLine(ASM_MOVECXEAX4, "");

			pDoubleStr = new CStr("+");
			pDoubleStr->AddText(pP->GetStr());
			switch(dwPType)
			{
				case 8:		// ST08 to MEM
//							WriteASMLine(ASM_MOVMEMST08, pP->GetStr());
							break;

				case 9:		// EAX/EDX to MEM
//							WriteASMLine(ASM_MOVMEMEAX4, pP->GetStr());
//							WriteASMLine(ASM_MOVEAXEDX4, "");
//							WriteASMLine(ASM_MOVMEMEAX4, pDoubleStr->GetStr());
							break;
	
				default:	// EAX to MEM
							dwCorrectASMCode=DetermineASMCall(ASM_MOVEAXMEM1,dwPType);
							WriteASMLine(dwCorrectASMCode, pP->GetStr());
							dwCorrectASMCode=DetermineASMCall(ASM_MOVEAXECXREL1,dwPType);
							WriteASMLine(dwCorrectASMCode, "");
							break;
			}
			SAFE_DELETE(pDoubleStr);
			break;

		case PMODE_EBPREL: 	// EAX to [MEM] (that is, the memory pointed to by EBP+x)

			// Store EAX (as using EAX to get seg-memaddress)
			WriteASMLine(ASM_MOVECXEAX4, "");

			switch(dwPType)
			{
				case 8:		// ST08 to MEM
							break;

				case 9:		// EAX/EDX to MEM
							break;
	
				default:	// EAX to MEM
							dwCorrectASMCode=DetermineASMCall(ASM_MOVEAXEBP1,dwPType);
							WriteASMLine(dwCorrectASMCode, (pP->GetStr()+2));
							dwCorrectASMCode=DetermineASMCall(ASM_MOVEAXECXREL1,dwPType);
							WriteASMLine(dwCorrectASMCode, "");
							break;
			}
			break;
	}
}

bool CASMWriter::WriteASMTaskP1(DWORD dwLine, DWORD dwTask, CResultData* pP1Result)
{
	DWORD dwP1Type = 0;
	CStr* pP1Str = NULL;
	DWORD dwP1Offset = 0;
	CStr* pP1OffsetStr = NULL;
	if(pP1Result)
	{
		dwP1Type = pP1Result->m_dwType;
		pP1Str = pP1Result->m_pStringToken;
		dwP1Offset = pP1Result->m_dwDataOffset;
		pP1OffsetStr = pP1Result->m_pAdditionalOffset;
	}
	return WriteASMTaskCore(dwLine, dwTask, pP1Str, pP1OffsetStr, dwP1Type, dwP1Offset, NULL, NULL, 0, 0);
}

bool CASMWriter::WriteASMTaskP2(DWORD dwLine, DWORD dwTask, CResultData* pP1Result, CResultData* pP2Result)
{
	DWORD dwP1Type = 0;
	CStr* pP1Str = NULL;
	DWORD dwP1Offset = 0;
	CStr* pP1OffsetStr = NULL;
	DWORD dwP2Type = 0;
	CStr* pP2Str = NULL;
	DWORD dwP2Offset = 0;
	CStr* pP2OffsetStr = NULL;
	if(pP1Result)
	{
		dwP1Type = pP1Result->m_dwType;
		pP1Str = pP1Result->m_pStringToken;
		dwP1Offset = pP1Result->m_dwDataOffset;
		pP1OffsetStr = pP1Result->m_pAdditionalOffset;
	}
	if(pP2Result)
	{
		dwP2Type = pP2Result->m_dwType;
		pP2Str = pP2Result->m_pStringToken;
		dwP2Offset = pP2Result->m_dwDataOffset;
		pP2OffsetStr = pP2Result->m_pAdditionalOffset;
	}
	return WriteASMTaskCore(dwLine, dwTask, pP1Str, pP1OffsetStr, dwP1Type, dwP1Offset, pP2Str, pP2OffsetStr, dwP2Type, dwP2Offset);
}
bool CASMWriter::WriteASMTaskP3(DWORD dwLine, DWORD dwTask, CResultData* pP1Result, CResultData* pP2Result, CResultData* pP3Result)
{
	DWORD dwP1Type = 0;
	CStr* pP1Str = NULL;
	DWORD dwP1Offset = 0;
	CStr* pP1OffsetStr = NULL;
	DWORD dwP2Type = 0;
	CStr* pP2Str = NULL;
	DWORD dwP2Offset = 0;
	CStr* pP2OffsetStr = NULL;
	DWORD dwP3Type = 0;
	CStr* pP3Str = NULL;
	DWORD dwP3Offset = 0;
	CStr* pP3OffsetStr = NULL;
	if(pP1Result)
	{
		dwP1Type = pP1Result->m_dwType;
		pP1Str = pP1Result->m_pStringToken;
		dwP1Offset = pP1Result->m_dwDataOffset;
		pP1OffsetStr = pP1Result->m_pAdditionalOffset;
	}
	if(pP2Result)
	{
		dwP2Type = pP2Result->m_dwType;
		pP2Str = pP2Result->m_pStringToken;
		dwP2Offset = pP2Result->m_dwDataOffset;
		pP2OffsetStr = pP2Result->m_pAdditionalOffset;
	}
	if(pP3Result)
	{
		dwP3Type = pP3Result->m_dwType;
		pP3Str = pP3Result->m_pStringToken;
		dwP3Offset = pP3Result->m_dwDataOffset;
		pP3OffsetStr = pP3Result->m_pAdditionalOffset;
	}
	return WriteASMTaskCore(dwLine, dwTask, pP1Str, pP1OffsetStr, dwP1Type, dwP1Offset, pP2Str, pP2OffsetStr, dwP2Type, dwP2Offset, pP3Str, pP3OffsetStr, dwP3Type, dwP3Offset);
}

bool CASMWriter::WriteASMTaskCoreP1(DWORD dwLine, DWORD dwTask, CStr* pP1, DWORD dwP1Type)
{
	return WriteASMTaskCore(dwLine, dwTask, pP1, NULL, dwP1Type, 0, NULL, NULL, 0, 0);
}

bool CASMWriter::WriteASMTaskCoreP2(DWORD dwLine, DWORD dwTask, CStr* pP1, DWORD dwP1Type, CStr* pP2, DWORD dwP2Type)
{
	return WriteASMTaskCore(dwLine, dwTask, pP1, NULL, dwP1Type, 0, pP2, NULL, dwP2Type, 0);
}

bool CASMWriter::WriteASMTaskCore(DWORD dwLine, DWORD dwTask, CStr* pP1, CStr* pP1Off, DWORD dwP1Type, DWORD dwP1Offset, CStr* pP2, CStr* pP2Off, DWORD dwP2Type, DWORD dwP2Offset)
{
	return WriteASMTaskCore(dwLine, dwTask, pP1, pP1Off, dwP1Type, dwP1Offset, pP2, pP2Off, dwP2Type, dwP2Offset,NULL,NULL,0,0);
}

bool CASMWriter::WriteASMTaskCore(DWORD dwLine, DWORD dwTask,	CStr* pP1, CStr* pP1Off, DWORD dwP1Type, DWORD dwP1Offset,
																CStr* pP2, CStr* pP2Off, DWORD dwP2Type, DWORD dwP2Offset,
																CStr* pP3, CStr* pP3Off, DWORD dwP3Type, DWORD dwP3Offset )
{
	// Assign Line for DBM Code
	m_dwLineNumber = dwLine;

	// Determine Modes
	DWORD dwP1Mode=DetMode(pP1, dwP1Type, dwP1Offset);
	DWORD dwP2Mode=DetMode(pP2, dwP2Type, dwP2Offset);
	DWORD dwP3Mode=DetMode(pP3, dwP3Type, dwP3Offset);

	// Batches of ASM Ops to perform a single task
	if(dwTask==ASMTASK_ASSIGNTOEAX)
	{
		WriteASMXtoEAX(dwP1Mode, pP1, pP1Off, dwP1Type, dwP1Offset);
		WriteASMComment("ASSIGN X TO EAX", "", "", "");
	}
	if(dwTask==ASMTASK_ASSIGN)
	{
		if(pP2)
		{
//			bool bWrongTypes=false;//LEEFIX 041002-allow it for things like *add=a#
//			if(dwP2Type%100!=dwP1Type%100) bWrongTypes=true;
//			if(dwP1Type!=501 && dwP2Type!=501)//type501 is anytype
//			{
//				WriteASMComment("DIFFERENT ASSIGN TYPES IN X TO X", "", "", "");
//			}
//			else
//			{
				WriteASMXtoEAX(dwP2Mode, pP2, pP2Off, dwP2Type, dwP2Offset);
				WriteASMEAXtoX(dwP1Mode, pP1, pP1Off, dwP1Type, dwP1Offset);
				WriteASMComment("ASSIGN X TO X", "", "", "");
//			}
		}
		else
		{
			WriteASMEAXtoX(dwP1Mode, pP1, pP1Off, dwP1Type, dwP1Offset);
			WriteASMComment("ASSIGN EAX TO X", "", "", "");
		}
	}
	if(dwTask==ASMTASK_PUSH)
	{
		if(pP1)
		{
			if(strnicmp(pP1->GetStr(),"fs@",3)==NULL)
			{
				WriteASMComment("PUSH TO STACK", "", "", "");
			}
		}
		WriteASMXtoEAX(dwP1Mode, pP1, pP1Off, dwP1Type, dwP1Offset);
		WriteASMEAXtoX(PMODE_STACK, NULL, NULL, dwP1Type, dwP1Offset);
		WriteASMComment("PUSH TO STACK", "", "", "");
	}
	if(dwTask==ASMTASK_PUSHADDRESS)
	{
		// leefix - include dataofsfet if any
		if ( dwP1Mode==PMODE_MEM )
		{
			WriteASMLine(ASM_MOVEAXIMM4, pP1->GetStr());
		}
		if ( dwP1Mode==PMODE_EBP )
		{
			WriteASMLine(ASM_MOVEAXEBP, NULL );
			WriteASMLine(ASM_ADDEAX4, pP1->GetStr()+2 );
		}
		if ( dwP1Offset>0 )
		{
			CStr* pNum = new CStr("");
			pNum->SetNumericText(dwP1Offset);
			WriteASMLine(ASM_ADDEAX4, pNum->GetStr());
			SAFE_DELETE(pNum);
		}
		WriteASMLine(ASM_PUSHEAX, NULL);
		WriteASMComment("PUSH ADDRESS TO STACK", "", "", "");
	}
	if(dwTask==ASMTASK_CALL)
	{
		// Cut Full Param into DLL and COMMAND Strings#
		DWORD dwPos = pP1->FindFirstChar(',');
		LPSTR pDLLString = pP1->GetLeftOfPosition(dwPos);
		LPSTR pCommandString = pP1->GetRightOfPosition(dwPos);

		// Add To DLL&Command Table
		DWORD dwIndex = AddCommandToTable(pDLLString, pCommandString);

		// Write current ESP into ErrorLineDWORD if third party DLLs to check that they have not been tampered with
		bool bProtectedByESPDetection = false;
		if ( AddProtectionToSelectedDLLs ( pDLLString ) ) 
		{
			WriteASMLine(ASM_MOVEBXMEM4, "@$_SLN_");
			WriteASMLine(ASM_MOVMEMEBX4, "@$_TEMPA_");
			WriteASMLine(ASM_MOVMEMESP4, "@$_SLN_");
			bProtectedByESPDetection=true;
		}

		// Produce token Command Call token
		CStr* pTokenCommandStr = new CStr("[");
		pTokenCommandStr->AddNumericText(dwIndex);
		WriteASMLine(ASM_MOVEBXIMM4, pTokenCommandStr->GetStr());
		WriteASMLine(ASM_CALLEBX, "");

		// Comment Details
		WriteASMComment("CALL", pDLLString, pCommandString, "");

		// Restore SLN after CALL for RTE tracing
		if ( bProtectedByESPDetection ) 
		{
			WriteASMLine(ASM_MOVEBXMEM4, "@$_TEMPA_");
			WriteASMLine(ASM_MOVMEMEBX4, "@$_SLN_");
		}

		// Free usage
		SAFE_DELETE(pDLLString);
		SAFE_DELETE(pCommandString);
		SAFE_DELETE(pTokenCommandStr);

	}
	if(dwTask==ASMTASK_POPEAX)
	{
		WriteASMLine(ASM_POPEAX, "");
		WriteASMComment("POP EAX FROM STACK", "", "", "");
	}
	if(dwTask==ASMTASK_POPEBX)
	{
		WriteASMLine(ASM_POPEBX, "");
		WriteASMComment("POP EBX FROM STACK", "", "", "");
	}
	if(dwTask==ASMTASK_UNKNOWN)
	{
		WriteASMLine(ASM_UNKNOWN, "");
		WriteASMComment("NOT IMPLEMENTED", "", "", "");
	}
	if(dwTask==ASMTASK_CONDITION)
	{
		if(pP1)
		{
			if(pP1->GetChar(0)=='@')
			{
				WriteASMXtoEAX(dwP1Mode, pP1, pP1Off, dwP1Type, dwP1Offset);
				if ( GetCondToggle() ) WriteASMLine ( ASM_NOTEAX4, 0 ); // lee - 240306 - u6b4 - NOT it back for correct conditional operation
				WriteASMLine(ASM_CMPEAX4, "0");
			}
			else
			{
				// IMM
				if(pP1->GetValue()==0)
				{
					WriteASMLine(ASM_MOVEAXIMM4, "0");
					if ( GetCondToggle() ) WriteASMLine ( ASM_NOTEAX4, 0 ); // lee - 240306 - u6b4 - NOT it back for correct conditional operation
					WriteASMLine(ASM_CMPEAX4, "0");
				}
				else
				{
					WriteASMLine(ASM_MOVEAXIMM4, "1");
					if ( GetCondToggle() ) WriteASMLine ( ASM_NOTEAX4, 0 ); // lee - 240306 - u6b4 - NOT it back for correct conditional operation
					WriteASMLine(ASM_CMPEAX4, "0");
				}
			}
		}
		WriteASMComment("CONDITION COMPARE", "", "", "");
	}
	if(dwTask==ASMTASK_CONDITIONDATA)
	{
		if(pP1)
		{
			if(pP1->GetChar(0)!='@')
			{
				// Create DWORD String for IMM
				DWORD dwDWORDRep=0;
				DWORD dwExtraDWORD=0;
				CStr* pDWORD1Str=NULL;
				dwDWORDRep = pP1->GetDWORDRepresentation(dwP1Type, &dwExtraDWORD);
				pDWORD1Str = new CStr("");
				pDWORD1Str->SetDWORDNumericText(dwDWORDRep);			

				// IMM
				WriteASMLine(ASM_CMPEAX4, pDWORD1Str->GetStr());

				// Delete DWORD String
				SAFE_DELETE(pDWORD1Str);
			}
		}
		WriteASMComment("CONDITION COMPARE", "", "", "");
	}
	if(dwTask==ASMTASK_CONDJUMPNE)
	{
		WriteASMLine(ASM_JNE, pP1->GetStr());
		WriteASMComment("JUMP IF NOT EQUAL", "", "", "");
	}
	if(dwTask==ASMTASK_CONDJUMPE)
	{
		WriteASMLine(ASM_JE, pP1->GetStr());
		WriteASMComment("JUMP IF EQUAL", "", "", "");
	}
	if(dwTask==ASMTASK_JUMP)
	{
		WriteASMLine(ASM_JMP, pP1->GetStr());
		WriteASMComment("DIRECT JUMP", "", "", "");
	}
	if(dwTask==ASMTASK_JUMPSUBROUTINE)
	{
		WriteASMLine(ASM_CALLMEM, pP1->GetStr());
		WriteASMComment("DIRECT SUBCALL", "", "", "");
	}
	if(dwTask==ASMTASK_RETURN)
	{
		// Unscheduled RETs are dangerous=crash, so default is safe return
		if(1)
		{
			// Get ESP into EAX
			WriteASMLine(ASM_MOVEAXESP, "");
			// Get _ESP_ into EBX
			WriteASMLine(ASM_MOVEBXMEM4, "@$_ESP_");
			// Compare the values
			WriteASMLine(ASM_CMPEAXEBX4, NULL);
			// Jump over line that sets the flag
			WriteASMLine(ASM_JNE, "5");
			// Line that exits the program due to illegal RETURN CALL
			WriteASMLine(ASM_JMP, "$labelend");
			// oh and the actual return!
			WriteASMLine(ASM_RET, "");
			// Comment for safe return
			WriteASMComment("SAFERETURN", "", "", "");
		}
		else
		{
			// Hard RET
			WriteASMLine(ASM_RET, "");
			WriteASMComment("RETURN", "", "", "");
		}
	}
	if(dwTask==ASMTASK_PURERETURN)
	{
		// Hard RET
		WriteASMLine(ASM_RET, "");
		WriteASMComment("RETURN", "", "", "");
	}
	if(dwTask==ASMTASK_ADDESP)
	{
		WriteASMLine(ASM_ADDESP, pP1->GetStr());
		WriteASMComment("ADD ESP", "", "", "");
	}
	if(dwTask==ASMTASK_SUBESP)
	{
		WriteASMLine(ASM_SUBESP, pP1->GetStr());
		WriteASMComment("SUB ESP", "", "", "");
	}
	if(dwTask==ASMTASK_STOREESP)
	{
		WriteASMLine(ASM_MOVMEMESP4, pP1->GetStr());
		WriteASMComment("STORE STACK IN MEM", "", "", "");
	}
	if(dwTask==ASMTASK_RESTOREESP)
	{
		WriteASMLine(ASM_MOVESPMEM4, pP1->GetStr());
		WriteASMComment("RESTORE STACK FROM MEM", "", "", "");
	}
	if(dwTask==ASMTASK_PUSHEBP)
	{
		WriteASMLine(ASM_PUSHEBP, "");
		WriteASMComment("PUSH EBP", "", "", "");
	}
	if(dwTask==ASMTASK_PUSHESP)
	{
		WriteASMLine(ASM_PUSHESP, "");
		WriteASMComment("PUSH ESP", "", "", "");
	}
	if(dwTask==ASMTASK_POPEBP)
	{
		WriteASMLine(ASM_POPEBP, "");
		WriteASMComment("POP EBP", "", "", "");
	}
	if(dwTask==ASMTASK_MOVEBPESP)
	{
		WriteASMLine(ASM_MOVEBPESP, "");
		WriteASMComment("MOV EBP ESP", "", "", "");
	}
	if(dwTask==ASMTASK_MOVESPEBP)
	{
		WriteASMLine(ASM_MOVESPEBP, "");
		WriteASMComment("MOV ESP EBP", "", "", "");
	}
	if(dwTask==ASMTASK_PUSHREGISTERS)
	{
		WriteASMLine(ASM_PUSHAD, "");
		WriteASMComment("PUSH REGISTERS", "", "", "");
	}
	if(dwTask==ASMTASK_POPREGISTERS)
	{
		WriteASMLine(ASM_POPAD, "");
		WriteASMComment("POP REGISTERS", "", "", "");
	}	
	if(dwTask==ASMTASK_CLEARSTACK)
	{
		// SET EAX base
		WriteASMLine(ASM_MOVEAXESP, "");

		// BATCHES OF DWORDS
		DWORD dwTotalToClear = pP1->GetDWORDRepresentation(1, NULL);
		DWORD dwDWORDSteps = dwTotalToClear/4;
		DWORD dwDWORDLeft = dwTotalToClear-(dwDWORDSteps*4);
		if(dwDWORDSteps>0)
		{
			CStr* pIterations = new CStr();
			pIterations->SetNumericText(dwDWORDSteps);

			// SET ECX max
			WriteASMLine(ASM_MOVECXIMM4, pIterations->GetStr());

			// MOV SIB[EAX:ECX*4], 0
			WriteASMLine2IMM(ASM_MOVSIB4IMM4, NULL, "0", 2);

			// LOOP BACK
			WriteASMLine2IMM(ASM_LOOP, NULL, "-9", 0);

			SAFE_DELETE(pIterations);
		}
		if(dwDWORDLeft>0)
		{
			// Advance EAX base to skip zero'd batch areas
			CStr* pAdvance = new CStr();
			pAdvance->SetNumericText(((dwDWORDSteps+1)*4)-1);
			WriteASMLine(ASM_ADDEAX4, pAdvance->GetStr());
			SAFE_DELETE(pAdvance);

			CStr* pIterations = new CStr();
			pIterations->SetNumericText(dwDWORDLeft);

			// SET ECX max
			WriteASMLine(ASM_MOVECXIMM4, pIterations->GetStr());

			// MOV SIB[EAX:ECX*4], 0
			WriteASMLine2IMM(ASM_MOVSIB4IMM1, NULL, "0", 0);

			// LOOP BACK
			WriteASMLine2IMM(ASM_LOOP, NULL, "-6", 0);

			SAFE_DELETE(pIterations);
		}

		// Comment
		WriteASMComment("CLEAR STACK", "", "", "");
	}
	if(dwTask==ASMTASK_SETNORETURNIFESPLEAK)
	{
		// Get ESP into EAX
		WriteASMLine(ASM_MOVEAXESP, "");

		// Get _ESP_ into EBX
		WriteASMLine(ASM_MOVEBXMEM4, pP1->GetStr());

		// Compare the values
		WriteASMLine(ASM_CMPEAXEBX4, NULL);

		// Jump over line that sets the flag
		WriteASMLine(ASM_JE, "10");

		// Line that sets the flag to say 'no return'
		WriteASMLine2(ASM_MOVMEMIMM4, "@$_ESC_", "3");

		// Comment
		WriteASMComment("FLAG NORETURN IF ESP<>STOREDESP", "", "", "");
	}
	if(dwTask==ASMTASK_DEBUGSTATEMENTHOOK
	|| dwTask==ASMTASK_DEBUGJUMPHOOK
	|| dwTask==ASMTASK_DEBUGRETURNHOOK)
	{
		if(dwTask!=ASMTASK_DEBUGRETURNHOOK)
		{
			// Push Four Debug Items to Stack
			DWORD iProgID = 0;
			DWORD iLineNum = dwLine;
			DWORD iStartChar = dwP1Type;
			DWORD iEndChar = dwP2Type;
			CStr* pProgStr = new CStr("");
			CStr* pLineStr = new CStr("");
			CStr* pStartStr = new CStr("");
			CStr* pEndStr = new CStr("");
			pProgStr->SetDWORDNumericText(iProgID);
			pLineStr->SetDWORDNumericText(iLineNum);
			pStartStr->SetDWORDNumericText(iStartChar);
			pEndStr->SetDWORDNumericText(iEndChar);
			WriteASMLine1IMM(ASM_PUSHIMM4, pEndStr->GetStr(), 2);
			WriteASMLine1IMM(ASM_PUSHIMM4, pStartStr->GetStr(), 2);
			WriteASMLine1IMM(ASM_PUSHIMM4, pLineStr->GetStr(), 2);
			WriteASMLine1IMM(ASM_PUSHIMM4, pProgStr->GetStr(), 2);
			SAFE_DELETE(pProgStr);
			SAFE_DELETE(pLineStr);
			SAFE_DELETE(pStartStr);
			SAFE_DELETE(pEndStr);
		}

		// Produce token Command Call token
		if(dwTask==ASMTASK_DEBUGSTATEMENTHOOK) WriteASMLine(ASM_MOVEAXIMM4, "[1");
		if(dwTask==ASMTASK_DEBUGJUMPHOOK) WriteASMLine(ASM_MOVEAXIMM4, "[2");
		if(dwTask==ASMTASK_DEBUGRETURNHOOK) WriteASMLine(ASM_MOVEAXIMM4, "[3");
		WriteASMLine(ASM_CALLEAX, "");

		// Free stack items
		if(dwTask!=ASMTASK_DEBUGRETURNHOOK)
		{
			WriteASMLine(ASM_POPEBX, "");
			WriteASMLine(ASM_POPEBX, "");
			WriteASMLine(ASM_POPEBX, "");
			WriteASMLine(ASM_POPEBX, "");
		}

		// Comment Details
		WriteASMComment("CALL", "Debug Hook", "", "");
	}
	if(dwTask==ASMTASK_RUNTIMEERRORHOOK)
	{
		// Move Line Number to register for RTE trace
		CStr* pLineStr = new CStr("");
		DWORD dwNeverZero = dwLine;
		if ( dwLine>0 )
		{
			pLineStr->SetNumericText(dwNeverZero);
			WriteASMLine2(ASM_MOVMEMIMM4, "@$_SLN_", pLineStr->GetStr());
		}
		else
		{
			// do NOT erase _SLN_ value with zero (can use last valid one)
			// but STILL NEED to fill in an instruction here to preserve jump distances
			WriteASMLine2(ASM_MOVMEMIMM4, "@$_ESC_", "1");
		}
		SAFE_DELETE(pLineStr);

		// Debug Mode requires break, not quit..
		if(g_DebugInfo.DebugModeOn())
		{
			// If runtime error DWORD is not zero, error occurred
			WriteASMLine(ASM_MOVEAXMEM4, "@$_ERR_");
			WriteASMLine(ASM_CMPEAX4, "0");
			WriteASMLine(ASM_JE, "25");

			// Work out BREAK Position
			CStr* pData = new CStr("");
			DWORD dwPosition=g_DebugInfo.GetLastBreakPoint();
			pData->SetNumericText(dwPosition);
			WriteASMLine2(ASM_MOVMEMIMM4, "@$_REK_", pData->GetStr());
			SAFE_DELETE(pData);

			// Set Escape value so Debugger is entered
			WriteASMLine2(ASM_MOVMEMIMM4, "@$_ESC_", "1");

			// Jump To End of Program (it will skil quit because of breakvalue)
			WriteASMLine(ASM_JMP, "$labelend");

			// Comment Details
			WriteASMComment("CALL", "Debug Runtime Error Hook", "", "");
		}
		else
		{
			// If runtime error DWORD is not zero, error occurred
			WriteASMLine(ASM_MOVEAXMEM4, "@$_ERR_");
			WriteASMLine(ASM_CMPEAX4, "0");
			WriteASMLine(ASM_JNE, "$labelend");

			// Comment Details
			WriteASMComment("CALL", "Normal Runtime Error Hook", "", "");
		}
	}

	//
	// ASM SWITCHES
	//

	// If INC uses non-memory, must use ADD instead
	CStr* pLocalStrForSwitch=NULL;
	if((dwTask==ASMTASK_INCVAR && dwP1Mode!=PMODE_MEM)
	|| (dwTask==ASMTASK_DECVAR && dwP1Mode!=PMODE_MEM))
	{
		if(dwTask==ASMTASK_INCVAR) dwTask=ASMTASK_ADD;
		if(dwTask==ASMTASK_DECVAR) dwTask=ASMTASK_SUB;

		// P2 becomes an IMM=1 value
		dwP2Mode=PMODE_IMM;
		pLocalStrForSwitch = new CStr("1");
		pP2=pLocalStrForSwitch;
		pP2Off=NULL;
		dwP2Type=7;
		dwP2Offset=0;

		// P3 becomes result var
		dwP3Mode=dwP1Mode;
		pP3=pP1;
		pP3Off=pP1Off;
		dwP3Type=dwP1Type;
		dwP3Offset=dwP1Offset;
	}

	//
	// HARDCODED ASM COMMANDS
	//

	if(dwTask==ASMTASK_ADD || dwTask==ASMTASK_SUB
	|| dwTask==ASMTASK_MUL || dwTask==ASMTASK_DIV || dwTask==ASMTASK_MOD
	|| dwTask==ASMTASK_AND || dwTask==ASMTASK_OR || dwTask==ASMTASK_NOT
	|| dwTask==ASMTASK_SHR || dwTask==ASMTASK_SHL || dwTask==ASMTASK_XOR
	|| dwTask==ASMTASK_BITNOT)
	{
		// MATH Machine Code
		if(dwP1Type==3 || dwP1Type==8 || dwP1Type==9)
		{
			// cannot hard code strings and doubles (yet)
//			WriteASMComment("!CANNOT! HARDMATH none ints", "", "", "");
			g_pErrorReport->SetError(dwLine, ERR_SYNTAX+50);
			return false;
		}
		else
		{
			// Treat FLOAT bitwize & logic as DWORD
			if(dwP1Type==2) dwP1Type=7;

			if(dwP2Mode==PMODE_IMM)
			{
				// Ensure IMM is DWORD
				DWORD dwExtraDWORD=0;
				DWORD dwDWORDRep = pP2->GetDWORDRepresentation(dwP2Type, &dwExtraDWORD);
				pP2->SetNumericText(dwDWORDRep);

				// mov eax,[a]
				WriteASMXtoEAX(dwP1Mode, pP1, pP1Off, dwP1Type, dwP1Offset);

				switch(dwTask)
				{
					case ASMTASK_ADD:
					{
						// add eax,imm
						DWORD dwCorrectASMCode=DetermineASMCall(ASM_ADDEAX1,dwP1Type);
						DWORD dwIMMSize=dwCorrectASMCode-ASM_ADDEAX1;
						WriteASMLine2IMM(dwCorrectASMCode, NULL, pP2->GetStr(), dwIMMSize);
					}
					break;

					case ASMTASK_SUB:
					{
						// sub eax,imm
						DWORD dwCorrectASMCode=DetermineASMCall(ASM_SUBEAX1,dwP1Type);
						DWORD dwIMMSize=dwCorrectASMCode-ASM_SUBEAX1;
						WriteASMLine2IMM(dwCorrectASMCode, NULL, pP2->GetStr(), dwIMMSize);
					}
					break;

					case ASMTASK_MUL:
					{
						// mov ebx,imm + mul eax,ebx
						DWORD dwCorrectASMCode=DetermineASMCall(ASM_MOVEBXIMM1,dwP1Type);
						DWORD dwIMMSize=dwCorrectASMCode-ASM_MOVEBXIMM1;
						WriteASMLine2IMM(dwCorrectASMCode, NULL, pP2->GetStr(), dwIMMSize);
						dwCorrectASMCode=DetermineASMCall(ASM_MULEAXEBX1,dwP1Type);
						WriteASMLine(dwCorrectASMCode, "");
					}
					break;

					case ASMTASK_DIV:
					case ASMTASK_MOD:
					{
						// mov ebx,imm + div eax,ebx
						DWORD dwCorrectASMCode=DetermineASMCall(ASM_MOVEBXIMM1,dwP1Type);
						DWORD dwIMMSize=dwCorrectASMCode-ASM_MOVEBXIMM1;
						WriteASMLine2IMM(dwCorrectASMCode, NULL, pP2->GetStr(), dwIMMSize);

						// leefix - 250604 - u54 - avoid division by zero with RT error
						WriteASMLine(ASM_CMPEBX4, "0");
						WriteASMLine(ASM_JNE, "15");

						// runtime error if not leaped over
						WriteASMLine2(ASM_MOVMEMIMM4, "@$_ERR_", "119");
						WriteASMLine(ASM_JMP, "3");

						// actual division
						WriteASMLine(ASM_CDQ, "");
						dwCorrectASMCode=DetermineASMCall(ASM_DIVEAXEBX1,dwP1Type);
						WriteASMLine(dwCorrectASMCode, "");

						// mod takes only the remainder
						if(dwTask==ASMTASK_MOD)
						{
							dwCorrectASMCode=DetermineASMCall(ASM_MOVEAXEDX1,dwP1Type);
							WriteASMLine(dwCorrectASMCode, "");
						}
					}
					break;

					case ASMTASK_AND:
					{
						DWORD dwCorrectASMCode=DetermineASMCall(ASM_ANDEAX1,dwP1Type);
						DWORD dwIMMSize=dwCorrectASMCode-ASM_ANDEAX1;
						WriteASMLine2IMM(dwCorrectASMCode, NULL, pP2->GetStr(), dwIMMSize);
					}
					break;

					case ASMTASK_OR:
					{
						DWORD dwCorrectASMCode=DetermineASMCall(ASM_OREAX1,dwP1Type);
						DWORD dwIMMSize=dwCorrectASMCode-ASM_OREAX1;
						WriteASMLine2IMM(dwCorrectASMCode, NULL, pP2->GetStr(), dwIMMSize);
					}
					break;

					case ASMTASK_NOT:
					{
						// LEEFIX - 041002 - NOT is a boolean task, so screen out all but first bit
//						DWORD dwCorrectASMCode=DetermineASMCall(ASM_NOTEAX1,dwP1Type);
//						WriteASMLine(dwCorrectASMCode, "");
//						dwCorrectASMCode=DetermineASMCall(ASM_ANDEAX1,dwP1Type);
//						DWORD dwIMMSize=dwCorrectASMCode-ASM_ANDEAX1;
//						WriteASMLine2IMM(dwCorrectASMCode, NULL, "1", dwIMMSize);

						// lee - 010306 - u60 - actually toggle condition which reads EAX to JNE
						SetCondToggle(true);

						// lee - 240306 - u6b4 - also, NOT as well for legacy support (above flag NOTs it back)
						DWORD dwCorrectASMCode=DetermineASMCall(ASM_NOTEAX1,dwP1Type);
						WriteASMLine(dwCorrectASMCode, "");
					}
					break;

					case ASMTASK_BITNOT:
					{
						DWORD dwCorrectASMCode=DetermineASMCall(ASM_NOTEAX1,dwP1Type);
						WriteASMLine(dwCorrectASMCode, "");
					}
					break;


					case ASMTASK_XOR:
					{
						DWORD dwCorrectASMCode=DetermineASMCall(ASM_XOREAX1,dwP1Type);
						DWORD dwIMMSize=dwCorrectASMCode-ASM_XOREAX1;
						WriteASMLine2IMM(dwCorrectASMCode, NULL, pP2->GetStr(), dwIMMSize);
					}
					break;

					case ASMTASK_SHL:
					{
						DWORD dwCorrectASMCode=DetermineASMCall(ASM_SHLEAX1,dwP1Type);
						DWORD dwIMMSize=0;//can only be IMM8
						WriteASMLine2IMM(dwCorrectASMCode, NULL, pP2->GetStr(), dwIMMSize);
					}
					break;

					case ASMTASK_SHR:
					{
						DWORD dwCorrectASMCode=DetermineASMCall(ASM_SHREAX1,dwP1Type);
						DWORD dwIMMSize=0;//can only be IMM8
						WriteASMLine2IMM(dwCorrectASMCode, NULL, pP2->GetStr(), dwIMMSize);
					}
					break;
				}

				// mov [r],eax
				WriteASMEAXtoX(dwP3Mode, pP3, pP3Off, dwP3Type, dwP3Offset);
			}
			else
			{
				// Where DIV/MOD task, all 4bytes of EBX are CMP'd so clear EAX now
				if(dwTask==ASMTASK_DIV || dwTask==ASMTASK_MOD)
				{
					// and only if sub-4byte EAX op used
					if(dwP2Type==4 || dwP2Type==5 || dwP2Type==6)//bool,byte,word only
					{
						WriteASMLine(ASM_MOVEAXIMM4, "0");
					}
				}

				// mov eax,[b]
				WriteASMXtoEAX(dwP2Mode, pP2, pP2Off, dwP2Type, dwP2Offset);

				// mov [b] to stack
				WriteASMLine(ASM_PUSHEAX, "");

				// mov eax,[a]
				WriteASMXtoEAX(dwP1Mode, pP1, pP1Off, dwP1Type, dwP1Offset);

				// put [b] into EBX from stack
				WriteASMLine(ASM_POPEBX, "");

				switch(dwTask)
				{
					case ASMTASK_ADD:
					{
						// add eax,ebx
						DWORD dwCorrectASMCode=DetermineASMCall(ASM_ADDEAXEBX1,dwP1Type);
						WriteASMLine(dwCorrectASMCode, "");
					}
					break;

					case ASMTASK_SUB:
					{
						// sub eax,ebx
						DWORD dwCorrectASMCode=DetermineASMCall(ASM_SUBEAXEBX1,dwP1Type);
						WriteASMLine(dwCorrectASMCode, "");
					}
					break;

					case ASMTASK_MUL:
					{
						// mul eax,ebx
						DWORD dwCorrectASMCode=DetermineASMCall(ASM_MULEAXEBX1,dwP1Type);
						WriteASMLine(dwCorrectASMCode, "");
					}
					break;

					case ASMTASK_DIV:
					case ASMTASK_MOD:
					{
						// avoid divide by zero
// leefix - 350604 - old way was silent skip, new way is runtime error
//						WriteASMLine(ASM_CMPEBX4, "0");
//						WriteASMLine(ASM_JE, "3");
						WriteASMLine(ASM_CMPEBX4, "0");
						WriteASMLine(ASM_JNE, "15");

						// runtime error if not leaped over
						WriteASMLine2(ASM_MOVMEMIMM4, "@$_ERR_", "119");
						WriteASMLine(ASM_JMP, "3");
						
						// div eax,ebx
						WriteASMLine(ASM_CDQ, "");
						DWORD dwCorrectASMCode=DetermineASMCall(ASM_DIVEAXEBX1,dwP1Type);
						WriteASMLine(dwCorrectASMCode, "");

						// mod takes only the remainder
						if(dwTask==ASMTASK_MOD)
						{
							dwCorrectASMCode=DetermineASMCall(ASM_MOVEAXEDX1,dwP1Type);
							WriteASMLine(dwCorrectASMCode, "");
						}
					}
					break;

					case ASMTASK_AND:
					{
						DWORD dwCorrectASMCode=DetermineASMCall(ASM_ANDEAXEBX1,dwP1Type);
						WriteASMLine(dwCorrectASMCode, "");
					}
					break;

					case ASMTASK_OR:
					{
						DWORD dwCorrectASMCode=DetermineASMCall(ASM_OREAXEBX1,dwP1Type);
						WriteASMLine(dwCorrectASMCode, "");
					}
					break;

					case ASMTASK_NOT:
					{
						// NOT is a boolean task, so screen out all but first bit
//						DWORD dwCorrectASMCode=DetermineASMCall(ASM_NOTEAX1,dwP1Type);
//						WriteASMLine(dwCorrectASMCode, "");

						// lee - 010306 - u60 - defer to witching JE to JNE below..
						SetCondToggle(true);

						// lee - 240306 - u6b4 - also, NOT as well for legacy support (above flag NOTs it back)
						DWORD dwCorrectASMCode=DetermineASMCall(ASM_NOTEAX1,dwP1Type);
						WriteASMLine(dwCorrectASMCode, "");
					}
					break;

					case ASMTASK_BITNOT:
					{
						DWORD dwCorrectASMCode=DetermineASMCall(ASM_NOTEAX1,dwP1Type);
						WriteASMLine(dwCorrectASMCode, "");
					}
					break;

					case ASMTASK_XOR:
					{
						DWORD dwCorrectASMCode=DetermineASMCall(ASM_XOREAXEBX1,dwP1Type);
						WriteASMLine(dwCorrectASMCode, "");
					}
					break;

					case ASMTASK_SHL:
					{
						// mov ebx to CL (ecx byte part)
						WriteASMLine(ASM_MOVECXEBX4, "");

						// do shift 0-31 limit
						DWORD dwCorrectASMCode=DetermineASMCall(ASM_SHLEAXCLC1,dwP1Type);
						WriteASMLine(dwCorrectASMCode, "");
					}
					break;

					case ASMTASK_SHR:
					{
						// mov ebx to CL (ecx byte part)
						WriteASMLine(ASM_MOVECXEBX4, "");

						// do shift 0-31 limit
						DWORD dwCorrectASMCode=DetermineASMCall(ASM_SHREAXCLC1,dwP1Type);
						WriteASMLine(dwCorrectASMCode, "");
					}
					break;
				}

				// mov [r],eax
				WriteASMEAXtoX(dwP3Mode, pP3, pP3Off, dwP3Type, dwP3Offset);
			}

			// Comment
			if(dwTask==ASMTASK_ADD) WriteASMComment("ADD", "", "", "");
			if(dwTask==ASMTASK_SUB) WriteASMComment("SUB", "", "", "");
			if(dwTask==ASMTASK_MUL) WriteASMComment("MUL", "", "", "");
			if(dwTask==ASMTASK_DIV) WriteASMComment("DIV", "", "", "");
			if(dwTask==ASMTASK_MOD) WriteASMComment("MOD", "", "", "");
			if(dwTask==ASMTASK_AND) WriteASMComment("AND", "", "", "");
			if(dwTask==ASMTASK_OR)  WriteASMComment("OR", "", "", "");
			if(dwTask==ASMTASK_NOT) WriteASMComment("NOT", "", "", "");
			if(dwTask==ASMTASK_XOR) WriteASMComment("XOR", "", "", "");
			if(dwTask==ASMTASK_BITNOT) WriteASMComment("BIT NOT", "", "", "");
			if(dwTask==ASMTASK_SHL) WriteASMComment("SHL", "", "", "");
			if(dwTask==ASMTASK_SHR) WriteASMComment("SHR", "", "", "");
		}
	}

	if(dwTask==ASMTASK_EQUAL || dwTask==ASMTASK_NOTEQUAL
	|| dwTask==ASMTASK_GREATER || dwTask==ASMTASK_GREATEREQUAL
	|| dwTask==ASMTASK_LESS || dwTask==ASMTASK_LESSEQUAL )
	{
		// COMPARE Machine Code
		if(dwP1Type==3 || dwP1Type==8 || dwP1Type==9)
		{
			// cannot hard code floats and doubles (yet)
//			WriteASMComment("!CANNOT! HARDCOMPARE none ints", "", "", "");
			g_pErrorReport->SetError(dwLine, ERR_SYNTAX+50);
			return false;
		}
		else
		{
			// Treat FLOAT bitwize & logic as DWORD
			if(dwP1Type==2) dwP1Type=7;

			if(dwP2Mode==PMODE_IMM)
			{
				// Ensure IMM is DWORD
				DWORD dwExtraDWORD=0;
				DWORD dwDWORDRep = pP2->GetDWORDRepresentation(dwP2Type, &dwExtraDWORD);
				pP2->SetNumericText(dwDWORDRep);

				// mov eax,imm
				DWORD dwCorrectASMCode=DetermineASMCall(ASM_MOVEAXIMM1,dwP1Type);
				DWORD dwIMMSize=dwCorrectASMCode-ASM_MOVEAXIMM1;
				WriteASMLine2IMM(dwCorrectASMCode, NULL, pP2->GetStr(), dwIMMSize);
			}
			else
			{
				// mov eax,[b]
				WriteASMXtoEAX(dwP2Mode, pP2, pP2Off, dwP2Type, dwP2Offset);
			}

			// push [b] to stack
			WriteASMLine(ASM_PUSHEAX, "");

			// mov eax,[a]
			WriteASMXtoEAX(dwP1Mode, pP1, pP1Off, dwP1Type, dwP1Offset);

			// mov ebx,eax
			WriteASMLine(ASM_MOVEBXEAX4, "");

			// pop [b] from stack to EDX
			WriteASMLine(ASM_POPEDX, "");

			// cmp and setcc
			WriteASMLine(ASM_MOVEAXIMM4, "0");
			DWORD dwCorrectASMCode=DetermineASMCall(ASM_CMPEDXEBX1,dwP1Type);
			WriteASMLine(dwCorrectASMCode, "");
			switch(dwTask)
			{
				case ASMTASK_EQUAL: WriteASMLine(ASM_SETE, ""); break;
				case ASMTASK_NOTEQUAL: WriteASMLine(ASM_SETNE, ""); break;
				case ASMTASK_GREATER: WriteASMLine(ASM_SETG, ""); break;
				case ASMTASK_GREATEREQUAL: WriteASMLine(ASM_SETGE, ""); break;
				case ASMTASK_LESS: WriteASMLine(ASM_SETL, ""); break;
				case ASMTASK_LESSEQUAL: WriteASMLine(ASM_SETLE, ""); break;
			}

			// mov [r],eax
			WriteASMEAXtoX(dwP3Mode, pP3, pP3Off, dwP3Type, dwP3Offset);
		}

		// Comment
		if(dwTask==ASMTASK_EQUAL) WriteASMComment("EQUAL", "", "", "");
		if(dwTask==ASMTASK_NOTEQUAL) WriteASMComment("NOTEQUAL", "", "", "");
		if(dwTask==ASMTASK_GREATER) WriteASMComment("GREATER", "", "", "");
		if(dwTask==ASMTASK_GREATEREQUAL) WriteASMComment("GREATEREQUAL", "", "", "");
		if(dwTask==ASMTASK_LESS) WriteASMComment("LESS", "", "", "");
		if(dwTask==ASMTASK_LESSEQUAL) WriteASMComment("LESSEQUAL", "", "", "");
	}

	if(dwTask==ASMTASK_INCVAR)
	{
		// INC Machine Code
		if(dwP1Type==3 || dwP1Type==8 || dwP1Type==9)
		{
//			WriteASMComment("!CANNOT! INC VAR", "", "", "");
			g_pErrorReport->SetError(dwLine, ERR_SYNTAX+51);
			return false;
		}
		else
		{
			// Treat FLOAT as DWORD (replace with float ADD)
			if(dwP1Type==2) dwP1Type=7;

			if(dwP1Mode==PMODE_MEM)
			{
				DWORD dwCorrectASMCode=DetermineASMCall(ASM_INCMEM1,dwP1Type);
				WriteASMLine(dwCorrectASMCode, pP1->GetStr());
			}
			WriteASMComment("INC VAR", "", "", "");
		}
	}
	if(dwTask==ASMTASK_DECVAR)
	{
		if(dwP1Type==3 || dwP1Type==8 || dwP1Type==9)
		{
//			WriteASMComment("!CANNOT! DEC VAR", "", "", "");
			g_pErrorReport->SetError(dwLine, ERR_SYNTAX+51);
			return false;
		}
		else
		{
			// Treat FLOAT as DWORD (replace with float SUB)
			if(dwP1Type==2) dwP1Type=7;

			// DEC Machine Code
			if(dwP1Mode==PMODE_MEM)
			{
				DWORD dwCorrectASMCode=DetermineASMCall(ASM_DECMEM1,dwP1Type);
				WriteASMLine(dwCorrectASMCode, pP1->GetStr());
			}
			WriteASMComment("DEC VAR", "", "", "");
		}
	}
	if(dwTask==ASMTASK_BREAKPOINTRESUME)
	{
		// Check if breakpoint active
		WriteASMCheckBreakPointVar();

		// LEAP-FORWARDS Marker OpCode to skip breakpoint-resume
		WriteASMLeapMarkerJump(ASM_JE, 0);

//		Cannot Resume in deep nest of stack - new m/c means stack data useless
//		// Restore stack as it was before we left the program
//		WriteASMCall(m_dwLineNumber, "dbprocore.dll", "?StackSnapshotRestore@@YAKXZ");
	
//		// When return, EAX holds the stack-ptr-adjustment value
//		WriteASMLine(ASM_SUBESPEAX, NULL);

//		// Reload registers with all values as it was before we left
//		WriteASMLine(ASM_POPAD, "");

		// Jump to breakpoint
		WriteASMLine(ASM_MOVEBXMEM4, "@$_REK_");
		WriteASMLine2(ASM_MOVMEMIMM4, "@$_REK_", "0");
		WriteASMLine(ASM_JMPEBX, 0);

		// Complete LEAP-FORWARD Marker (jump here if skip breakpoint resume)
		g_pASMWriter->WriteASMLeapMarkerEnd(0);

		// Comment on this task
		WriteASMComment("BREAKPOINT RESUME", "", "", "");
	}
	if(dwTask==ASMTASK_PUSHINTERNALARRAYINDEX)
	{
		// Find Array location (in EAX)
		if(dwP1Mode==PMODE_MEMARR) WriteASMLine(ASM_MOVEAXMEM4, pP1->GetStr());
		if(dwP1Mode==PMODE_EBPARR) WriteASMLine(ASM_MOVEAXEBP4, pP1->GetStr()+2);

		/* old code crashes when safe array switched off and A() used instead of A( ) when A was not DIMMED
		// If Array Check Active
		if(GetArrayCheckFlag())
		{
			// Make Sure Array Exists
			WriteASMLine(ASM_CMPEAX4, "0");

			// Leap Marker OpCode
			WriteASMLeapMarkerJump(ASM_JE, 4);
		}

		// Get array index from -4 location within array header - put in EDX
		WriteASMLine(ASM_MOVEDXEAXOFF4, "-4");

		// Copy Push EDX to stack
		WriteASMLine(ASM_PUSHEDX, "");

		// If Array Check Active
		if(GetArrayCheckFlag())
		{
			// Complete Leap Marker (so we jump here)
			WriteASMLeapMarkerEnd(4);
		}
		*/

		// Force a check of the array as we NEED it to find the array index (safe arrays or no)
		WriteASMLine(ASM_CMPEAX4, "0");
		WriteASMLeapMarkerJump(ASM_JE, 4);
		WriteASMLine(ASM_MOVEDXEAXOFF4, "-4");
		WriteASMLine(ASM_PUSHEDX, "");
		WriteASMLeapMarkerEnd(4);

		// Comment on this task
		WriteASMComment("PUSH INT. ARRAY INDEX TO STACK", "", "", "");
	}
	if(dwTask==ASMTASK_CALCARRAYOFFSET)
	{
		// Find Array location (in EAX)
		if(dwP2Mode==PMODE_MEMARR) WriteASMLine(ASM_MOVEAXMEM4, pP2->GetStr());
		if(dwP2Mode==PMODE_EBPARR) WriteASMLine(ASM_MOVEAXEBP4, pP2->GetStr()+2);

		// If Array Check Active
		if(GetArrayCheckFlag())
		{
			// Make Sure Array Exists
			WriteASMLine(ASM_CMPEAX4, "0");

			// Leap Marker OpCode
			WriteASMLeapMarkerJump(ASM_JE, 4);
		}

		// Always have first dimension D1
		WriteASMLine(ASM_POPEBX, "");

		// Loop through subsequent dimensions D2-D9 (dwP1Offset used as carrier)
		int iCount=0;
		int iCountMax = (DWORD)dwP1Offset-1;
		while(iCount<iCountMax)
		{
			CStr* pValue = new CStr("");
			int iHeaderOffset = (-56)+(iCount*4);
			pValue->SetNumericText(iHeaderOffset);
			WriteASMLine(ASM_POPEDX, "");
			WriteASMLine(ASM_MULEDXEAXOFF4, pValue->GetStr());
			WriteASMLine(ASM_ADDEBXEDX4, "");
			SAFE_DELETE(pValue);
			iCount++;
		}

		// Store EBX in Offset variable
		WriteASMLine(ASM_MOVEAXEBX4, "");
		WriteASMEAXtoX(dwP1Mode, pP1, NULL, 7, 0);

		// If Array Check Active
		if(GetArrayCheckFlag())
		{
			// Complete Leap Marker (so we jump here)
			WriteASMLeapMarkerEnd(4);
		}

		// Comment on this task
		WriteASMComment("CALCULATE ARRAY OFFSET", "", "", "");
	}
	if(dwTask==ASMTASK_PUSHUDT)
	{
		// UDT address to EAX

// did not account for pasing UCTs from with user functions
//		WriteASMLine(ASM_MOVEAXIMM4, pP1->GetStr());
		if ( dwP1Mode==PMODE_MEM )
		{
			WriteASMLine(ASM_MOVEAXIMM4, pP1->GetStr());
		}
		if ( dwP1Mode==PMODE_EBP )
		{
			WriteASMLine(ASM_MOVEAXEBP, NULL );
			WriteASMLine(ASM_ADDEAX4, pP1->GetStr()+2 );
		}
		if ( dwP1Offset>0 )
		{
			CStr* pNum = new CStr("");
			pNum->SetNumericText(dwP1Offset);
			WriteASMLine(ASM_ADDEAX4, pNum->GetStr());
			SAFE_DELETE(pNum);
		}

		// Advance EAX to end of UDT data (UDT Size)
		WriteASMLine(ASM_ADDEAX4, pP2->GetStr());

		// advance to last data element
		for ( DWORD n=0; n<dwP2Offset; n++)
		{
			// decrement udtptr
			WriteASMLine(ASM_SUBEAX4, "4");

			// push udtptr to stack
			WriteASMLine(ASM_PUSHFROMEAX, NULL);
		}

		// Comment on this task
		WriteASMComment("PUSH UDT TO STACK", "", "", "");
	}

	// Free usages
	SAFE_DELETE(pLocalStrForSwitch);

	return true;
}

bool CASMWriter::WriteASMLine(DWORD dwOp, LPSTR pOpData)
{
	// DBM Code
	CStr strDBMLine(256);
	strDBMLine.SetNumericText(m_dwLineNumber);
	strDBMLine.AddText(" ");
	strDBMLine.AddText(m_pASMDebugString[dwOp]);
	strDBMLine.AddText(" ");
	strDBMLine.AddText(pOpData);
	if(g_pDBMWriter->OutputDBM(&strDBMLine)==false) return false;

	// ASM Code
	if(m_bASMOpData[dwOp]==true)
		CreateASMMiddle(m_iASMPreOp[dwOp], m_iASMOp1[dwOp], m_iASMOp2[dwOp], pOpData);
	else
		CreateASMMiddle(m_iASMPreOp[dwOp], m_iASMOp1[dwOp], m_iASMOp2[dwOp], "");

	// Complete
	return true;
}

bool CASMWriter::WriteASMLine2(DWORD dwOp, LPSTR pOpData, LPSTR pOpData2)
{
	// DBM Code
	CStr strDBMLine(256);
	strDBMLine.SetNumericText(m_dwLineNumber);
	strDBMLine.AddText(" ");
	strDBMLine.AddText(m_pASMDebugString[dwOp]);
	strDBMLine.AddText(" ");
	strDBMLine.AddText(pOpData);
	strDBMLine.AddText(", ");
	strDBMLine.AddText(pOpData2);
	if(g_pDBMWriter->OutputDBM(&strDBMLine)==false) return false;

	// ASM Code
	CreateASMMiddleCore(m_iASMPreOp[dwOp], m_iASMOp1[dwOp], m_iASMOp2[dwOp], pOpData, pOpData2, false, 0);

	// Complete
	return true;
}

bool CASMWriter::WriteASMLine1IMM(DWORD dwOp, LPSTR pOpData, DWORD dwSizeIMM)
{
	// DBM Code
	CStr strDBMLine(256);
	strDBMLine.SetNumericText(m_dwLineNumber);
	strDBMLine.AddText(" ");
	strDBMLine.AddText(m_pASMDebugString[dwOp]);
	strDBMLine.AddText(" ");
	strDBMLine.AddText(pOpData);
	if(g_pDBMWriter->OutputDBM(&strDBMLine)==false) return false;

	// ASM Code
	CreateASMMiddleCore(m_iASMPreOp[dwOp], m_iASMOp1[dwOp], m_iASMOp2[dwOp], pOpData, NULL, true, dwSizeIMM);

	// Complete
	return true;
}

bool CASMWriter::WriteASMLine2IMM(DWORD dwOp, LPSTR pOpData, LPSTR pOpData2, DWORD dwSizeIMM)
{
	// DBM Code
	CStr strDBMLine(256);
	strDBMLine.SetNumericText(m_dwLineNumber);
	strDBMLine.AddText(" ");
	strDBMLine.AddText(m_pASMDebugString[dwOp]);
	strDBMLine.AddText(" ");
	strDBMLine.AddText(pOpData);
	strDBMLine.AddText(", ");
	strDBMLine.AddText(pOpData2);
	if(g_pDBMWriter->OutputDBM(&strDBMLine)==false) return false;

	// ASM Code
	CreateASMMiddleCore(m_iASMPreOp[dwOp], m_iASMOp1[dwOp], m_iASMOp2[dwOp], pOpData, pOpData2, true, dwSizeIMM);

	// Complete
	return true;
}

bool CASMWriter::WriteASMComment(LPSTR pTitle, LPSTR pC1, LPSTR pC2, LPSTR pC3)
{
	// Ensure Comments appear at same horiz position
	DWORD dwNumOfLineChars = g_pDBMWriter->EatCarriageReturn();
	DWORD dwAdvance = 30-dwNumOfLineChars;
	if(dwNumOfLineChars>30) dwAdvance=0;

	// DBM Code
	CStr strDBMLine("");
	for(DWORD n=0; n<dwAdvance; n++) strDBMLine.AddText(" ");
	strDBMLine.AddText("; ");
	strDBMLine.AddText(pTitle);
	strDBMLine.AddText(" ");
	if(pC1)
	{
		strDBMLine.AddText(pC1);
		strDBMLine.AddText(" ");
	}
	if(pC2)
	{
		strDBMLine.AddText(pC2);
		strDBMLine.AddText(" ");
	}
	if(pC3)
	{
		strDBMLine.AddText(pC3);
	}
	if(g_pDBMWriter->OutputDBM(&strDBMLine)==false) return false;

	// Complete
	return true;
}

bool CASMWriter::WriteASMLeapMarkerTop(void)
{
	// Record Where We Are Now
	m_pRecordTopBytePosition = m_pMachineBlock;

	// Complete
	return true;
}

bool CASMWriter::WriteASMLineLeapToTop(DWORD dwOp)
{
	// DBM Code
	CStr strDBMLine(256);
	strDBMLine.SetNumericText(m_dwLineNumber);
	strDBMLine.AddText(" ");
	strDBMLine.AddText(m_pASMDebugString[dwOp]);
	strDBMLine.AddText(" ");
	strDBMLine.AddText("LEAP TO TOP");
	if(g_pDBMWriter->OutputDBM(&strDBMLine)==false) return false;

	// Calculate Offset For This Leap To Top
	int iOffset=(m_pRecordTopBytePosition-m_pMachineBlock)-6;

	// ASM Code
	CStr* pOffsetStr = new CStr();
	pOffsetStr->SetNumericText(iOffset);
	CreateASMMiddle(m_iASMPreOp[dwOp], m_iASMOp1[dwOp], m_iASMOp2[dwOp], pOffsetStr->GetStr());
	SAFE_DELETE(pOffsetStr);

	// Complete
	return true;
}

bool CASMWriter::WriteASMLeapMarkerJumpToTop(void)
{
	WriteASMLine(ASM_CMPEAX4, "0");
	WriteASMLineLeapToTop(ASM_JNE);
	return true;
}

bool CASMWriter::WriteASMLineLeap(DWORD dwOp, DWORD di)
{
	// DBM Code
	CStr strDBMLine(256);
	strDBMLine.SetNumericText(m_dwLineNumber);
	strDBMLine.AddText(" ");
	strDBMLine.AddText(m_pASMDebugString[dwOp]);
	strDBMLine.AddText(" ");
	strDBMLine.AddText("LEAP");
	if(g_pDBMWriter->OutputDBM(&strDBMLine)==false) return false;

	// ASM Code
	CreateASMMiddle(m_iASMPreOp[dwOp], m_iASMOp1[dwOp], m_iASMOp2[dwOp], "0");

	// Complete
	return true;
}

bool CASMWriter::WriteASMLeapMarkerJump(DWORD dwOp, DWORD di)
{
	// Write Line As Normal
	WriteASMLineLeap(dwOp, di);

	// Record Where JUMP Offset Must Go
	m_pRecordRefPosition[di] = 1+m_dwProgramRefPointer;
	m_pRecordBytePosition[di] = m_pMachineBlock;

	// Complete
	return true;
}

bool CASMWriter::WriteASMLeapMarkerJumpNotEqual(DWORD di)
{
	return WriteASMLeapMarkerJump(ASM_JNE, di);
}

bool CASMWriter::WriteASMLeapForwardMarker(void)
{
	// Check if escape value is zero
	WriteASMLine(ASM_MOVEAXMEM4, "@$_ESC_");
	WriteASMLine(ASM_CMPEAX4, "0");

	// LEAP-FORWARDS Marker OpCode (different from leap-back)
	WriteASMLeapMarkerJump(ASM_JE, 0);

	// Complete
	return true;
}

bool CASMWriter::WriteASMLeapMarkerEnd(DWORD di)
{
	if(m_pRecordRefPosition[di]>0)
	{
		// Prepare for actual indexing
		m_pRecordRefPosition[di]-=2;

		// Get old ref-string
		LPSTR pRefStr = (LPSTR)*(m_pProgramRefLabel+m_pRecordRefPosition[di]);
		if(pRefStr)
		{
			delete pRefStr;
			pRefStr=NULL;
		}

		// Calculate Leap Offset 
		DWORD dwLeapOffset = m_pMachineBlock-m_pRecordBytePosition[di];

		// Create NEW ref-string from offset value
		CStr* pTempStr = new CStr("");
		pTempStr->SetNumericText(dwLeapOffset);
		pRefStr = new char[strlen(pTempStr->GetStr())+1];
		strcpy(pRefStr, pTempStr->GetStr());
		*(m_pProgramRefLabel+m_pRecordRefPosition[di])=(DWORD)pRefStr;
		SAFE_DELETE(pTempStr);

		// Clear leap flag
		m_pRecordRefPosition[di]=0;
		m_pRecordBytePosition[di]=0;
	}
	else
	{
		// No marker to complete
	}

	// Complete
	return true;
}

bool CASMWriter::WriteASMCheckBreakPointVar(void)
{
	WriteASMLine(ASM_MOVEAXMEM4, "@$_REK_");
	WriteASMLine(ASM_CMPEAX4, "0");
	return true;
}

bool CASMWriter::WriteASMForceEscapeAtCodeBREAK(void)
{
	// Force an escape by setting ec to 1
	WriteASMLine2(ASM_MOVMEMIMM4, "@$_ESC_", "1");

	// Complete
	return true;
}

void CASMWriter::SetBreakPointValue(void)
{
	WriteASMLine2(ASM_MOVMEMIMM4, "@$_REK_", "1");
}

DWORD CASMWriter::AddCommandToTable(LPSTR pDLLString, LPSTR pCommandString)
{
	// Skip non-DLL commands
	if(pDLLString==NULL) return g_pStatementList->GetDLLIndexCounter();
	if(strlen(pDLLString)<2) return g_pStatementList->GetDLLIndexCounter();

	// Record DLL As Actually Being Used
	DWORD dwIndex = g_pStatementList->GetDLLIndexCounter() + 1;
	if(g_pDLLTable->AddUniqueString(pDLLString+1, &dwIndex))
		g_pStatementList->IncDLLIndexCounter(1);

	// Record Command As Actually Being Used
	CStr* pRawCommandString = new CStr("");
	pRawCommandString->SetNumericText(dwIndex);
	pRawCommandString->AddText(",");
	pRawCommandString->AddText(pCommandString+1);
	dwIndex = g_pStatementList->GetCommandIndexCounter() + 1;
	if(g_pCommandTable->AddUniqueString(pRawCommandString->GetStr(), &dwIndex))
		g_pStatementList->IncCommandIndexCounter(1);

	// Free usgaes
	SAFE_DELETE(pRawCommandString);

	return dwIndex;
}

bool CASMWriter::AddProtectionToSelectedDLLs(LPSTR pDLLString)
{
	// ANYTHING ending with "Master.DLL" is protected
	if ( strnicmp ( pDLLString + strlen ( pDLLString ) - 10, "Master.dll", 10 )==NULL )
		return true;
	else
		return false;
}
