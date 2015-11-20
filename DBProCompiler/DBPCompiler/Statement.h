// Statement.h: interface for the CStatement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATEMENT_H__2A1543E2_9870_4E5E_B056_C09192997E8D__INCLUDED_)
#define AFX_STATEMENT_H__2A1543E2_9870_4E5E_B056_C09192997E8D__INCLUDED_

// Common Includes
#include "windows.h"

// Custom Includes
#include "InstructionTableEntry.h"
#include "Declaration.h"
#include "Str.h"
#include "ParserResultData.h"

// Token Def
#define			COMMATK				10001
#define			CRTK				10002

#define			ENDTK				10010
#define			EXITTK				10011

#define			DOTK				10021
#define			LOOPTK				10022
#define			WHILETK				10023
#define			ENDWHILETK			10024
#define			REPEATTK			10025
#define			UNTILTK				10026
#define			FORTK				10031
#define			NEXTTK				10032

#define			USERFUNCTIONTK		10101
#define			EXITUSERFUNCTIONTK	10102
#define			ENDUSERFUNCTIONTK	10103
#define			USERFUNCTIONCALLTK	10104

#define			IFTK				10151
#define			ELSETK				10152
#define			ENDIFTK				10153
#define			ELSEENDIFTK			10154
#define			ELSECRTK			10155

#define			GOTOTK				10201
#define			GOSUBTK				10202

#define			SELECTTK			10211
#define			ENDSELECTTK			10212
#define			CASETK				10213
#define			ENDCASETK			10214
#define			CASEDEFAULTTK		10215

#define			TYPETK				10301
#define			ENDTYPETK			10302
#define			GLOBALTK			10303
#define			LOCALTK				10304
#define			DIMTK				10305
#define			UNDIMTK				10306
#define			ASTK				10307
#define			BOOLEANTK			10311
#define			BYTETK				10312
#define			WORDTK				10313
#define			DWORDTK				10314
#define			INTEGERTK			10315
#define			FLOATTK				10316
#define			STRINGTK			10317
#define			DOUBLETK			10318

#define			REMLINETK			10501
#define			REMSTARTTK			10502
#define			REMENDTK			10503

#define			LABELTK				10701
#define			DATATK				10702

#define			ASSIGNMENTTK		11001
#define			INSTRUCTIONTK		11004

// Class Prototypes
class CParameter;
class CDatatype;
class CMathOp;

// Class Def
class CStatement  
{
	public:
		CStatement();
		virtual ~CStatement();
		void FreeObjects(void);
		void Free(void);

		void			Add(CStatement *pNext);
		CStatement*		GetNext(void) { return m_pNext; }
		CStatement*		FindLastStatement(void);
		void			SetData(DWORD LineNumber, DWORD m_dwObjectType, void* pPtr);
		void			SetNext(CStatement* pNext) { m_pNext=pNext; }

		void			SetLineNumber(DWORD line) { m_dwLineNumber=line; }
		void			SetObjectType(DWORD type) { m_dwObjectType=type; }
		void			SetObjectClass(void* pPtr) { m_pObjectClass=pPtr; }
		void			SetParameter(CParameter* pParam) { m_pParameters=pParam; }
		void			SetLine(DWORD dwLine);
		DWORD			GetLineNumber(void) { return m_dwLineNumber; }

		void			SetLineAndCharPos(DWORD dwLine);
		void			SetLineAndCharPos(DWORD dwLine, int iFlag);

		void			SurpressJumpChecks(void) { m_bPerformJumpChecks=false; }

	public:
		bool			DoPreScanBlock(DWORD RequiredTerminator);
		bool			DoLocalScanBlock(DWORD RequiredTerminator);
		bool			DoBlock(DWORD RequiredTerminator, DWORD* dwLastToken);
		bool			DoInitCode(void);
		bool			DoEndCode(void);
		CStatement*		AddInternalStatement(DWORD dwCodeIndex, DWORD dwInternalCode);
		bool			DoStatement(DWORD TokenID);
		bool			DoLoop(DWORD StatementLineNumber, DWORD TokenID);
		bool			DoJump(DWORD StatementLineNumber, DWORD TokenID);
		bool			DoType(DWORD StatementLineNumber, DWORD TokenID);
		bool			DoLabel(DWORD StatementLineNumber, DWORD TokenID);
		bool			DoDataStatement(DWORD StatementLineNumber, DWORD TokenID);
		bool			DoDeclaration(bool bVariableDeclaration, DWORD dwTerminatorType, CDeclaration** pDec, bool bDoneDim, bool bAutoInitialiseData, bool bIsGlobal, bool bDefineOnly);
		bool			DoInstruction(DWORD StatementLineNumber, DWORD TokenID);
		bool			DoAssignment(DWORD StatementLineNumber, DWORD TokenID);
		bool			DoAllocation(DWORD StatementLineNumber, LPSTR pVarName, LPSTR pValue);
		bool			DoDeAllocation(DWORD StatementLineNumber);
		bool			DoUserFunction(DWORD StatementLineNumber, DWORD TokenID);
		bool			DoUserFunctionCall(DWORD StatementLineNumber, DWORD TokenID);
		bool			DoUserFunctionExit(DWORD StatementLineNumber, DWORD TokenID);
		bool			DoParameterListString(CStr* pParamString, CParameter** ppFirstParameter);
		bool			DoExpressionList(CParameter** ppParameter, bool* bNoMoreParams);
		bool			DoExpressionListString(CParameter** ppParameter, CStr* pOptionalString, DWORD* pdwReturnDistance, bool* bTerminator);
		bool			DoExpression(CStr* pStr, CParameter* pParameter);
		bool			FindHighestPres(CStr* pStr, DWORD *dwPosition, DWORD *dwType);

		DWORD			GetMainToken(void);
		DWORD			GetToken(void);
		LPSTR			SkipAllComments ( LPSTR pPointer, LPSTR pPtrEnd );
		void			SkipToCR(void);
		LPSTR			SeekToSeperator(LPSTR pPointer, bool bAdvanceLine, bool bStopAtComment);
		LPSTR			SeekToCRReadOnly(LPSTR pPointer);
		LPSTR			SeekToRemEnd(LPSTR pPointer);
		DWORD			GetTokenToSeperator(void);
		DWORD			FindToken(LPSTR pPointer, bool bIncrementLineNumber);
		DWORD			DetermineNameToken(LPSTR pToken);
		DWORD			DetermineToken(LPSTR pToken);
		bool			DetermineIfReservedWord(LPSTR pToken);
		bool			DetermineIfFunctionName(LPSTR pWord, bool bIncludeUserFunctions);
		LPSTR			ProduceNextTokenEx(LPSTR* pString, bool bIncrementLineNumber, bool bProduceCRTK, bool bIncludeCommas, bool bIgnoreSpacesAsSeperators);
		LPSTR			ProduceNextToken(LPSTR* pString, bool bIncrementLineNumber, bool bProduceCRTK, bool bIncludeCommas);
		LPSTR			ProduceNextArrayToken(LPSTR* pOrigPointer);
		LPSTR			ProduceFullSegment(LPSTR* pString);
		LPSTR			SeperateInitFromType(LPSTR pPossibleTypeAndInit);
		bool			SeekCharAsPrevChar(unsigned char c, int* piBacktrak);
		bool			SeekCharAsNextChar(unsigned char c, DWORD* pdwAdvance);
		LPSTR			GetStringToEndOfLine(void);
		bool			SeperateValueFromArrayString(LPSTR* pArrayString, LPSTR* pArrValue, bool bMustBeLiteralDim);
		bool			ContainsAssignmentOperator(CStr* pString);
		bool			RemoveEdgeBracketFromSegment(LPSTR pPointer, DWORD *pdwSPos, DWORD *pdwEPos);
		bool			ExtractDetailsFromForNext(CStr* pVar, CStr* pInit, CStr* pEnd, CStr* pStep);
		bool			ReplaceTHENandELSEwithSep(void);

		DWORD			PeekToken(LPSTR pPointer);
		DWORD			PeekLabel(LPSTR pPointer);
		CStr*			GetLabel(LPSTR* pPointer);
		void			AdvancePastCRandSPACES(LPSTR* pPointerPtr);

		bool			AddInternalLabel(CStr** pReturnString);
		bool			FindCorrectInstruction(CInstructionTableEntry** pRef, CParameter* pFirstParameter, DWORD dwOrigValue, DWORD dwOrigType, DWORD dwOrigParamMax, DWORD* pdwValidInstructionToUse, bool* pbIfFindTypeA);
	
		DWORD			GetObjectLineNumber(void) { return m_dwLineNumber; }
		DWORD			GetObjectType(void) { return m_dwObjectType; }
		void*			GetObjectClass(void) { return m_pObjectClass; }
		CParameter*		GetParameter(void) { return m_pParameters; }

		bool			WriteDBM(void);
		bool			WriteDBMBit(DWORD dwLineNumber, LPSTR pText, LPSTR pResult);

	private:

		// Debug Data
		DWORD			m_dwLineNumber;
		DWORD			m_dwStartChar;
		DWORD			m_dwEndChar;
		bool			m_bPerformJumpChecks;

		// Object Data
		DWORD			m_dwObjectType;
		void			*m_pObjectClass;
		CParameter*		m_pParameters;

		// Hierarchy Data
		CStatement		*m_pNext;
};

class CParameter  
{
	public:
		CParameter();
		virtual ~CParameter();

		CMathOp* GetMathItem(void) { return m_pMainMathOp; }
		CParameter* GetNext(void) { return m_pNext; }
		CParameter* GetPrev(void) { return m_pPrev; }
		void SetNext(CParameter* pPtr) { m_pNext=pPtr; }
		CParameter* GetLast(void);

		void Add(CParameter* pParameter);
		void SetMathItem(CMathOp* pItem) { m_pMainMathOp = pItem; }
		bool MakeParamList(CStr* pStrList);
		bool ValidateWithCorrectCall(CStr* pValidParamTypes, DWORD* pdwScore, DWORD dwInternalCode);
		bool CastAllParametersToInstruction(CInstructionTableEntry* pRef);

		bool SetParamAsLabel(CStr* pStr);
		DWORD Count(void);

		bool WriteDBM(void);

	private:

		// Uses math object to store value
		CMathOp*			m_pMainMathOp;

		// Hierarchy Data
		CParameter*			m_pNext;
		CParameter*			m_pPrev;
};

class CMathOp
{
	public:
		CMathOp();
		virtual ~CMathOp();
		CMathOp* GetNext(void) { return m_pNext; }

		void SetResult(LPSTR pString, DWORD dwType, DWORD dwDataOffset);
		void SetResultData(CResultData ResultData);
		void SetResultType(DWORD dwType) { m_Result.m_dwType=dwType; }
		void SetResultStruct(CStructTable* pStruct) { m_Result.m_pStruct=pStruct; }
		void SetArrayOffsetResult(LPSTR pString);
		CStr* FindResultStringTokenForDBM(void);
		CResultData* FindResultDataForDBM(void);
		DWORD FindResultTypeValueForDBM(void);

		CResultData* GetResultData(void);
		CResultData* FindResultData(void);
		CStr* GetResultStringToken(void) { return m_Result.m_pStringToken; }
		CStr* GetResultArrayOffset(void) { return m_Result.m_pAdditionalOffset; }
		DWORD GetResultType(void) { return m_Result.m_dwType; }
		DWORD GetResultDataOffset(void) { return m_Result.m_dwDataOffset; }
		
		DWORD GetResultOffsetLValueTypeValue(void) { return m_dwOffsetLValueTypeValue; }
		bool GetConcatFlag(void) { return m_bConcatFlagUsed; }
		void SetConcatFlag(bool bFlag) { m_bConcatFlagUsed=bFlag; }

		bool CalculateDataOffsetAndTypeFromFieldString(CStr* pVarName, DWORD dwArrayType, CStr* pFieldData, DWORD* pdwSize, DWORD* pdwLType, DWORD* pdwSizeOfWholeType, CStructTable** ppStruct);
		bool ResolveStructValue(CStr* pExpressionValue);
		DWORD TranslateStructToOffsetForDBM(CStr* pString);
		bool TranslateStringTokenForDBM(void);

		void SetLineNumber(DWORD dwLine) { m_dwLineNumber=dwLine; }
		DWORD GetLineNumber(void) { return m_dwLineNumber; }

		void SetMathSymbol(DWORD dwM) { m_dwMathSymbol=dwM; }
		DWORD GetMathSymbol(void) { return m_dwMathSymbol; }

		void Add(CMathOp* pNext);

		bool DoValue(CStr* pStr);
		bool DoCastOnMathOp(CMathOp** ppMathOp, DWORD dwTypeMode);
		bool DoValueFunction(CStr* pExpression);
		bool DoValueComplexVariable(CStr* pExpression);
		bool TokeniseStructuresOfDataString(CStr* pEntireData, DWORD* pdwLValueType);
		bool DoValueSingleVariable(CStr* pExpression);
		bool DoValueLiteral(CStr* pExpression, DWORD dwTypeValue);
		bool DoValueLabel(CStr* pExpressionValue);

		bool FindHighestPres(CStr* pStr, DWORD *dwPosition, DWORD *dwType, DWORD *dwSymbWidth, DWORD *dwIsSciNot);
		bool CheckForSymbol(CStr* pString, DWORD dwSP, DWORD *dwMathType, DWORD *dwPriority, DWORD *dwSymbolWidth);
		bool ProduceNewTempToken(CStr* pTempVarToken, DWORD dwTypeMode);

		bool IsReserved ( CStr* pExpressionValue );
		bool IsItLabelFollowedByBracket(CStr* pExpressionValue, DWORD *pdwLabelLength);
		bool IsFunction(CStr* pExpressionValue);
		bool IsLiteral(CStr* pExpressionValue, DWORD* pdwTypeValue);
		bool IsSingleVariable(CStr* pExpressionValue);
		bool IsLabel(CStr* pExpressionValue);
		bool IsComplexVariable(CStr* pExpressionValue);
		bool IsAnything(CStr* pExpressionValue);

		bool SearchForFunction(CStr* pExpressionValue);

		DWORD ChopOffOneItemFromLeft(CStr* pStr);
		DWORD ChopOffOneItemFromRight(CStr* pStr);

//		DWORD DetermineInternalCommandCode(DWORD dwMathSymbol, DWORD dwTypeValue);
		bool DetermineIfPointerMaths(DWORD dwMathSymbol, DWORD dwTypeValue);

		bool WriteDBM(void);
		bool WriteDBMBit(DWORD dwLineNumber);
		bool WriteDBMLine(DWORD dwLineNumber, LPSTR pText, LPSTR pResult);

	private:

		// Debug Data
		DWORD			m_dwLineNumber;

		// Result Data
		CResultData		m_Result;

		// Used when returnning type of L-value to original @address of structure
		DWORD			m_dwOffsetLValueTypeValue;

		// Used when need to determine if CONCAT Flag used in params (printc)
		bool			m_bConcatFlagUsed;

		// Further math operations
		DWORD			m_dwMathSymbol;
		CMathOp*		m_pLeftMathOp;
		CMathOp*		m_pRightMathOp;

		// Seen as a work class..
		CStatement*		m_pStatement;

		// Hierarchy Data
		CMathOp*		m_pNext;
};

#endif // !defined(AFX_STATEMENT_H__2A1543E2_9870_4E5E_B056_C09192997E8D__INCLUDED_)
