#include "StdAfx.h"
#include "SourceFile.h"

SourceFile::SourceFile(CString newFilename, int newLine)
{
	filename = newFilename;
	line = newLine;
}

SourceFile::~SourceFile(){}
