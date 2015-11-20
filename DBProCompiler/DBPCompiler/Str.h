// Str.h: interface for the CStr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STR_H__95C8EB95_D88D_48CF_9F08_36248C3E570E__INCLUDED_)
#define AFX_STR_H__95C8EB95_D88D_48CF_9F08_36248C3E570E__INCLUDED_

#include "windows.h"

#include <DB3.h>
#include <DB3Factory.h>

#include "PerfMacros.h"

class CResultData;

class CStr: public db3::TObject<CStr>
{
	public:
		CStr();
		virtual ~CStr();
	public:
		CStr(LPSTR pText);
		CStr(DWORD dwTextSize);
		void		Enlarge(DWORD length);
		LPSTR		GetStr(void) const { if(m_pStr) return m_pStr; else return ""; }
		double		GetValue(void) const;
		void		SetText(LPSTR pStr);
		void		SetText(CStr* pStrText);
		void		AddText(LPSTR pStr);
		void		AddText(CStr* pStrText);
		void		AddChar(char cChar);
		void		InsertText(LPSTR pStr);
		void		SetNumericText(DWORD dwNumText);
		void		SetUnsignedNumericText(DWORD dwNumText);
		void		SetDWORDNumericText(DWORD dwNumText);
		void		AddNumericText(DWORD dwNumText);
		void		AddDoubleText(double dNumText);
#ifdef __AARON_STRPERF__
		DWORD		Length(void) const { return m_dwLen; }
#else
		DWORD		Length(void) const { return strlen(m_pStr); }
#endif
		LPSTR		GetLeftOfPosition(DWORD Position) const;
		LPSTR		GetRightOfPosition(DWORD Position) const;
		void		CopyToPtr(LPSTR pPointer) const;
		void		CopyFromPtr(LPSTR pPointer, LPSTR pPointerEnd, DWORD length);

		bool		MakeUpper(void);
		bool		ReplaceSemicolons(void);
		bool		CheckChar(DWORD dwPos, unsigned char cChar) const;
		bool		CheckChars(DWORD dwPos, DWORD num, LPSTR pText) const;
		bool		SetChar(DWORD dwPos, DWORD value);
		DWORD		FindFirstChar(char cChar) const;
		DWORD		FindFirstCharAtBracketLevel(char cChar) const;
		DWORD		FindLastChar(char cChar) const;
		unsigned char GetChar(DWORD dwPos) const;

		bool		IsAlpha(DWORD dwPos) const;
		bool		IsAlphaNumericLabel(DWORD dwPos) const;
		bool		IsMathCharLabel(DWORD dwPos) const;
		bool		IsSpaceCharacter(DWORD dwPos) const;
		bool		ContainsASOperator(void) const;

		bool		IsSciNot(void) const;
		void		ResolveSciNot(void);

		void		EatTrailingEdgeSpacesandTabs(void);
		void		EatEdgeSpacesandTabs(DWORD* pdwHowMany);
		void		EatSpeechMarks(void);
		bool		EatLeadingChars(void);
		void		EatNonImportantChars(void);
		bool		CropEqualEdgeBrackets(DWORD* pdwHowMany);
		bool		CropAll(DWORD *pHowManyCroppedTotal);
		void		Shorten(DWORD dwNewLength);
		void		Reverse(void);
		void		EatChar(DWORD dwPos);

		bool		ConvertDataListToMathList(CStr* pDimensionString);

		bool		IsTextALabel(void) const;
		bool		IsTextASingleVariable(void) const;
		bool		IsTextAComplexVariable(void) const;
		bool		IsTextAnAlphaNumericValue(void) const;
		bool		IsTextInBrackets(void) const;
		bool		IsTextSpeechMarked(void) const;
		bool		IsTextArrayDimensionDef(void) const;
		bool		IsTextNumericValue(void) const;
		bool		IsTextIntegerOnlyValue(void) const;
		bool		IsTextHexValue(void) const;
		bool		IsTextBinaryValue(void) const;
		bool		IsTextOctalValue(void) const;
		bool		IsTextLValue(void) const;
		bool		IsTextRValue(void) const;

		DWORD		GetDWORDRepresentation(DWORD dwTypeValue, DWORD* dwExtraDWORD) const;
		bool		IsIntegerBiggerThanDWORD(void) const;
		bool		IsFloatBiggerThanDWORD(void) const;

		void		TrimToPathOnly(void);

		bool		TranslateForDBM(CResultData* pResultPtr);

		bool		IsConstant(void) const;

	private:
#ifdef __AARON_STRPERF__
		db3::uint	m_dwSize, m_dwLen;
		char		*m_pStr;
#else
		DWORD		m_dwSize;
		LPSTR		m_pStr;
#endif
};

#endif // !defined(AFX_STR_H__95C8EB95_D88D_48CF_9F08_36248C3E570E__INCLUDED_)
