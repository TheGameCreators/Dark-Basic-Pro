// Dark Memory Manager

/*

TO DO


*/

#define _CRT_SECURE_NO_WARNINGS
#include "DarkMemoryManager.h"

// If this is not set, memory manager will be totally excluded
#ifdef  __USE_MEMORY_MANAGER__

#include <stdlib.h>
#include <iostream>
#include <memory.h>
#include <windows.h>
#include <direct.h>
#include <vector>
#include "Shlobj.h" // so can access Shell to get DOCUMENTS folder locationfv

// Max number of allocated files allowed at one time
#define MM_MAX_ALLOC_SIZE 100000

using namespace std;

// Externals set by the code running MM
extern char g_MM_DLLName [ 256 ];
extern char g_MM_FunctionName [ 256 ];
//

// Globals
char mm_OriginalDirectory [ 256 ] = "";
bool mm_BeenRun = false;
FILE* mm_File = NULL;
FILE* mm_SnapFile = NULL;
DWORD mm_StartTime = 0;
int mm_AllocsMade = 0;
int mm_FreesMade = 0;
size_t mm_MemoryUsed = 0;
size_t mm_MemoryFreed = 0;
int mm_Count = 0;
size_t mm_StartMemory = 0;
// End of Globals

// So we can track every allocation and tell when it is freed
struct mm_Alloc
{
	DWORD time;
	char function[256];
	void* address;
	size_t size;
	int index;
};

// Fixed size as any memory allocation will set off MM
int mm_AllocMax = 0;
mm_Alloc mm_Allocs[MM_MAX_ALLOC_SIZE];

// Operator New override
void* operator new(size_t size)
{
   void *p;
   
	p = malloc(size);

	// No memory spare?
    if(p == NULL) 
	{
		MessageBox(NULL, "Could not allocate memory", "MEMORY MANAGER ERROR", NULL);
    }

	// If mm_init() hasn't been called, lets call it
	if (!mm_BeenRun)
		mm_init();

	// Write the allocation to the log and add it to our alloc list
	if ( mm_File )
	{
		// time stamp
		mm_Time();
		char s[256];
		sprintf_s ( s , "- %s %s\tAllocated %u bytes\n" , g_MM_DLLName , g_MM_FunctionName , size );
		// increase memory used
		mm_MemoryUsed += size;
		// add to list
		mm_Add_Alloc(p,size);
		fputs ( s , mm_File );
		// write to file immediately
		fflush ( mm_File );
		// increase how many allocs
		mm_AllocsMade++;
	}

	// return the memory
    return (p);
}

// Operator delete override
void operator delete(void* ptr)
{
	// If null warn about it
	if ( !ptr )
	{
		// DAVE, can you log these to a file instead, as the messagebox can be annoying when there are 300 of them!
		// MessageBox(NULL, "Tried to delete null" , "MEMORY MANAGER ERROR", NULL);
		return;
	}

	// if init hasnt been called, call it
	if (!mm_BeenRun)
		mm_init();

	// add to log file and remove the item from our list
	if ( mm_File )
	{
		// time stamp
		mm_Time();
		char s[256];
		// log
		sprintf_s ( s , "+ %s %s\tFreed %u bytes\n" , g_MM_DLLName , g_MM_FunctionName , mm_GetSize(ptr) );
		// add to memory freed
		mm_MemoryFreed += mm_GetSize(ptr);
		// remove from list
		mm_Delete_Alloc(ptr);
		fputs ( s , mm_File );
		// write immediately
		fflush ( mm_File );
		// add to frees made
		mm_FreesMade++;
	}

	// free the memory
	free(ptr);

}

// initial setup
void mm_init()
{
	// init has been called
	mm_BeenRun = true;

	// store current dir
	char pOldDir[512];
	GetCurrentDirectory ( 512, pOldDir );

	// get init folder and story it
	//GetCurrentDirectory ( 256, mm_OriginalDirectory );
	SHGetFolderPath( NULL, CSIDL_PERSONAL, NULL, 0, mm_OriginalDirectory );
	strcat ( mm_OriginalDirectory, "\\FPSC Reloaded Files\\" );
	SetCurrentDirectory ( mm_OriginalDirectory );

	// initial time
	mm_StartTime = timeGetTime();

	// make the folders if they dont exist, empty them if they do
	_mkdir("mmlogs");
	mm_EmptyFolder ( "mmlogs" );

	_mkdir("mmsnapshots");
	mm_EmptyFolder ( "mmsnapshots" );

	// kick off the log file
	char name[256];
	sprintf_s ( name , "mmlogs\\%s_Memory_Usage_Log.txt" , g_MM_DLLName );
	FILE* file;
	file = fopen ( name  , "w" );

	if ( !file )
	{
		MessageBox(NULL, "Failed to create log file" , "MEMORY MANAGER ERROR", NULL);
	}

	// store log file, it stays open throughout
	mm_File = file;

	sprintf_s ( name , "==== %s Memory Usage Log ====\n" , g_MM_DLLName );
	fputs ( name , file );

	// stamp memory
	fputs ( "Starting Memory = " , file );
	mm_Memory();

	// write immediately
	fflush ( file );

	//===

	// create snapshot file
	sprintf_s ( name , "mmsnapshots\\%s_Memory_Usage_Snapshot.txt" , g_MM_DLLName );
	file = fopen ( name  , "w" );

	// failed?
	if ( !file )
	{
		MessageBox(NULL, "Failed to create snapshot file" , "MEMORY MANAGER ERROR", NULL);
	}

	// store snapshot file (although these do get closes as they are written to after a snapshot so we can compile them later)
	mm_SnapFile = file;

	sprintf_s ( name , "==== %s Memory Usage Snapshots ====\n" , g_MM_DLLName );
	fputs ( name , file );

	// stamp memory
	mm_Memory();

	// write immediately
	fflush ( file );
	// close snapshot file
	fclose ( file );

	//===

	// set our alloc list to null - null is how we know it is not in use, allowing us to reuse
	for ( int c = 0 ; c < MM_MAX_ALLOC_SIZE ; c++ )
	{	
		mm_Allocs[c].address = NULL;
	}

	// restore current dir
	SetCurrentDirectory ( pOldDir );
}

// memory stamp routine
void mm_Memory()
{
	// check the file exists, if not, exit out
	if (!mm_File ) return;

	// memory locals
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    char s[256];

	if ( mm_StartMemory == 0 )
		mm_StartMemory = (size_t)status.ullAvailPhys;

	// mb
	float mb = ( status.ullAvailPhys / 1024.0f ) / 1024.0f;
	// bytes
	size_t b = (size_t)status.ullAvailPhys;

	// write it out
	sprintf_s ( s , "%f mb (%u bytes)\n" , mb , b );
	fputs ( s , mm_File );
}

// time stamp
void mm_Time()
{
	// if the log file doesnt exist, exit
	if (!mm_File ) return;

	// stamp the time in
	char s[256];
	sprintf_s ( s , "%010d: " , timeGetTime() - mm_StartTime );
	fputs ( s , mm_File );
}

// empty a folder of any .txt files
bool mm_EmptyFolder ( char* szDirectory )
{
	// grab current folder so we can return to it after
	char szOriginalDirectory [ 256 ] = "";
	GetCurrentDirectory ( 256, szOriginalDirectory );

	// check folder passed exists and attempt to set it as the folder
	if ( strlen ( szDirectory ) > 0 )
	{
		if ( !SetCurrentDirectory ( szDirectory ) )
			return false;
	}
	else
		return false;

	// locals
	WIN32_FIND_DATA fileData;
	HANDLE hSearch;

	// iterate through all .txt files and delete them
	hSearch = FindFirstFile ( "*.txt", &fileData );

	if ( hSearch != INVALID_HANDLE_VALUE )
		DeleteFile(fileData.cFileName);
	
	while ( FindNextFile ( hSearch, &fileData ) ) 
		DeleteFile(fileData.cFileName);
	
	FindClose ( hSearch );

	// put folder back to what it was
	SetCurrentDirectory ( szOriginalDirectory );

	return true;
}

// add an entry into the list
void mm_Add_Alloc ( void* p , size_t size)
{
	// if found stays -1 something is wrong
	int found = -1;

	// find a free slot
	for ( int c = 0 ; c < MM_MAX_ALLOC_SIZE ; c++ )
	{
		if ( mm_Allocs[c].address == NULL )
		{
			// found one
			if ( c > mm_AllocMax ) mm_AllocMax = c;
			found = c;
			break;
		}
	}

	// we didnt find a free slot
	if ( found == -1 )
	{
		char s[256];
		sprintf_s ( s , "Run out of room in %s" , g_MM_DLLName );
		MessageBox(NULL, s , "MEMORY MANAGER ERROR", NULL);
		return;
	}

	// story allocation
	// address
	mm_Allocs[found].address = p;
	// size of chunk
	mm_Allocs[found].size = size;
	// function name that called it
	strcpy_s ( mm_Allocs[found].function , g_MM_FunctionName );
	// time of allocation
	mm_Allocs[found].time = timeGetTime();
	// increase index count
	mm_Allocs[found].index = mm_Count++;

}

// find and remove an entry from the list
void mm_Delete_Alloc ( void* p )
{
	// if found stays -1 then the item was not there
	int found = -1;

	for ( int c = 0 ; c <= mm_AllocMax ; c++ )
	{
		// found it
		if ( mm_Allocs[c].address == p )
		{
			found = c;
			break;
		}
	}

	// didnt find it
	if ( found == -1 )
	{
		char s[256];
		sprintf_s ( s , "Memory has not been allocated %s" , g_MM_DLLName );
		MessageBox(NULL, s , "MEMORY MANAGER ERROR", NULL);
		return;
	}

	// set it to null so it can be reused
	mm_Allocs[found].address = NULL;

}

// get size of allocated memory from our list
size_t mm_GetSize ( void* p )
{
	// if found remains -1 then it has failed to find it
	int found = -1;

	for ( int c = 0 ; c <= mm_AllocMax ; c++ )
	{
		// found
		if ( mm_Allocs[c].address == p )
		{
			found = c;
			break;
		}
	}

	// didnt find it
	if ( found == -1 )
	{
		char s[256];
		sprintf_s ( s , "Memory has not been allocated %s" , g_MM_DLLName );
		MessageBox(NULL, s , "MEMORY MANAGER ERROR", NULL);
		return 0;
	}

	// return the size of the memory allocated
	return mm_Allocs[found].size;

}

// write out a snap shot
__declspec ( dllexport ) void mm_SnapShot()
{
	// if init has not been called, call it
	if (!mm_BeenRun)
		mm_init();

	// open the snap shot file and append to it
	char name[256];
	sprintf_s ( name , "%s\\mmsnapshots\\%s_Memory_Usage_Snapshot.txt" , mm_OriginalDirectory , g_MM_DLLName );
	mm_SnapFile = fopen ( name  , "a" );

	if ( !mm_SnapFile )
	{
		MessageBox(NULL, "Failed to create snapshot file" , "MEMORY MANAGER ERROR", NULL);
	}

	// write out the data
	char s[256];
	fputs ( "\n" , mm_SnapFile );
	sprintf_s ( s , "Snapshot Time: %d\n" , timeGetTime()-mm_StartTime );
	fputs ( s , mm_SnapFile );
	fputs ( "\n" , mm_SnapFile );

	sprintf_s ( s , "Allocations made: %d\n" , mm_AllocsMade );
	fputs ( s , mm_SnapFile );
	sprintf_s ( s , "Frees made:       %d\n" , mm_FreesMade );
	fputs ( s , mm_SnapFile );

	fputs ( "\n" , mm_SnapFile );

	sprintf_s ( s , "Memory Used:      %u\n" , mm_MemoryUsed );
	fputs ( s , mm_SnapFile );
	sprintf_s ( s , "Memory Freed:     %u\n" , mm_MemoryFreed );
	fputs ( s , mm_SnapFile );

	fputs ( "\n" , mm_SnapFile );

	sprintf_s ( s , "Unfreed Memory:   %u\n" , mm_MemoryUsed - mm_MemoryFreed );
	fputs ( s , mm_SnapFile );

	fputs ( "\n" , mm_SnapFile );

	// write out any memory not freed in our list, starting with the lowest index to ensure they are listed in order
	// since the list elements can be reused, they will likely not be sitting in order
	for ( int a = 0 ; a <= mm_Count; a++ )
	{
		for ( int c = 0 ; c <= mm_AllocMax ; c++ )
		{
			if ( mm_Allocs[c].address != NULL )
			{
				if ( mm_Allocs[c].index == a )
				{
					sprintf_s ( s , "Unfreed Alloc: %s, %s, size: %u, time: %d\n" , g_MM_DLLName , mm_Allocs[c].function, mm_Allocs[c].size, mm_Allocs[c].time );
					fputs ( s , mm_SnapFile );
				}
			}
		}
	}

	fputs ( "\n" , mm_SnapFile );

	fputs ( "=====================================\n" , mm_SnapFile );
	// write immiedately
	fflush ( mm_SnapFile );
	// close file
	fclose ( mm_SnapFile );
}

// Report is called by the main app and pulls information from any snapshot files, using the latest info only
void mm_Report()
{
	// store original folder
	char szOriginalDirectory [ 256 ] = "";
	GetCurrentDirectory ( 256, szOriginalDirectory );

	// set our folder to the snapshots folder if we can, if not leave nice and quiet
	if ( strlen ( mm_OriginalDirectory ) > 0 )
	{
		char szDir[256];
		sprintf_s ( szDir , "%s\\mmsnapshots" ,mm_OriginalDirectory );

		if ( !SetCurrentDirectory ( szDir ) )
			return;
	}
	else
		return;

	// memory snapshot file
	char name[256];
	sprintf_s ( name , "%s\\FPSCR_Memory_SnapShot.txt" , mm_OriginalDirectory );
	FILE* file;
	file = fopen ( name , "w" );

	// csv version
	sprintf_s ( name , "%s\\FPSCR_Memory_SnapShot.csv" , mm_OriginalDirectory );
	FILE* fileCSV;
	fileCSV = fopen ( name , "w" );

	// failed
	if ( !file )
	{
		MessageBox(NULL, "Failed to create snapshot file" , "MEMORY MANAGER ERROR", NULL);
	}

	sprintf_s ( name , "==== FPSCR_Memory_SnapShot ====\n\n" );
	fputs ( name , file );

	// locals
	float mb = ( mm_StartMemory / 1024.0f ) / 1024.0f;
	size_t b = (size_t)mm_StartMemory;
	sprintf_s ( name , "Starting Memory: %f mb (%u bytes)\n" , mb , b );
	fputs ( name , file );

	// grab the ending memory of our app
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);

	mb = ( status.ullAvailPhys / 1024.0f ) / 1024.0f;
	b = (size_t)status.ullAvailPhys;

	sprintf_s ( name , "Ending Memory:   %f mb (%u bytes)\n" , mb , b );
	fputs ( name , file );
	fputs ( "\n" , file );

	// csv header
	sprintf_s ( name , "Name,Allocs,Frees,Used,Freed,Unfreed\n" );
	fputs ( name , fileCSV );

	// locals
	WIN32_FIND_DATA fileData;
	HANDLE hSearch;
	FILE* tFile;
	char lastName[256];
	char lastAllocMade[256];
	int iAllocMade = 0;
	char lastFreesMade[256];
	int iFreesMade = 0;
	char lastUsedMade[256];
	int iUsedMade = 0;
	char lastFreedMade[256];
	int iFreedcMade = 0;
	char lastUnfreedMade[256];
	int iUnfreedMade = 0;
	char s[256];

	char szDir[256];
	sprintf_s ( szDir , "%s\\mmsnapshots" ,mm_OriginalDirectory );
	SetCurrentDirectory ( szDir );

	// iterate through all snapshot files and pull the last snapshot out of each
	hSearch = FindFirstFile ( "*.txt", &fileData );

	if ( hSearch != INVALID_HANDLE_VALUE )
	{
		tFile = fopen ( fileData.cFileName , "r" );
		strcpy ( lastName , fileData.cFileName );
		lastName[strlen(fileData.cFileName)-26] = '\0';
		if (tFile) 
		{
			while (fgets( s, 256, tFile)!=NULL)
			{
				if ( strstr ( s , "Allocations made:" ) )
				{
					strcpy ( lastAllocMade , s );
					iAllocMade = atoi ( lastAllocMade+18 );
				}
				if ( strstr ( s , "Frees made:" ) )
				{
					strcpy ( lastFreesMade , s );
					iFreesMade = atoi ( lastFreesMade+18 );
				}
				if ( strstr ( s , "Memory Used:" ) )
				{
					strcpy ( lastUsedMade , s );
					iUsedMade = atoi ( lastUsedMade+18 );
				}
				if ( strstr ( s , "Memory Freed:" ) )
				{
					strcpy ( lastFreedMade , s );
					iFreedcMade = atoi ( lastFreedMade+18 );
				}
				if ( strstr ( s , "Unfreed Memory:" ) )
				{
					strcpy ( lastUnfreedMade , s );
					iUnfreedMade = atoi ( lastUnfreedMade+18 );
				}
			}
			fclose(tFile);

			// store the info in txt and csv
			sprintf_s ( name , "%-15s -> Allocs: %-10d, Frees: %-10d, Used: %-10d, Freed: %-10d, Unfreed: %-10d\n" , lastName , iAllocMade, iFreesMade, iUsedMade, iFreedcMade, iUnfreedMade );
			fputs ( name , file );

			sprintf_s ( name , "Name,Allocs, Frees, Used, Freed, Unfreed\n" );
			sprintf_s ( name , "%s,%d,%d,%d,%d,%d\n" , lastName , iAllocMade, iFreesMade, iUsedMade, iFreedcMade, iUnfreedMade);
			fputs ( name , fileCSV );
		}
	}
		
	// find the next file and pull info out
	while ( FindNextFile ( hSearch, &fileData ) ) 
	{
		tFile = fopen ( fileData.cFileName , "r" );
		strcpy ( lastName , fileData.cFileName );
		lastName[strlen(fileData.cFileName)-26] = '\0';
		if (tFile) 
		{
			while (fgets( s, 256, tFile)!=NULL)
			{
				if ( strstr ( s , "Allocations made:" ) )
				{
					strcpy ( lastAllocMade , s );
					iAllocMade = atoi ( lastAllocMade+18 );
				}
				if ( strstr ( s , "Frees made:" ) )
				{
					strcpy ( lastFreesMade , s );
					iFreesMade = atoi ( lastFreesMade+18 );
				}
				if ( strstr ( s , "Memory Used:" ) )
				{
					strcpy ( lastUsedMade , s );
					iUsedMade = atoi ( lastUsedMade+18 );
				}
				if ( strstr ( s , "Memory Freed:" ) )
				{
					strcpy ( lastFreedMade , s );
					iFreedcMade = atoi ( lastFreedMade+18 );
				}
				if ( strstr ( s , "Unfreed Memory:" ) )
				{
					strcpy ( lastUnfreedMade , s );
					iUnfreedMade = atoi ( lastUnfreedMade+18 );
				}
			}
			fclose(tFile);

			// store the info in text and csv
			sprintf_s ( name , "%-15s -> Allocs: %-10d, Frees: %-10d, Used: %-10d, Freed: %-10d, Unfreed: %-10d\n" , lastName , iAllocMade, iFreesMade, iUsedMade, iFreedcMade, iUnfreedMade );
			fputs ( name , file );

			sprintf_s ( name , "Name,Allocs, Frees, Used, Freed, Unfreed\n" );
			sprintf_s ( name , "%s,%d,%d,%d,%d,%d\n" , lastName , iAllocMade, iFreesMade, iUsedMade, iFreedcMade, iUnfreedMade);
			fputs ( name , fileCSV );
		}
	}
	
	FindClose ( hSearch );

	// set folder back
	SetCurrentDirectory ( szOriginalDirectory );

	// close files
	fflush ( file );
	fclose ( file );
	fflush ( fileCSV );
	fclose ( fileCSV );

}

#endif
