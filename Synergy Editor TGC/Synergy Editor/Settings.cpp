#include "StdAfx.h"
#include "Settings.h"
#include "Utilities.h"

CString Settings::IniFile; // Temporary file to hold ini file location

bool Settings::AllowToolTips;
bool Settings::AllowInvalidateOnCaretMove;
bool Settings::AllowStatusBarHelp;
bool Settings::AllowBracketBalanceCheck;
bool Settings::AllowReadCompilerStatus;
bool Settings::AllowCompilerAccelerator;
bool Settings::AllowAutoIntellisense;
bool Settings::AllowAutoIndentBlock;
bool Settings::AllowAutoCloseBlock;
bool Settings::HighlightCurrentLine;
bool Settings::GoToRememberedLine;
bool Settings::ShowOutputWindow;
bool Settings::ClearOutputOnCompile;
bool Settings::ShowWelcomeScreen;
bool Settings::OpenAllIncludes;
bool Settings::BlockCertificateViewer;
bool Settings::AutoLineConcat;
bool Settings::ReplaceTabs;

bool Settings::CollapseFunctions;
bool Settings::CollapseForLoops;
bool Settings::CollapseWhileLoops;
bool Settings::CollapseRepeatLoops;
bool Settings::CollapseCommentBlocks;
bool Settings::CollapseTypeDeclarations;

CString Settings::DBPLocation;
CString Settings::TempLocation;
CString Settings::EditorFontFace;
CString Settings::MainHelpURL;
bool Settings::OpenHelpInNewTab;

int Settings::EditorFontSize;
int Settings::IntellisenseCase;
int Settings::OnStartup;
int Settings::MouseScroll;
int Settings::TabWidth;

CString Settings::LastProject;
CString Settings::SerialCode;
CString Settings::Precompilers;
CString Settings::StickyIncludes;

// Defaults
CString Settings::m_DBPropertyScreenType;
CString Settings::m_DBPropertyTitle;
CString Settings::m_DBPropertyIcon;
CString Settings::m_DBPropertyExe;

CString Settings::m_DBPropertyFullResolution;
CString Settings::m_DBPropertyWindowResolution;
bool Settings::m_DBPropertyCompressMedia;
bool Settings::m_DBPropertyEncryptMedia;
CString Settings::m_DBPropertyFileVersion;
CString Settings::m_DBPropertyInternalVersion;
CString Settings::m_DBPropertyBuildType;
CString Settings::m_DBPropertyFileComments;
CString Settings::m_DBPropertyFileCompany;
CString Settings::m_DBPropertyFileDescription;
CString Settings::m_DBPropertyFileCopyright;

// Colours
COLORREF Settings::m_ColourStandardText;
COLORREF Settings::m_ColourKeyword;
COLORREF Settings::m_ColourString;
COLORREF Settings::m_ColourComment;
COLORREF Settings::m_ColourSymbol;
COLORREF Settings::m_ColourFunction;
COLORREF Settings::m_ColourType;
COLORREF Settings::m_ColourVariable;
COLORREF Settings::m_ColourBackground;

// Backup
bool Settings::AllowBackup;
int Settings::BackupLocation;
CString Settings::BackupDirectory;
bool Settings::BackupOnCompile;
bool Settings::BackupOnSave;
bool Settings::BackupOnProjectOpen;
bool Settings::BackupOnProjectClose;
int Settings::BackupInterval;
int Settings::BackupType;
int Settings::BackupRemoveInterval;

// Code View
bool Settings::AllowCodeView;
int Settings::CodeViewOrder;
bool Settings::AllowCheckLine;
bool Settings::AllowVariableFix;

bool Settings::LoadSettings()
{
	bool created = false;

	IniFile = Utilities::ExtractPath(Utilities::GetApplicationPath()) + _T("Editor.ini");

	CFileStatus status;
	if(!CFile::GetStatus(IniFile,status))
	{
		created = true;
		TCHAR  sResName[5]  = _T("#200");
		TCHAR sRestype[13] = _T("INI");
		HRSRC hres = FindResource(NULL, sResName, sRestype);
		int size = SizeofResource(NULL, hres);
		HGLOBAL    hbytes = LoadResource(NULL, hres);
	        
		char const* pData = reinterpret_cast<char const*>(LockResource(hbytes)); 

		CString newIniFile(pData, size);

		UnlockResource(hbytes);
		FreeResource(hbytes);

		CStdioFile file;
		BOOL open = file.Open(IniFile,CFile::modeWrite | CFile::modeCreate|CFile::modeNoTruncate);
		if(!open)
		{
			AfxMessageBox(_T("DarkBASIC Professional Editor encountered an error as it attempted to create a new settings file..\n\nIf you are using Windows Vista/7 then either:\n1). Copy the DarkBASIC Professional Editor directory into a path with write permissions\n2). Run DarkBASIC Professional Editor with administrative permissions"), MB_ICONERROR | MB_OK);
		}
		else
		{
			file.WriteString(newIniFile);
			file.Flush();
			file.Close();
		}
	}	

	handleBool(_T("AllowToolTips"), _T("TRUE"), AllowToolTips);
	handleBool(_T("AllowInvalidateOnCaretMove"), _T("TRUE"), AllowInvalidateOnCaretMove);
	handleBool(_T("AllowStatusBarHelp"), _T("TRUE"), AllowStatusBarHelp);
	handleBool(_T("AllowBracketBalanceCheck"), _T("FALSE"), AllowBracketBalanceCheck);
	handleBool(_T("AllowReadCompilerStatus"), _T("TRUE"), AllowReadCompilerStatus);
	handleBool(_T("AllowCompilerAccelerator"), _T("TRUE"), AllowCompilerAccelerator);
	handleBool(_T("AllowAutoIntellisense"), _T("FALSE"), AllowAutoIntellisense);
	handleBool(_T("AllowAutoIndentBlock"), _T("FALSE"), AllowAutoIndentBlock);
	handleBool(_T("AllowAutoCloseBlock"), _T("FALSE"), AllowAutoCloseBlock);
	handleBool(_T("OpenHelpInNewTab"), _T("FALSE"), OpenHelpInNewTab);
	handleBool(_T("HighlightCurrentLine"), _T("FALSE"), HighlightCurrentLine);
	handleBool(_T("GoToRememberedLine"), _T("FALSE"), GoToRememberedLine);	
	handleBool(_T("ShowOutputWindow"), _T("FALSE"), ShowOutputWindow);	
	handleBool(_T("ClearOutputOnCompile"), _T("FALSE"), ClearOutputOnCompile);
	handleBool(_T("ShowWelcomeScreen"), _T("FALSE"), ShowWelcomeScreen);
	handleBool(_T("OpenAllIncludes"), _T("FALSE"), OpenAllIncludes);
	handleBool(_T("BlockCertificateViewer"), _T("FALSE"), BlockCertificateViewer);	
	handleBool(_T("AutoLineConcat"), _T("FALSE"), AutoLineConcat);		
	handleBool(_T("ReplaceTabs"), _T("FALSE"), ReplaceTabs);

	handleBool(_T("CollapseFunctions"), _T("FALSE"), CollapseFunctions);
	handleBool(_T("CollapseForLoops"), _T("FALSE"), CollapseForLoops);
	handleBool(_T("CollapseWhileLoops"), _T("FALSE"), CollapseWhileLoops);
	handleBool(_T("CollapseRepeatLoops"), _T("FALSE"), CollapseRepeatLoops);
	handleBool(_T("CollapseCommentBlocks"), _T("FALSE"), CollapseCommentBlocks);
	handleBool(_T("CollapseTypeDeclarations"), _T("FALSE"), CollapseTypeDeclarations);	

	CString temp;
	handleStr(_T("DBPLocation"), DBPLocation,temp);
	if(temp != _T(""))
	{
		DBPLocation = temp;	
	}	
	else
	{
		DBPLocation = Utilities::GetDBPLocation();
	}

	handleStr(_T("EditorFontFace"), _T("Courier New"), EditorFontFace);
	handleStr(_T("MainHelpURL"), _T("main.htm"), MainHelpURL);
	handleStr(_T("LastProject"), _T(""), LastProject);
	handleStr(_T("SerialCode"), _T(""), SerialCode);
	handleStr(_T("Precompilers"), _T(""), Precompilers);
	handleStr(_T("StickyIncludes"), _T(""), StickyIncludes);
	handleStr(_T("TempLocation"), TempLocation, TempLocation);
	if(!Utilities::EnsurePathExists(Settings::TempLocation))
	{
		Settings::TempLocation = L""; // Force the path to be blank so we get the windows path
		Settings::TempLocation = Utilities::GetTemporaryPath();
	}
	if(Settings::TempLocation.Right(1) != _T("\\"))
	{
		Settings::TempLocation += _T("\\");
	}

	handleStr(_T("DBPropertyFileVersion"), _T(""), m_DBPropertyFileVersion);
	handleStr(_T("DBPropertyInternalVersion"), _T(""), m_DBPropertyInternalVersion);
	handleStr(_T("DBPropertyBuildType"), _T("exe"), m_DBPropertyBuildType);
	handleStr(_T("DBPropertyFileComments"), _T(""), m_DBPropertyFileComments);
	handleStr(_T("DBPropertyFileCompany"), _T(""), m_DBPropertyFileCompany);
	handleStr(_T("DBPropertyFileDescription"), _T(""), m_DBPropertyFileDescription);
	handleStr(_T("DBPropertyFileCopyright"), _T(""), m_DBPropertyFileCopyright);

	handleStr(_T("DBPropertyFullResolution"), _T("640x480x32"), m_DBPropertyFullResolution);
	handleStr(_T("DBPropertyWindowResolution"), _T("640x480"), m_DBPropertyWindowResolution);
	handleStr(_T("DBPropertyExe"), _T("Application.exe"), m_DBPropertyExe);
	handleStr(_T("DBPropertyIcon"), _T(""), m_DBPropertyIcon);
	handleStr(_T("DBPropertyTitle"), _T("Dark Basic Pro Project"), m_DBPropertyTitle);
	handleStr(_T("DBPropertyScreenType"), _T("window"), m_DBPropertyScreenType);

	handleInt(_T("EditorFontSize"), _T("10"), EditorFontSize);
	handleInt(_T("IntellisenseCase"), _T("0"), IntellisenseCase);
	handleInt(_T("OnStartup"), _T("1"), OnStartup);
	handleInt(_T("MouseScroll"), _T("-1"), MouseScroll);
	handleInt(_T("TabWidth"), _T("4"), TabWidth);

	handleBool(_T("DBPropertyCompressMedia"), _T("FALSE"), m_DBPropertyCompressMedia);
	handleBool(_T("DBPropertyEncryptMedia"), _T("FALSE"), m_DBPropertyEncryptMedia);

	// Colours
	handleColor(_T("ColourStandardText"), _T("0,0,0"), m_ColourStandardText);
	handleColor(_T("ColourKeyword"), _T("0,0,255"), m_ColourKeyword);
	handleColor(_T("ColourString"), _T("139,0,0"), m_ColourString);
	handleColor(_T("ColourComment"), _T("0,100,0"), m_ColourComment);
	handleColor(_T("ColourSymbol"), _T("150,58,70"), m_ColourSymbol);
	handleColor(_T("ColourFunction"), _T("0,0,200"), m_ColourFunction);	
	handleColor(_T("ColourBackground"), _T("255,255,255"), m_ColourBackground);
	handleColor(_T("ColourType"), _T("0,0,200"), m_ColourType);	
	handleColor(_T("ColourVariable"), _T("0,0,0"), m_ColourVariable);

	// Backup
	handleBool(_T("AllowBackup"), _T("FALSE"), AllowBackup);
	handleInt(_T("BackupLocation"), _T("0"), BackupLocation);
	handleStr(_T("BackupDirectory"), _T(""), BackupDirectory);
	handleBool(_T("BackupOnCompile"), _T("FALSE"), BackupOnCompile);
	handleBool(_T("BackupOnSave"), _T("FALSE"), BackupOnSave);
	handleBool(_T("BackupOnProjectOpen"), _T("FALSE"), BackupOnProjectOpen);
	handleBool(_T("BackupOnProjectClose"), _T("FALSE"), BackupOnProjectClose);
	handleInt(_T("BackupInterval"), _T("-1"), BackupInterval);
	handleInt(_T("BackupType"), _T("0"), BackupType);
	handleInt(_T("BackupRemoveInterval"), _T("-1"), BackupRemoveInterval);

	// Semantic Parsing
	handleBool(_T("AllowCodeView"), _T("TRUE"), AllowCodeView);
	handleBool(_T("AllowCheckLine"), _T("TRUE"), AllowCheckLine);
	handleInt(_T("CodeViewOrder"), _T("0"), CodeViewOrder);
	handleBool(_T("AllowVariableFix"), _T("FALSE"), AllowVariableFix);

	return created;
}

void Settings::SaveSettings()
{
	IniFile = Utilities::ExtractPath(Utilities::GetApplicationPath()) + _T("Editor.ini");
	CString intConvertor;

	WritePrivateProfileStringW(_T("Editor"), _T("AllowToolTips"), AllowToolTips ? _T("TRUE") : _T("FALSE"), IniFile);
	WritePrivateProfileStringW(_T("Editor"), _T("AllowInvalidateOnCaretMove"), AllowInvalidateOnCaretMove ? _T("TRUE") : _T("FALSE"), IniFile);
	WritePrivateProfileStringW(_T("Editor"), _T("AllowStatusBarHelp"), AllowStatusBarHelp ? _T("TRUE") : _T("FALSE"), IniFile);
	WritePrivateProfileStringW(_T("Editor"), _T("AllowBracketBalanceCheck"), AllowBracketBalanceCheck ? _T("TRUE") : _T("FALSE"), IniFile);
	WritePrivateProfileStringW(_T("Editor"), _T("AllowReadCompilerStatus"), AllowReadCompilerStatus ? _T("TRUE") : _T("FALSE"), IniFile);
	WritePrivateProfileStringW(_T("Editor"), _T("AllowCompilerAccelerator"), AllowCompilerAccelerator ? _T("TRUE") : _T("FALSE"), IniFile);
	WritePrivateProfileStringW(_T("Editor"), _T("AllowAutoIntellisense"), AllowAutoIntellisense ? _T("TRUE") : _T("FALSE"), IniFile);	
	WritePrivateProfileStringW(_T("Editor"), _T("AllowAutoIndentBlock"), AllowAutoIndentBlock ? _T("TRUE") : _T("FALSE"), IniFile);	
	WritePrivateProfileStringW(_T("Editor"), _T("AllowAutoCloseBlock"), AllowAutoCloseBlock ? _T("TRUE") : _T("FALSE"), IniFile);	
	WritePrivateProfileStringW(_T("Editor"), _T("HighlightCurrentLine"), HighlightCurrentLine ? _T("TRUE") : _T("FALSE"), IniFile);		
	WritePrivateProfileStringW(_T("Editor"), _T("GoToRememberedLine"), GoToRememberedLine ? _T("TRUE") : _T("FALSE"), IniFile);			
	WritePrivateProfileStringW(_T("Editor"), _T("ShowOutputWindow"), ShowOutputWindow ? _T("TRUE") : _T("FALSE"), IniFile);		
	WritePrivateProfileStringW(_T("Editor"), _T("ClearOutputOnCompile"), ClearOutputOnCompile ? _T("TRUE") : _T("FALSE"), IniFile);		
	WritePrivateProfileStringW(_T("Editor"), _T("ShowWelcomeScreen"), ShowWelcomeScreen ? _T("TRUE") : _T("FALSE"), IniFile);
	WritePrivateProfileStringW(_T("Editor"), _T("AutoLineConcat"), AutoLineConcat ? _T("TRUE") : _T("FALSE"), IniFile);	
	WritePrivateProfileStringW(_T("Editor"), _T("OpenAllIncludes"), OpenAllIncludes ? _T("TRUE") : _T("FALSE"), IniFile);	
	WritePrivateProfileStringW(_T("Editor"), _T("ReplaceTabs"), ReplaceTabs ? _T("TRUE") : _T("FALSE"), IniFile);

	WritePrivateProfileStringW(_T("Editor"), _T("DBPLocation"), DBPLocation, IniFile);

	WritePrivateProfileStringW(_T("Editor"), _T("EditorFontFace"), EditorFontFace, IniFile);
	intConvertor.Format(_T("%d"),EditorFontSize);
	WritePrivateProfileStringW(_T("Editor"), _T("EditorFontSize"), intConvertor, IniFile);
	intConvertor.Format(_T("%d"),IntellisenseCase);
	WritePrivateProfileStringW(_T("Editor"), _T("IntellisenseCase"), intConvertor, IniFile);
	intConvertor.Format(_T("%d"),OnStartup);
	WritePrivateProfileStringW(_T("Editor"), _T("OnStartup"), intConvertor, IniFile);	
	intConvertor.Format(_T("%d"),MouseScroll);
	WritePrivateProfileStringW(_T("Editor"), _T("MouseScroll"), intConvertor, IniFile);	
	intConvertor.Format(_T("%d"),TabWidth);
	WritePrivateProfileStringW(_T("Editor"), _T("TabWidth"), intConvertor, IniFile);	

	WritePrivateProfileStringW(_T("Editor"), _T("LastProject"), LastProject, IniFile);
	WritePrivateProfileStringW(_T("Editor"), _T("MainHelpURL"), MainHelpURL, IniFile);
	WritePrivateProfileStringW(_T("Editor"), _T("OpenHelpInNewTab"), OpenHelpInNewTab ? _T("TRUE") : _T("FALSE"), IniFile);
	WritePrivateProfileStringW(_T("Editor"), _T("SerialCode"), SerialCode, IniFile);	
	WritePrivateProfileStringW(_T("Editor"), _T("Precompilers"), Precompilers, IniFile);	
	WritePrivateProfileStringW(_T("Editor"), _T("StickyIncludes"), StickyIncludes, IniFile);
	WritePrivateProfileStringW(_T("Editor"), _T("TempLocation"), TempLocation, IniFile);		

	WritePrivateProfileStringW(_T("Editor"), _T("CollapseFunctions"), CollapseFunctions ? _T("TRUE") : _T("FALSE"), IniFile);	
	WritePrivateProfileStringW(_T("Editor"), _T("CollapseForLoops"), CollapseForLoops ? _T("TRUE") : _T("FALSE"), IniFile);	
	WritePrivateProfileStringW(_T("Editor"), _T("CollapseWhileLoops"), CollapseWhileLoops ? _T("TRUE") : _T("FALSE"), IniFile);	
	WritePrivateProfileStringW(_T("Editor"), _T("CollapseRepeatLoops"), CollapseRepeatLoops ? _T("TRUE") : _T("FALSE"), IniFile);	
	WritePrivateProfileStringW(_T("Editor"), _T("CollapseCommentBlocks"), CollapseCommentBlocks ? _T("TRUE") : _T("FALSE"), IniFile);	
	WritePrivateProfileStringW(_T("Editor"), _T("CollapseTypeDeclarations"), CollapseTypeDeclarations ? _T("TRUE") : _T("FALSE"), IniFile);	

	WritePrivateProfileStringW(_T("Editor"), _T("DBPropertyFileVersion"),m_DBPropertyFileVersion, IniFile);	
	WritePrivateProfileStringW(_T("Editor"), _T("DBPropertyInternalVersion"),m_DBPropertyInternalVersion, IniFile);	
	WritePrivateProfileStringW(_T("Editor"), _T("DBPropertyBuildType"),m_DBPropertyBuildType, IniFile);	
	WritePrivateProfileStringW(_T("Editor"), _T("DBPropertyFileComments"),m_DBPropertyFileComments, IniFile);	
	WritePrivateProfileStringW(_T("Editor"), _T("DBPropertyFileCompany"),m_DBPropertyFileCompany, IniFile);	
	WritePrivateProfileStringW(_T("Editor"), _T("DBPropertyFileDescription"),m_DBPropertyFileDescription, IniFile);	
	WritePrivateProfileStringW(_T("Editor"), _T("DBPropertyFileCopyright"),m_DBPropertyFileCopyright, IniFile);	
	WritePrivateProfileStringW(_T("Editor"), _T("DBPropertyFullResolution"),m_DBPropertyFullResolution, IniFile);	
	WritePrivateProfileStringW(_T("Editor"), _T("DBPropertyWindowResolution"),m_DBPropertyWindowResolution, IniFile);	
	WritePrivateProfileStringW(_T("Editor"), _T("DBPropertyExe"),m_DBPropertyExe, IniFile);	
	WritePrivateProfileStringW(_T("Editor"), _T("DBPropertyIcon"), m_DBPropertyIcon, IniFile);	
	WritePrivateProfileStringW(_T("Editor"), _T("DBPropertyTitle"),m_DBPropertyTitle, IniFile);	
	WritePrivateProfileStringW(_T("Editor"), _T("DBPropertyScreenType"),m_DBPropertyScreenType, IniFile);	

	WritePrivateProfileStringW(_T("Editor"), _T("DBPropertyCompressMedia"), m_DBPropertyCompressMedia ? _T("TRUE") : _T("FALSE"), IniFile);	
	WritePrivateProfileStringW(_T("Editor"), _T("DBPropertyEncryptMedia"), m_DBPropertyEncryptMedia ? _T("TRUE") : _T("FALSE"), IniFile);	

	// Colours
	CString colour;
	colour.Format(_T("%d,%d,%d"), GetRValue(m_ColourStandardText), GetGValue(m_ColourStandardText), GetBValue(m_ColourStandardText)); 
	WritePrivateProfileStringW(_T("Editor"), _T("ColourStandardText"), colour, IniFile);	

	colour.Format(_T("%d,%d,%d"), GetRValue(m_ColourKeyword), GetGValue(m_ColourKeyword), GetBValue(m_ColourKeyword)); 
	WritePrivateProfileStringW(_T("Editor"), _T("ColourKeyword"), colour, IniFile);	

	colour.Format(_T("%d,%d,%d"), GetRValue(m_ColourFunction), GetGValue(m_ColourFunction), GetBValue(m_ColourFunction)); 
	WritePrivateProfileStringW(_T("Editor"), _T("ColourFunction"), colour, IniFile);

	colour.Format(_T("%d,%d,%d"), GetRValue(m_ColourType), GetGValue(m_ColourType), GetBValue(m_ColourType)); 
	WritePrivateProfileStringW(_T("Editor"), _T("ColourType"), colour, IniFile);	

	colour.Format(_T("%d,%d,%d"), GetRValue(m_ColourString), GetGValue(m_ColourString), GetBValue(m_ColourString)); 
	WritePrivateProfileStringW(_T("Editor"), _T("ColourString"), colour, IniFile);	

	colour.Format(_T("%d,%d,%d"), GetRValue(m_ColourComment), GetGValue(m_ColourComment), GetBValue(m_ColourComment)); 
	WritePrivateProfileStringW(_T("Editor"), _T("ColourComment"), colour, IniFile);	

	colour.Format(_T("%d,%d,%d"), GetRValue(m_ColourSymbol), GetGValue(m_ColourSymbol), GetBValue(m_ColourSymbol)); 
	WritePrivateProfileStringW(_T("Editor"), _T("ColourSymbol"), colour, IniFile);	

	colour.Format(_T("%d,%d,%d"), GetRValue(m_ColourVariable), GetGValue(m_ColourVariable), GetBValue(m_ColourVariable)); 
	WritePrivateProfileStringW(_T("Editor"), _T("ColourVariable"), colour, IniFile);
	
	colour.Format(_T("%d,%d,%d"), GetRValue(m_ColourBackground), GetGValue(m_ColourBackground), GetBValue(m_ColourBackground)); 
	WritePrivateProfileStringW(_T("Editor"), _T("ColourBackground"), colour, IniFile);

	// Backup
	WritePrivateProfileStringW(_T("Editor"), _T("AllowBackup"), AllowBackup  ? _T("TRUE") : _T("FALSE"), IniFile);
	intConvertor.Format(_T("%d"),BackupLocation);
	WritePrivateProfileStringW(_T("Editor"), _T("BackupLocation"), intConvertor, IniFile);
	WritePrivateProfileStringW(_T("Editor"), _T("BackupDirectory"), BackupDirectory, IniFile);
	WritePrivateProfileStringW(_T("Editor"), _T("BackupOnCompile"), BackupOnCompile ? _T("TRUE") : _T("FALSE"), IniFile);
	WritePrivateProfileStringW(_T("Editor"), _T("BackupOnSave"), BackupOnSave ? _T("TRUE") : _T("FALSE"), IniFile);
	WritePrivateProfileStringW(_T("Editor"), _T("BackupOnProjectOpen"), BackupOnProjectOpen ? _T("TRUE") : _T("FALSE"), IniFile);
	WritePrivateProfileStringW(_T("Editor"), _T("BackupOnProjectClose"), BackupOnProjectClose ? _T("TRUE") : _T("FALSE"), IniFile);
	intConvertor.Format(_T("%d"),BackupInterval);
	WritePrivateProfileStringW(_T("Editor"), _T("BackupInterval"), intConvertor, IniFile);
	intConvertor.Format(_T("%d"),BackupType);
	WritePrivateProfileStringW(_T("Editor"), _T("BackupType"), intConvertor, IniFile);
	intConvertor.Format(_T("%d"),BackupRemoveInterval);
	WritePrivateProfileStringW(_T("Editor"), _T("BackupRemoveInterval"), intConvertor, IniFile);

	// Semantic
	WritePrivateProfileStringW(_T("Editor"), _T("AllowCodeView"), AllowCodeView ? _T("TRUE") : _T("FALSE"), IniFile);
	WritePrivateProfileStringW(_T("Editor"), _T("AllowCheckLine"), AllowCheckLine ? _T("TRUE") : _T("FALSE"), IniFile);
	intConvertor.Format(_T("%d"),CodeViewOrder);
	WritePrivateProfileStringW(_T("Editor"), _T("CodeViewOrder"), intConvertor, IniFile);
	WritePrivateProfileStringW(_T("Editor"), _T("AllowVariableFix"), AllowVariableFix ? _T("TRUE") : _T("FALSE"), IniFile);	
}

// Reads a char array and converts it into a CString
void inline Settings::handleStr(CString name, CString def, CString& field)
{
	TCHAR tszBuf[MAX_PATH];
	int size = GetPrivateProfileString(_T("Editor"),name,def,tszBuf,MAX_PATH,IniFile);
	field = CString(tszBuf);
}

void inline Settings::handleBool(CString name, CString def, bool& field)
{
	CString temp;
	handleStr(name, def, temp);
	field = (temp == _T("TRUE"));
}

void inline Settings::handleInt(CString name, CString def, int& field)
{
	CString temp;
	handleStr(name, def, temp);
	field = _ttoi(temp);
}

void inline Settings::handleColor(CString name,CString def, COLORREF& field)
{
	CString temp;
	handleStr(name, def, temp);

	int curPos= 0;
	int mode = 0;
	int r,g,b;

	CString resToken= temp.Tokenize(_T(","),curPos);
	while (resToken != _T(""))
	{
		if(mode == 0)
		{
			r = _ttoi(resToken);
		}
		else if(mode == 1)
		{
			g = _ttoi(resToken);
		}
		else if(mode == 2)
		{
			b = _ttoi(resToken);
		}
		mode++;
	   resToken= temp.Tokenize(_T(","),curPos);
	};

	field = RGB(r, g, b);
}
