//
// Generate a Hardware Key and Validates Product Certificates
//

// Includes
#include <time.h>
#include "certificatekey.h"

//lee-280206 - u60 redundant, using DLL now
//#include "stdafx.h"

// DBPRO Trial Duration hardcoded
#ifdef ONEHUNDREDDAYTRIAL
 #define TRIALTIME 100
#else
 #define TRIALTIME 30
#endif

// Global data
char			g_pHardwareHashString [ 256 ] = {0};

// Global Product List
// 0 = 000 = Dark Basic Pro
// 1 = 001 = Advanced Terrain
// 2 = 002 = Enhancements
// 3 = 003 = Advanced Physics P
// 4 = 004 = Advanced Physics C
// 5 = 005 = Advanced Physics P+C
// 6 = 006 = EZ Rotate
// 7 = 007 = MEQON Physics
// 8 = 008 = LUA Scripting
// 9 = 009 = TextureMax
// 10 = 010 = DarkBasic Professional Extends
// 11 = 011 = Dark Physics
// 12 = 012 = Dark AI
// 13 = 013 = Dark Lights
// 14 = 014 = STYX
// 15 = 015 = CONVSEO
// 16 = 016 = Generic6
// 17 = 017 = Generic7
// 18 = 018 = Generic8
// 19 = 019 = Generic9

// Global product list data
LPSTR g_pProductList [ InternalProductListMax ] =
{
	"dbpro","dbpro60DAY","dbpro",
	"advterrainv1","advterrainv160DAY","advterrainv1",
	"advsystemv1","advsystemv160DAY","advsystemv1",
	"advphysicspv1","advphysicspv160DAY","advphysicspv1",
	"advphysicscv1","advphysicscv160DAY","advphysicscv1",
	"advphysicspcv1","advphysicspcv160DAY","advphysicspcv1",
	"ezrotatev1","ezrotatev160DAY","ezrotatev1",
	"meqonv1","meqonv160DAY","meqonv1",
	"luav1","luav160DAY","luav1",
	"texturemaxv1", "texturemaxv160DAY", "texturemaxv1",
	"extendsv1","extendsv1","extendsv1",
	"darkphysicsv1","darkphysicsv160DAY","darkphysicsv1",
	"darkaiv1","darkaiv160DAY","darkaiv1",
	"darklightsv1","darklightsv160DAY","darklightsv1",
	"styx","styx60DAY","styx",
	"convseo","convseo60DAY","convseo",
	"slot6","slot660DAY","slot6",
	"slot7","slot760DAY","slot7",
	"slot8","slot860DAY","slot8",
	"slot9","slot960DAY","slot9"
};
DWORD g_FillerToSeperateTablesA [ 10244 ];
LPSTR g_pProductDescList [ InternalProductListMax ] =
{
	"Dark Basic Professional Trial","Dark Basic Professional 60DAY","Dark Basic Professional",
	"Advanced Terrain Trial","Advanced Terrain 60DAY","Advanced Terrain",
	"Enhancements Trial","Enhancements 60DAY","Enhancements",
	"Advanced Physics (P) Trial","Advanced Physics (P) 60DAY","Advanced Physics (P)",
	"Advanced Physics (C) Trial","Advanced Physics (C) 60DAY","Advanced Physics (C)",
	"Advanced Physics (PC) Trial","Advanced Physics (PC) 60DAY","Advanced Physics (PC)",
	"EZRotate Trial","EZRotate 60DAY","EZRotate",
	"Meqon Trial","Meqon 60DAY","Meqon",
	"Lua Trial","Lua 60DAY","Lua",
	"Texture Max Trial", "Texture Max 60 Day", "Texture Max",
	"DarkBasic Professional Extends Trial","DarkBasic Professional Extends 60DAY","DarkBasic Professional Extends",
	"Dark Physics Trial","Dark Physics 60DAY","Dark Physics",
	"Dark AI Trial","Dark AI 60DAY","Dark AI",
	"Dark Lights Trial","Dark Lights 60DAY","Dark Lights",
	"StyX Trial","StyX 60DAY","StyX",
	"ConvSEO Trial","ConvSEO 60DAY","ConvSEO",
	"Protected Plugin Trial","Protected Plugin 60DAY","Protected Plugin",
	"Protected Plugin Trial","Protected Plugin 60DAY","Protected Plugin",
	"Protected Plugin Trial","Protected Plugin 60DAY","Protected Plugin",
	"Protected Plugin Trial","Protected Plugin 60DAY","Protected Plugin"
};
LPSTR g_pProductIcon [ InternalProductListMax ] =
{
	"dbpt.ico","dbpf.ico","dbpf.ico",
	"terraint.ico","terrainf.ico","terrainf.ico",
	"systemt.ico","systemf.ico","systemf.ico",
	"physicspt.ico","physicspf.ico","physicspf.ico",
	"physicsct.ico","physicscf.ico","physicscf.ico",
	"physicspct.ico","physicspcf.ico","physicspcf.ico",
	"ezrotatet.ico","ezrotatef.ico","ezrotatef.ico",
	"meqont.ico","meqonf.ico","meqonf.ico",
	"luat.ico","luaf.ico","luaf.ico",
	"texturemaxt.ico","texturemaxf.ico","texturemaxf.ico",
	"extends_trial.ico","extends_registered.ico","extends_registered.ico",
	"darkphysicst.ico","darkphysicsf.ico","darkphysicsf.ico",
	"darkait.ico","darkaif.ico","darkaif.ico",
	"darklightst.ico","darklightsf.ico","darklightsf.ico",
	"styxt.ico","styxf.ico","styxf.ico",
	"convseot.ico","convseof.ico","convseof.ico",
	"slot6t.ico","slot6f.ico","slot6f.ico",
	"slot7t.ico","slot7f.ico","slot7f.ico",
	"slot8t.ico","slot8f.ico","slot8f.ico",
	"slot9t.ico","slot9f.ico","slot9f.ico"
};
// DBPRO Weblinks (localised option)
// 240706 - Spanish requested localized link to their order page
#ifdef SPANISHLINKS
LPSTR g_pProductIconLink [ InternalProductListMax ] =
{
	"https://comercio.softonic.com/pv/2772/0/1","", "www.darkbasic.es", 
	"http://darkbasicpro.thegamecreators.com/?f=plugin_terrain_order","", "http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_enhancements_order","","http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_physics1_order","","http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_physics2_order","","http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_physics3_order","","http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_ezrotate_order","","http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_meqon_order","","http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_lua_order","","http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_texturemax_order","","http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_extends_order","","http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_darkphysics","","http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_darkai","","http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_darklights","","http://www.thegamecreators.com",
	"http://www.thegamecreators.com","","http://www.thegamecreators.com",
	"http://www.thegamecreators.com","","http://www.thegamecreators.com",
	"http://www.thegamecreators.com","","http://www.thegamecreators.com",
	"http://www.thegamecreators.com","","http://www.thegamecreators.com",
	"http://www.thegamecreators.com","","http://www.thegamecreators.com",
	"http://www.thegamecreators.com","","http://www.thegamecreators.com"
};
#else
LPSTR g_pProductIconLink [ InternalProductListMax ] =
{
	"http://darkbasicpro.thegamecreators.com/?f=order_online&cert=XXX","", "http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_terrain_order","", "http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_enhancements_order","","http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_physics1_order","","http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_physics2_order","","http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_physics3_order","","http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_ezrotate_order","","http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_meqon_order","","http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_lua_order","","http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_texturemax_order","","http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_extends_order","","http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_darkphysics","","http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_darkai","","http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_darklights","","http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_styx","","http://www.thegamecreators.com",
	"http://darkbasicpro.thegamecreators.com/?f=plugin_convseo","","http://www.thegamecreators.com",
	"http://www.thegamecreators.com","","http://www.thegamecreators.com",
	"http://www.thegamecreators.com","","http://www.thegamecreators.com",
	"http://www.thegamecreators.com","","http://www.thegamecreators.com",
	"http://www.thegamecreators.com","","http://www.thegamecreators.com"
};
#endif
LPSTR g_pProductDLLNames [ InternalProductListMax ] =
{
	"dbprocore.dll","dbprocore.dll","dbprocore.dll",
	"AdvancedTerrain.dll","AdvancedTerrain.dll","AdvancedTerrain.dll",
	"Enhancements.dll","Enhancements.dll","Enhancements.dll",
	"PhysicsMain_P_Master.dll","PhysicsMain_P_Master.dll","PhysicsMain_P_Master.dll",
	"PhysicsMain_C_Master.dll","PhysicsMain_C_Master.dll","PhysicsMain_C_Master.dll",
	"PhysicsMain_PC_Master.dll","PhysicsMain_PC_Master.dll","PhysicsMain_PC_Master.dll",
	"EZRotate.dll","EZRotate.dll","EZRotate.dll",
	"Meqon.dll","Meqon.dll","Meqon.dll",
	"Lua.dll","Lua.dll","Lua.dll",
	"TextureMax.dll", "TextureMax.dll", "TextureMax.dll",
	"DBProExtends.dll","DBProExtends.dll","DBProExtends.dll",
	"DarkPhysics.dll","DarkPhysics.dll","DarkPhysics.dll",
	"DarkAI.dll","DarkAI.dll","DarkAI.dll",
	"lightmapper.dll","lightmapper.dll","lightmapper.dll",
	"styx*","styx*","styx*",
	"convseo.dll","convseo.dll","convseo.dll",
	"slot6.dll","slot1.dll","slot6.dll",
	"slot7.dll","slot1.dll","slot7.dll",
	"slot8.dll","slot1.dll","slot8.dll",
	"slot9.dll","slot1.dll","slot9.dll"
};
DWORD g_FillerToSeperateTablesB [ 1080 ];
DWORD g_pProductCodes [ InternalProductListMax ] =
{
	0, 0, 0,		// dark basic Pro base code
	1, 1, 1,		// advanced terrain
	2, 2, 2,		// enhancements
	3, 3, 3,		// physics P
	4, 4, 4,		// physics C
	5, 5, 5,		// physics P+C
	6, 6, 6,		// ezrotate
	7, 7, 7,		// meqon
	8, 8, 8,		// lua
	9, 9, 9,		// texture max
	10, 10, 10,		// extends
	11, 11, 11,		// darkphysics
	12, 12, 12,		// darkai
	13, 13, 13,		// darklights
	14, 14, 14,		// styx
	15, 15, 15,		// convseo
	16, 16, 16,		// slot6
	17, 17, 17,		// slot7
	18, 18, 18,		// slot8
	19, 19, 19		// slot9
};
DWORD g_pProductDuration [ InternalProductListMax ] =
{
	TRIALTIME, 60, 0,		// dark basic Pro
	7, 60, 0,		// advanced terrain
	7, 60, 0,		// enhancements
	7, 60, 0,		// physics P
	7, 60, 0,		// physics C
	7, 60, 0,		// physics PC
	7, 60, 0,		// ezrotate
	7, 60, 0,		// meqon
	7, 60, 0,		// lua
	7, 60, 0,		// texture max
	7, 60, 0,		// extends
	7, 60, 0,		// Dark Physics
	7, 60, 0,		// Dark AI
	7, 60, 0,		// Dark Lights
	7, 60, 0,		// styx
	7, 60, 0,		// convseo
	7, 60, 0,		// slot6
	7, 60, 0,		// slot7
	7, 60, 0,		// slot8
	7, 60, 0		// slot9
};

// Also add text to code in TGCOnlineDlg.cpp Line: 831 (free certificates reminders)

// Global trial data
int  g_DurationOfDemo = TRIALTIME;

// Init MD5DLL Global DLL Calls
HMODULE					hMD5DLLHandle = NULL;
TMD5ChecksumGetMD5		gCMD5ChecksumGetMD5;
TMD5ChecksumFreeMD5		gCMD5ChecksumFreeMD5;

void InitMD5DLL ( void )
{
	hMD5DLLHandle = LoadLibrary("MD5.DLL");
	if ( hMD5DLLHandle )
	{
		gCMD5ChecksumGetMD5	= ( TMD5ChecksumGetMD5 ) GetProcAddress ( hMD5DLLHandle, "?GetMD5@@YAPADPAEI@Z" );
		gCMD5ChecksumFreeMD5	= ( TMD5ChecksumFreeMD5 ) GetProcAddress ( hMD5DLLHandle, "?FreeMD5@@YAXPAD@Z" );
	}
	else
	{
		//leeadd - 280206 - u60 - now uses a MD5.DLL in the Compiler folder
		MessageBox ( NULL, "Upgrade 6.0 onwards requires the MD5.DLL in the Compiler folder", "MD5 Error", MB_OK );
	}
}

void FreeMD5DLL ( void )
{
	FreeLibrary ( hMD5DLLHandle );
}

// Registry Function
bool WriteToRegistry(char* PerfmonNamesKey, char* key, LPSTR Datavalue)
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
		DWORD Size = strlen(Datavalue);
		DWORD Type = REG_SZ;
        Status = RegSetValueEx ( hKeyNames, key, 0, Type, (LPBYTE)Datavalue, Size );
	}

	// free usages
	RegCloseKey(hKeyNames);
	hKeyNames=0;

	return true;
}
bool ReadFromRegistry ( LPSTR PerfmonNamesKey, LPSTR key, LPSTR* pReturnString )
{
	HKEY hKeyNames = 0;
	DWORD Status;
	char ObjectType[256];

	// Name of key and optiontype
	strcpy(ObjectType,"Num");

	// Try to create it first
	Status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, PerfmonNamesKey, 0L, KEY_READ, &hKeyNames);
		
	// We got the handle, now use it
	LPSTR pData = NULL;
    if(Status==ERROR_SUCCESS)
	{
		DWORD Size = 256;
		DWORD Type = REG_SZ;
		pData = new char [ Size ];
		memset ( pData, 0, Size );
		Status = RegQueryValueEx(hKeyNames, key, NULL, &Type, (LPBYTE)pData, &Size);
		RegCloseKey(hKeyNames);
	}

	// Return data poiner
	*pReturnString = pData;

	// complete
	return true;
}

// Memory Concatenator
LPSTR AddToString ( LPSTR pOldString, LPSTR pNewString, bool bDeleteNewStrAndAddCR )
{
	// add extra CR
	DWORD dwExtra = 1;
	if ( bDeleteNewStrAndAddCR ) dwExtra=3;

	// add strings together to make new string
	DWORD dwNewSize = 0;
	LPSTR pReturnString = NULL;
	if ( pOldString==NULL )
	{
		dwNewSize = strlen(pNewString)+dwExtra;
		pReturnString = new char [ dwNewSize ];
		memcpy ( pReturnString, pNewString, dwNewSize );
	}
	else
	{
		DWORD dwOldSize = strlen(pOldString);
		dwNewSize = strlen(pOldString)+strlen(pNewString)+dwExtra;
		pReturnString = new char [ dwNewSize ];
		memcpy ( pReturnString, pOldString, dwOldSize );
		memcpy ( pReturnString + dwOldSize, pNewString, strlen(pNewString) );
	}

	// add carriage return
	if ( pReturnString )
	{
		if ( dwExtra==3 )
		{
			// full CR addition
			pReturnString [ dwNewSize-3 ]	= 13;
			pReturnString [ dwNewSize-2 ]	= 10;
			pReturnString [ dwNewSize-1 ]	= 0;
		}
		else
			pReturnString [ dwNewSize-1 ]	= 0;
	}

	// delete input strings
	if ( pOldString )
	{
		delete pOldString;
		pOldString=NULL;
	}
	if ( bDeleteNewStrAndAddCR )
	{
		if ( pNewString )
		{
			delete pNewString;
			pNewString=NULL;
		}
	}

	// complete
	return pReturnString;
}

LPSTR GenerateMangledDate ( int iDay )
{
	// create date string
	char datenumber[32];
	wsprintf ( datenumber, " %d ", iDay );

	// return string
	LPSTR pProductDate = NULL;
	pProductDate = AddToString ( pProductDate, ">>", false );
	pProductDate = AddToString ( pProductDate, datenumber, false );
	pProductDate = AddToString ( pProductDate, "<<", false );

	// mangle the return string
	char pMangled [ 1024 ];
	strcpy ( pMangled, pProductDate );
	strcat ( pMangled, "MANGLEDIT" );
	delete pProductDate;

	// make MD5
	InitMD5DLL();
	LPSTR pMangledString = (LPSTR) gCMD5ChecksumGetMD5 ( (BYTE*)pMangled, (UINT)strlen(pMangled) );

	// create dynamic alloc for string
	LPSTR pNewMangledString = new char [ strlen(pMangledString)+1 ];
	strcpy ( pNewMangledString, pMangledString );

	// free MD5
	gCMD5ChecksumFreeMD5 ( pMangledString );
	FreeMD5DLL();

	// return final string
	return pNewMangledString;
}

int GetCurrentDay(void)
{
	// Read system date and encrypt to a single number
	time_t timet;
	time( &timet );
	long ltime = (long)timet;
	return (((ltime/60)/60)/24);
}

//
// Functions for TGCProtectedApp
//
bool StampTodaysDate ( LPSTR pVerifierName, int iToday )
{
	// always update record to 'todays date'
	LPSTR pTodayString = GenerateMangledDate ( iToday );
	LPSTR pPRKey = "SOFTWARE\\Dark Basic\\Dark Basic Pro";
	WriteToRegistry ( pPRKey, pVerifierName, pTodayString );
	delete pTodayString;
	return true;
}

bool CheckRegistryRecord ( LPSTR pVerifierName, int iToday )
{
	// make sure record is EQUAL or BEFORE todays date (as far as trial goes)
	// otherwise the user has put date back...

	// scan registry for processor information
	LPSTR pVerifierString = NULL;
	LPSTR pPRKey = "SOFTWARE\\Dark Basic\\Dark Basic Pro";
	ReadFromRegistry ( pPRKey, pVerifierName, &pVerifierString );

	// no record means someone has deleted it - error
	if ( pVerifierString==NULL ) return false;
	if ( strcmp ( pVerifierString, "")==NULL ) return false;

	// use record and compare against 
	bool bRecordValid = false;
	for ( int iDay=iToday; iDay>iToday-g_DurationOfDemo; iDay-- )
	{
		// each day
		LPSTR pSearchString = GenerateMangledDate ( iDay );
		if ( _stricmp ( pSearchString, pVerifierString )==NULL )
		{
			// today is AFTER/EQUAL what record shows
			bRecordValid = true;
			iDay=iToday-g_DurationOfDemo;
		}
		delete pSearchString;
	}
	delete pVerifierString;

	// always update record to 'todays date' after valid compile
	if ( bRecordValid==true )
	{
		// always stamp the date
		StampTodaysDate ( pVerifierName, iToday );
	}

	// registry record validity
	return bRecordValid;
}

int IsStillActive ( int iProductIndex, LPSTR pMangledDate )
{
	// all full versions are except
	if ( iProductIndex%3==2 )
		return 0;

	// each product has its own duration
	g_DurationOfDemo = g_pProductDuration [ iProductIndex ];

	// work out mangled date for today, yesterday and back to find a match with that passed in
	int iTrialValidDaysLeft = -1;

	// check date against valid dates
	int iToday = GetCurrentDay();
	for ( int iDay=iToday; iDay>iToday-g_DurationOfDemo; iDay-- )
	{
		// each day
		LPSTR pThisDayString = NULL;
		pThisDayString = GenerateMangledDate ( iDay );
		if ( _stricmp ( pThisDayString, pMangledDate )==NULL )
		{
			// calender shows user within trial period (quick check against registry record)
			if ( CheckRegistryRecord ( g_pProductList[iProductIndex], iToday ) )
			{
				iTrialValidDaysLeft=g_DurationOfDemo-(iToday-iDay);
				iDay=iToday-g_DurationOfDemo;
			}
		}
		delete pThisDayString;
	}

	// date valid
	return iTrialValidDaysLeft;
}

void GetKindOfProduct ( LPSTR pKindOfProduct, int iKind )
{
	if ( iKind==0 ) strcpy ( pKindOfProduct, "TRIAL-CERTIFICATE" );
	if ( iKind==1 ) strcpy ( pKindOfProduct, "60DAY-CERTIFICATE" );
	if ( iKind==2 ) strcpy ( pKindOfProduct, "FULL-CERTIFICATE" );
}

int AmIActive ( int iProductIndex, LPSTR pDaysLeft )
{
	// ABSOLUTE SYSTEM CLOSEDOWN IF GOES BEYOND A FIXED DATE
	#ifdef ONEHUNDREDDAYTRIAL
	{
		// get todays date
		struct tm *newtime;
        time_t long_time;
        time( &long_time );         
        newtime = localtime( &long_time );

		// if within trial range, okay, else shutdown!
		bool bShutDown=true;
		if ( newtime->tm_year==(2004-1900) )
			if ( newtime->tm_mon>=6 && newtime->tm_mon<=10 )
				bShutDown=false;

		// expire if outside range
		if ( bShutDown==true )
		{
			MessageBox ( NULL, "This Trial Expires After DEC 1ST 2004", "Trial Has Expired", MB_OK);
			PostQuitMessage ( 0 );
			return 0;
		}
	}
	#endif

	// determine kind of product
	char pKindOfProduct[256];
	GetKindOfProduct ( pKindOfProduct, iProductIndex%3 );

	// create a valid certificate to test against
	int iActiveResult = 0;
	char pValidProductCertificate [ 1024 ];
	if ( strlen ( g_pHardwareHashString )==0 ) return 0;
	strcpy ( pValidProductCertificate, g_pHardwareHashString );
	strcat ( pValidProductCertificate, g_pProductList [ iProductIndex ] );
	strcat ( pValidProductCertificate, pKindOfProduct );

	// make MD5
	InitMD5DLL();
	LPSTR pValidProductHash = (LPSTR) gCMD5ChecksumGetMD5 ( (BYTE*)pValidProductCertificate, (UINT)strlen(pValidProductCertificate) );
	
	// scan certificate files and find match with passed-in product
	if ( _chdir ( "certificates" )==0 )
	{
		// file scan this folder
		struct _finddata_t m_filedata;
		long hInternalFile = _findfirst("*.*", &m_filedata);
		if ( hInternalFile )
		{
			// go through all files
			int iFileReturnValue = 0;
			while ( iFileReturnValue != -1 )
			{
				// get file or folder
				LPSTR pCertificateFilename = m_filedata.name;
				DWORD dwType = m_filedata.attrib;
				if(_stricmp(pCertificateFilename,".")!=NULL && stricmp(pCertificateFilename, "..")!=NULL)
				{
					if(!(dwType & _A_SUBDIR))
					{
						// return true the moment we find a valid certificate
						if ( strcmp ( pCertificateFilename, pValidProductHash )==NULL )
						{
							// get mangled date from INI
							char pMangledDate [ 256 ];
							char pAbsCertificateFilename [ 1024 ];
							_getcwd ( pAbsCertificateFilename, 1024 );
							strcat ( pAbsCertificateFilename, "\\");
							strcat ( pAbsCertificateFilename, pCertificateFilename );
							GetPrivateProfileString ( "PRODUCT", "DATE", "", pMangledDate, 256, pAbsCertificateFilename );

							// if trial, or full version, is it still active?
							int iDaysLeft = IsStillActive ( iProductIndex, pMangledDate );
							if ( iDaysLeft != -1 )
							{
								if ( pDaysLeft )
								{
									// no trial or days left string
									if ( iDaysLeft==0 )
										strcpy ( pDaysLeft, "" );
									else
										wsprintf ( pDaysLeft, "%d", iDaysLeft );
								}
								iActiveResult = 1;//valid
							}
							else
							{
								// certificate expired, kill date stamp
								StampTodaysDate ( g_pProductList[iProductIndex], 0 );
								iActiveResult = 2;
							}

							iFileReturnValue=-1;
							break;
						}
					}
				}

				// next file
				iFileReturnValue = _findnext(hInternalFile, &m_filedata);
			}

			// close filescan
			_findclose(hInternalFile);
			hInternalFile=NULL;
		}

		// leave folder
		_chdir ( ".." );
	}

	// free MD5
	gCMD5ChecksumFreeMD5 ( pValidProductHash );
	FreeMD5DLL();

	// this product is not active
	return iActiveResult;
}

LPSTR GetHWKey ( void )
{
	return g_pHardwareHashString;
}


//
// Functions for TGCOnline
//

void GenerateHWKey ( void )
{
	// return vat
	LPSTR pString = NULL;

	// scan registry for processor information
	LPSTR pProcessorIdentifier, pProcessorNameString, pProcessorVendorIdentifier;
	LPSTR pPRKey = "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0";
	ReadFromRegistry ( pPRKey, "Identifier", &pProcessorIdentifier );
	ReadFromRegistry ( pPRKey, "ProcessorNameString", &pProcessorNameString );
	ReadFromRegistry ( pPRKey, "VendorIdentifier", &pProcessorVendorIdentifier );

	// scan registry for harddrive information
	char pHDKey [ 1024 ];
	LPSTR pHardDriveIdentifier [ 5 ][ 8 ][ 2 ];
	memset ( pHardDriveIdentifier, 0, sizeof(pHardDriveIdentifier) );
	for ( int iPort=0; iPort<5; iPort++ )
	{
		for ( int iBus=0; iBus<8; iBus++ )
		{
			for ( int iTarget=0; iTarget<2; iTarget++ )
			{
				// get identifier of hardware device
				wsprintf ( pHDKey, "HARDWARE\\DEVICEMAP\\Scsi\\Scsi Port %d\\Scsi Bus %d\\Target ID %d\\Logical Unit Id 0", iPort, iBus, iTarget );
				ReadFromRegistry ( pHDKey, "Identifier", &pHardDriveIdentifier[iPort][iBus][iTarget] );
			}
		}
	}

	// build single string with all hardware profile data in
	if ( pProcessorIdentifier ) pString = AddToString ( pString, pProcessorIdentifier, true );
	if ( pProcessorNameString ) pString = AddToString ( pString, pProcessorNameString, true );
	if ( pProcessorVendorIdentifier ) pString = AddToString ( pString, pProcessorVendorIdentifier, true );
	for ( iPort=0; iPort<5; iPort++ )
		for ( int iBus=0; iBus<8; iBus++ )
			for ( int iTarget=0; iTarget<2; iTarget++ )
				if ( pHardDriveIdentifier[iPort][iBus][iTarget] ) pString = AddToString ( pString, pHardDriveIdentifier[iPort][iBus][iTarget], true );

	// final step is to hash down this information to an MD5
	InitMD5DLL();
	LPSTR lpHardwareHash = gCMD5ChecksumGetMD5 ( (BYTE*)pString, (UINT)strlen(pString) );
	strcpy ( g_pHardwareHashString, lpHardwareHash );
	gCMD5ChecksumFreeMD5(lpHardwareHash);
	FreeMD5DLL();

	// free usages
	delete pString;
}

//
// Functions for TGCOnline
//

void ReadLocalHWKey ( void )
{
	// return vat
	char pString [ _MAX_PATH ];
	strcpy ( pString, "" );

	// read local file for HWKEY string
	if ( _chdir ( "certificates" )==0 )
	{
		char pAbsLocalFilename [ _MAX_PATH ];
		getcwd ( pAbsLocalFilename, 1024 );
		strcat ( pAbsLocalFilename, "\\local.ini");
		GetPrivateProfileString ( "LOCAL", "CERT", "", pString, 256, pAbsLocalFilename );
		_chdir ( ".." );
	}

	// if no string
	if ( strcmp ( pString, "" )==NULL )
	{
		// must generate from scratch
		GenerateHWKey();
	}
	else
	{
		// assign hash to global string
		strcpy ( g_pHardwareHashString, pString );
	}
}

LPSTR GenerateCertificateForProduct ( int iIndex )
{
	// try this product name
	LPSTR pProductName = g_pProductList [ iIndex ];

	// determine kind of product
	char pKindOfProduct[256];
	GetKindOfProduct ( pKindOfProduct, iIndex%3 );

	// build local activation-hash (HARDWAREKEY+PRIVATEPRODUCTNAME+KIND)
	char pLocalActivationString [ 1024 ];
	strcpy ( pLocalActivationString, g_pHardwareHashString );
	strcat ( pLocalActivationString, pProductName );
	strcat ( pLocalActivationString, pKindOfProduct );

	// make and free MD5
	InitMD5DLL();
	LPSTR pLocalActivationReturnMD5 = (LPSTR) gCMD5ChecksumGetMD5 ( (BYTE*)pLocalActivationString, (UINT)strlen(pLocalActivationString) );
	LPSTR pLocalActivationHash = new char [ strlen(pLocalActivationReturnMD5)+1 ];
	strcpy ( pLocalActivationHash, pLocalActivationReturnMD5 );
	gCMD5ChecksumFreeMD5(pLocalActivationReturnMD5);
	FreeMD5DLL();

	// return created string
	return pLocalActivationHash;
}

int FindProductName ( LPSTR pResponseCertificate )
{
	// go through all products to find a match with certificate MD5
	for ( int iIndex=0; iIndex<InternalProductListMax; iIndex++ )
	{
		// get product certificate for index
		LPSTR pLocalActivationHash = GenerateCertificateForProduct ( iIndex );

		// compare with response certificate
		if ( strcmp ( pLocalActivationHash, pResponseCertificate )==NULL )
		{
			// product is listed at iIndex
			if ( pLocalActivationHash )
			{
				delete pLocalActivationHash;
				pLocalActivationHash=NULL;
			}
			return iIndex;
		}
		else
		{
			if ( pLocalActivationHash )
			{
				delete pLocalActivationHash;
				pLocalActivationHash=NULL;
			}
		}
	}

	// could not match certificate with any known product
	return -1;
}

int FindIndexOfPluginByDLLName ( LPSTR pDLLName )
{
	// go through all product dll names
	for ( int iIndex=0; iIndex<InternalProductListMax; iIndex++ )
	{
		// match against latest dll names
		LPSTR pThisDLLName = g_pProductDLLNames [ iIndex ];

		// leeadd - 250906 - u62c - multiple DLLs under one certificate
		if ( pThisDLLName )
		{
			DWORD dwLengthOfKnown = strlen(pThisDLLName)-1;
			if ( pThisDLLName [ dwLengthOfKnown ]=='*' )
			{
				// multiple DLL detect (wildcard MYNAME*)
				if ( strnicmp ( pThisDLLName, pDLLName, dwLengthOfKnown )==NULL )
					return iIndex;
			}
			else
			{
				// single DLL detect (MYDLL.DLL)
				if ( stricmp ( pThisDLLName, pDLLName )==NULL )
					return iIndex;
			}
		}
	}

	// not found
	return -1;
}

LPSTR BuildProductKey ( int iIndex )
{
	// try this product name
	LPSTR pProductName = g_pProductList [ iIndex ];

	// determine kind of product
	char pKindOfProduct[256];
	GetKindOfProduct ( pKindOfProduct, iIndex%3 );

	// build local activation-hash (HARDWAREKEY+PRIVATEPRODUCTNAME)
	char pLocalActivationString [ 1024 ];
	strcpy ( pLocalActivationString, g_pHardwareHashString );
	strcat ( pLocalActivationString, pProductName );
	strcat ( pLocalActivationString, pKindOfProduct );

	// make and free MD5
	InitMD5DLL();
	LPSTR pLocalActivationHash = (LPSTR) gCMD5ChecksumGetMD5 ( (BYTE*)pLocalActivationString, (UINT)strlen(pLocalActivationString) );
	LPSTR pReturnString = new char [ strlen(pLocalActivationHash)+1 ];
	strcpy ( pReturnString, pLocalActivationHash );
	gCMD5ChecksumFreeMD5(pLocalActivationHash);
	FreeMD5DLL();

	// return string
	return pReturnString;
}

LPSTR GenerateInstalledProducts ( LPSTR pYourInstalled, LPSTR pValidFor, LPSTR pDays, LPSTR pValidCert, LPSTR pExpiredCert, LPSTR pDBProNotActivated )
{
	// return vat
	LPSTR pInstalledAndActiveProducts = NULL;

	// go through all products
	int iCertificatesCount = 0;
	for ( int iProductIndex=0; iProductIndex<InternalProductListMax; iProductIndex++ )
	{
		// status code
		char pDaysLeftString [ 256 ];
		strcpy ( pDaysLeftString, "");
		int iStatusCode = AmIActive ( iProductIndex, pDaysLeftString );
		if ( iStatusCode > 0 )
		{
			// first product creates header
			if ( iCertificatesCount==0 )
			{
				pInstalledAndActiveProducts = AddToString ( pInstalledAndActiveProducts, pYourInstalled, false );
				pInstalledAndActiveProducts = AddToString ( pInstalledAndActiveProducts, " ", false );
			}
			else
				pInstalledAndActiveProducts = AddToString ( pInstalledAndActiveProducts, "\r\n", false );

			// add to installed products list
			pInstalledAndActiveProducts = AddToString ( pInstalledAndActiveProducts, g_pProductDescList [ iProductIndex ], false );

			// valid?
			if ( iStatusCode==1 )
			{
				if ( strcmp ( pDaysLeftString, "")!=NULL )
				{
					pInstalledAndActiveProducts = AddToString ( pInstalledAndActiveProducts, " ", false );
					pInstalledAndActiveProducts = AddToString ( pInstalledAndActiveProducts, pValidFor, false );
					pInstalledAndActiveProducts = AddToString ( pInstalledAndActiveProducts, " ", false );
					pInstalledAndActiveProducts = AddToString ( pInstalledAndActiveProducts, pDaysLeftString, false );
					pInstalledAndActiveProducts = AddToString ( pInstalledAndActiveProducts, " ", false );
					pInstalledAndActiveProducts = AddToString ( pInstalledAndActiveProducts, pDays, false );				
				}
				else
				{
					pInstalledAndActiveProducts = AddToString ( pInstalledAndActiveProducts, " ", false );
					pInstalledAndActiveProducts = AddToString ( pInstalledAndActiveProducts, pValidCert, false );
				}
			}
			else
			{
				pInstalledAndActiveProducts = AddToString ( pInstalledAndActiveProducts, " ", false );
				pInstalledAndActiveProducts = AddToString ( pInstalledAndActiveProducts, pExpiredCert, false );
			}

			// count the certificate/product
			iCertificatesCount++;
		}
	}

	// if no certificates, presume a trial version or new ESD version
	if ( iCertificatesCount==0 )
	{
		pInstalledAndActiveProducts = AddToString ( pInstalledAndActiveProducts, pDBProNotActivated, false );
		pInstalledAndActiveProducts = AddToString ( pInstalledAndActiveProducts, "\r\n", false );
	}

	// complete
	return pInstalledAndActiveProducts;
}
