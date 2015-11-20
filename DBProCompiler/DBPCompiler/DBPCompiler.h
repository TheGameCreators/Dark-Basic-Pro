// DBPCompiler.h: interface for the CDBPCompiler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBPCOMPILER_H__59BB1DE5_04A2_4BBC_9790_52F5C94E07F9__INCLUDED_)
#define AFX_DBPCOMPILER_H__59BB1DE5_04A2_4BBC_9790_52F5C94E07F9__INCLUDED_

// Common Includes
#include "windows.h"

// PATH Defines
#define PATH_MAX 20
#define PATH_ROOTPATH 1
#define PATH_SETUPFILE 2
#define PATH_ERRORSFILE 3
#define PATH_PLUGINSFOLDER 4
#define PATH_TEMPFOLDER 5
#define PATH_TEMPDBMFILE 6 
#define PATH_TEMPEXBFILE 7
#define PATH_TEMPERRORFILE 8
#define PATH_DEBUGGERFILE 9
#define PATH_COMPRESSDLLFILE 10
#define PATH_PLUGINSUSERFOLDER 11
#define PATH_PLUGINSLICENSEDFOLDER 12
#define PATH_CURRENTFOLDER 13

// External Words Array
#define EXTWORDSMAX			32
#define MAX_EXCLUSIONS		256

// Define Class
class CDBPCompiler  
{
	public:
		CDBPCompiler(LPSTR pCompilerFilename);
		virtual ~CDBPCompiler();

	public:
		bool			PerformCompileOnProject(void);
		bool			LoadDBA(LPSTR pDBAFilename);
		bool			LoadRaw(LPSTR pDBAFilename, LPSTR* ppData, DWORD* pdwDataSize);
		bool			LoadRawFromMMF(LPSTR pDBAFilename, LPSTR* ppData, DWORD* pdwDataSize);
		bool			UnfoldFileDataIncludes(void);
		void			EnsureDataMemBugEnough(LPSTR pPtr, DWORD dwPredictSize, LPSTR* pNewData, DWORD* dwNewDataSize, LPSTR* pWritePtrOut);
		bool			UnfoldFileDataConstants(void);
		bool			CopyData(LPSTR* ppData, DWORD* pdwDataSize, LPSTR pAdd, DWORD dwAddSize);
		bool			SeekIncludeToken(LPSTR* ppData, LPSTR pPtrEnd, DWORD* pdwAdvance, LPSTR* ppIncludeFilename);
		bool			MakeProgram(void);

		LPSTR			ReplaceTokens(LPSTR pFilename);

		bool			ProjectExists(void) { return m_bProjectExists; }
		bool			LoadProjectFile(LPSTR pFilename);
		bool			GetAllProjectFields(LPSTR pFilename);
		LPSTR			GetProjectFile(LPSTR pFieldName);
		LPSTR			GetProjectField(LPSTR pFieldName);
		bool			GetProjectState(LPSTR pFieldName, bool bDefault);
		bool			GetProjectState(LPSTR pFieldName);
		bool			GetProjectStateMatch(LPSTR pFieldName, LPSTR pCompareStr);
		DWORD			GetProjectDisplayInfo(LPSTR pFieldName, DWORD dwDisplayItem);
		bool			FreeProjectFile(void);

		LPSTR			GetProgramName(void) { return m_pEXEFilename; }

	public:
		DWORD			GetFileData(void) { return m_FileDataSize; }
		LPSTR			GetFilePtr(void) { return m_pFileData; }

	public:
		bool			PathExists(LPSTR pPath);
		void			SetInternalFile(DWORD dwFileID, LPSTR pFilename);
		LPSTR			GetInternalFile(DWORD dwFileID);
		bool			FileExists(LPSTR);
		void			GatherAllExternalWords(LPSTR pWordsFile);
		LPSTR			GetWord ( int iID );
		bool			EstablishRequiredBaseFiles(void);
		LPSTR			GetWordString(int id) { return m_pWord[id]; }

		bool			GetDebugMode(void) { return m_bDebugModeOn; }
		bool			GetRuntimeErrorMode(void) { return m_bRuntimeErrorsOn; }
		bool			GetProduceDBMFile(void) { return m_bProduceDBMFileOn; }
		bool			GetFullScreenMode(void) { return m_bFullScreenModeOn; }
		bool			GetFullDesktopMode(void) { return m_bFullDesktopModeOn; }
		bool			GetDesktopMode(void) { return m_bDesktopModeOn; }
		DWORD			GetStartDisplayWidth(void) { return m_dwStartDisplayWidth; }
		DWORD			GetStartDisplayHeight(void) { return m_dwStartDisplayHeight; }
		DWORD			GetStartDisplayDepth(void) { return m_dwStartDisplayDepth; }
		bool			GetHiddenMode(void) { return m_bHiddenModeOn; }
		bool			GetGenerateHelpTxtMode(void) { return m_bGenerateHelpTxtOn; }
		bool			GetEXEAloneState(void) { return m_bEXEAloneState; }
		bool			GetEXEInstallerState(void) { return m_bEXEInstallerState; }
		bool			GetCompressPCKState(void) { return m_bCompressPCKState; }
		bool			GetInternalMediaState(void) { return m_bInternalMediaState; }
		bool			GetEncryptionState(void) { return m_bEncryptionState; }
		bool			GetSpeedOverStabilityFlag(void) { return m_bSpeedOverStabilityState; }

	public:
		bool			RemoveAndRecordBreakpoints(void);
		bool			ClearBreakPointList(void);
		bool			AddToBreakPointList(DWORD dwLine);
		bool			FinishBreakPointList(void);

		DWORD			GetBreakPointIndex(void) { return m_dwBreakpointIndex; }
		DWORD			GetBreakPointLine(DWORD nIndex) { return m_pBreakpointList[nIndex]; }
		void			IncBreakPointIndex(void) { m_dwBreakpointIndex++; }
		DWORD			GetBreakPointMax(void) { return m_dwBreakpointMax; }

	public:

		// Compiler Path Info
		CStr*			m_pCompilerFilename;
		CStr*			m_pCompilerPathOnly;

		// Original Source File Data for display in debugger
		DWORD			m_dwOriginalFileDataSize;
		LPSTR			m_pOriginalFileData;

		// Main Source File Data for parsing
		DWORD			m_FileDataSize;
		LPSTR			m_pFileData;

		// Project File Data
		bool			m_bProjectExists;
		DWORD			m_ProjectFileDataSize;
		LPSTR			m_pProjectFileData;

		// Project Settings
		CStr*			m_pAbsolutePathToProjectFile;
		CStr*			m_pRelativePathToProjectFile;
		LPSTR			m_pFinalDBASource;
		LPSTR			m_pEXEFilename;
		bool			m_bSourceIsMMF;

		// Project Compiler Debug Settings
		bool			m_bDebugModeOn;
		bool			m_bRuntimeErrorsOn;
		bool			m_bProduceDBMFileOn;

		// Project Compiler Misc Settings
		bool			m_bFullScreenModeOn;
		bool			m_bFullDesktopModeOn;
		bool			m_bDesktopModeOn;
		DWORD			m_dwStartDisplayWidth;
		DWORD			m_dwStartDisplayHeight;
		DWORD			m_dwStartDisplayDepth;
		bool			m_bHiddenModeOn;
		bool			m_bGenerateHelpTxtOn;
		bool			m_bEXEAloneState;
		bool			m_bEXEInstallerState;
		bool			m_bCompressPCKState;
		bool			m_bInternalMediaState;
		bool			m_bEncryptionState;
		bool			m_bDoubleLiterals;
		bool			m_bSpeedOverStabilityState;
		bool			m_bRemoveSafetyCode;
		bool			m_bSafeArrays;
		bool			m_bLocalTempFolder;

		// Internal Files Database
		CStr*			m_pInternalFile[PATH_MAX];

		// Breakpoint List Data
		DWORD			m_dwBreakpointSize;
		DWORD*			m_pBreakpointList;
		DWORD			m_dwBreakpointIndex;
		DWORD			m_dwBreakpointMax;

		// External Words Array
		char			m_pWord[EXTWORDSMAX][_MAX_PATH];

		// Exclusion Files
		DWORD			g_dwExcludeFilesCount;
		LPSTR			g_pExcludeFiles [ MAX_EXCLUSIONS ];
};

#endif // !defined(AFX_DBPCOMPILER_H__59BB1DE5_04A2_4BBC_9790_52F5C94E07F9__INCLUDED_)
