#define _CRT_SECURE_NO_WARNINGS

#include "SteamCheckForWorkshop.h"
#include <stdio.h>

// Steam Multiplayer DLL functions needed
HMODULE SteamMultiplayerModule = NULL;

typedef void	(*t_SteamGetWorkshopItemPathDLL)(LPSTR);
typedef int		(*t_SteamIsWorkshopLoadingOnDLL)();

t_SteamGetWorkshopItemPathDLL Steam_SteamGetWorkshopItemPathDLL = NULL;
t_SteamIsWorkshopLoadingOnDLL Steam_SteamIsWorkshopLoadingOnDLL = NULL;
// End of Steam Multiplayer DLL Functions needed


bool CheckForWorkshopFile ( LPSTR VirtualFilename)
{

	if ( !VirtualFilename ) return false;
	if ( strlen ( VirtualFilename ) < 3 ) return false;

	char* tempCharPointerCheck = NULL;
	tempCharPointerCheck = strrchr( VirtualFilename, '\\' );
	if ( tempCharPointerCheck == VirtualFilename+strlen(VirtualFilename)-1 ) return false;
	if ( VirtualFilename[0] == '.' ) return false;
	if ( strstr ( VirtualFilename , ".fpm" ) ) return false;

	// encrypted file check
	char szEncryptedFilename[_MAX_PATH];
	char szEncryptedFilenameFolder[_MAX_PATH];
	char* tempCharPointer = NULL;
	strcpy ( szEncryptedFilenameFolder, VirtualFilename );

	// replace and forward slashes with backslash
	for ( int c = 0 ; c < strlen(szEncryptedFilenameFolder); c++ )
	{
		if ( szEncryptedFilenameFolder[c] == '/' ) 
			szEncryptedFilenameFolder[c] = '\\';
	}

	tempCharPointer = strrchr( szEncryptedFilenameFolder, '\\' );
	if ( tempCharPointer && tempCharPointer != szEncryptedFilenameFolder+strlen(szEncryptedFilenameFolder)-1 )
	{
		tempCharPointer[0] = 0;
		sprintf ( szEncryptedFilename , "%s\\_e_%s" , szEncryptedFilenameFolder , tempCharPointer+1 );
	}
	else
	{
		sprintf ( szEncryptedFilename , "_e_%s" , szEncryptedFilenameFolder );
	}
	FILE* tempFile = NULL;
	tempFile = fopen ( szEncryptedFilename ,"r" );
	if ( tempFile )
	{
		fclose ( tempFile );
		strcpy ( VirtualFilename , szEncryptedFilename );
		return true;
	}
	// end of encrypted file check

	// Check if the functions pointers exist, if they don't - jolly well make them!
	if ( Steam_SteamIsWorkshopLoadingOnDLL==NULL )
	{
		// Setup pointers to Steam functions
		SteamMultiplayerModule = LoadLibrary ( "SteamMultiplayer.dll" );

		if ( !SteamMultiplayerModule )
		{
			//MessageBox(NULL, "Unable to find SteamMultiplayer", "SteamMultiplayer Error", NULL);
			return false;
		}

		Steam_SteamGetWorkshopItemPathDLL=(t_SteamGetWorkshopItemPathDLL)GetProcAddress( SteamMultiplayerModule , "?SteamGetWorkshopItemPathDLL@@YAXPAD@Z" );
		Steam_SteamIsWorkshopLoadingOnDLL=(t_SteamIsWorkshopLoadingOnDLL)GetProcAddress ( SteamMultiplayerModule , "?SteamIsWorkshopLoadingOnDLL@@YAHXZ" );	
		// End of setup pointers to steam functions
	}

	// CHECK FOR WORKSHOP ITEM
	if ( !Steam_SteamIsWorkshopLoadingOnDLL || !Steam_SteamGetWorkshopItemPathDLL ) 
		return false;	

	if ( Steam_SteamIsWorkshopLoadingOnDLL() == 1 )
	{
		char szWorkshopFilename[_MAX_PATH];
		char szWorkshopFilenameFolder[_MAX_PATH];
		char szWorkShopItemPath[_MAX_PATH];
		Steam_SteamGetWorkshopItemPathDLL(szWorkShopItemPath);
		//strcpy ( szWorkShopItemPath,"D:\\Games\\Steam\\steamapps\\workshop\\content\\266310\\378822626");
		// If the string is empty then there is no active workshop item, so we can return
		if ( strcmp ( szWorkShopItemPath , "" ) == 0 ) return false;
		char* tempCharPointer = NULL;
		strcpy ( szWorkshopFilenameFolder, VirtualFilename );

		// only check if the workshop item path isnt blank
		if ( strcmp ( szWorkShopItemPath , "" ) )
		{
			// replace and forward slashes with backslash
			for ( unsigned int c = 0 ; c < strlen(szWorkshopFilenameFolder); c++ )
			{
				if ( szWorkshopFilenameFolder[c] == '/' )
					szWorkshopFilenameFolder[c] = '\\';

				if ( szWorkshopFilenameFolder[c] == '_' )
					szWorkshopFilenameFolder[c] = '@';
			}

			// strip off any path to files folder
			bool found = true;
			while ( found )
			{
			char* stripped = strstr ( szWorkshopFilenameFolder , "Files\\" );
			if ( !stripped )
				stripped = strstr ( szWorkshopFilenameFolder , "files\\" );

			if ( stripped )
				strcpy ( szWorkshopFilenameFolder , stripped+6 );
			else
				found = false;
			}

			bool last = false;
			char tempstring[MAX_PATH];
			strcpy ( tempstring, szWorkshopFilenameFolder);
			strcpy ( szWorkshopFilenameFolder , "" );
			// replace and forward slashes with backslash
			for ( unsigned int c = 0 ; c < strlen(tempstring); c++ )
			{
				if ( tempstring[c] == '/' || tempstring[c] == '\\' ) 
				{
					if ( last == false )
					{
						strcat ( szWorkshopFilenameFolder , "_" );
						last = true;
					}
				}
				else
				{
					strcat ( szWorkshopFilenameFolder , " " );
					szWorkshopFilenameFolder[strlen(szWorkshopFilenameFolder)-1] = tempstring[c];
					last = false;
				}
			}

			//NEED TO CHECK IF THE FILE EXISTS FIRST, IF IT DOES WE COPY IT
			char szTempName[_MAX_PATH];
			strcpy ( szTempName , szWorkShopItemPath );
			strcat ( szTempName , "\\" );
			strcat ( szTempName , szWorkshopFilenameFolder );

			/*FILE* tempy = NULL;
			tempy = fopen ( "f:\\DUMPFILE.txt" ,"a" );
			if ( tempy )
			{
				fputs ( szTempName , tempy );
				fputs ( "\n" , tempy );
				fclose ( tempy );
			}*/

			FILE* tempFile = NULL;
			tempFile = fopen ( szTempName ,"r" );
			if ( tempFile )
			{
				fclose ( tempFile );
				int szTempNamelength = strlen(szTempName);
				int virtualfilelength = strlen(VirtualFilename);				
				strcpy ( VirtualFilename , szTempName );

				/*FILE* tempy = NULL;
				tempy = fopen ( "f:\\DUMPFILE.txt" ,"a" );
				if ( tempy )
				{					
					fputs ( szWorkShopItemPath , tempy );
					fputs ( "\n" , tempy );
					fputs ( VirtualFilename , tempy );
					fputs ( "\n" , tempy );
					fputs ( "============" , tempy );
					fputs ( "\n" , tempy );
					fclose ( tempy );
				}*/

				return true;
			}
			else // check for encrypted version
			{
				char* tempCharPointer = NULL;

				tempCharPointer = strrchr( szTempName, '\\' );
				if ( tempCharPointer && tempCharPointer != szTempName+strlen(szTempName)-1 )
				{
					tempCharPointer[0] = 0;
					sprintf ( szWorkshopFilename , "%s\\_w_%s" , szTempName , tempCharPointer+1 );
				}
				else
				{
					sprintf ( szWorkshopFilename , "_w_%s" , szTempName );
				}
				FILE* tempFile = NULL;
				tempFile = fopen ( szWorkshopFilename ,"r" );
				if ( tempFile )
				{
					fclose ( tempFile );
					strcpy ( VirtualFilename , szWorkshopFilename );
					return true;
				}
			}
		}
	}

	return false;
	// END OF CHECK FOR WORKSHOP ITEM
}

bool Steam_CanIUse_W_()
{

	// Check if the functions pointers exist, if they don't - jolly well make them!
	if ( Steam_SteamIsWorkshopLoadingOnDLL==NULL )
	{
		// Setup pointers to Steam functions
		SteamMultiplayerModule = LoadLibrary ( "SteamMultiplayer.dll" );

		if ( !SteamMultiplayerModule )
		{
			//MessageBox(NULL, "Unable to find SteamMultiplayer", "SteamMultiplayer Error", NULL);
			return false;
		}

		Steam_SteamGetWorkshopItemPathDLL=(t_SteamGetWorkshopItemPathDLL)GetProcAddress( SteamMultiplayerModule , "?SteamGetWorkshopItemPathDLL@@YAXPAD@Z" );
		Steam_SteamIsWorkshopLoadingOnDLL=(t_SteamIsWorkshopLoadingOnDLL)GetProcAddress ( SteamMultiplayerModule , "?SteamIsWorkshopLoadingOnDLL@@YAHXZ" );	
		// End of setup pointers to steam functions
	}

	if ( !Steam_SteamIsWorkshopLoadingOnDLL || !Steam_SteamGetWorkshopItemPathDLL ) 
		return false;	

	if ( Steam_SteamIsWorkshopLoadingOnDLL() == 1 )
		return true;

	return false;
}