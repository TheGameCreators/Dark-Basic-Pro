#pragma once

class Settings
{
public:
	static bool AllowToolTips;
	static bool AllowInvalidateOnCaretMove;
	static bool AllowStatusBarHelp;
	static bool AllowBracketBalanceCheck;
	static bool AllowReadCompilerStatus;
	static bool AllowCompilerAccelerator;
	static bool AllowAutoIntellisense;
	static bool AllowAutoIndentBlock;
	static bool AllowAutoCloseBlock;
	static bool HighlightCurrentLine;
	static bool GoToRememberedLine;
	static bool ShowOutputWindow;
	static bool ClearOutputOnCompile;
	static bool ShowWelcomeScreen;
	static bool OpenAllIncludes;
	static bool BlockCertificateViewer;
	static bool AutoLineConcat;
	static bool ReplaceTabs;

	static bool CollapseFunctions;
	static bool CollapseForLoops;
	static bool CollapseWhileLoops;
	static bool CollapseRepeatLoops;
	static bool CollapseCommentBlocks;
	static bool CollapseTypeDeclarations;

	static CString DBPLocation;
	static CString Precompilers;
	static CString StickyIncludes;
	static CString TempLocation;

	static CString EditorFontFace;

	static CString MainHelpURL;
	static bool OpenHelpInNewTab;

	static int EditorFontSize;
	static int IntellisenseCase;
	static int MouseScroll; // Lines scrolled by the mouse
	static int TabWidth; // Width of tabs

	static int OnStartup;
	static CString LastProject;

	static CString SerialCode;

	// Defaults
	static CString m_DBPropertyScreenType;
	static CString m_DBPropertyTitle;
    static CString m_DBPropertyIcon;
	static CString m_DBPropertyExe;
    
	static CString m_DBPropertyFullResolution;
    static CString m_DBPropertyWindowResolution;
    static bool m_DBPropertyCompressMedia;
    static bool m_DBPropertyEncryptMedia;
    static CString m_DBPropertyFileVersion;
    static CString m_DBPropertyInternalVersion;
    static CString m_DBPropertyBuildType;
	static CString m_DBPropertyFileComments;
	static CString m_DBPropertyFileCompany;
	static CString m_DBPropertyFileDescription;
	static CString m_DBPropertyFileCopyright;

	// Colours
	static COLORREF m_ColourStandardText;
	static COLORREF m_ColourKeyword;
	static COLORREF m_ColourString;
	static COLORREF m_ColourComment;
	static COLORREF m_ColourSymbol;
	static COLORREF m_ColourFunction;
	static COLORREF m_ColourType;
	static COLORREF m_ColourVariable;
	static COLORREF m_ColourBackground;

	// Backup
	static bool AllowBackup;
	static int BackupLocation;
	static CString BackupDirectory;
	static bool BackupOnCompile;
	static bool BackupOnSave;
	static bool BackupOnProjectOpen;
	static bool BackupOnProjectClose;
	static int BackupInterval;
	static int BackupType;
	static int BackupRemoveInterval;

	// Semantic Parsing
	static bool AllowCodeView;
	static int CodeViewOrder;
	static bool AllowCheckLine;
	static bool AllowVariableFix;

	// Methods
	static bool LoadSettings();
	static void SaveSettings();

private:
	static void handleStr(CString name, CString def, CString& field);
	static void handleBool(CString name, CString def, bool& field);
	static void handleInt(CString name, CString def, int& field);
	static void handleColor(CString name,CString def, COLORREF& field);

	static CString IniFile; // Temporary file to hold ini file location
};
