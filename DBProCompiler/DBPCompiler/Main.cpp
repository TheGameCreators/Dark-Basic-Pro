//
// Main Compiler
//

// Protection System
#define _CRT_SECURE_NO_DEPRECATE
#include "..\\TGCOnline\\CertificateKey.h"

// Common Includes
#include "macros.h"
#include "Windows.h"
#include "DebugInfo.h"
#include "Error.h"
#include "direct.h"
#include "time.h"

// Custom Includes
#include "Str.h"
#include "DBPCompiler.h"

#include <DB3Time.h>

// Internal data
HWND g_hTempWindow = NULL;
HWND g_igLoader_HWND = NULL;
char g_ActualCompilerFilename[256];

// External Class Pointers
extern CDBPCompiler*		g_pDBPCompiler;
extern CError*				g_pErrorReport;
extern CDebugInfo			g_DebugInfo;

HRESULT GetDXVersion( DWORD* pdwDirectXVersion, TCHAR* strDirectXVersion, int cchDirectXVersion )
{
	return S_OK;
}

LRESULT CALLBACK WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
    {
		case WM_USER+51:
			{
				// Memory to be used to store string sent
				DWORD dwDataSize=0;
				LPSTR pData=NULL;

				// First Four Bytes are Size of Message
				HANDLE hFileMap = OpenFileMapping(FILE_MAP_READ,FALSE,"DBPROCLITEXT");
				if(hFileMap)
				{
					LPVOID lpVoid = MapViewOfFile(hFileMap,FILE_MAP_READ,0,0,4);
					if(lpVoid)
					{
						DWORD dwUserMessageSize = *(DWORD*)lpVoid;
						UnmapViewOfFile(lpVoid);
						CloseHandle(hFileMap);

						// Open Message
						hFileMap = OpenFileMapping(FILE_MAP_READ,FALSE,"DBPROCLITEXT");
						lpVoid = MapViewOfFile(hFileMap,FILE_MAP_READ,0,0,dwUserMessageSize+4);
						if(lpVoid)
						{
							// Place data into datamem
							dwDataSize = dwUserMessageSize;
							pData = new char[dwUserMessageSize+1];
							ZeroMemory(pData, dwUserMessageSize+1);
							memcpy(pData, (LPSTR)lpVoid+4, dwUserMessageSize);

							// Close Message
							UnmapViewOfFile(lpVoid);
						}
					}
					CloseHandle(hFileMap);
				}

				// Set CLI Text to be used by parser-loop
				g_DebugInfo.SetMessageArrived(true);
				g_DebugInfo.SetCLISize(dwDataSize);
				g_DebugInfo.SetCLIText(pData);
			}
			return TRUE;

		case WM_CLOSE:
			PostQuitMessage(0);
			return TRUE;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
    }

	// Default Action
    return DefWindowProc(hWnd, message, wParam, lParam);
}

/* ABANDONNED PROTECTION VIA REGISTRY %AND ASSIST - BEEN CRACKED IN 2002

// Define for TICK-COUNT-PROTECTION
#ifdef DEMOPROTECTEDMODE
 #define TICKLIMIT 2
#else
 #define TICKLIMIT 10000
#endif

// Internal Support for AssistNet Mode
bool bCheckAssistNetOnce	= false;
bool bAssistNetMode			= false;
DWORD dwTickLimit			= TICKLIMIT;

bool WriteStringToRegistryEx(char* PerfmonNamesKey, char* valuekey, char* string)
{
	HKEY hKeyNames = 0;
	DWORD Status;
	DWORD dwDisposition;
	char ObjectType[256];
	strcpy(ObjectType,"Num");
	Status = RegCreateKeyEx(HKEY_LOCAL_MACHINE, PerfmonNamesKey, 0L, ObjectType, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | KEY_WRITE, NULL, &hKeyNames, &dwDisposition);
	if(dwDisposition==REG_OPENED_EXISTING_KEY)
	{
		RegCloseKey(hKeyNames);
		Status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, PerfmonNamesKey, 0L, KEY_WRITE, &hKeyNames);
	}
    if(Status==ERROR_SUCCESS)
	{
        Status = RegSetValueEx(hKeyNames, valuekey, 0, REG_SZ, (LPBYTE)string, (strlen(string)+1)*sizeof(char));
	}
	RegCloseKey(hKeyNames);
	hKeyNames=0;
	return true;
}

void ReadStringFromRegistryEx(char* PerfmonNamesKey, char* valuekey, char* string)
{
	HKEY hKeyNames = 0;
	DWORD Status;
	char ObjectType[256];
	DWORD Datavalue = 0;

	strcpy(string,"");
	strcpy(ObjectType,"Num");
	Status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, PerfmonNamesKey, 0L, KEY_READ, &hKeyNames);
    if(Status==ERROR_SUCCESS)
	{
		DWORD Type=REG_SZ;
		DWORD Size=256;
		Status = RegQueryValueEx(hKeyNames, valuekey, NULL, &Type, NULL, &Size);
		if(Size<255)
			RegQueryValueEx(hKeyNames, valuekey, NULL, &Type, (LPBYTE)string, &Size);

		RegCloseKey(hKeyNames);
	}
}

bool IsTickRequiredYet(int iMode)
{
	// In Demo Mode, no tick required
	#ifdef DEMOPROTECTEDMODE
	return false;
	#endif

	// Location of tickcount
	char String[256];
	char keyname[256];
	strcpy(keyname, "Software\\");
	strcat(keyname, "Microsoft\\");
	strcat(keyname, "Windows Info");
	ReadStringFromRegistryEx(keyname, "fileinfo", String);
	int iCount = atoi(String);
	bool bRes=false;

	// Modes..
	if(iMode==0)
	{
		// Check number
		if(iCount==0) iCount=dwTickLimit+1;

		// If less than zero, being hacked
		if(iCount<0)
		{
			// Check if CD present
			bRes=true;
		}

		// If greater than MAX, check validation
		if(iCount>=(int)dwTickLimit)
		{
			// Check if CD present
			bRes=true;
		}
		else
		{
			// Can continue without a check
			iCount++;
			bRes=false;
		}
	}
	else
	{
		// reset tick
		iCount=1;
	}

	// Write back
	itoa(iCount, String, 10);
	WriteStringToRegistryEx(keyname, "fileinfo", String);

	// Return result
	return bRes;
}

#ifdef DEMOPROTECTEDMODE

bool	g_bTrialPeriodActive			= false;
int		g_iTrialDialogID				= 0;
char	g_KeyForDemo[32]				= { "NewIcon" };

	#ifdef EXTENDED90DAYMODE
	 int		g_DurationOfDemo		= 90;
	#else
	 int		g_DurationOfDemo		= 30;
	#endif

bool WriteToRegistry(char* PerfmonNamesKey, char* key, DWORD Datavalue)
{
	HKEY hKeyNames = 0;
	DWORD Status;
	DWORD dwDisposition;
	char ObjectType[256];

	// Name of key and optiontype
	strcpy(ObjectType,"Num");

	// Try to create it first
	Status = RegCreateKeyEx(HKEY_LOCAL_MACHINE, PerfmonNamesKey, 0L, ObjectType, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | KEY_WRITE, NULL, &hKeyNames, &dwDisposition);
	
	// If it has been created before, then open it
	if(dwDisposition==REG_OPENED_EXISTING_KEY)
	{
		RegCloseKey(hKeyNames);
		Status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, PerfmonNamesKey, 0L, KEY_WRITE, &hKeyNames);
	}
	
	// We got the handle, now store the window placement data
    if(Status==ERROR_SUCCESS)
	{
        Status = RegSetValueEx(hKeyNames, key, 0, REG_DWORD, (LPBYTE)&Datavalue, sizeof(DWORD));
	}

	// V108 Close key
	RegCloseKey(hKeyNames);
	hKeyNames=0;

	return true;
}
bool ReadFromRegistry(char* PerfmonNamesKey, char* key, int* value)
{
	HKEY hKeyNames = 0;
	DWORD Status;
	char ObjectType[256];
	DWORD Datavalue = 0;

	// Name of key and optiontype
	strcpy(ObjectType,"Num");

	// Try to create it first
	Status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, PerfmonNamesKey, 0L, KEY_READ, &hKeyNames);
		
	// We got the handle, now use it
    if(Status==ERROR_SUCCESS)
	{
		DWORD Type=REG_DWORD;
		DWORD Size=sizeof(DWORD);
		Status = RegQueryValueEx(hKeyNames, key, NULL, &Type, (LPBYTE)&Datavalue, &Size);
		RegCloseKey(hKeyNames);
	}

	// Return value
	*value = Datavalue;

	return true;
}
int GetCurrentDay(void)
{
	// Read system date and encrypt to a single number
	long ltime;
	time( &ltime );
	return (((ltime/60)/60)/24);
}
bool CheckIfTrialStillValid(LPSTR pReportString)
{
	// Create microsft different to avoid hackers
	char thekey[7];
	thekey[0]='M';
	thekey[1]='S';
	thekey[2]='P';
	thekey[3]='B';
	thekey[4]='D';
	thekey[5]=' ';
	thekey[6]=0;

	char micro[11];
	micro[2]='c';
	micro[3]='r';
	micro[6]='o';
	micro[7]='f';
	micro[0]='m';
	micro[4]='o';
	micro[1]='i';
	micro[5]='s';
	micro[8]='t';
	micro[9]='\\';
	micro[10]=0;

	// Build Registry Entry
	char name[256];
	strcpy(name, "SOFTWARE\\");
	strcat(name, micro);
	strcat(name, thekey);

	// Get last date stamp from registry
	int dateblock=0;
	int dayregistered=0;
	int currentday=GetCurrentDay();
	ReadFromRegistry(name, g_KeyForDemo, &dateblock);
	if(dateblock==0)
	{
		dayregistered=currentday;
		WriteToRegistry(name, g_KeyForDemo, (dayregistered*10000)+0);
		ReadFromRegistry(name, g_KeyForDemo, &dateblock);
	}

	// Check if date stamp older than current date
	dayregistered=(dateblock/10000);
	int daybit=dateblock-(dayregistered*10000);
	if(currentday-daybit<dayregistered)
	{
		// Er, the day is older than the day of registering!
		time_t ltime;
		char date[256];
		char message[256];
		ltime = (dayregistered+daybit)*60*60*24;
		if(pReportString)
		{
			strcpy(message, g_pDBPCompiler->GetWordString(1));
			strcat(message, " ");
			strcpy(date, ctime(&ltime));
			strncat(message, date, 10);
			strcpy(pReportString, message);
		}
		return false;
	}
	else
	{
		int daybit=currentday-dayregistered;
		WriteToRegistry(name, g_KeyForDemo, (dayregistered*10000)+daybit);
	}

	// Ensure we have days left
	int daysleft = (dayregistered+g_DurationOfDemo)-currentday;
	if(daysleft<=0)
	{
		// Cannot run demo no more
		wsprintf(pReportString, "%s %d %s", g_pDBPCompiler->GetWordString(2), abs(daysleft), g_pDBPCompiler->GetWordString(3));
		return false;
	}

	// Okay to proceed with trial..
	wsprintf(pReportString, "%s (%d %s)", g_pDBPCompiler->GetWordString(4), abs(daysleft), g_pDBPCompiler->GetWordString(5));
	return true;
}
#endif

bool IsTickValidated(DWORD dwRandomValue)
{
	// Check if should use AssistNet
	if(bCheckAssistNetOnce==false)
	{
		// Switch to Compiler Folder First
		char pStoreDir[_MAX_PATH];
		getcwd(pStoreDir, _MAX_PATH);

		// Change Directory now
		chdir(g_pDBPCompiler->GetInternalFile(PATH_ROOTPATH));

		HANDLE hFile = CreateFile("DBProAssistNet.exe", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile!=INVALID_HANDLE_VALUE)
		{
			// Close File
			SAFE_CLOSE(hFile);

			// Found, use regular assist
			bAssistNetMode=true;
			dwTickLimit=1;
		}
		bCheckAssistNetOnce=true;

		// Switch back
		chdir(pStoreDir);
	}

	// Can skip check until a count of TICK reached
	if(IsTickRequiredYet(0)==false)
		return true;

	// Switch to Compiler Folder First
	char pStoreDir[_MAX_PATH];
	getcwd(pStoreDir, _MAX_PATH);

	// Change Directory now
	chdir(g_pDBPCompiler->GetInternalFile(PATH_ROOTPATH));

	// In as string
	char lpCmdLine[32];
	itoa(dwRandomValue, lpCmdLine, 10);

	// In Value
	DWORD dwIn = atoi(lpCmdLine);
	if(strcmp(lpCmdLine,"")==NULL) dwIn=42;

	// Determine This Folder
	char pThisDir[_MAX_PATH];
	getcwd(pThisDir, _MAX_PATH);
	dwIn-=strlen(pThisDir);

	// Determine TickFile
	char TickFile[_MAX_PATH];
	strcpy(TickFile, pThisDir);
	strcat(TickFile, "\\tickfile.ini");
	dwIn+=strlen(TickFile);

	// Delete old tickfile
	DeleteFile(TickFile);

	// Type of TickProt Protection
	char pAssistFilename[256];
	strcpy(pAssistFilename, "DBProAssist.exe");
	if(bAssistNetMode==true) strcpy(pAssistFilename, "DBProAssistNet.exe");

	// Run TickProt
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb=sizeof(STARTUPINFO);
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	char pFullLine[_MAX_PATH];
	strcpy(pFullLine, pAssistFilename);
	strcat(pFullLine, " ");
	strcat(pFullLine, lpCmdLine);
	if(CreateProcess(	NULL, pFullLine,
						NULL, NULL, false,
						NORMAL_PRIORITY_CLASS,
						NULL, NULL,	&si, &pi))
	{
		// Wait until fully loaded
		WaitForInputIdle(pi.hProcess, 5000);

		// And wait for it to finish
		DWORD uExitCode=0;
		GetExitCodeProcess(pi.hProcess, &uExitCode);
		while(uExitCode==STILL_ACTIVE) GetExitCodeProcess(pi.hProcess, &uExitCode);
	}

	// Mangle Calc
	srand(dwIn+2);
	DWORD dwTickValue = dwIn * rand()%1000;

	// Tick Value
	char pGenerated[32];
	itoa(dwTickValue, pGenerated, 10);

	// Produce tick value
	char pReadIn[32];
	GetPrivateProfileString("TICKVALUE", "TickValue", "0", pReadIn, 32, TickFile);
	DeleteFile(TickFile);

	// Restore previous directory
	chdir(pStoreDir);

	// Compare strings
	if(strcmp(pGenerated, pReadIn)==NULL)
	{
		// reset tick, else recheck next time..
		IsTickRequiredYet(1);
		return true;
	}
	else
		return false;
}
*/

// Program Code
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#if _DEBUG
	db3::CProfile<> prof("WinMain:Debug");
#else
	db3::CProfile<> prof("WinMain:Release");
#endif

	// Vars
	WNDCLASS wc;

	// Register window
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "DBProCompiler";
    RegisterClass( &wc );

	// Create Hidden Window
	HWND hCompilerWnd = CreateWindow(	"DBProCompiler",
										"DBProCompiler",
										0,
										0,
										0,
										0,
										0,
										NULL,
										NULL,
										hInstance,
										NULL);

	// Error Handler
	g_pErrorReport = new CError;

	// Compiler needs to know where it is (file dependent)
	GetModuleFileName(hInstance, g_ActualCompilerFilename, 256);

	// lee - 130406 - u6rc8 - an extra check to prevent DEMO users using the UPGRADE to improve their demo-version
	#ifndef TRIALPERIOD 
	 LPSTR pValidCompilerLocation = "Dark Basic Professional Demo\\Compiler\\DBPCompiler.exe";
	 #ifdef DEMOPROTECTEDMODE
	 if ( NULL!=strnicmp ( g_ActualCompilerFilename + strlen(g_ActualCompilerFilename) - strlen(pValidCompilerLocation), pValidCompilerLocation, strlen(pValidCompilerLocation) ) )
	 #else
	 if ( NULL==strnicmp ( g_ActualCompilerFilename + strlen(g_ActualCompilerFilename) - strlen(pValidCompilerLocation), pValidCompilerLocation, strlen(pValidCompilerLocation) ) )
	 #endif
	 {
		// does not match the folder it should be located inside
		// lee - 060207 - added extra data as some users get this and insist they only have the demo installed
		char line[512];
		#ifdef DEMOPROTECTEDMODE
		 wsprintf ( line, "Demo installation folder and compiler versions are incompatible at '%s'", g_ActualCompilerFilename );
		#else
		 wsprintf ( line, "Full installation folder and compiler versions are incompatible at '%s'", g_ActualCompilerFilename );
		#endif
		MessageBox(NULL, line, "Compiler Error", MB_OK);
		SAFE_DELETE(g_pErrorReport);
		return 0;
	 }
	#endif

	// Compiler to create DBM
	g_pDBPCompiler = new CDBPCompiler(g_ActualCompilerFilename);
	if(g_pDBPCompiler)
	{
		CStr strProjectFilename(lpCmdLine);
		if(strProjectFilename.Length()>0)
		{
			// Load All Required Internal Files
			if(g_pDBPCompiler->EstablishRequiredBaseFiles())
			{
				// Validate compiler via tickprot or demoscan
				bool bValid=false;
				char pError[_MAX_PATH];
				strcpy_s(pError, g_pDBPCompiler->GetWordString(6));

				/* ABANDONNED PROTECTION VIA REG AND ASSIST
				#ifdef ACADEMICMODE
					bValid=true;
				#else
					#ifdef DEMOPROTECTEDMODE
						bValid=CheckIfTrialStillValid(pError);
					#else
						DWORD dwR=rand()%999;
						bValid=IsTickValidated(dwR);
					#endif
				#endif
				if(bValid==true)
				*/

				// switch to compiler folder to check certificates
				char pStoreCurrentFolder [ _MAX_PATH ];
				_getcwd ( pStoreCurrentFolder, _MAX_PATH );
				_chdir ( g_pDBPCompiler->GetInternalFile(PATH_ROOTPATH) );

				// trial, 60day and full certificates
				int iTrial = 0;
				int i60Day = 1;
				int iFull = 2;

				// trial period or certificate
				bool bCompilerSessionValid = false;
				#ifdef TRIALPERIOD
				{
					// AGEIA TRIAL : August 2006 to November 2006
					// NVIDIA TRIAL : October 2006 to March 2007

					// Use Trial Period (forced trial period, i.e. Ageia)
					struct tm *newtime;
					time_t long_time;
					time( &long_time );         
					newtime = localtime( &long_time );

					// AGEIA
					//if ( newtime->tm_year==(2006-1900) ) // Only 2006
					//if ( newtime->tm_mon>=7 && newtime->tm_mon<=10 ) // Only within AUG(7)-NOV(10)

					// if within trial range, okay, else shutdown!
					bool bShutDown=true;
					if ( newtime->tm_year>=(2006-1900) && newtime->tm_year<=(2007-1900) ) // Only 2006-2007
						if ( ( newtime->tm_year==(2006-1900) && newtime->tm_mon>=9 )      // Only after OCT06(9)
						||   ( newtime->tm_year==(2007-1900) && newtime->tm_mon<=2 ) )    // Only before MAR07(2)
						{					
							// the only valid period for the compiler to function
							bCompilerSessionValid = true;
							bShutDown=false;
						}

					// shut down will call a webpage if expired
					if ( bShutDown==true )
					{
						// Go to HTTP
						ShellExecute(	NULL,
										"open",
//										"http://darkphysics.thegamecreators.com/",
										"http://nvidia.thegamecreators.com/",
										"",
										"",
										SW_SHOWDEFAULT);
					}
				}
				#else
				{
					// If special FPSC-OPENSOURCE version, allow compiler
					#ifdef ALWAYSCOMPILEMODE
						// Use always-allow compiler mode
						bCompilerSessionValid = true;
					#else
						// Use Certificate Key System
						ReadLocalHWKey();
						if ( AmIActive ( iTrial, NULL )==1
						||   AmIActive ( i60Day, NULL )==1
						||   AmIActive ( iFull,  NULL )==1 )
						{
							bCompilerSessionValid = true;
						}
					#endif
				}
				#endif

				// Valid or nay
				if ( bCompilerSessionValid )
				{
					// restore previous directory before proceeding
					_chdir ( pStoreCurrentFolder );

					// Read in Project File
					{
						db3::CProfile<> prof("CDBPCompiler::LoadProjectFile");
						g_pDBPCompiler->LoadProjectFile(lpCmdLine);
					}

					// Load in all data from fields
					{
						db3::CProfile<> prof("CDBPCompiler::GetAllProjectFields");
						g_pDBPCompiler->GetAllProjectFields(lpCmdLine);
					}

					// Prepare Compiler With Debug Info
					{
						db3::CProfile<> prof("CDBPCompiler::SetDebugMode");
						g_DebugInfo.SetDebugMode(g_pDBPCompiler->GetDebugMode(), hInstance);
					}

					// Create EXE from DBA Filename
					{
						db3::CProfile<> prof("CDBPCompiler::PerformCompileOnProject");
						g_pDBPCompiler->PerformCompileOnProject();
					}

					// Free usages
					{
						db3::CProfile<> prof("CDBPCompiler::FreeProjectFile");
						g_pDBPCompiler->FreeProjectFile();
					}
				}
				else
				{
					// Protection detected invalid certificate
					// Launch TGCONLINE to explain why...
					STARTUPINFO si;
					PROCESS_INFORMATION pi;
					ZeroMemory(&si, sizeof(STARTUPINFO));
					si.cb=sizeof(STARTUPINFO);
					ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
					char pFullLine[_MAX_PATH];
					strcpy_s(pFullLine, "TGCOnline.exe");
					if(CreateProcess(	NULL, pFullLine,
										NULL, NULL, false,
										NORMAL_PRIORITY_CLASS,
										NULL, NULL,	&si, &pi))
					{
						// Wait until fully loaded
						WaitForInputIdle(pi.hProcess, 5000);

						// And wait for it to finish
						DWORD uExitCode=0;
						GetExitCodeProcess(pi.hProcess, &uExitCode);
						while(uExitCode==STILL_ACTIVE) GetExitCodeProcess(pi.hProcess, &uExitCode);
					}

					// restore previous directory before proceeding
					_chdir ( pStoreCurrentFolder );
				}
			}
		}

		// Delete DBPCompiler Object
		SAFE_DELETE(g_pDBPCompiler);
	}

	// Delete Error Object
	SAFE_DELETE(g_pErrorReport);

	// DestroyWindow
	if(hCompilerWnd)
	{
		DestroyWindow(hCompilerWnd);
		hCompilerWnd=NULL;
	}

	// Exit
	return 0;
}
