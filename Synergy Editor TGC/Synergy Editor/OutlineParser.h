#pragma once

class OutlineParser : public CBCGPOutlineParser
{
protected:
	virtual Lexeme GetNext (const CString& strIn, int& nOffset, const int nSearchTo);

	BOOL NewCompare (const CString& strBuffer, const int nBufferOffset, const CString& strCompareWith, int& nEndOffset);
};
