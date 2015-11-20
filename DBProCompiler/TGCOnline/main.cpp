//
// Includes
//

#include "stdafx.h"
#include "resource.h"
#include "certificatekey.h"
#include "httpcomm.h"

#import "actskn43.ocx" no_implementation raw_interfaces_only raw_native_types
using namespace ACTIVESKINLib;
#include "atlbase.h"
#include <Afxdisp.h>

// Global data
LPSTR		g_pOrderKeyString				= NULL;
LPSTR		g_pFeedbackAreaString			= NULL;
LPSTR		g_pInstalledAndActiveProducts	= NULL;

//
// Server Function
//

LPSTR SendRequestToServer ( void )
{
	// connection details
	LPSTR pServer = "www.darkbasic.com";
	LPSTR pPage = "activate.php";

	// init connection
	HTTP_Connect ( pServer );

	// prepare data to post
	LPSTR pPostData = new char [ 1024 ];
	strcpy ( pPostData, "" );
	strcat ( pPostData, "orderkey=" );
	strcat ( pPostData, g_pOrderKeyString );
	strcat ( pPostData, "&" );
	strcat ( pPostData, "hardwarekey=" );
	strcat ( pPostData, g_pHardwareHashString );
	// test info
	strcat ( pPostData, "&" );
	strcat ( pPostData, "success=" );
	strcat ( pPostData, g_pOrderKeyString );
	DWORD dwPostDataSize = strlen (pPostData);

	// prepare header
	LPSTR pHeader = new char[256];
	wsprintf ( pHeader, "Content-Type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\n", dwPostDataSize );
	DWORD dwHeaderSize = -1L;

	// issue request and get data back
	LPSTR pData = HTTP_RequestData ( "POST", pPage, pHeader, dwHeaderSize, pPostData, dwPostDataSize );

	// free usages
	if ( pHeader )
	{
		delete pHeader;
		pHeader=NULL;
	}
	if ( pPostData )
	{
		delete pPostData;
		pPostData=NULL;
	}

	// free connection
	HTTP_Disconnect();

	// complete
	return pData;
}

LPSTR GenerateInstalledProducts ( void )
{
	// return string
	LPSTR pInstalledAndActiveProducts = NULL;
	pInstalledAndActiveProducts = new char [ 1024 ];
	memset ( pInstalledAndActiveProducts, 0, 1024 );

	// scan certificates folder and list them
	int iCertificatesCount = 0;
	if ( _chdir ( "certificates" )==0 )
	{
		// get absolute path to current location (for INI file)
		char pAbsPath [ _MAX_PATH ];
		char pINIPath [ _MAX_PATH ];
		getcwd ( pAbsPath, 256 );

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
				LPSTR pName = m_filedata.name;
				DWORD dwType = m_filedata.attrib;
				if(stricmp(pName,".")!=NULL && stricmp(pName, "..")!=NULL)
				{
					if(!(dwType & _A_SUBDIR))
					{
						// first product creates header
						if ( iCertificatesCount==0 )
							strcat ( pInstalledAndActiveProducts, "Your currently installed products are:\r\n\r\n" );

						// for each file (treat as INI certificate file)
						strcpy ( pINIPath, pAbsPath );
						strcat ( pINIPath, "\\" );
						strcat ( pINIPath, pName );

						// extract data of product
						char pDescOfProduct [ _MAX_PATH ];
						GetPrivateProfileString ( "PRODUCT", "DESCRIPTION", "", pDescOfProduct, _MAX_PATH, pINIPath );
						char pNameOfProduct [ _MAX_PATH ];
						GetPrivateProfileString ( "PRODUCT", "PRODUCTNAME", "", pNameOfProduct, _MAX_PATH, pINIPath );

						// add to installed products list
						strcat ( pInstalledAndActiveProducts, pDescOfProduct );

						// build local activation-hash (HARDWAREKEY+PRIVATEPRODUCTNAME)
						char pLocalActivationString [ 1024 ];
						strcpy ( pLocalActivationString, g_pHardwareHashString );
						strcat ( pLocalActivationString, pNameOfProduct );
						CString pLocalActivationCString = CMD5Checksum::GetMD5 ( (BYTE*)pLocalActivationString, (UINT)strlen(pLocalActivationString) );
						LPSTR pLocalActivationHash = pLocalActivationCString.GetBuffer(pLocalActivationCString.GetLength());

						// compare HWKey+pNameOfProduct with name of certificate
						if ( strcmp ( pLocalActivationHash, pName )==NULL )
							strcat ( pInstalledAndActiveProducts, " (valid certificate)" );
						else
							strcat ( pInstalledAndActiveProducts, " (invalid certificate)" );

						// complete list entry
						strcat ( pInstalledAndActiveProducts, "\r\n" );

						// count the certificate/product
						iCertificatesCount++;
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

	// if no certificates, presume a trial version or new ESD version
	if ( iCertificatesCount==0 )
	{
		strcat ( pInstalledAndActiveProducts, "Dark Basic Professional - Not Activated\r\n" );
	}

	// complete
	return pInstalledAndActiveProducts;
}

//
// Main Functions
//

// DLL Window Message Handler
BOOL CALLBACK DialogProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// handle commands to the dialog
	switch(uMsg)
	{
		case WM_INITDIALOG:
		{
			int a = 42;
		}
		break;

		case WM_COMMAND:
		{
			WORD wID = LOWORD(wParam);
			switch ( wID )
			{
				case IDC_BACK :	// BACK Button
				{
					// initial feedback area text
					if ( g_pInstalledAndActiveProducts ) delete g_pInstalledAndActiveProducts;
					g_pInstalledAndActiveProducts = GenerateInstalledProducts();
					SetDlgItemText ( hwndDlg, IDC_FEEDBACKAREA, g_pInstalledAndActiveProducts );

					// clear backtrack button
					HWND hControlWnd = GetDlgItem ( hwndDlg, IDC_BACK );
					EnableWindow ( hControlWnd, FALSE );

					// help text
					SetDlgItemText ( hwndDlg, IDC_DESCTEXT3, "Enter the Order Key of a product you wish to activate." );
				}
				break;

				case IDC_NEXT :	// NEXT Button
				{					
					// feedback text for user prompt
					if ( g_pFeedbackAreaString ) delete g_pFeedbackAreaString;
					g_pFeedbackAreaString = new char [ 10240 ];
					strcpy ( g_pFeedbackAreaString, "Requesting Activation Information for Order Key...\r\n" );

					// Clear feedback area with sending prompt
					SetDlgItemText ( hwndDlg, IDC_DESCTEXT3, "" );
					SetDlgItemText ( hwndDlg, IDC_FEEDBACKAREA, g_pFeedbackAreaString );
					UpdateWindow ( hwndDlg );

					// Send request to server
					LPSTR pActivationString = SendRequestToServer();

					// data required from response
					LPSTR pResponseStatus = NULL;
					LPSTR pResponseResponse = NULL;
					LPSTR pResponseCertificate = NULL;

					// go through activation string
					LPSTR pPtrEnd = pActivationString+strlen(pActivationString), pPtrStart = pActivationString;
					for ( LPSTR pPtr = pActivationString; pPtr<=pPtrEnd; pPtr++ )
					{
						// take each line
						if ( *pPtr==10 || pPtr==pPtrEnd )
						{
							// prepare contents of line
							LPSTR pAdd = g_pFeedbackAreaString+strlen(g_pFeedbackAreaString);
							DWORD dwLength = pPtr-pPtrStart;

							// store information
							if ( strnicmp ( pPtrStart, "status: ", 8 )==NULL )
							{
								// status from response
								DWORD dwFieldDataOnly = dwLength-8;
								pResponseStatus = new char [ dwFieldDataOnly+1 ];
								memcpy ( pResponseStatus, pPtrStart+8, dwFieldDataOnly );
								pResponseStatus [ dwFieldDataOnly ] = 0;
							}
							if ( strnicmp ( pPtrStart, "response: ", 10 )==NULL )
							{
								// status from response
								DWORD dwFieldDataOnly = dwLength-10;
								pResponseResponse = new char [ dwFieldDataOnly+1 ];
								memcpy ( pResponseResponse, pPtrStart+10, dwFieldDataOnly );
								pResponseResponse [ dwFieldDataOnly ] = 0;

								// feedback prompt
								strcat ( g_pFeedbackAreaString, pResponseResponse );
								strcat ( g_pFeedbackAreaString, "\r\n" );
							}
							if ( strnicmp ( pPtrStart, "certificate: ", 13 )==NULL )
							{
								// certificate from response
								DWORD dwFieldDataOnly = dwLength-13;
								pResponseCertificate = new char [ dwFieldDataOnly+1 ];
								memcpy ( pResponseCertificate, pPtrStart+13, dwFieldDataOnly );
								pResponseCertificate [ dwFieldDataOnly ] = 0;
							}

							// prepare of next line
							pPtrStart = pPtr+1;
						}
					}

					// if certificate returned
					if ( pResponseCertificate )
					{
						// secret name for private product (between Server and Client)
						int iIndexToInternalProduct = FindProductName ( pResponseCertificate );

						// found match with a product
						if ( iIndexToInternalProduct!=-1 )
						{
							// customer has permission to use this product element
							strcat ( g_pFeedbackAreaString, "Order Key Verified. Product is now activated.\r\n" );
							strcat ( g_pFeedbackAreaString, "\r\n" );

							// create a certificate in the certificates folder:
							if ( _chdir ( "certificates" )==-1 )
								mkdir ( "certificates" );
							else
								_chdir ( ".." );

							if ( _chdir ( "certificates" )==0 )
							{
								// get absolute path to current location (for INI file)
								char pAbsPath [ _MAX_PATH ];
								getcwd ( pAbsPath, 256 );

								// for each file (treat as INI certificate file)
								char pINIPath [ _MAX_PATH ];
								strcpy ( pINIPath, pAbsPath );
								strcat ( pINIPath, "\\" );
								strcat ( pINIPath, pResponseCertificate );

								// description of product
								LPSTR pDescOfProduct = g_pProductDescList [ iIndexToInternalProduct ];
								WritePrivateProfileString ( "PRODUCT", "DESCRIPTION", pDescOfProduct, pINIPath );

								// name of product
								LPSTR pProductName = g_pProductList [ iIndexToInternalProduct ];
								WritePrivateProfileString ( "PRODUCT", "PRODUCTNAME", pProductName, pINIPath );

								// leave folder
								_chdir ( ".." );
							}

							//
							// When the compiler loads, it first scans for certificate
							// files which inside them contain:
							//
							//   DBPRO in the body = 'compiler active'
							//   PHYSICS in the body = 'physics commands active'
							//   NODETREE in the body = 'new new static commands'
							//   ADDON1 in the body = 'Mikes new commands'
							//

							// DEFEATING THE PRIATE ON THE LOCAL MACHINE
							// =========================================
							//
							//  problem:
							//   pirate can copy certificate1.txt to friends machine
							//   and when the friend runs compiler, they get the features!
							//
							//  solution:
							//   the body of the file is encrypted with the HW key (MD5)
							//
							//  example of CERTIFICATE1.TXT
							//   DBPRO actually looks like F67H7698H678G67G678GHJ86HJ43
							//
							//  pirate response 1:
							//   writes a keygen when makes a file based on users hardware
							//
							//  lee:
							//   a) i make it impossible for them to know the HW key
							//   b) the product name should look like garbage
							//
							//  pirate response 2:
							//   hacks compiler exe so it does not even need certificates
							//
							// lee:
							//   a) we keep changing the compiler each version - so would they
							//	 b) we could even make a policy of changing it every month,
							//      even if we have not actually changed anything!
							//
							//      BOTTOM LINE - if they want updates, they must be using
							//                    an official version of the compiler!
						}
					}

					// feedback dump
					strcat ( g_pFeedbackAreaString, "\r\n" );
					strcat ( g_pFeedbackAreaString, "\r\n" );
					strcat ( g_pFeedbackAreaString, "\r\n" );
					strcat ( g_pFeedbackAreaString, "RICH TASK\r\n" );
					strcat ( g_pFeedbackAreaString, "1. ORDER KEY DATABASE (should it be customer/product key)\r\n" );
					strcat ( g_pFeedbackAreaString, "2. CORRECT STATUS IN RESPONSE\r\n" );
					strcat ( g_pFeedbackAreaString, "3. NEW ACTIVATIONS - STORE THEIR HARDWARE KEY\r\n" );
					strcat ( g_pFeedbackAreaString, "4. EXISTING CUSTOMERS - HARDWARE KEYS MATCH?\r\n" );
					strcat ( g_pFeedbackAreaString, "5. REPLACE 'CERTIFICATE ONE' WITH 'PRODUCT'\r\n" );
					strcat ( g_pFeedbackAreaString, "\r\n" );
					strcat ( g_pFeedbackAreaString, "\r\n" );
					strcat ( g_pFeedbackAreaString, "DUMP:\r\n" );
					strcat ( g_pFeedbackAreaString, pActivationString );
					strcat ( g_pFeedbackAreaString, "\r\n" );

					// free usages
					if ( pActivationString )
					{
						delete pActivationString;
						pActivationString=NULL;
					}
					if ( pResponseStatus )
					{
						delete pResponseStatus;
						pResponseStatus=NULL;
					}
					if ( pResponseResponse )
					{
						delete pResponseResponse;
						pResponseResponse=NULL;
					}
					if ( pResponseCertificate )
					{
						delete pResponseCertificate;
						pResponseCertificate=NULL;
					}

					// on return, report 
					SetDlgItemText ( hwndDlg, IDC_FEEDBACKAREA, g_pFeedbackAreaString );

					// and clear ORDER KEY
					SetDlgItemText ( hwndDlg, IDC_ORDERKEY, "" );

					// and allow backtrack to first screen
					HWND hControlWnd = GetDlgItem ( hwndDlg, IDC_BACK );
					EnableWindow ( hControlWnd, TRUE );
				}
				break;

				case IDCANCEL :	// Cancel Button
				{
					PostQuitMessage(0);
				}
				break;

				case IDC_ORDERKEY :	// ORDERKEY Editbox
				{
					switch ( HIWORD ( wParam ) )
					{
						case EN_CHANGE :
						{
							// Get current order key entered
							if ( g_pOrderKeyString ) delete g_pOrderKeyString;
							g_pOrderKeyString = new char [ 256 ];
							GetDlgItemText ( hwndDlg, IDC_ORDERKEY, g_pOrderKeyString, 256 );

							// status of next button highlight state
							HWND hControlWnd = GetDlgItem ( hwndDlg, IDC_NEXT );
							if ( strlen ( g_pOrderKeyString )>0 )
							{
								SetDlgItemText ( hwndDlg, IDC_DESCTEXT3, "Click Next to activate the product" );
								EnableWindow ( hControlWnd, TRUE );
							}
							else
							{
								SetDlgItemText ( hwndDlg, IDC_DESCTEXT3, "Activate another product or click Back to view installed products." );
								EnableWindow ( hControlWnd, FALSE );
							}
						}
						break;
					}
				}
				break;
			}
		}
		break;
	}

	// continue processing dialog messages
	return 0;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	// Inits
	AfxOleInit();
	AfxEnableControlContainer();

	// create dialog
	HWND hWnd = CreateDialog ( hInstance, MAKEINTRESOURCE(IDD_TGCONLINE_DIALOG), NULL, DialogProc );
	hWnd = CreateDialog ( hInstance, MAKEINTRESOURCE(IDD_TGCONLINE_LEE), NULL, DialogProc );

	// apply skin after dialog created
	if ( hWnd )
	{
//		CWnd* pCWndPtr = CWnd::FromHandle(::GetDlgItem(hWnd, IDC_SKIN21));
//		CComQIPtr<ISkin> pSkin = pCWndPtr->GetControlUnknown();
//		pSkin->ApplySkin((long)hWnd);
	}

	// set icon
	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));
	SendMessage ( hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon );

	// position window in center of screen
	RECT rect;
	GetWindowRect ( hWnd, &rect );
	int iScreenWidth = GetSystemMetrics ( SM_CXSCREEN );
	int iScreenHeight = GetSystemMetrics ( SM_CYSCREEN );
	SetWindowPos ( hWnd, NULL, ((iScreenWidth-rect.right)/2), ((iScreenHeight-rect.bottom)/2), 0, 0, SWP_NOSIZE | SWP_NOZORDER );
	ShowWindow ( hWnd, SW_SHOW );

	// generate hardware key here
	LPSTR pHardwareKeyString = GenerateHWKey();

	// show hardware key raw string (test mode)
	SetDlgItemText ( hWnd, IDC_FEEDBACKAREA, pHardwareKeyString );

	// calculate the hash value from the above hardware-string
	g_pHardwareHash = CMD5Checksum::GetMD5 ( (BYTE*)pHardwareKeyString, (UINT)strlen(pHardwareKeyString) );
	g_pHardwareHashString = g_pHardwareHash.GetBuffer(g_pHardwareHash.GetLength());
	SetDlgItemText ( hWnd, IDC_HARDWAREKEY, g_pHardwareHashString );

	// feedback initial display shows installed and active products
	g_pInstalledAndActiveProducts = GenerateInstalledProducts();
	SetDlgItemText ( hWnd, IDC_FEEDBACKAREA, g_pInstalledAndActiveProducts );

	// issue help string
	SetDlgItemText ( hWnd, IDC_DESCTEXT3, "Enter the Order Key of a product you wish to activate." );
	
	// free usages
	if ( pHardwareKeyString )
	{
		delete pHardwareKeyString;
		pHardwareKeyString=NULL;
	}
	
	// message pump
	MSG msg;
	while (GetMessage(&msg, NULL, NULL, NULL)) 
	{ 
		// process regular messages
		if (!IsWindow(hWnd) || !IsDialogMessage(hWnd, &msg)) 
		{ 
			TranslateMessage(&msg); 
			DispatchMessage(&msg); 
		}
	}

	// free usages
	if ( g_pOrderKeyString )
	{
		delete g_pOrderKeyString;
		g_pOrderKeyString=NULL;
	}
	if ( g_pFeedbackAreaString )
	{
		delete g_pFeedbackAreaString;
		g_pFeedbackAreaString=NULL;
	}
	if ( g_pInstalledAndActiveProducts )
	{
		delete g_pInstalledAndActiveProducts;
		g_pInstalledAndActiveProducts=NULL;
	}

	// all is well
	return 0;
}
