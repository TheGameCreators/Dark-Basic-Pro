#include "StdAfx.h"
#include "WriteFiles.h"
#include "Variables.h"
#include "MainFrm.h"
#include "Settings.h"
#include "Synergy Editor.h"
#include "Doc.h"
#include "Utilities.h"
#include "CompileSupport.h"

bool WriteFiles::Write()
{
	CStdioFile write, read;

	App* pApp = (App*)AfxGetApp();
	ASSERT_VALID(pApp);
	MainFrame* pMainWnd = (MainFrame*)AfxGetMainWnd();
	ASSERT_VALID(pMainWnd);

	CString file = Utilities::GetTemporaryPath() + _T("_Temp.dbsource");

	CString create = _T("Created temporary file ") + file;
	#ifndef NONVERBOSEOUTPUTMODE
	pMainWnd->AddOutputText(create);
	#endif

	CString extractedName;
	
	CDocTemplate* pTemplate = pApp->pDocTemplate;
	ASSERT_VALID(pTemplate);

	POSITION posDocument;
	Doc *pDoc, *pFoundDoc;

	::DeleteFileW(file);

	// Save all documents
	WriteAll();

	CString line;

	Variables::m_NumberOfLines = 0;
	Variables::Lines.clear(); // Clear vector

	if(!write.Open(file, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate))
	{
		AfxMessageBox(_T("DarkBASIC Professional Editor could not open the temporary source file ") + file + _T(" for writing.\n\nIf you are using Windows Vista/7 then either:\n1). Choose a temporary path with write permissions in the options or ensure your project is in a path with write permissions\n2). Run DarkBASIC Professional Editor with administrative permissions"), MB_ICONEXCLAMATION | MB_OK);
		return false;
	}
	std::vector <SourceFile*>::iterator Iter;
	for ( Iter = Variables::m_IncludeFiles.begin(); Iter != Variables::m_IncludeFiles.end(); Iter++ )
	{
		// Get document if possible
		posDocument = pTemplate->GetFirstDocPosition();
		extractedName = Utilities::ExtractFilename((*Iter)->filename);
		pFoundDoc = NULL;

		while(posDocument != NULL)
		{
			pDoc = (Doc*)pTemplate->GetNextDoc(posDocument);
			ASSERT_VALID(pDoc);
			if(Utilities::RemoveModifiedFlag(pDoc->GetTitle()) == extractedName)
			{
				pFoundDoc = pDoc;
				break;
			}
		}

		// Check the file
		CFileStatus status;
		if(!CFile::GetStatus((*Iter)->filename,status))
		{
			AfxMessageBox(_T("Could not locate source file ") + (*Iter)->filename);
			write.Close();
			return false;
		}


		// Write to temp
		BOOL open = read.Open((*Iter)->filename, CFile::modeRead);
		{
			if(!open)
			{
				continue;
			}
			int linesRead = 0;
			// Used by the concatenation engine
			CString lineBuffer = _T("");
			CString commentBuffer = _T("");
			bool holdWrite;
			bool lastConcat = false;
			while(read.ReadString(line))
			{
				holdWrite = false;
				if(line != _T(""))
				{
					if(Variables::m_SavingBecauseStep) // Every line has a !
					{
						line = _T("!") + line;
					}
					else if(Variables::m_SavingBecauseDebug && pFoundDoc != NULL && pFoundDoc->IsABreakpoint(linesRead)) // Only marked lines have a !
					{
						line = _T("!") + line;
					}
				}
				int length = (line.GetLength() - 1);
				if(Settings::AutoLineConcat && line != _T("") && line.GetAt(length) == '_') // Allow line concatenation
				{
					lineBuffer += line.Mid(0, length);
					commentBuffer += _T("// DarkBASIC Professional Editor placeholder to ensure line count works correctly with line concatenation\n");
					holdWrite = true;
					lastConcat = true;
				}
				else if(lastConcat)
				{
					line = lineBuffer + line + _T("\n") + commentBuffer;
                    lineBuffer = _T("");    // IRM 20090906 - Must clear this buffer ready for the next concat
					lastConcat = false;
				}
				else
				{
					line.Append(_T("\n"));
				}
				if(!holdWrite)
				{
					write.WriteString(line);
				}
				linesRead++;
				Variables::m_NumberOfLines++;
			}
			Variables::Lines.push_back(linesRead);
		}
		read.Close();
	}
	write.Flush();
	write.Close();

	return true;
}

void WriteFiles::WriteAll()
{
	App* app = (App*)AfxGetApp();

	POSITION posDocument = app->pDocTemplate->GetFirstDocPosition();

	while(posDocument != NULL)
	{
		CDocument* pDoc = app->pDocTemplate->GetNextDoc(posDocument);

		CString path = pDoc->GetPathName();
		CString pathUpper = path;
		pathUpper.MakeUpper();
		if(pathUpper.Right(3) != _T("DBA"))
		{
			path.Append(_T(".dba"));
		}
		if(pDoc->IsModified())
		{
			pDoc->DoSave(path, TRUE);
		}
	}
}

bool WriteFiles::WriteHelp()
{
	CStdioFile write, read;

	CString path = Utilities::GetTemporaryPath() + _T("_Temp.dbsource");

	if(Utilities::CheckFileExists(path))
	{
		DeleteFile(path);
	}

	if(!write.Open(path, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate))
	{
		AfxMessageBox(_T("DarkBASIC Professional Editor could not open the temporary source file ") + path + _T(" for writing.\n\nIf you are using Windows Vista/7 then either:\n1). Choose a temporary path with write permissions in the options or ensure your project is in a path with write permissions\n2). Run DarkBASIC Professional Editor with administrative permissions"), MB_ICONEXCLAMATION | MB_OK);
		return false;
	}
	if(!read.Open(CompileSupport::m_strHelpFile, CFile::modeRead))
	{
		AfxMessageBox(_T("DarkBASIC Professional Editor could not open the help source file ") + CompileSupport::m_strHelpFile + _T(" for reading."), MB_ICONEXCLAMATION | MB_OK);
		return false;
	}
	CString line;
	Variables::m_NumberOfLines = 0;
	while(read.ReadString(line))
	{
		line.AppendChar('\n');
		write.WriteString(line);
		Variables::m_NumberOfLines++;
	}
	read.Close();
	write.Flush();
	write.Close();

	MainFrame* pMainWnd = (MainFrame*)AfxGetMainWnd();
	ASSERT_VALID(pMainWnd);

	CString create = _T("Created temporary file ") + path;	
	#ifndef NONVERBOSEOUTPUTMODE
	pMainWnd->AddOutputText(create);
	#endif

	return true;
}
