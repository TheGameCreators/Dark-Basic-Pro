#include "StdAfx.h"
#include "Keyword.h"

Keyword::Keyword(CString m_keyword, CString m_help, CString m_url)
{
	keyword = m_keyword;
	help = m_help;
	url = m_url;
	length = keyword.GetLength();
}
