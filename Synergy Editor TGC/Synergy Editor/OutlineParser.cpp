#include "StdAfx.h"
#include "OutlineParser.h"

BOOL OutlineParser::NewCompare (const CString& strBuffer, const int nBufferOffset,
								  const CString& strCompareWith, int& nEndOffset)
{
	// Returns equal chars num:
	const int nLength = strCompareWith.GetLength ();
	const int nBufLength = strBuffer.GetLength ();	

	if(nLength > nBufLength - nBufferOffset)
	{
		nEndOffset = nBufferOffset;
		return FALSE;
	}

	TCHAR toCheck;
	if(nBufferOffset > 0)
	{
		toCheck = strBuffer [nBufferOffset  - 1];
		if (toCheck != '\n' && toCheck != '\t' )
		{
			nEndOffset = nBufferOffset;
			return FALSE;
		}
	}

	int i = 0;

	// Check that the token fits in at the offset
	for (i = 0; i < nLength && nBufferOffset + i < nBufLength; i++)
	{
		if ((_tcsnicmp (((LPCTSTR) strCompareWith) + i, ((LPCTSTR) strBuffer) + nBufferOffset + i, 1) != 0))
		{
			nEndOffset = nBufferOffset;
			return FALSE;
		}
	}

	if(nBufferOffset + i < nBufLength)
	{
		toCheck = strBuffer [nBufferOffset + i];
		if(toCheck != '\n' && toCheck != ' ')
		{
			nEndOffset = nBufferOffset;
			return FALSE;
		}
	}

	nEndOffset = nBufferOffset + i - 1;
	return (i > 0) && (i == nLength);
}
//************************************************************************************
Lexeme OutlineParser::GetNext (const CString& strIn, int& nOffset, const int nSearchTo)
{
	while (nOffset >= 0 && nOffset < strIn.GetLength () && nOffset <= nSearchTo)
	{
		if (IsEscapeSequence (strIn, nOffset))
		{
			continue;
		}

		for (int i = 0; i <= m_arrBlockTypes.GetUpperBound (); i++)
		{
			BlockType* pBlockType = m_arrBlockTypes [i];
			ASSERT (pBlockType != NULL);

			// Nested blocks:
			if (pBlockType->m_bAllowNestedBlocks)
			{
				int nEndOffset;
				if (NewCompare (strIn, nOffset, pBlockType->m_strOpen, nEndOffset))
				{
					Lexeme lexem (i, LT_BlockStart, nOffset, nEndOffset);
					nOffset += pBlockType->m_strOpen.GetLength ();
					return lexem;
				}
				else if (NewCompare (strIn, nOffset, pBlockType->m_strClose, nEndOffset))
				{
					Lexeme lexem (i, LT_BlockEnd, nOffset, nEndOffset);
					nOffset += pBlockType->m_strClose.GetLength ();
					return lexem;
				}
			}
			// NonNested blocks:
			else
			{
				int nEndOffset;
				if (NewCompare (strIn, nOffset, pBlockType->m_strOpen, nEndOffset))
				{
					Lexeme lexem (i, LT_CompleteBlock, nOffset, nEndOffset);
					nOffset += pBlockType->m_strOpen.GetLength ();

					if (!pBlockType->m_strClose.IsEmpty ())
					{
						// find close token skipping escape sequences:
						while  (nOffset >= 0 && 
								nOffset < strIn.GetLength () && 
								nOffset <= nSearchTo)
						{
							if (IsEscapeSequence (strIn, nOffset))
							{
								continue;
							}

							if (NewCompare (strIn, nOffset, pBlockType->m_strClose, nEndOffset))
							{
								nOffset += pBlockType->m_strClose.GetLength ();
								if (pBlockType->m_strClose == _T("\n"))
								{
									nOffset--;
								}

								lexem.m_nEnd = nOffset - 1;
								return lexem;
							}

							nOffset++;
						}
					}
					
					if (pBlockType->m_bIgnore)
					{
						nOffset = lexem.m_nStart;
					}
					else
					{
						lexem.m_nEnd = strIn.GetLength () - 1;
						return lexem;
					}
				}
			}

		}

		nOffset++;
	}

	return Lexeme (0, LT_EndOfText, 0, 0);
}
