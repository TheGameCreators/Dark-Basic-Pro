#pragma once

#include <vector>
#include "SourceFile.h"

class Variables
{
public:
	static CString m_ProjectName;
	static CString m_ProjectPath;
	static CString m_ProjectFile;
	static CString m_BackupLocation;

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

	static int m_NumberOfLines;
	static std::vector<int> Lines; // Number of lines in each file, used for error tracking

	static std::vector <SourceFile*> m_IncludeFiles;
	static std::vector <CString> m_DBPropertyMedia;
	static std::vector <CString> m_DBPropertyCursors;
	static std::vector <CString> m_ToDo;
	static std::vector <CString> m_Comments;

	static bool m_ProjectChanged;

	// Temp variable holders
	static bool m_SavingBecauseStep;
	static bool m_SavingBecauseDebug;
	static int m_OpenFileLine; // The line that the file being opened should go to

	// Timer
	static enum TimerTypes { Backup = 1, SemanticParser, FileChangeChecker, RunBalloon };

public:
	static void Init();
	static void AddInclude(CString file);
	static void RemoveInclude(CString old);
	static void ClearIncludes();
	static void UpdateInclude(int index, CString newFile);
	static void MakeIncludeMain(CString path);
	static int IncludeExists(CString path);

	static void AddMedia(CString item);
	static void RemoveMedia(CString old);
	static int MediaExists(CString path);

	static void AddCursor(CString item);
	static void RemoveCursor(CString old);
	static int CursorExists(CString path);
	
	static void AddToDo(CString item);
	static void RemoveToDo(CString old);
	static void ChangeToDo(CString item, BOOL Checked);
	static void RenameToDo(CString oldToDo, CString newToDo);
	static void MoveToDoUp(CString item);
	static void MoveToDoDown(CString item);
	static void AddComment(CString item);
	static void ClearComment();

	static CString m_CommandLineArgs;
};
