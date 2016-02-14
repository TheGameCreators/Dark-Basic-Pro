#pragma once

#include <vector>
#include "Keyword.h"
#include "KeywordDialog.h"

class Keywords
{
public:
	static void ReadKeywords();
	static void RebuildKeywords();
	static UINT RebuildKeywordsThread(LPVOID pParam);	
	static void TidyUp();

	static int LongestKeyword;

	static std::vector <Keyword*> m_AKeywords;
	static std::vector <Keyword*> m_BKeywords;
	static std::vector <Keyword*> m_CKeywords;
	static std::vector <Keyword*> m_DKeywords;
	static std::vector <Keyword*> m_EKeywords;
	static std::vector <Keyword*> m_FKeywords;
	static std::vector <Keyword*> m_GKeywords;
	static std::vector <Keyword*> m_HKeywords;
	static std::vector <Keyword*> m_IKeywords;
	static std::vector <Keyword*> m_JKeywords;
	static std::vector <Keyword*> m_KKeywords;
	static std::vector <Keyword*> m_LKeywords;
	static std::vector <Keyword*> m_MKeywords;
	static std::vector <Keyword*> m_NKeywords;
	static std::vector <Keyword*> m_OKeywords;
	static std::vector <Keyword*> m_PKeywords;
	static std::vector <Keyword*> m_QKeywords;
	static std::vector <Keyword*> m_RKeywords;
	static std::vector <Keyword*> m_SKeywords;
	static std::vector <Keyword*> m_TKeywords;
	static std::vector <Keyword*> m_UKeywords;
	static std::vector <Keyword*> m_VKeywords;
	static std::vector <Keyword*> m_WKeywords;
	static std::vector <Keyword*> m_XKeywords;
	static std::vector <Keyword*> m_YKeywords;
	static std::vector <Keyword*> m_ZKeywords;
	static std::vector <Keyword*> m_MiscKeywords;

	static int a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z;
private:
	static KeywordDialog dlg;

	static void CreateKeywords();
	static void Kill(std::vector <Keyword*> &vec);
	static void SplitLine(CString line, CString &keyword, CString &url, CString &help);
	static int GetChecksumName();
};
