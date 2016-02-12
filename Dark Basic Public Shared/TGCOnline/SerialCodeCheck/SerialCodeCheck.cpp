//
// Serial Code Check Code
//

//#include "stdafx.h"
#include "SerialCodeCheck.h"

bool IsSerialCodeValid ( char* pszSerial )
{
	// test a valid serial number
	if ( pszSerial==NULL )
		return false;

	//char szSerial [ 255 ] = "01DB2211211175d3";		// valid
	//char szSerial [ 255 ] = "01DB2311211175d3";			// invalid
	char szSerial [ 255 ];
	strcpy ( szSerial, pszSerial );

	// by default all codes invalid
	bool bValid = false;
		
	// check for the 1st part of the string 01DB
	if ( szSerial [ 0 ] == '0' && szSerial [ 1 ] == '1' && szSerial [ 2 ] == 'D' && szSerial [ 3 ] == 'B' )
	{
		char szNumberCheck [ 8 ] = "";

		sprintf ( 
					szNumberCheck,
					"%c%c%c%c%c%c%c%c",
					szSerial [  4 ],
					szSerial [  5 ],
					szSerial [  6 ],
					szSerial [  7 ],
					szSerial [  8 ],
					szSerial [  9 ],
					szSerial [ 10 ],
					szSerial [ 11 ]
				);

		MD5 numberCheck;

		unsigned int len = strlen ( ( char* ) szNumberCheck );

		// make an MD5 from this
		numberCheck.update   ( ( unsigned char* ) szNumberCheck, len );
		numberCheck.finalize ( );

		char szVerifyCheck [ 255 ] = "";

		sprintf ( szVerifyCheck, "%s", numberCheck.hex_digest ( ) );

		// check that 7, 3, 1 and 5 match
		if ( szVerifyCheck [ 7 ] == szSerial [ 12 ] )
			if ( szVerifyCheck [ 3 ] == szSerial [ 13 ] )
				if ( szVerifyCheck [ 1 ] == szSerial [ 14 ] )
					if ( szVerifyCheck [ 5 ] == szSerial [ 15 ] )
						bValid = true;
	}

	// return final state of check
	return bValid;
}
