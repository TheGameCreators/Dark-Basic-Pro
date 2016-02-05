
////////////////////////////////////////////////////////////////////
// INFORMATION /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

/*
	HARD DRIVE COMMANDS
*/

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// DEFINES AND INCLUDES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

#define DARKSDK	__declspec ( dllexport )
#include "stdafx.h"
#include "winioctl.h"
#include <stdio.h>
#include "core.h"

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// FUNCTIONS ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

DARKSDK int   GetDriveCount				( void );
DARKSDK int   GetDriveCylinderCount		( int iID );
DARKSDK int   GetDriveTrackCount		( int iID );
DARKSDK int   GetDriveSectorsPerTrack	( int iID );
DARKSDK int   GetDriveBytesPerSector	( int iID );
DARKSDK int   GetDriveTotalSize    	    ( int iID, int iReturn );
DARKSDK int   GetDriveUsedSpace			( int iID, int iReturn );
DARKSDK int   GetDriveFreeSpace			( int iID, int iReturn );
DARKSDK int   GetDriveFileLengthSupport ( int iID );
DARKSDK DWORD GetDriveSerial			( DWORD dwReturn, int iID );
DARKSDK DWORD GetDriveFileSystem		( DWORD dwReturn, int iID );

DARKSDK int   GetCDCount  ( void );
DARKSDK DWORD GetCDLetter ( DWORD dwReturn, int iID );

		void  HardDriveSetup ( void );

/*
	// export names for string table -

	GET DRIVE COUNT[%L%?GetDriveCount@@YAHXZ%
	GET DRIVE CYLINDER COUNT[%LL%?GetDriveCylinderCount@@YAHH@Z%ID
	GET DRIVE TRACK COUNT[%LL%?GetDriveTrackCount@@YAHH@Z%ID
	GET DRIVE SECTORS PER TRACK[%LL%?GetDriveSectorsPerTrack@@YAHH@Z%ID
	GET DRIVE BYTES PER SECTOR[%LL%?GetDriveBytesPerSector@@YAHH@Z%ID
	GET DRIVE TOTAL SIZE[%LLL%?GetDriveTotalSize@@YAHHH@Z%ID
	GET DRIVE USED SPACE[%LLL%?GetDriveUsedSpace@@YAHHH@Z%ID
	GET DRIVE FREE SPACE[%L:L%?GetDriveFreeSpace@@YAHHH@Z%ID
	GET DRIVE FILE LENGTH SUPPORT[%LL%?GetDriveFileLengthSupport@@YAHH@Z%ID
	GET DRIVE SERIAL$[%SL%?GetDriveSerial@@YAKKH@Z%ID
	GET DRIVE FILE SYSTEM$[%SL%?GetDriveFileSystem@@YAKKH@Z%ID

	GET CD COUNT[%L%?GetCDCount@@YAHXZ%
	GET CD LETTER$[%SL%?GetCDLetter@@YAKKH@Z%ID
*/

struct sHardDrive
{
	LARGE_INTEGER	liCylinderCount;
	DWORD			dwTracksPerCylinder; 
	DWORD			dwSectorsPerTrack; 
	DWORD			dwBytesPerSector; 

	ULONGLONG		ulTotalBytes;
	ULONGLONG		ulTotalMB;
	ULONGLONG		ulTotalGB;
};

#define MAX_HARD_DRIVE 24

sHardDrive	g_HardDrives      [ MAX_HARD_DRIVE ];
char		g_HardDiskLetters [ MAX_HARD_DRIVE ] [ 4 ] = 
														{
															"c:\\",	"d:\\",	"e:\\",	"f:\\",	"g:\\",	"h:\\",
															"i:\\",	"j:\\",	"k:\\",	"l:\\",	"m:\\",	"n:\\",
															"o:\\",	"p:\\",	"q:\\",	"r:\\",	"s:\\",	"t:\\",
															"u:\\",	"v:\\",	"w:\\",	"x:\\",	"y:\\",	"z:\\"
														};
int			g_iHardDriveCount = 0;

bool CheckHardDriveID ( int iID )
{
	if ( iID < 0 || iID > 24 )
	{
		Error ( 7 );
		return false;
	}

	return true;
}

void HardDriveSetup ( void )
{
	// retrieve information about the hard drive

	// local variables
	int				iIndex          = 0;		// drive index
	HANDLE			hDevice         = NULL;		// handle to device
	DWORD			dwBytesReturned = 0;		// bytes returned from io control

	DISK_GEOMETRY	drive;						// drive structure
	char			szDrive [ 256 ];			// to store drive name

	// get a handle to the device
	hDevice = CreateFile ( "\\\\.\\PhysicalDrive0", 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );

	// check handle is valid
	if ( hDevice == INVALID_HANDLE_VALUE )
	{
		return;
	}

	// loop round for all drives
	while ( hDevice != INVALID_HANDLE_VALUE )
	{
		// get information
		if ( !DeviceIoControl (
									hDevice,						// handle to device
									IOCTL_DISK_GET_DRIVE_GEOMETRY,	// control type
									NULL,							// no input data
									0,								// use 0 because we have no input data
									&drive,							// pointer to drive structure
									sizeof ( DISK_GEOMETRY ),		// size of data
									&dwBytesReturned,				// number of bytes returned
									( LPOVERLAPPED ) NULL			// ignored
							  ) )
									return;

		// calculate the disk size
		ULONGLONG TotalBytes = drive.Cylinders.QuadPart * ( ULONG ) drive.TracksPerCylinder * ( ULONG ) drive.SectorsPerTrack * ( ULONG ) drive.BytesPerSector;
		
		g_HardDrives [ g_iHardDriveCount ].liCylinderCount     = drive.Cylinders;
		g_HardDrives [ g_iHardDriveCount ].dwTracksPerCylinder = drive.TracksPerCylinder;
		g_HardDrives [ g_iHardDriveCount ].dwSectorsPerTrack   = drive.SectorsPerTrack;
		g_HardDrives [ g_iHardDriveCount ].dwBytesPerSector    = drive.BytesPerSector;
		g_HardDrives [ g_iHardDriveCount ].ulTotalBytes        = TotalBytes;
		g_HardDrives [ g_iHardDriveCount ].ulTotalMB           = TotalBytes / 1024 / 1024;
		g_HardDrives [ g_iHardDriveCount ].ulTotalGB           = TotalBytes / 1024 / 1024 / 1024;

		g_iHardDriveCount++;

		// close this handle
		CloseHandle ( hDevice );

		// set up string for next drive
		sprintf ( szDrive, "\\\\.\\PhysicalDrive%d", ++iIndex );

		// access the next drive
		hDevice = CreateFile ( szDrive, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );
	}
}

int GetDriveCount ( void )
{
	return g_iHardDriveCount;
}

int GetDriveCylinderCount ( int iID )
{
	if ( !CheckHardDriveID ( iID ) )
		return 0;

	return ( int ) g_HardDrives [ iID ].liCylinderCount.LowPart;
}

int GetDriveTrackCount ( int iID )
{
	if ( !CheckHardDriveID ( iID ) )
		return 0;

	return ( int ) g_HardDrives [ iID ].dwTracksPerCylinder;
}

int GetDriveSectorsPerTrack	( int iID )
{
	if ( !CheckHardDriveID ( iID ) )
		return 0;

	return ( int ) g_HardDrives [ iID ].dwSectorsPerTrack;
}

int GetDriveBytesPerSector ( int iID )
{
	if ( !CheckHardDriveID ( iID ) )
		return 0;

	return ( int ) g_HardDrives [ iID ].dwBytesPerSector;
}

int GetDriveTotalSize ( int iID, int iReturn )
{
	if ( !CheckHardDriveID ( iID ) )
		return 0;

	if ( iReturn == 0 )
	{
		return ( int ) g_HardDrives [ iID ].ulTotalBytes;
	}
	else if ( iReturn == 1 )
	{
		return ( int ) g_HardDrives [ iID ].ulTotalMB;
	}
	else
		return ( int ) g_HardDrives [ iID ].ulTotalGB;
}

int GetDriveSpace ( int iID, int iMode, int iReturn )
{
	if ( !CheckHardDriveID ( iID ) )
		return 0;

	// when iMode is 0 we're looking for used space, when it's 1
	// we're after free space
	
	ULARGE_INTEGER	ulAvailableToCaller,
					ulDisk,
					ulFree;

	// make sure we're dealing with a fixed drive
	if ( GetDriveType ( g_HardDiskLetters [ iID ] ) == DRIVE_FIXED )
	{
		// get the information we need
		if ( GetDiskFreeSpaceEx ( g_HardDiskLetters [ iID ], &ulAvailableToCaller, &ulDisk, &ulFree ) )
		{
			// set up used values and the final value we need
			ULONGLONG ulUsed = ulDisk.QuadPart - ulFree.QuadPart;
			ULONGLONG ulValue;

			// see if we're going to return free or used data
			if ( iMode == 0 )
				ulValue = ulFree.QuadPart;
			else
				ulValue = ulUsed;

			// convert to mb
			if ( iReturn == 1 )
				ulValue = ulValue / 1024 / 1024;
			
			// convert to gb
			if ( iReturn == 2 )
				ulValue = ulValue / 1024 / 1024 / 1024;

			// return and cast the value to integer
			return ( int ) ulValue;
		}
	}

	// invalid
	return -1;
}

int GetDriveUsedSpace ( int iID, int iReturn )
{
	if ( !CheckHardDriveID ( iID ) )
		return 0;

	return GetDriveSpace ( iID, 1, iReturn );
}

int GetDriveFreeSpace ( int iID, int iReturn )
{
	if ( !CheckHardDriveID ( iID ) )
		return 0;

	return GetDriveSpace ( iID, 0, iReturn );
}

int GetDriveFileLengthSupport ( int iID )
{
	if ( !CheckHardDriveID ( iID ) )
		return 0;

	DWORD dwMaxFileLen = 0;
			
	if ( GetVolumeInformation ( g_HardDiskLetters [ iID ], NULL, 0, NULL, &dwMaxFileLen, NULL, NULL, 0 ) )
		return ( int ) dwMaxFileLen;

	return -1;
}

DWORD GetDriveSerial ( DWORD dwReturn, int iID )
{
	if ( !CheckHardDriveID ( iID ) )
		return 0;

	DWORD dwSerialNumber    = 0;
	char  szSerialA [ 256 ] = "";
	char  szSerialB [ 256 ] = "";

	if ( GetVolumeInformation ( g_HardDiskLetters [ iID ], NULL, 0, &dwSerialNumber, NULL, NULL, NULL, 0 ) )
	{
		// copy the serial number into the buffer
		sprintf ( szSerialA, "%X", dwSerialNumber );

		// now insert the "-" into the middle
		strcpy ( szSerialB, "" );
		memcpy ( szSerialB, szSerialA,               sizeof ( char ) * 4 );
		memcpy ( &szSerialB [ 4 ], "-",              sizeof ( char ) * 1 );
		memcpy ( &szSerialB [ 5 ], &szSerialA [ 4 ], sizeof ( char ) * 4 );

		// end the string
		szSerialB [ 9 ] = 0;
	}
	
	// return the string
	return ( DWORD ) SetupString ( szSerialB );
}

DWORD GetDriveFileSystem ( DWORD dwReturn, int iID )
{
	if ( !CheckHardDriveID ( iID ) )
		return 0;

	char szFS         [ 256 ] = "";
	char szFileSystem [ 256 ] = "";
	
	if ( GetVolumeInformation ( g_HardDiskLetters [ iID ], NULL, 0, NULL, NULL, NULL, szFS, 256 ) )
		sprintf ( szFileSystem, "%s", szFS );
	
	return ( DWORD ) SetupString ( szFileSystem );
}

int GetCDCount ( void )
{
	int iCount = 0;
	
	for ( int iCounter = 0; iCounter < MAX_HARD_DRIVE; iCounter++ )
	{
		if ( GetDriveType ( g_HardDiskLetters [ iCounter] ) == DRIVE_CDROM )
			iCount++;
	}

	return iCount;
}

DWORD GetCDLetter ( DWORD dwReturn, int iNTHCDIndex )
{
	if ( !CheckHardDriveID ( iNTHCDIndex ) )
		return 0;

	/* mikes old code for this
	char szLetter [ 4 ] = "";
	for ( int iCounter = 0; iCounter < MAX_HARD_DRIVE; iCounter++ )
		if ( GetDriveType ( g_HardDiskLetters [ iCounter ] ) == DRIVE_CDROM )
			strcpy ( szLetter, g_HardDiskLetters [ iCounter ] );
	*/

	// default is no letter
	char szLetter [ 4 ] = "";

	// lee - 070406 - u6rc7 - return CD letter corresponding to nth one found
	if ( iNTHCDIndex > 0 )
	{
		int iCountDown = iNTHCDIndex - 1;
		for ( int iCounter = 0; iCounter < MAX_HARD_DRIVE; iCounter++ )
		{
			if ( GetDriveType ( g_HardDiskLetters [ iCounter ] ) == DRIVE_CDROM )
			{
				if ( iCountDown==0 )
				{
					strcpy ( szLetter, g_HardDiskLetters [ iCounter ] );
					break;
				}
				else
					iCountDown--;
			}
		}
	}

	// return string
	return ( DWORD ) SetupString ( szLetter );
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
