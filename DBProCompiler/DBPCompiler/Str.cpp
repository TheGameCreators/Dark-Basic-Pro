// Str.cpp: implementation of the CStr class.
//
//////////////////////////////////////////////////////////////////////

//
//	NOTE: Heap allocations are MORE expensive than stack allocations. ;-)
//

#include "macros.h"
#include "stdio.h"
#include "Str.h"
#include "float.h"
#include <math.h>

#include "StructTable.h"
#include "InstructionTable.h"
#include "DataTable.h"

// External Class Pointers
extern CStructTable *g_pStructTable;
extern CInstructionTable *g_pInstructionTable;
extern CDataTable *g_pStringTable;

#ifdef __AARON_STRPERF__
DB_ENTER_NS()
	
enum
{
	Granularity = 32
};

inline db3::uint AlignSize(db3::uint x)
{
	return (x + Granularity) - ((x + Granularity)%Granularity);
}

template<typename T>
static bool Memory(T *&P, uint n)
{
	void *&p = reinterpret_cast<void *&>(P);
	void *q;

	if (!n)
	{
		if (p != nullptr)
		{
			free(p);
			p = nullptr;
		}

		return true;
	}

	q = !p ? malloc(n) : realloc(p, n);
	if (!q)
		return false;

	p = q;
	return true;
}
static bool Duplicate(char *&dst, const char *src, db3::uint &cap, db3::uint &len, db3::uint srclen=0)
{
	char *p;
	db3::uint l, n;

	if (!src)
		return Memory(dst, (cap = len = 0));

	p = dst;
	l = srclen ? srclen : strlen(src);

	if (l >= cap)
	{
		n = AlignSize(l + 1);

		if (!Memory(p, n))
			return false;

		cap = n;
	}

	memcpy(p, src, l);
	p[l] = '\0';

	dst = p;
	len = l;

#ifdef _DEBUG
	if (len != strlen(dst))
		DB3_CRASH_MSG("len != strlen(dst)");
#endif

	return true;
}

DB_LEAVE_NS()

static struct SCaseConvMap
{
	db3::uint Convert[256];

	inline SCaseConvMap()
	{
		int x;

		for(x=0; x<256; x++)
		{
			Convert[x] = x;
		}

		for(x='A'; x<='Z'; x++)
		{
			Convert[x] = x - 'A' + 'a';
			Convert[x] |= 1<<8;
			Convert[x - 'A' + 'a'] |= 1<<8;
		}

		for(x='0'; x<='9'; x++)
			Convert[x] |= 1<<9;

		Convert['_'] |= 1<<8; //underscore is treated as alpha...
	}
	__forceinline char Get(char c)
	{
		return Convert[static_cast<db3::u8>(c)];
	}
	__forceinline bool IsAlpha(char c)
	{
		return (Convert[static_cast<db3::u8>(c)]>>8)&true;
	}
	__forceinline bool IsDigit(char c)
	{
		return (Convert[static_cast<db3::u8>(c)]>>9)&true;
	}
} g_ConvMap;

__forceinline bool CmpChars(char a, char b)
{
	db3::u8 x = g_ConvMap.Get(a) - g_ConvMap.Get(b);
	db3::u8 y = (x & 0x7F) + 0x7F;
	y = ~(y | x | 0x7F);

	return (bool)((y>>7) & true);
}
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifdef __AARON_STRPERF__
# define CTOR_INIT_LEN(x) ,m_dwLen(x)
#else
# define CTOR_INIT_LEN(x)
#endif
#define CTOR_INIT_NULL() m_dwSize(0) CTOR_INIT_LEN(0), m_pStr(nullptr)

#define IS_CHECK_LENGTH_OK 0
#if defined(_DEBUG) && defined(__AARON_STRPERF__) && IS_CHECK_LENGTH_OK
static void CheckLength(const char *p, db3::uint l)
{
	if (strlen(p) != l)
		DB3_CRASH_MSG("strlen(p) != l");
}
#else
# define CheckLength(p,l) (void)0
#endif

CStr::CStr(): CTOR_INIT_NULL()
{
}
CStr::~CStr()
{
#ifdef __AARON_STRPERF__
	db3::Memory(m_pStr, 0);
#else
	SAFE_DELETE(m_pStr);
#endif
}

CStr::CStr(LPSTR pText): CTOR_INIT_NULL()
{
#ifdef __AARON_STRPERF__
	db3::Duplicate(m_pStr, pText, m_dwSize, m_dwLen);
#else
	if(pText)
	{
		int length=strlen(pText);
		m_pStr = new char[length+1];
		if(m_pStr) ZeroMemory(m_pStr, length+1);
		if(m_pStr) strcpy(m_pStr, pText);
		m_dwSize = length;
	}
	else
	{
		int length=0;
		m_pStr = new char[length+1];
		if(m_pStr) ZeroMemory(m_pStr, length+1);
		m_dwSize = length;
	}
#endif
}

CStr::CStr(DWORD dwTextSize): CTOR_INIT_NULL()
{
#ifdef __AARON_STRPERF__
	dwTextSize++;

	if (db3::Memory(m_pStr, dwTextSize))
	{
		memset(m_pStr, 0, dwTextSize);

		m_dwSize = dwTextSize;
		m_dwLen = 0;
	}
#else
	int length=dwTextSize;
	m_pStr = new char[length+1];
	if(m_pStr) ZeroMemory(m_pStr, length+1);
	m_dwSize = length;
#endif
}

void CStr::Enlarge(DWORD length)
{
#ifdef __AARON_STRPERF__
	if (length < m_dwSize)
		return;

	db3::uint n = db3::AlignSize(length + 1);

	if (!db3::Memory(m_pStr, n))
		return;

	m_dwSize = n;
	if (m_dwLen >= m_dwSize)
	{
		m_dwLen = m_dwSize - 1;
		m_pStr[m_dwLen] = '\0';
	}

	CheckLength(m_pStr, m_dwLen);
#else
	char* pNewStr = new char[length+1];
	if(pNewStr) ZeroMemory(pNewStr, length+1);
	if(pNewStr && m_pStr) strcpy(pNewStr, m_pStr);
	if(m_pStr) delete m_pStr;
	m_dwSize=length;
	m_pStr=pNewStr;
#endif
}

void CStr::SetText(LPSTR pText)
{
#ifdef __AARON_STRPERF__
	db3::Duplicate(m_pStr, pText, m_dwSize, m_dwLen);
#else
	if(pText)
	{
		DWORD length=strlen(pText);
		if(length>m_dwSize) Enlarge(length);
		if(m_pStr) strcpy(m_pStr, pText);
	}
#endif
}

void CStr::SetText(CStr* pStrText)
{
#ifdef __AARON_STRPERF__
	db3::Duplicate(m_pStr, pStrText->m_pStr, m_dwSize, m_dwLen, pStrText->m_dwLen);
#else
	if(m_pStr && pStrText)
	{
		DWORD length=pStrText->Length();
		if(length>m_dwSize) Enlarge(length);
		if(m_pStr) strcpy(m_pStr, pStrText->GetStr());
	}
#endif
}

void CStr::AddText(LPSTR pText)
{
#ifdef __AARON_STRPERF__
	db3::uint len;
	
	if (!pText)
		return;

	len = strlen(pText);
	Enlarge(m_dwLen + len);

	if (m_dwLen + len >= m_dwSize)
		return; //failed

	memcpy(&m_pStr[m_dwLen], pText, len + 1);
	m_dwLen += len;

	CheckLength(m_pStr, m_dwLen);
#else
	if(pText)
	{
		DWORD length=strlen(pText);
		if(m_pStr) length+=strlen(m_pStr);
		if(length>m_dwSize) Enlarge(length);
		if(m_pStr) strcat(m_pStr, pText);
	}
#endif
}

void CStr::AddText(CStr* pStrText)
{
#ifdef __AARON_STRPERF__
	db3::uint len = pStrText->m_dwLen;
	Enlarge(m_dwLen + len);

	if (m_dwLen + len >= m_dwSize)
		return; //failed

	memcpy(&m_pStr[m_dwLen], pStrText->m_pStr, len + 1);
	m_dwLen += len;

	CheckLength(m_pStr, m_dwLen);
#else
	if(m_pStr && pStrText)
	{
		DWORD length=pStrText->Length();
		if(m_pStr) length+=strlen(m_pStr);
		if(length>m_dwSize) Enlarge(length);
		if(m_pStr) strcat(m_pStr, pStrText->GetStr());
	}
#endif
}

void CStr::InsertText(LPSTR pStr)
{
#ifdef __AARON_STRPERF__
	db3::uint len;

	if (!pStr)
		return;

	len = strlen(pStr);
	Enlarge(m_dwLen + len);

	memmove_s(&m_pStr[len], m_dwSize - len, m_pStr, m_dwLen + 1);
	memcpy(m_pStr, pStr, len);

	CheckLength(m_pStr, m_dwLen);
#else
	if(m_pStr && pStr)
	{
		DWORD length=strlen(pStr);
		if(m_pStr) length+=strlen(m_pStr);
		if(length>m_dwSize) Enlarge(length);
		if(m_pStr)
		{
			LPSTR pTemp = new char[length+1];
			strcpy(pTemp, m_pStr);
			strcpy(m_pStr, pStr);
			strcat(m_pStr, pTemp);
			delete pTemp;
		}
	}
#endif
}

void CStr::AddChar(char cChar)
{
#ifdef __AARON_STRPERF__
	char buf[2];

	buf[0] = cChar;
	buf[1] = '\0';

	AddText(buf);
#else
	if(m_pStr)
	{
		DWORD length=Length();
		if(length+1>m_dwSize) Enlarge(length+1);
		m_pStr[length]=cChar;
		m_pStr[length+1]=0;
	}
#endif
}

void CStr::SetNumericText(DWORD dwNumText)
{
#ifdef __AARON_STRPERF__
	char buf[32];

	buf[0] = '\0';
	_itoa_s(dwNumText, buf, sizeof(buf), 10);

	SetText(buf);
#else
	LPSTR pText = new char[32];
	itoa(dwNumText, pText, 10);
	if(pText)
	{
		DWORD length=strlen(pText);
		if(length>m_dwSize) Enlarge(length);
		if(m_pStr) strcpy(m_pStr, pText);
	}
	if(pText) delete pText;
#endif
}

void CStr::SetUnsignedNumericText(DWORD dwNumText)
{
#ifdef __AARON_STRPERF__
	char buf[32];

	buf[0] = '\0';
	_ultoa_s(dwNumText, buf, sizeof(buf), 10);

	SetText(buf);
#else
	LPSTR pText = new char[32];
	ultoa(dwNumText, pText, 10);
	if(pText)
	{
		DWORD length=strlen(pText);
		if(length>m_dwSize) Enlarge(length);
		if(m_pStr) strcpy(m_pStr, pText);
	}
	if(pText) delete pText;
#endif
}

void CStr::SetDWORDNumericText(DWORD dwNumText)
{
#ifdef __AARON_STRPERF__
	SetUnsignedNumericText(dwNumText);
#else
	LPSTR pText = new char[256];
	sprintf(pText, "%u", dwNumText);
	if(pText)
	{
		DWORD length=strlen(pText);
		if(length>m_dwSize) Enlarge(length);
		if(m_pStr) strcpy(m_pStr, pText);
	}
	if(pText) delete pText;
#endif
}

void CStr::AddNumericText(DWORD dwNumText)
{
#ifdef __AARON_STRPERF__
	char buf[32];

	buf[0] = '\0';
	_itoa_s(dwNumText, buf, sizeof(buf), 10);

	AddText(buf);
#else
	LPSTR pText = new char[32];
	itoa(dwNumText, pText, 10);
	if(pText)
	{
		DWORD length=strlen(pText);
		if(m_pStr) length+=strlen(m_pStr);
		if(length>m_dwSize) Enlarge(length);
		if(m_pStr) strcat(m_pStr, pText);
	}
	if(pText) delete pText;
#endif
}

void CStr::AddDoubleText(double dNumText)
{
#ifdef __AARON_STRPERF__
	char buf[32];

	buf[0] = '\0';
	sprintf_s(buf, sizeof(buf), "%f", dNumText);

	AddText(buf);
#else
	LPSTR pText = new char[128];
	sprintf(pText, "%f", dNumText);
//	gcvt(dNumText, 12, pText);
	if(pText)
	{
		DWORD length=strlen(pText);
		if(m_pStr) length+=strlen(m_pStr);
		if(length>m_dwSize) Enlarge(length);
		if(m_pStr) strcat(m_pStr, pText);
	}
	if(pText) delete pText;
#endif
}

double CStr::GetValue(void) const
{
	if(m_pStr)
	{
		return atof(m_pStr);
	}
	return 0; 
}

LPSTR CStr::GetLeftOfPosition(DWORD Position) const
{
#ifdef __AARON_STRPERF__
	char *pText = new char[Position + 1];
	if (!pText)
		return nullptr;

	memcpy(pText, m_pStr, Position);
	pText[Position] = '\0';

	return pText;
#else
	LPSTR pText = new char[Length()+1];
	if(pText)
	{
		strcpy(pText, GetStr());
		pText[Position]=0;
		return pText;
	}
	else
		return NULL;
#endif
}

LPSTR CStr::GetRightOfPosition(DWORD Position) const
{
#ifdef __AARON_STRPERF__
	db3::uint len;

	len = m_dwLen - Position;
	char *pText = new char[len + 1];
	if (!pText)
		return nullptr;

	memcpy(pText, &m_pStr[Position], len);
	pText[len] = '\0';

	return pText;
#else
	LPSTR pText = new char[Length()+1];
	if(pText)
	{
		strcpy(pText, GetStr());
		strrev(pText);
		pText[Length()-Position]=0;
		strrev(pText);
		return pText;
	}
	else
		return NULL;
#endif
}

void CStr::CopyToPtr(LPSTR pPointer) const
{
	if(m_pStr)
	{
#ifdef __AARON_STRPERF__
		memcpy(pPointer, m_pStr, m_dwLen);
#else
		DWORD length=strlen(m_pStr);
		for(DWORD n=0; n<length; n++)
			*(pPointer+n) = m_pStr[n];
#endif
	}
}

void CStr::CopyFromPtr(LPSTR pPointer, LPSTR pPointerEnd, DWORD length)
{
#ifdef __AARON_STRPERF__
	Enlarge(length + 1);
	if (length >= m_dwSize)
		return; //fail

	if (pPointer + length > pPointerEnd)
		length -= pPointerEnd - pPointer;

	memcpy(m_pStr, pPointer, length);
	m_pStr[length] = '\0';
	m_dwLen = strlen(m_pStr);
#else
	if(m_pStr)
	{
		if(length+1>m_dwSize) Enlarge(length+1);
		if(pPointer+length>pPointerEnd) length=length-(pPointerEnd-pPointer);
		for(DWORD n=0; n<length; n++)
			m_pStr[n] = *(pPointer+n);
		m_pStr[n] = 0;
	}
#endif
}

bool CStr::MakeUpper(void)
{
#ifdef __AARON_STRPERF__
	char *p;

	if (!m_pStr)
		return false;

	for(p=m_pStr; *p!='\0'; p++)
	{
		if (*p>='a' && *p<='z')
			*p = *p - 'a' + 'A';
	}

	return true;
#else
	if(m_pStr)
	{
		// Make string into upper case
		SetText(strupr(GetStr()));
		return true;
	}

	// soft fail
	return false;
#endif
}


bool CStr::ReplaceSemicolons(void)
{
	bool bConcatFlag=false;
	if(m_pStr && Length()>0)
	{
		// If find semi-colon as last char, set concat flag
		if(GetChar(Length()-1)==';')
		{
			SetChar(Length()-1,' ');
			bConcatFlag=true;
		}

		// Replace semi colins (-1 BUT Not last one (concat token))
		DWORD dwSpeechMark=0;
		for(DWORD n=0; n<Length()-1; n++)
		{
			if(GetChar(n)=='"') dwSpeechMark=1-dwSpeechMark;
			if(dwSpeechMark==0)
			{
				if(GetChar(n)==';')
				{
					SetChar(n,',');
				}
			}
		}
	}

	CheckLength(m_pStr, m_dwLen);
	return bConcatFlag;
}

bool CStr::CheckChar(DWORD dwPos, unsigned char cChar) const
{
#ifdef __AARON_STRPERF__
	if (!m_pStr)
		return false;

	return CmpChars(m_pStr[dwPos], cChar);
#else
	if(m_pStr)
	{
		int diff='a'-'A';
		unsigned char schar = (unsigned char)m_pStr[dwPos];
		if(schar>='a' && schar<='z') schar-=diff;
		if(schar==cChar)
			return true;
	}

	// soft fail
	return false;
#endif
}

bool CStr::CheckChars(DWORD dwPos, DWORD num, LPSTR pText) const
{
#ifdef __AARON_STRPERF__
	if (!m_pStr || !pText)
		return false;

	db3::uint n=static_cast<db3::uint>(num);
	for(db3::uint i=0; i<n; i++)
	{
		if (!CmpChars(m_pStr[dwPos + i], pText[i]))
			return false;
	}

	return true;
#else
	if(m_pStr)
	{
		if(pText)
		{
			int diff='a'-'A';
			for(DWORD n=0; n<num; n++)
			{
				unsigned char schar = (unsigned char)m_pStr[dwPos+n];
				if(schar>='a' && schar<='z') schar-=diff;
				if(schar!=pText[n])
				{
					// soft fail
					return false;
				}
			}
			return true;
		}
	}

	// soft fail
	return false;
#endif
}

bool CStr::SetChar(DWORD dwPos, DWORD value)
{
	if(m_pStr)
	{
		m_pStr[dwPos]=(unsigned char)value;
		if (!value)
		{
			if (dwPos < m_dwLen)
				m_dwLen = dwPos;
		}

		CheckLength(m_pStr, m_dwLen);
		return true;
	}
	// soft fail
	return false;
}

unsigned char CStr::GetChar(DWORD dwPos) const
{
	if(m_pStr)
		return m_pStr[dwPos];
	else
		return 0;
}

DWORD CStr::FindFirstChar(char cChar) const
{
	if(m_pStr)
	{
#ifdef __AARON_STRPERF__
		char *p;

		p = strchr(m_pStr, cChar);
		if (!p)
			return 0;

		return p - m_pStr;
#else
		DWORD length=Length();
		for(DWORD n=0; n<length; n++)
			if(m_pStr[n]==cChar)
				return n; 
#endif
	}
	return 0;
}

DWORD CStr::FindFirstCharAtBracketLevel(char cChar) const
{
	//
	//	TODO: Refactor this
	//
	int iBracketCount=0;
	int iSpeech=0;
	if(m_pStr)
	{
		DWORD length=Length();
		for(DWORD n=0; n<length; n++)
		{
			if(m_pStr[n]=='"') iSpeech=1-iSpeech;
			if(iSpeech==0)
			{
				if(m_pStr[n]=='(') iBracketCount++;
				if(m_pStr[n]==')') iBracketCount--;
				if(m_pStr[n]==cChar && iBracketCount==0)
					return n; 
			}
		}
	}
	return 0;
}

DWORD CStr::FindLastChar(char cChar) const
{
	if(m_pStr)
	{
		DWORD length=Length();
		for(int n=length-1; n>=0; n--)
			if(m_pStr[n]==cChar)
				return n; 
	}
	return 0;
}

bool CStr::IsAlpha(DWORD dwPos) const
{
	if(m_pStr)
	{
#ifdef __AARON_STRPERF__
		return g_ConvMap.IsAlpha(m_pStr[dwPos]);
#else
		if(	(m_pStr[dwPos]=='_')
		||	(m_pStr[dwPos]>='a' && m_pStr[dwPos]<='z') 
		||	(m_pStr[dwPos]>='A' && m_pStr[dwPos]<='Z') )
		{
			return true;
		}
#endif
	}
	// soft fail
	return false;
}

bool CStr::IsAlphaNumericLabel(DWORD dwPos) const
{
	if(m_pStr)
	{
#ifdef __AARON_STRPERF__
		return g_ConvMap.IsAlpha(m_pStr[dwPos]) | g_ConvMap.IsDigit(m_pStr[dwPos]);
#else
		if(	(m_pStr[dwPos]=='_')
		||	(m_pStr[dwPos]>='a' && m_pStr[dwPos]<='z') 
		||	(m_pStr[dwPos]>='A' && m_pStr[dwPos]<='Z')
		||	(m_pStr[dwPos]>='0' && m_pStr[dwPos]<='9') )
		{
			return true;
		}
#endif
	}
	// soft fail
	return false;
}

bool CStr::IsMathCharLabel(DWORD dwSP) const
{
	if(m_pStr)
	{
		if(CheckChar	(dwSP,'^'))				return true;
		if(CheckChar	(dwSP,'*'))				return true;
		if(CheckChar	(dwSP,'/'))				return true;
		if(CheckChar	(dwSP,'+'))				return true;
		if(CheckChar	(dwSP,'-'))				return true;
		if(CheckChar	(dwSP,'='))				return true;
		if(CheckChar	(dwSP,'>'))				return true;
		if(CheckChar	(dwSP,'<'))				return true;
		if(CheckChars	(dwSP,2,">>"))			return true;
		if(CheckChars	(dwSP,2,"<<"))			return true;
		if(CheckChars	(dwSP,2,"<>"))			return true;
		if(CheckChars	(dwSP,2,">="))			return true;
		if(CheckChars	(dwSP,2,"=>"))			return true;
		if(CheckChars	(dwSP,2,"<="))			return true;
		if(CheckChars	(dwSP,2,"=<"))			return true;
		if(CheckChars	(dwSP,4," OR "))		return true;
		if(CheckChars	(dwSP,5," AND "))		return true;
		if(CheckChars	(dwSP,5," NOT "))		return true;
		if(CheckChars	(dwSP,5," XOR "))		return true;
	}
	// soft fail
	return false;
}

bool CStr::IsSpaceCharacter(DWORD dwPos) const
{
	if(m_pStr)
	{
		if(	(m_pStr[dwPos]==' ') )
		{
			return true;
		}
	}
	// soft fail
	return false;
}

bool CStr::ContainsASOperator(void) const
{
	DWORD dwOnlySpacesNow=0;
	DWORD dwInSpeechMarks=0;
	if(m_pStr)
	{
		DWORD length = Length();
		for(DWORD n=0; n<length; n++)
		{
			if(dwOnlySpacesNow==0 && IsAlphaNumericLabel(n)==false)
				dwOnlySpacesNow=1;

			if(dwOnlySpacesNow==1 && IsAlphaNumericLabel(n)==true)
			{
				// soft fail
				return false;
			}

			if(m_pStr[n]=='"') dwInSpeechMarks=1-dwInSpeechMarks;
			if(dwInSpeechMarks==0)
			{
				if(strnicmp((m_pStr+n)," as ",4)==NULL)
					return true;
			}
		}
	}
	// soft fail
	return false;
}

bool CStr::IsSciNot(void) const
{
	bool bStillValid=true;
	DWORD dwN = 0;
	if(m_pStr)
	{
		//DWORD length=strlen(m_pStr);
		DWORD length = m_dwLen;
		for(DWORD dwPos=0; dwPos<length; dwPos++)
		{
			if(	(m_pStr[dwPos]=='+')
			||	(m_pStr[dwPos]=='-') 
			||	(m_pStr[dwPos]=='.') 
			||	(m_pStr[dwPos]=='e') 
			||	(m_pStr[dwPos]=='E')
			||	(m_pStr[dwPos]>='0' && m_pStr[dwPos]<='9') )
			{
				// valid
				if(m_pStr[dwPos]=='e') dwN = dwPos;
				if(m_pStr[dwPos]=='E') dwN = dwPos;
			}
			else
			{
				// not valid
				bStillValid=false;
				break;
			}
		}
	}

	// all characters for scifi not, check if format okay
	if ( bStillValid && dwN > 0 ) 
	{
		dwN--; // dwN was at E
		bool bANonNumericInStringCanOnlyBeVariable = false;
		int iDetectNotation = 1;
		int iN = dwN;
		while ( iN>=0 )
		{
			bool bActivity = false;
			if ( iDetectNotation==1 && iN>=0 )
			{
				unsigned char num = m_pStr[iN];
				if ( num>='0' && num<='9' )
				{
					// found first numeric before E, this may be scientific notation!
					iDetectNotation=2;
					iN--;
				}
				else
				{
					if ( m_pStr[iN]==' ' )
					{
						// allow spaces to pass
					}
					else
					{
						// anything else and this is no scientific notation!
						break;
					}
				}
			}
			if ( iDetectNotation==2 && iN>=0 )
			{
				unsigned char num = m_pStr[iN];
				if ( num=='.' || (num>='0' && num<='9') )
				{
					// numerics are allowed
				}
				else
				{
					if ( m_pStr[iN]==' ' )
					{
						// allow spaces to pass
					}
					else
					{
						// anything else and this is no scientific notation!
						bANonNumericInStringCanOnlyBeVariable = true;
						break;
					}
				}
			}
			iN--;
		}
		if ( bANonNumericInStringCanOnlyBeVariable==true ) iDetectNotation=0;
		if ( iDetectNotation!=2 ) bStillValid  = false;
	}
	else
	{
		// has numerics of sci but not structure
		bStillValid = false;
	}

	// return whether scifi not value or not
	return bStillValid;
}

void CStr::ResolveSciNot(void)
{
	// turn a scifi not into a regular large number for compiler
	EatNonImportantChars();
	DWORD dwE = FindFirstChar('e');
	if ( dwE==0 ) dwE = FindFirstChar('E');
	if ( dwE>0 )
	{
		CStr pExpressionValueL(1), pExpressionValueR(1);
		char str[512];
		//CStr* pExpressionValueL = new CStr(1);
		pExpressionValueL.SetText(this);
		pExpressionValueL.Shorten(dwE);
		//CStr* pExpressionValueR = new CStr(1);
		pExpressionValueR.SetText(this->GetStr()+dwE+1);
		double dLeftValue = atof ( pExpressionValueL.GetStr() );
		double dRightValue = atof ( pExpressionValueR.GetStr() );
		double dPowerValue = pow ( 10, dRightValue );
		dLeftValue = dLeftValue * dPowerValue;
		sprintf ( str, "%.23f", dLeftValue ); //was "%.63f" which is... waaaaaay more than is possible
		SetText ( str );
		//SAFE_DELETE ( pExpressionValueL );
		//SAFE_DELETE ( pExpressionValueR );
	}

	CheckLength(m_pStr, m_dwLen);
}

void CStr::EatEdgeSpacesandTabs(DWORD* pdwHowMany)
{
#ifdef __AARON_STRPERF__
	char *s, *e;

	if (!m_pStr)
		return;

	for(s=m_pStr; *s<=' '; s++)
	{
		if (*s=='\0')
			break;
	}

	for(e = &m_pStr[m_dwLen]; *e<=' '; e--)
	{
		if (e == s)
			break;
	}

	// "  abc   "
	//  012345678
	//..  ^
	// "  abc   "
	//  012345678
	//..    ^

	if (*e!='\0')
		e++;

	if (pdwHowMany && *pdwHowMany==0)
		*pdwHowMany = s - m_pStr;

	m_dwLen = e - s;

	memmove_s(m_pStr, m_dwSize, s, m_dwLen);
	m_pStr[m_dwLen] = '\0';

	CheckLength(m_pStr, m_dwLen);
#else
	if(m_pStr)
	{
		for(int twice=0; twice<2; twice++)
		{
			bool bOnlyStart=true;
			DWORD w=0;
			DWORD length = Length();
			for(DWORD n=0; n<length; n++)
			{
				if((unsigned char)m_pStr[n]!=32
				&& (unsigned char)m_pStr[n]!=9
				&& (unsigned char)m_pStr[n]!=13
				&& (unsigned char)m_pStr[n]!=10)
				{
					// Store number of chars eaten
					if(pdwHowMany && twice==0 && bOnlyStart==true)
						if(*pdwHowMany==0)
							*pdwHowMany=n;

					bOnlyStart=false;
				}
				
				if(bOnlyStart==false)
					m_pStr[w++]=m_pStr[n];
			}
			m_pStr[w++]=0;

			// Reverse string
			strrev(m_pStr);
		}
	}
#endif
}

void CStr::EatTrailingEdgeSpacesandTabs(void)
{
#ifdef __AARON_STRPERF__
	if (!m_pStr)
		return;

	char *e;
	for(e=&m_pStr[m_dwLen]; *e<=' '; e--)
	{
		if (e == m_pStr)
			break;
	}

	e++;

	*e = '\0';
	m_dwLen = e - m_pStr;

	CheckLength(m_pStr, m_dwLen);
#else
	if(m_pStr)
	{
		// Reverse string
		strrev(m_pStr);

		// Eat spaces at start
		bool bOnlyStart=true;
		DWORD w=0;
		DWORD length = Length();
		for(DWORD n=0; n<length; n++)
		{
			if((unsigned char)m_pStr[n]!=32
			&& (unsigned char)m_pStr[n]!=9
			&& (unsigned char)m_pStr[n]!=13
			&& (unsigned char)m_pStr[n]!=10)
				if(bOnlyStart==true)
					bOnlyStart=false;
			
			if(bOnlyStart==false)
				m_pStr[w++]=m_pStr[n];
		}
		m_pStr[w++]=0;

		// Reverse string
		strrev(m_pStr);
	}
#endif
}

void CStr::EatChar(DWORD dwPos)
{
#ifdef __AARON_STRPERF__
	if (!m_pStr || dwPos >= m_dwLen)
		return;

	memmove_s(&m_pStr[dwPos], m_dwSize - dwPos, &m_pStr[dwPos + 1], m_dwLen - dwPos);
	m_dwLen--;

	CheckLength(m_pStr, m_dwLen);
#else
	if(m_pStr)
	{
		DWORD dwLen = strlen(m_pStr);
		for(DWORD n=dwPos; n<dwLen; n++)
		{
			m_pStr[n]=m_pStr[n+1];
		}
		m_dwLen--;
	}
#endif
}

void CStr::EatSpeechMarks(void)
{
#ifdef __AARON_STRPERF__
	if (!m_pStr || m_dwLen < 1)
		return;

	if (m_pStr[0]!='\"' || m_pStr[m_dwLen - 1]!='\"')
		return;

	memmove_s(m_pStr, m_dwSize, &m_pStr[1], m_dwLen - 2);
	m_dwLen -= 2;

	m_pStr[m_dwLen] = '\0';

	CheckLength(m_pStr, m_dwLen);
#else
	if(m_pStr)
	{
		if(IsTextSpeechMarked())
		{
			int length = Length()-2;
			if ( length >= 0 )
			{
				for(int n=0; n<length; n++)
				{
					m_pStr[n]=m_pStr[n+1];
				}
				m_pStr[n]=0;
			}
		}
	}
#endif
}

bool CStr::EatLeadingChars(void)
{
#ifdef __AARON_STRPERF__
	char *s, *p, *e;

	if (!m_pStr || !m_dwLen)
		return false;

	for(s=m_pStr; *s==' ' || *s=='\t'; s++)
	{
		if (*s=='\0')
			break;
	}

	for(p = &m_pStr[m_dwLen - 1]; *p==' ' || *p=='\t'; p--)
	{
		if (p <= s)
			break;
	}

	e = p + 1;
	m_dwLen = e - s;

	memmove_s(m_pStr, m_dwSize, s, m_dwLen);
	m_pStr[m_dwLen] = '\0';

	CheckLength(m_pStr, m_dwLen);

	return true;
#else
	if(m_pStr)
	{
		for(int twice=0; twice<2; twice++)
		{
			bool bOnlyStart=true;
			DWORD w=0;
			DWORD length = Length();
			for(DWORD n=0; n<length; n++)
			{
				if((unsigned char)m_pStr[n]!=32
				&& (unsigned char)m_pStr[n]!=9)
					bOnlyStart=false;
				
				if(bOnlyStart==false)
					m_pStr[w++]=m_pStr[n];
			}
			m_pStr[w++]=0;

			// Reverse string
			strrev(m_pStr);
		}
	}
	return true;
#endif
}

void CStr::EatNonImportantChars(void)
{
	if(m_pStr)
	{
		DWORD dwInSpeechMarks=0;
		CStr pNewStr("");
		DWORD length=Length();
		for(DWORD n=0; n<length; n++)
		{
			bool bValid=true;
			if(m_pStr[n]=='"') dwInSpeechMarks=1-dwInSpeechMarks;
			if(dwInSpeechMarks==0)
				if(IsSpaceCharacter(n)) bValid=false;

			if(bValid)
				pNewStr.AddChar(m_pStr[n]);
		}
		SetText(pNewStr.GetStr());
		//SAFE_DELETE(pNewStr);
	}

	CheckLength(m_pStr, m_dwLen);
}

bool CStr::CropEqualEdgeBrackets(DWORD* pdwHowMany)
{
	if(m_pStr)
	{
		int iSpeechMarks=0;
		DWORD dwOpenPos=0;
		int iBracketCount=0;
		DWORD length=Length();
		for(DWORD n=0; n<length; n++)
		{
			if(m_pStr[n]=='"') iSpeechMarks=1-iSpeechMarks;
			if(iSpeechMarks==0)
			{
				if(m_pStr[n]=='(')
				{
					if(iBracketCount==0 && dwOpenPos==0 && n==0)
					{
						dwOpenPos=1+n;
					}
					iBracketCount++;
				}
				if(m_pStr[n]==')')
				{
					iBracketCount--;

					// Check if this is last bracket
					bool bBracketIsAtEnd=false;
					if(m_pStr[n+1]==0) bBracketIsAtEnd=true;

					// Gobble the bracket pair
					if(iBracketCount==0 && dwOpenPos>0)
					{
						if(bBracketIsAtEnd)
						{
							m_pStr[dwOpenPos-1]=32;
							m_pStr[n]=32;
						}
						break;
					}
				}
			}
		}
		DWORD oldLen=Length();
		EatEdgeSpacesandTabs(NULL);
		DWORD iNestCount=oldLen-Length();
		if(pdwHowMany) *pdwHowMany = iNestCount;
		CheckLength(m_pStr, m_dwLen);
		if(iNestCount>0) return true;
	}
	// soft fail
	return false;
}

bool CStr::CropAll(DWORD *pHowManyCroppedTotal)
{
	bool bStay=true;
	while(bStay)
	{
		// Eat all spaces and count ones ate at start
		DWORD HowManyStartSpacesCropped=0;
		EatEdgeSpacesandTabs(&HowManyStartSpacesCropped);
		*pHowManyCroppedTotal+=HowManyStartSpacesCropped;

		// Eat all bracket pairs and count ones ate at start
		DWORD HowManyStartBracketsCropped=0;
		if(CropEqualEdgeBrackets(&HowManyStartBracketsCropped)==false) bStay=false;
		*pHowManyCroppedTotal+=HowManyStartBracketsCropped;
	}

	CheckLength(m_pStr, m_dwLen);
	return true;
}

void CStr::Shorten(DWORD dwNewLength)
{
	if(m_pStr)
	{
		m_pStr[dwNewLength]=0;
#ifdef __AARON_STRPERF__
		m_dwLen = dwNewLength;
#endif
	}

	CheckLength(m_pStr, m_dwLen);
}

void CStr::Reverse(void)
{
	if(m_pStr) strrev(m_pStr);
}

bool CStr::ConvertDataListToMathList(CStr* pDimensionString)
{
	// Create new string
	DWORD dwPos=0;
	//CStr* pNewStr = new CStr("");
	CStr pNewStr(pDimensionString ? "(" : "");

	// Full dimension strings
	//CStr* pDimStr = NULL;
	//CStr* pDimItem = new CStr("");
	CStr pDimStr(pDimensionString ? pDimensionString->GetStr() : nullptr);
	CStr pDimItem("");

	int iBracketCount=0;
	DWORD dwInSpeechMarks=0;
	if(m_pStr)
	{
		DWORD length=Length();
		for(DWORD n=0; n<length; n++)
		{
			if(m_pStr[n]=='(') iBracketCount++;
			if(m_pStr[n]==')') iBracketCount--;
			if(iBracketCount==0 && m_pStr[n]==',')
			{
				if(pDimensionString==NULL)
				{
					// normal size calc
					pNewStr.AddText("*");
				}
				else
				{
					// Build proper calc to array mem
					if(pDimItem.Length()==0)
					{
						pNewStr.AddText(")+(");
					}
					else
					{
						pNewStr.AddText(")");
						pNewStr.AddText("*");
						pNewStr.AddText(pDimItem.GetStr());
						pNewStr.AddText("+(");
					}

					// Get next dimension item
					DWORD dwSep = pDimStr.FindFirstChar(',');
					if(dwSep==0) dwSep=pDimStr.Length();
					//LPSTR pRest = pDimStr.GetRightOfPosition(dwSep);
					char *pRest = &pDimStr.m_pStr[dwSep];
					pDimStr.SetChar(dwSep,0);
					pDimItem.SetText(pDimStr.GetStr());
					if(pRest)
					{
						//if(strlen(pRest)>1)
						if (pDimStr.Length() - dwSep > 1)
						{
							pDimStr.SetText(pRest+1);
							//SAFE_DELETE(pRest);
						}
					}
					else
						pDimStr.SetText("");
				}
				n++;
			}

			// build new str
			pNewStr.AddChar(m_pStr[n]);
		}
		if(pDimensionString)
		{
			if(Length()==0)
			{
				pNewStr.SetText("");
			}
			else
			{
				pNewStr.AddText(")");
				pNewStr.AddText("*");
				pNewStr.AddText(pDimItem.GetStr());
			}
		}
	}

	// Pass new string as actual
	SetText(pNewStr.GetStr());

	// Free usages
	//SAFE_DELETE(pDimItem);
	//SAFE_DELETE(pDimStr);
	//SAFE_DELETE(pNewStr);

	// Complete
	CheckLength(m_pStr, m_dwLen);
	return true;
}

bool CStr::IsTextALabel(void) const
{
	bool bStillValid=false;
	if(m_pStr)
	{
		DWORD length=m_dwLen;//strlen(m_pStr);
		for(DWORD n=0; n<length; n++)
		{
			bStillValid=false;
			if(IsAlphaNumericLabel(n))			bStillValid=true;
			if(m_pStr[n]=='_')					bStillValid=true;
			if(n==length-1 && m_pStr[n]==':')	bStillValid=true;
			if(bStillValid==false)
				break;
		}

		// Label Must have colon as last character
		if(m_pStr[length-1]!=':')
			bStillValid=false;
	}

	return bStillValid;
}

bool CStr::IsTextASingleVariable(void) const
{
	bool bStillValid=false;
	if(m_pStr)
	{
		DWORD length=m_dwLen;//strlen(m_pStr);
		for(DWORD n=0; n<length; n++)
		{
			bStillValid=false;
			if(IsAlpha(n) && n==0)				bStillValid=true;
			if(IsAlphaNumericLabel(n) && n>0 )	bStillValid=true;
			if(m_pStr[n]=='*' && n==0)			bStillValid=true;
			if(m_pStr[n]=='#' && n==(length-1))	bStillValid=true;
			if(m_pStr[n]=='$')					bStillValid=true;
			if(m_pStr[n]=='@' && n==0 && (m_pStr[n+1]=='$' || m_pStr[n+1]==':')) bStillValid=true;

			if(bStillValid==false)
				break;
		}
	}

	return bStillValid;
}

bool CStr::IsTextAComplexVariable(void) const
{
	int iBracketCount=0;
	DWORD dwInSpeechMarks=0;
	bool bStillValid=false;
	if(m_pStr)
	{
		DWORD length=m_dwLen;//strlen(m_pStr);
		for(DWORD n=0; n<length; n++)
		{
			if(dwInSpeechMarks==0)
			{
				if(m_pStr[n]=='(') iBracketCount++;
				if(m_pStr[n]==')') iBracketCount--;
			}
			if(m_pStr[n]=='"') dwInSpeechMarks=1-dwInSpeechMarks;

			bStillValid=false;
			if(iBracketCount>0)
			{
				if(IsSpaceCharacter(n))				bStillValid=true;
			}
			if(dwInSpeechMarks==0 && iBracketCount==0)//LEEFIX - 171002 - Added Count==0 so anything inside brackets does not matter...
			{
				if(IsAlphaNumericLabel(n))			bStillValid=true;
				if(IsMathCharLabel(n))				bStillValid=true;
				if(m_pStr[n]=='#')					bStillValid=true;
				if(m_pStr[n]=='$')					bStillValid=true;
				if(m_pStr[n]=='@')					bStillValid=true;
				if(m_pStr[n]=='(')					bStillValid=true;
				if(m_pStr[n]==')')					bStillValid=true;
				if(m_pStr[n]=='.')					bStillValid=true;
				if(m_pStr[n]==',')					bStillValid=true;
			}
			else
				bStillValid=true;

			if(bStillValid==false)
				break;
		}
	}
	if(dwInSpeechMarks==1)
		bStillValid=false;

	return bStillValid;
}

bool CStr::IsTextAnAlphaNumericValue(void) const
{
	bool bStillValid=true;
	if(m_pStr)
	{
		DWORD length=m_dwLen;//strlen(m_pStr);
		for(DWORD n=0; n<length; n++)
		{
			if(IsAlphaNumericLabel(n)==false)
			{
				bStillValid=false;
				break;
			}
		}
	}
	return bStillValid;
}

bool CStr::IsTextInBrackets(void) const
{
	if(m_pStr)
	{
		DWORD length=m_dwLen;//strlen(m_pStr);
		if(m_pStr[0]=='(' && m_pStr[length-1]==')')
			return true;
	}
	// soft fail
	return false;
}

bool CStr::IsTextSpeechMarked(void) const
{
	if(m_pStr)
	{
		DWORD length=m_dwLen;//strlen(m_pStr);
		if(m_pStr[0]=='"' && m_pStr[length-1]=='"')
			return true;
	}
	// soft fail
	return false;
}

bool CStr::IsTextArrayDimensionDef(void) const
{
	bool bStillValid=true;
	if(m_pStr)
	{
		DWORD length=m_dwLen;//strlen(m_pStr);
		for(DWORD n=0; n<length; n++)
		{
			// Valid Decimal Symbols 0-9, +- and .
			bool bThisCharValid=false;
			if(m_pStr[n]>='0' && m_pStr[n]<='9')				bThisCharValid=true;
			if(m_pStr[n]==',')									bThisCharValid=true;

			if(bThisCharValid==false)
			{
				bStillValid=false;
				break;
			}
		}
	}
	return bStillValid;
}

bool CStr::IsTextNumericValue(void) const
{
	bool bStillValid=true;
	if(m_pStr)
	{
		DWORD length=m_dwLen;//strlen(m_pStr);
		for(DWORD n=0; n<length; n++)
		{
			// Valid Decimal Symbols 0-9, +- and .
			bool bThisCharValid=false;
			if(n==0 && ( m_pStr[n]=='-' || m_pStr[n]=='+'))		bThisCharValid=true;
			if(m_pStr[n]>='0' && m_pStr[n]<='9')				bThisCharValid=true;
			if(m_pStr[n]=='.')									bThisCharValid=true;

			if(bThisCharValid==false)
			{
				bStillValid=false;
				break;
			}
		}
	}
	return bStillValid;
}

bool CStr::IsTextIntegerOnlyValue(void) const
{
	bool bStillValid=true;
	if(m_pStr)
	{
		DWORD length=m_dwLen;//strlen(m_pStr);
		for(DWORD n=0; n<length; n++)
		{
			// Valid Decimal Symbols 0-9, +- and .
			bool bThisCharValid=false;
			if(n==0 && ( m_pStr[n]=='-' || m_pStr[n]=='+'))		bThisCharValid=true;
			if(m_pStr[n]>='0' && m_pStr[n]<='9')				bThisCharValid=true;

			if(bThisCharValid==false)
			{
				bStillValid=false;
				break;
			}
		}
	}
	return bStillValid;
}

bool CStr::IsTextHexValue(void) const
{
	bool bStillValid=true;
	if(m_pStr)
	{
		DWORD length=m_dwLen;//strlen(m_pStr);
		for(DWORD n=0; n<length; n++)
		{
			// Valid Decimal Symbols 0x initially, then 0-F digits
			bool bThisCharValid=false;
			if(n==0 && m_pStr[n]=='0')							bThisCharValid=true;
			if(n==1 && m_pStr[n]=='x')							bThisCharValid=true;
			if(n==1 && m_pStr[n]=='X')							bThisCharValid=true;
			if(n>=2 && m_pStr[n]>='0' && m_pStr[n]<='9')		bThisCharValid=true;
			if(n>=2 && m_pStr[n]>='a' && m_pStr[n]<='f')		bThisCharValid=true;
			if(n>=2 && m_pStr[n]>='A' && m_pStr[n]<='F')		bThisCharValid=true;

			if(bThisCharValid==false)
			{
				bStillValid=false;
				break;
			}
		}
	}
	return bStillValid;
}

bool CStr::IsTextBinaryValue(void) const
{
	bool bStillValid=true;
	if(m_pStr)
	{
		DWORD length=m_dwLen;//strlen(m_pStr);
		for(DWORD n=0; n<length; n++)
		{
			// Valid Decimal Symbols 0-9, +- and .
			bool bThisCharValid=false;
			if(n==0 &&  m_pStr[n]=='%')							bThisCharValid=true;
			if(n>=1 && (m_pStr[n]=='0' || m_pStr[n]=='1'))		bThisCharValid=true;

			if(bThisCharValid==false)
			{
				bStillValid=false;
				break;
			}
		}
	}
	return bStillValid;
}

bool CStr::IsTextOctalValue(void) const
{
	bool bStillValid=false;
	if(m_pStr && Length()>2)
	{
		bStillValid=true;
		DWORD length=m_dwLen;//strlen(m_pStr);
		for(DWORD n=0; n<length; n++)
		{
			// Valid Decimal Symbols 0c initially, then 0-7 digits
			bool bThisCharValid=false;
			if(n==0 && m_pStr[n]=='0')							bThisCharValid=true;
			if(n==1 && m_pStr[n]=='c')							bThisCharValid=true;
			if(n==1 && m_pStr[n]=='C')							bThisCharValid=true;
			if(n>=2 && m_pStr[n]>='0' && m_pStr[n]<='7')		bThisCharValid=true;

			if(bThisCharValid==false)
			{
				bStillValid=false;
				break;
			}
		}
	}
	return bStillValid;
}

bool CStr::IsTextLValue(void) const
{
	DWORD dwStage=0;
	int iBracketCount=0;
	DWORD dwInSpeechMarks=0;
	bool bStillValid=false;
	if(m_pStr)
	{
		DWORD length=m_dwLen;//strlen(m_pStr);
		for(DWORD n=0; n<length; n++)
		{
			if(dwInSpeechMarks==0)
			{
				if(m_pStr[n]=='(') iBracketCount++;
				if(m_pStr[n]==')') iBracketCount--;
			}
			if(m_pStr[n]=='"') dwInSpeechMarks=1-dwInSpeechMarks;

			// Control stage to select only ONE L-Value
			if(IsAlphaNumericLabel(n) && dwStage==0)		dwStage=1;
			if(dwInSpeechMarks==0 && iBracketCount==0)
			{
				if(IsSpaceCharacter(n) && dwStage==1)			dwStage=2;
				if(m_pStr[n]=='.' && dwStage>0)
					dwStage=0;
			}
	
			if(IsSpaceCharacter(n)==false && dwStage==2)
			{
				bStillValid=false;
				break;
			}

			bStillValid=false;
			if(dwInSpeechMarks==0 && iBracketCount==0)
			{
				if(IsAlphaNumericLabel(n))			bStillValid=true;
				if(IsSpaceCharacter(n))				bStillValid=true;
				if(m_pStr[n]=='*')					bStillValid=true;
				if(m_pStr[n]=='#')					bStillValid=true;
				if(m_pStr[n]=='$')					bStillValid=true;
				if(m_pStr[n]=='@')					bStillValid=true;
				if(m_pStr[n]=='(')					bStillValid=true;
				if(m_pStr[n]==')')					bStillValid=true;
				if(m_pStr[n]=='.')					bStillValid=true;
			}
			else
				bStillValid=true;

			if(bStillValid==false)
				break;
		}
	}
	if(dwInSpeechMarks==1)
		bStillValid=false;

	return bStillValid;
}

bool CStr::IsTextRValue(void) const
{
	if(IsTextAComplexVariable()) return true;
	// soft fail
	return false;
}

DWORD CStr::GetDWORDRepresentation(DWORD dwTypeValue, DWORD* dwExtraDWORD) const
{
	LPSTR EndString=NULL;
	DWORD dwResult=0;
	if(m_pStr)
	{
		// First convert string to integer base 10 format
		unsigned long num=0;
		CStr* pIntStr = new CStr("");
		LPSTR pNumericStr = m_pStr;
		if(IsTextBinaryValue())
		{
			num = strtoul( m_pStr+1, &EndString, 2 );//leefix-210703-hex,oct,bin are unsigned now
			pIntStr->SetUnsignedNumericText(num);
			pNumericStr = pIntStr->GetStr();
		}
		if(IsTextHexValue())
		{
			num = strtoul( m_pStr, &EndString, 16 );//leefix-210703-hex,oct,bin are unsigned now
			pIntStr->SetUnsignedNumericText(num);
			pNumericStr = pIntStr->GetStr();
		}
		if(IsTextOctalValue())
		{
			pIntStr->SetText("0");
			pIntStr->AddText(m_pStr+2);
			num = strtoul( pIntStr->GetStr(), &EndString, 8 );//leefix-210703-hex,oct,bin are unsigned now
			pIntStr->SetUnsignedNumericText(num);
			pNumericStr = pIntStr->GetStr();
		}

		// Scan in literal to determine true value
		int integervalue;
		float floatvalue;
		unsigned char booleanvalue;
		unsigned char bytevalue;
		WORD wordvalue;
		DWORD dwordvalue;
		double doublevalue;
		LONGLONG longlongvalue;
		sscanf(pNumericStr, "%i", &integervalue);
		sscanf(pNumericStr, "%f", &floatvalue);

		// leefix - 190905 - overflow fixed
		//sscanf(pNumericStr, "%u", &booleanvalue);
		//sscanf(pNumericStr, "%u", &bytevalue);
		//sscanf(pNumericStr, "%u", &wordvalue);
		DWORD dwGetU = 0;
		sscanf(pNumericStr, "%u", &dwGetU);
		booleanvalue=(unsigned char)dwGetU;
		sscanf(pNumericStr, "%u", &dwGetU);
		bytevalue=(unsigned char)dwGetU;
		sscanf(pNumericStr, "%u", &dwGetU);
		wordvalue=(WORD)dwGetU;

		sscanf(pNumericStr, "%u", &dwordvalue);
		sscanf(pNumericStr, "%lf", &doublevalue);
		sscanf(pNumericStr, "%I64d", &longlongvalue);

		// Obtain true DWORD value of this value
		switch(dwTypeValue)
		{
			case 1 : dwResult=*(DWORD*)&integervalue;		break;
			case 2 : dwResult=*(DWORD*)&floatvalue;			break;
			case 4 : dwResult=*(DWORD*)&booleanvalue;		break;
			case 5 : dwResult=*(DWORD*)&bytevalue;			break;
			case 6 : dwResult=*(DWORD*)&wordvalue;			break;
			case 7 : dwResult=*(DWORD*)&dwordvalue;			break;
			case 8 : dwResult=*(DWORD*)&doublevalue;	*dwExtraDWORD=*((DWORD*)(&doublevalue)+1);		break;
			case 9 : dwResult=*(DWORD*)&longlongvalue;	*dwExtraDWORD=*((DWORD*)(&longlongvalue)+1);		break;
		}

		// Free usages
		SAFE_DELETE(pIntStr);
	}

	return dwResult;
}

bool CStr::IsIntegerBiggerThanDWORD(void) const
{
	if(m_pStr)
	{
		int intvalue=(int)atol(m_pStr);
		__int64 value=_atoi64(m_pStr);
		if(intvalue!=value) return true;
	}
	// soft fail
	return false;
}

bool CStr::IsFloatBiggerThanDWORD(void) const
{
	if(m_pStr)
	{
		// Always use doubles for immediates
		// count now many dec places needed
		bool bFoundDot=false;
		LPSTR lpNum = GetStr();
		int iLatestCount=0, iCount=0;
		for(DWORD i=0; i<m_dwLen/*strlen(lpNum)*/; i++)
		{
			if(bFoundDot)
			{
				if((unsigned char)lpNum[i]!='0')
				{
					iLatestCount=iCount;
				}
				iCount=iCount+1;
			}
			if(lpNum[i]=='.') bFoundDot=true;
		}

		// if more than five needed, double, else float
		if(iLatestCount>=5)
			return true;
		else
			return false;
	}

	// soft fail
	return false;
}

void CStr::TrimToPathOnly(void)
{
	if(m_pStr)
	{
		DWORD dwFileSepPos = FindLastChar('\\');
		if(dwFileSepPos>0)
			SetChar(dwFileSepPos+1, 0);
		else
			SetText("");
	}

	CheckLength(m_pStr, m_dwLen);
}

bool CStr::TranslateForDBM(CResultData* pResultPtr)
{
	//
	//	TODO: This looks slow and heap-intensive; fix it.
	//

	// Ensure string present..
	if(m_pStr==NULL)
	{
		// Nothing to translate
		return true;
	}

	// When writing DBM, must convert FUNCTIONSTRUCT to Offset String
	bool bIsStructFunction=false;
	CStr* pString = new CStr(m_pStr);

	// Is it a struct from a userfunction
	if(pString->CheckChars(0,3,"FS@")==true)
	{
		LPSTR pRest = pString->GetRightOfPosition(3);
		pString->SetText(pRest);
		pString->EatEdgeSpacesandTabs(NULL);
		SAFE_DELETE(pRest);
		bIsStructFunction=true;
	}

	// Yes, proceed..
	if(bIsStructFunction)
	{
		// Is it a type or field speciier
		DWORD dwPos = pString->FindFirstChar('@');
		if(dwPos>0)
		{
			// Cannot compile with an untranslated FS
			bool bHaltCompilation=true;

			// Find subtype name
			LPSTR pSubtypename = pString->GetLeftOfPosition(dwPos);

			// Determine if field is array
			DWORD dwArrayType = 0;
			if(pString->GetChar(dwPos+1)=='&') dwArrayType=1;
			LPSTR pFieldname = pString->GetRightOfPosition(dwPos+1);

			// Field Specifier - Use Offset to Field
			DWORD dwOffset=0;
			DWORD dwSizeOfData=0;
			if(g_pStructTable->FindOffsetFromField(pSubtypename, pFieldname, &dwOffset, &dwSizeOfData))
			{
				// Find user function to get data
				DWORD dwTokenData=0, dwParamMax=0, dwLength=0;
				if(g_pInstructionTable->FindUserFunction(pSubtypename, 1, &dwTokenData, &dwParamMax, &dwLength))
				{
					// Find Declaration for structure
					CStructTable* pThisStruct = g_pStructTable->DoesTypeEvenExist(pSubtypename);
					CDeclaration* pLastParamDec = pThisStruct->GetDecChain();
					for(DWORD n=0; n<dwParamMax; n++) if(pLastParamDec->GetNext()) pLastParamDec=pLastParamDec->GetNext();

					// Work out size of param part of function struct
					DWORD dOffsetToLastParamInStruct=pLastParamDec->GetOffset();

					// Param or Local?
					int iDisplacement=0;
					if(dwOffset==0)
					{
						// Return value is -4 always
						iDisplacement=-4;
					}
					else
					{
						if(dwOffset<=dOffsetToLastParamInStruct)
						{
							// Parameters from functions start at +8 offset and + stack
							iDisplacement=dwOffset+4;
						}
						else
						{
							// Locals from functions start at -4 offset and deeper into stack(backwards)
							iDisplacement=((dOffsetToLastParamInStruct)-dwOffset)-dwSizeOfData;
						}
					}

					// Write offset out
					CStr* pStr = new CStr("@:");
					pStr->AddNumericText(iDisplacement);
// LEEFIX - 111002 - Properly handled in MathOp.cpp now as this affects how dimming arrays in functions work
//					DWORD dwEnsureType = pResultPtr->m_dwType;
//					if(dwArrayType==1 && (dwEnsureType<100 || dwEnsureType==1001)) dwEnsureType+=100;
					SetText(pStr->GetStr());
//					pResultPtr->m_dwType = dwEnsureType;
					SAFE_DELETE(pStr);
					bHaltCompilation=false;
				}
			}

			// Error during resolution of function structure
			if(g_pStatementList->GetWriteStarted()==true)
			{
				if(bHaltCompilation==true)
				{
					DWORD dwLine = g_pStatementList->GetTokenLineNumber();
					if(g_pStatementList->GetRefStatement()) dwLine = g_pStatementList->GetRefStatement()->GetLineNumber();
					CStr* pFullField = new CStr(pSubtypename);
					pFullField->AddText(":");
					pFullField->AddText(pFieldname);
					g_pErrorReport->SetError(dwLine, ERR_SYNTAX+44, pFullField->GetStr());
					SAFE_DELETE(pFullField);
					SAFE_DELETE(pSubtypename);
					SAFE_DELETE(pFieldname);
					SAFE_DELETE(pString);
					return false;
				}
			}

			// Free memory used
			SAFE_DELETE(pSubtypename);
			SAFE_DELETE(pFieldname);
		}
	}

	// If string, add to string table
	if(pString->IsTextSpeechMarked()==true)
	{
		// Strip Speech Marks from text
		pString->EatSpeechMarks();

		// Add String to database
		DWORD dwIndex = g_pStatementList->GetStringIndexCounter() + 1;
		if(g_pStringTable->AddString(pString->GetStr(), dwIndex))
			g_pStatementList->IncStringIndexCounter(1);

		if(dwIndex==0) return false;

		// Replace token with pointer to data table
		CStr* pStr = new CStr("$$");
		pStr->AddNumericText(dwIndex);
		SetText(pStr->GetStr());
		pResultPtr->m_dwType = 3;
		pResultPtr->m_dwDataOffset = 0;
		SAFE_DELETE(pStr);
	}

	// Free temp string
	SAFE_DELETE(pString);

	// Complete
	CheckLength(m_pStr, m_dwLen);
	return true;
}

bool CStr::IsConstant(void) const
{
	bool bStillValid=true;
	if(m_pStr)
	{
		DWORD length=m_dwLen;//strlen(m_pStr);
		for(DWORD n=0; n<length; n++)
		{
			if(IsAlphaNumericLabel(n)==false)		bStillValid=false;				
			if(bStillValid==false) break;
		}
	}
	return bStillValid;
}
