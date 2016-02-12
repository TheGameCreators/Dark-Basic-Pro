// TGCOnlineDlg.cpp : implementation file
//

// Includes
#include "stdafx.h"
#include "ExternalText.h"
#include "TGCOnline.h"
#include "TGCOnlineDlg.h"
#include "certificatekey.h"
#include "SerialCodeCheck\\SerialCodeCheck.h"
#include "httpcomm.h"

// Externals
extern CTGCOnlineApp theApp;

// Global data
DWORD		g_dwFreeOrFullActivate			= 0;
LPSTR		g_pOrderKeyString				= NULL;
LPSTR		g_pEmailString					= NULL;
LPSTR		g_pFeedbackAreaString			= NULL;
LPSTR		g_pInstalledAndActiveProducts	= NULL;
bool		g_bExtraReporting				= true;//false;

// Only for freebie maker
#ifdef FREECERTIFICATESVERSION
 bool		bBypassAllVerifyChecks			= false;
#endif

//
// CD Checks
//

bool FileExist(LPSTR pFilename)
{
	HANDLE hReadFile = CreateFile(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hReadFile!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(hReadFile);
		return true;
	}
	else
		return false;
}

DWORD FileSize(LPSTR pFilename)
{
	DWORD size = 0;
	HANDLE hReadFile = CreateFile(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hReadFile!=INVALID_HANDLE_VALUE)
	{
		size = GetFileSize(hReadFile, NULL);
		CloseHandle(hReadFile);
	}
	return size;
}

int CheckForValidDBProCD ( int a, int b, int c, int* piCodeMarkerCount, char* pErr )
{
	// error code to help me find verifier problems
	DWORD dwErr=200;

	// response init ABC
	int d=a+b+c, e=a+b;

	// MARKER-1
	(*piCodeMarkerCount)=(*piCodeMarkerCount)+1;

	// scan all drives
	char olddir[MAX_PATH];
	getcwd(olddir,MAX_PATH);
	bool bContinueLoopInRedundantMode=false;
	for ( int drivenum=2; drivenum<26; drivenum++ )
	{
		// go to root of each drive
		_chdir("");
		if ( _chdrive(drivenum)==0 )
		{
			// increment for each found drive
			dwErr++;
		}

		// bury true response in iteration
		if ( drivenum==2 )
		{
			// MARKER-2
			(*piCodeMarkerCount)=(*piCodeMarkerCount)+1;

			// modify response AB-
			d=d-c;
		}

		// find key file to recoginise CDROM with CD inserted (at least DBPro CD)
		if ( FileExist ( "autorun.inf" ) ) // typically on mine : drivenum=4 (D:CDROM)
		{
			// found autorun
			dwErr=100;

			// found CDROM with CD in it - scan for key files
			if ( bContinueLoopInRedundantMode==false )
			{
				// MARKER-3 (also add A to baffle)
				(*piCodeMarkerCount)=(*piCodeMarkerCount)+a+1;

				// List of files that MUST be found
				// setup.exe ENG
				// data2.cab ENG
				// SetupScreen.exe
				// SetupScreen.txt
				// Media\\Icons\\icon1.bmp
				// Media\\Models\\Text\\New Text 1.X

				// leeadd - 310305 - second attempt with ENGLISH\ folder
				bool bFailureToFindKeyFile = false;
				for ( int iAtt=0; iAtt<2; iAtt++ )
				{
					// starts of with success
					bFailureToFindKeyFile = false;

					// try ergular and within english folder (ie russian CD)
					char pTryFile[256];
					char pFolder[32];
					strcpy ( pFolder, "" );
					if ( iAtt==1 )  strcpy ( pFolder, "english\\" );

					// check files that should and should not exist
					strcpy ( pTryFile, pFolder ); strcat ( pTryFile, "setup.exe" );
					if ( !FileExist ( pTryFile ) ) { bFailureToFindKeyFile=true; dwErr=1; }
					strcpy ( pTryFile, "" ); strcat ( pTryFile, "SetupScreen.exe" );
					if ( !FileExist ( pTryFile ) ) { bFailureToFindKeyFile=true; dwErr=2; }
					strcpy ( pTryFile, "" ); strcat ( pTryFile, "SetupScreen.txt" );
					if ( !FileExist ( pTryFile ) ) { bFailureToFindKeyFile=true; dwErr=3; }
					strcpy ( pTryFile, "" ); strcat ( pTryFile, "Media\\Icons\\icon1.bmp" );
					if ( !FileExist ( pTryFile ) ) { bFailureToFindKeyFile=true; dwErr=4; }
					strcpy ( pTryFile, "" ); strcat ( pTryFile, "Media\\Bitmaps\\metallic.bmp" );
					if (  FileExist ( pTryFile ) ) { bFailureToFindKeyFile=true; dwErr=5; }
					strcpy ( pTryFile, "" ); strcat ( pTryFile, "SetupScreen.ini" );
					if (  FileExist ( pTryFile ) ) { bFailureToFindKeyFile=true; dwErr=6; }

					// an extra check - this file should be HUGE (100mb+)
					strcpy ( pTryFile, pFolder ); strcat ( pTryFile, "data2.cab" );
					if (  FileSize ( pTryFile )<102503612 ) { bFailureToFindKeyFile=true; dwErr=7; }

					// if success, can leave now
					if ( bFailureToFindKeyFile==false ) break;
				}

				// if all key files found
				if ( bFailureToFindKeyFile==false )
				{
					// valid response -B-
					bContinueLoopInRedundantMode=true;
					e=d-a;
				}
			}
		}
	}
 	
	// restore directory and drive
	_chdir(olddir);

	// record any error
	wsprintf(pErr, "ERR:%d", dwErr);

	// return response
	return e;
}

//
// Specific to DBPRO Editor
//

void CopyMainAppDLLSAcross ( int iIndexToInternalProduct )
{
	// copy the config file (EDITOR\dbp_config.dll)
	// remember EDITOR still using DLL, so append .new for LAUNCH.EXE to sort!
	char pCompilerPath [ _MAX_PATH ];
	getcwd ( pCompilerPath, 256 );

	// from this location
	char pCopyFileFrom [ _MAX_PATH ];
	strcpy ( pCopyFileFrom, pCompilerPath );
	strcat ( pCopyFileFrom, "\\TGCOnlineFiles\\" );
	strcat ( pCopyFileFrom, "dbp_config.dll" );
	if ( iIndexToInternalProduct==0 )
		strcat ( pCopyFileFrom, ".demo" );
	else
		strcat ( pCopyFileFrom, ".full" );

	// to this location
	char pCopyFileTo [ _MAX_PATH ];
	strcpy ( pCopyFileTo, pCompilerPath );
	strcat ( pCopyFileTo, "\\..\\Editor\\" );
	strcat ( pCopyFileTo, "dbp_config.dll" );
	strcat ( pCopyFileTo, ".new" );

	// do the copy
	if ( CopyFile ( pCopyFileFrom, pCopyFileTo, FALSE )==0 )
	{
		// failed to copy - silent
		bool bFailed = true;
	}
}

//
// Server Function
//

LPSTR SendVerifyPostToServer ( LPSTR pProduct, int count, int q, int r, int s )
{
	// connection details (default is free)
	LPSTR pServer = "www.darkbasic.com";
	LPSTR pPage = "tgco/register.php";
	LPSTR pEmail = g_pEmailString;

	// quit if no email
	if ( pProduct==NULL || pEmail==NULL )
	{
		if ( g_bExtraReporting )
		{
			strcat ( g_pFeedbackAreaString, g_pTGCWord[56] );
			strcat ( g_pFeedbackAreaString, "\r\n" );
		}
		return NULL;
	}

	// init connection
	HTTP_Connect ( pServer );

	// create a verifier
	char pAPString [ 256 ];
	LPSTR pVerifierString = new char [ 1024 ];
	strcpy ( pVerifierString, g_pHardwareHashString );
	strcat ( pVerifierString, pEmail );
	strcat ( pVerifierString, "TGC-CDCHECK" );
	wsprintf ( pAPString, "%d%d%d%d", count, q, r, s );
	strcat ( pVerifierString, pAPString );

	// make MD5
	InitMD5DLL();
	LPSTR pVerifier = (LPSTR) gCMD5ChecksumGetMD5 ( (BYTE*)pVerifierString, (UINT)strlen(pVerifierString) );
	if ( pVerifierString ) delete pVerifierString;

	// prepare data to post
	LPSTR pPostData = new char [ 2048 ];
	strcpy ( pPostData, "" );
	#ifdef TGCFREE
	strcat ( pPostData, "f=1" );
	strcat ( pPostData, "&" );
	#endif
	strcat ( pPostData, "m=" );
	strcat ( pPostData, g_pHardwareHashString );
	strcat ( pPostData, "&" );
	strcat ( pPostData, "p=" );
	strcat ( pPostData, pProduct );
	strcat ( pPostData, "&" );
	strcat ( pPostData, "v=" );
	strcat ( pPostData, pVerifier );
	strcat ( pPostData, "&" );
	strcat ( pPostData, "e=" );
	strcat ( pPostData, pEmail );
	strcat ( pPostData, "&" );
	wsprintf ( pAPString, "c=%d&q=%d&r=%d&s=%d", count, q, r, s );
	strcat ( pPostData, pAPString );
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

	// free MD5
	gCMD5ChecksumFreeMD5(pVerifier);
	FreeMD5DLL();

	// free connection
	HTTP_Disconnect();

	// complete
	return pData;
}

LPSTR SendRequestToServer ( LPSTR pProduct )
{
	// connection details (default is free)
	LPSTR pServer = "www.darkbasic.com";
	LPSTR pPage = "tgco/generate.php";
	LPSTR pEmail = g_pEmailString;
	LPSTR pPassword = g_pOrderKeyString;

	// if full version, full version details
	if ( g_dwFreeOrFullActivate==1 )
	{
		pServer = "www.darkbasic.com";
		pPage = "tgco/activate.php";
		pProduct = g_pOrderKeyString;
		pPassword = NULL;

		// canont proceed if no orderkey
		if ( pProduct==NULL )
		{
			if ( g_bExtraReporting )
			{
				strcat ( g_pFeedbackAreaString, g_pTGCWord[57] );
				strcat ( g_pFeedbackAreaString, "\r\n" );
			}
			return NULL;
		}
	}

	// init connection
	HTTP_Connect ( pServer );

	// create a verifier
	LPSTR pVerifierString = new char [ 1024 ];
	strcpy ( pVerifierString, g_pHardwareHashString );
	if ( g_dwFreeOrFullActivate==1 )
	{
		strcat ( pVerifierString, pEmail );
		strcat ( pVerifierString, "TGC-VKEY-FULL" );
	}
	else
	{
		char lowerproduct[256];
		strcpy ( lowerproduct, pProduct );
		strlwr ( lowerproduct );
		strcat ( pVerifierString, lowerproduct );
		strcat ( pVerifierString, "TGC-VKEY" );
	}

	// make MD5
	InitMD5DLL();
	LPSTR pVerifier = (LPSTR) gCMD5ChecksumGetMD5 ( (BYTE*)pVerifierString, (UINT)strlen(pVerifierString) );
	if ( pVerifierString ) delete pVerifierString;

	// prepare data to post
	LPSTR pPostData = new char [ 2048 ];

	// free version 
	if ( g_dwFreeOrFullActivate==0 )
	{
		// basic data
		strcpy ( pPostData, "" );
		strcat ( pPostData, "m=" );
		strcat ( pPostData, g_pHardwareHashString );
		strcat ( pPostData, "&" );
		strcat ( pPostData, "p=" );
		strcat ( pPostData, pProduct );
		strcat ( pPostData, "&" );
		strcat ( pPostData, "v=" );
		strcat ( pPostData, pVerifier );

		// optional data for trial version activations
		if ( pEmail && pPassword )
		{
			strcat ( pPostData, "&" );
			strcat ( pPostData, "e=" );
			strcat ( pPostData, pEmail );
			strcat ( pPostData, "&" );
			strcat ( pPostData, "u=" );
			strcat ( pPostData, pPassword );
		}
		if ( 0 )
		{
			strcat ( pPostData, "&" );
			strcat ( pPostData, "a=" );
			strcat ( pPostData, "0" );
		}
	}

	// full version 
	if ( g_dwFreeOrFullActivate==1 )
	{
		// basic data
		strcpy ( pPostData, "" );
		strcat ( pPostData, "m=" );
		strcat ( pPostData, g_pHardwareHashString );
		strcat ( pPostData, "&" );
		strcat ( pPostData, "v=" );
		strcat ( pPostData, pVerifier );
		strcat ( pPostData, "&" );
		strcat ( pPostData, "e=" );
		strcat ( pPostData, pEmail );
		strcat ( pPostData, "&" );
		strcat ( pPostData, "k=" );
		strcat ( pPostData, pProduct );
	}

	// post data size
	DWORD dwPostDataSize = strlen (pPostData);

	// debug information
	if ( g_bExtraReporting )
	{
//		strcat ( g_pFeedbackAreaString, "POSTDATA:\r\n" );
//		strcat ( g_pFeedbackAreaString, pPostData );
//		strcat ( g_pFeedbackAreaString, "\r\n" );
	}

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

	// free MD5
	gCMD5ChecksumFreeMD5(pVerifier);
	FreeMD5DLL();

	// free connection
	HTTP_Disconnect();

	// complete
	return pData;
}

/////////////////////////////////////////////////////////////////////////////
// CTGCOnlineDlg dialog

CTGCOnlineDlg::CTGCOnlineDlg(UINT uDialog) : CDialog ( MAKEINTRESOURCE ( uDialog ), NULL )
{
	//{{AFX_DATA_INIT(CTGCOnlineDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTGCOnlineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTGCOnlineDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTGCOnlineDlg, CDialog)
	//{{AFX_MSG_MAP(CTGCOnlineDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTGCOnlineDlg message handlers

BOOL CTGCOnlineDlg::OnInitDialog()
{
	// default init code
	CDialog::OnInitDialog();

	// Prepare external words
	FindExternalWords();

	// Quickly replace dialog texts
	SetWindowText( g_pTGCWord[39] );
	SetDlgItemText ( IDC_WELCOMETEXT, g_pTGCWord[38] );
	SetDlgItemText ( IDC_BACK, g_pTGCWord[41] );
	SetDlgItemText ( IDC_NEXT, g_pTGCWord[42] );
	SetDlgItemText ( IDCANCEL, g_pTGCWord[43] );
	SetDlgItemText ( IDC_BUY,  g_pTGCWord[44] );

	// get handle to window
	HWND hWnd = this->m_hWnd;

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
//	// set skin for dialog
//	CComQIPtr<ISkin> pSkin;
//	CWnd* hUnk = GetDlgItem(IDC_SKIN21);
//	if ( hUnk )
//	{
//		pSkin = hUnk->GetControlUnknown();
//		if ( pSkin ) pSkin->ApplySkin( (long)hWnd );
//	}
		
	// set icon
	HICON hIcon = LoadIcon(theApp.m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));
	SendMessage ( WM_SETICON, ICON_SMALL, (LPARAM)hIcon );

	// position window in center of screen
	RECT rect;
	GetWindowRect ( &rect );
	int iScreenWidth = GetSystemMetrics ( SM_CXSCREEN );
	int iScreenHeight = GetSystemMetrics ( SM_CYSCREEN );
//	SetWindowPos ( NULL, ((iScreenWidth-rect.right)/2), ((iScreenHeight-rect.bottom)/2), 0, 0, SWP_NOSIZE | SWP_NOZORDER );
	SetWindowPos ( &wndTopMost, ((iScreenWidth-rect.right)/2), ((iScreenHeight-rect.bottom)/2), 0, 0, SWP_NOSIZE );
	ShowWindow ( SW_SHOW );

	// generate hardware key here
//	GenerateHWKey(); //now inside localHWKey function (easier cert usage)
	ReadLocalHWKey();

	// feedback initial display shows installed and active products
	LPSTR pYourInstalled = g_pTGCWord[81];
	LPSTR pValidFor = g_pTGCWord[82];
	LPSTR pDays = g_pTGCWord[83];
	LPSTR pValidCert = g_pTGCWord[84];
	LPSTR pExpiredCert = g_pTGCWord[85];
	LPSTR pDBProNotActivated = g_pTGCWord[86];
	g_pInstalledAndActiveProducts = GenerateInstalledProducts( pYourInstalled, pValidFor, pDays, pValidCert, pExpiredCert, pDBProNotActivated );
	SetDlgItemText ( IDC_FEEDBACKAREA, g_pInstalledAndActiveProducts );

	// issue help string
	SetDlgItemText ( IDC_DESCTEXT3, "" );

	// Hide/Show appropriate button for TGC Mode
	#ifdef TGCVERIFIER
		// Prepare buttons for mode
		GetDlgItem ( IDC_TRIAL )->ShowWindow ( SW_HIDE );
		GetDlgItem ( IDC_BUY )->ShowWindow ( SW_HIDE );
		GetDlgItem ( IDC_HELPTEXT )->ShowWindow ( SW_HIDE );
		#ifdef TGCFREE
			SetWindowText( g_pTGCWord[99] );
			SetDlgItemText ( IDC_WELCOMETEXT, g_pTGCWord[100] );
			SetDlgItemText ( IDC_FEEDBACKAREA, g_pTGCWord[101] );
		#else
			SetDlgItemText ( IDC_FEEDBACKAREA, g_pTGCWord[1] );
		#endif
		// NEXT button active
		CWnd* pControlWnd = GetDlgItem ( IDC_NEXT );
		pControlWnd->EnableWindow ( TRUE );
		// Ensure first page
		m_bFirstPage=false;
	#else
	 #ifdef TGCTRIAL
		GetDlgItem ( IDC_TRIAL )->ShowWindow ( SW_SHOW );
		GetDlgItem ( IDC_BUY )->ShowWindow ( SW_HIDE );
		GetDlgItem ( IDC_HELPTEXT )->ShowWindow ( SW_HIDE );

		// product to trial set by external file
		char curdir[_MAX_PATH];
		char curini[_MAX_PATH];
		getcwd(curdir,_MAX_PATH);
		strcpy(curini,curdir);
		strcat(curini,"\\..\\Compiler\\TGCOnlineTrial.ini");
		GetPrivateProfileString("PRODUCT","TEXT",g_pTGCWord[2],m_pProductName,256,curini);
		SetDlgItemText ( IDC_TRIAL, m_pProductName );
		GetPrivateProfileString("PRODUCT","TRIAL","dbpro",m_pProductName,256,curini);

		// disable free trial button if already present
		if ( stricmp ( m_pProductName, "dbpro" )==NULL )
			if ( AmIActive ( 0, NULL )==1 )
				GetDlgItem ( IDC_TRIAL )->EnableWindow ( FALSE );

	 #else
	  #ifdef TGCSERIALCODE
		GetDlgItem ( IDC_TRIAL )->ShowWindow ( SW_HIDE );
		GetDlgItem ( IDC_BACK )->ShowWindow ( SW_HIDE );
		GetDlgItem ( IDC_BUY )->ShowWindow ( SW_HIDE );
		GetDlgItem ( IDC_TRIAL )->ShowWindow ( SW_HIDE );
		GetDlgItem ( IDC_DESCTEXT3 )->ShowWindow ( SW_HIDE );
		GetDlgItem ( IDC_HELPTEXT )->ShowWindow ( SW_HIDE );

		SetWindowText( g_pTGCWord[91] );
		GetDlgItem ( IDC_DESCTEXT1 )->ShowWindow ( SW_SHOW );
		GetDlgItem ( IDC_DESCTEXT1 )->ShowWindow ( SW_SHOW );
		SetDlgItemText ( IDC_WELCOMETEXT, g_pTGCWord[92] );
		SetDlgItemText ( IDC_DESCTEXT1, g_pTGCWord[93] );
		SetDlgItemText ( IDC_ORDERKEYLABEL, g_pTGCWord[94] );
		SetDlgItemText ( IDC_NEXT, g_pTGCWord[95] );
		SetDlgItemText ( IDCANCEL, g_pTGCWord[96] );
		SetDlgItemText ( IDC_FEEDBACKAREA, "" );

		GetDlgItem ( IDC_HARDWAREKEY )->ShowWindow ( SW_SHOW );
		GetDlgItem ( IDC_HARDWAREKEY )->EnableWindow ( TRUE );

  	  #else
		GetDlgItem ( IDC_TRIAL )->ShowWindow ( SW_HIDE );
		GetDlgItem ( IDC_BUY )->ShowWindow ( SW_SHOW );

		// U75 - 220909 - no longer need to make reference to CD/DVD verification
		//SetDlgItemText ( IDC_HELPTEXT, g_pTGCWord[3] );
		SetDlgItemText ( IDC_HELPTEXT, "" );

		// new FREE DARK BASIC PROFESSIONAL
		int iCompilerCertificatePresent = AmIActive ( 0, NULL );
		iCompilerCertificatePresent = iCompilerCertificatePresent + AmIActive ( 1, NULL );
		iCompilerCertificatePresent = iCompilerCertificatePresent + AmIActive ( 2, NULL );
		if ( iCompilerCertificatePresent==0 ) 
		{
			// no compiler certificate, so offer a free one
			GetDlgItem( IDC_GETFREE )->ShowWindow ( SW_SHOW );
			SetDlgItemText ( IDC_HELPTEXT, g_pTGCWord[105] );
		}

	  #endif
	 #endif
	#endif

	#ifdef FREECERTIFICATESVERSION
	SetDlgItemText ( IDC_BUY, "Activate For Free" );
	#endif

	return TRUE;
}

void CTGCOnlineDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

// build the certificate file
bool CTGCOnlineDlg::BuildTheCertificateFile ( LPSTR pAnySerial, LPSTR* ppResponseCertificate, int* piIndexToInternalProduct )
{
	// flag
	bool bSuccess=false;

	// override internal value with bypass value if debug version
	#ifdef FREECERTIFICATESVERSION
	if ( bBypassAllVerifyChecks )
	{
		*piIndexToInternalProduct = atoi(*ppResponseCertificate);
		delete *ppResponseCertificate;
		*ppResponseCertificate = BuildProductKey ( *piIndexToInternalProduct );
	}
	#endif

	// override internal value with DBPro full version if Serial Code System
	#ifdef TGCSERIALCODE
	{
 		*piIndexToInternalProduct = 2;
		delete *ppResponseCertificate;
		*ppResponseCertificate = BuildProductKey ( *piIndexToInternalProduct );
	}
	#endif

	// found match with a product
	if ( *piIndexToInternalProduct!=-1 )
	{
		// customer has permission to use this product element
		if ( g_pFeedbackAreaString )
		{
			if ( g_dwFreeOrFullActivate==0 )
			{
				// generic response
				if ( g_pTGCWord )
				{
					strcat ( g_pFeedbackAreaString, g_pTGCWord[26]);
					strcat ( g_pFeedbackAreaString, "\r\n");
					strcat ( g_pFeedbackAreaString, g_pTGCWord[27]);
					strcat ( g_pFeedbackAreaString, "\r\n");
				}
			}
			if ( g_dwFreeOrFullActivate==1 )
			{
				if ( g_pTGCWord )
				{
					strcat ( g_pFeedbackAreaString, g_pTGCWord[28] );
					strcat ( g_pFeedbackAreaString, "\r\n");
				}
			}
			strcat ( g_pFeedbackAreaString, "\r\n" );
		}

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
			strcat ( pINIPath, *ppResponseCertificate );

			// description of product
			LPSTR pDescOfProduct = g_pProductDescList [ *piIndexToInternalProduct ];
			WritePrivateProfileString ( "PRODUCT", "DESC", pDescOfProduct, pINIPath );

			// todays date
			int iToday = GetCurrentDay();
			LPSTR pProductDate = GenerateMangledDate ( iToday );

			// date of product
			WritePrivateProfileString ( "PRODUCT", "DATE", pProductDate, pINIPath );

			// if serial code system, mention this as the certificate is really an off-line freebie
			#ifdef TGCSERIALCODE
			WritePrivateProfileString ( "PRODUCT", "SERIAL", pAnySerial, pINIPath );
			#endif

			// also activate certificate by adding verifier date to registry (for trials)
			LPSTR pPRKey = "SOFTWARE\\Dark Basic\\Dark Basic Pro";
			WriteToRegistry ( pPRKey, g_pProductList[*piIndexToInternalProduct], pProductDate );
			delete pProductDate;
			pProductDate=NULL;

			// also delete trial certificate if full certificate activated
			int iPairCode = (*piIndexToInternalProduct)%3;
			if ( iPairCode==1 || iPairCode==2 )
			{
				// delete redundant previous certificate (0=trial/1=60day/2=full)
				LPSTR pRedundant = BuildProductKey ( (*piIndexToInternalProduct)-1 );

				// delete the file
				strcpy ( pINIPath, pAbsPath );
				strcat ( pINIPath, "\\" );
				strcat ( pINIPath, pRedundant );
				DeleteFile ( pINIPath );

				// free usages
				delete pRedundant;
				pRedundant=NULL;

				// optionally may leap from trial(0) to full(2)
				if ( iPairCode==2 )
				{
					// delete redundant previous certificate (0=trial/1=60day/2=full)
					LPSTR pRedundant = BuildProductKey ( (*piIndexToInternalProduct)-2 );

					// delete the file
					strcpy ( pINIPath, pAbsPath );
					strcat ( pINIPath, "\\" );
					strcat ( pINIPath, pRedundant );
					DeleteFile ( pINIPath );

					// free usages
					delete pRedundant;
					pRedundant=NULL;
				}
			}

			// finally write a raw HWKEY MD5 for the compiler to use (compile becomes intenpendent of any HW check)
			strcpy ( pINIPath, pAbsPath );
			strcat ( pINIPath, "\\local.ini" );

			// ensure local cert is not there first
			char pString [ _MAX_PATH ];
			strcpy ( pString, "" );
			GetPrivateProfileString ( "LOCAL", "CERT", "", pString, 256, pINIPath );
			if ( strcmp ( pString, "")==NULL )
			{
				// write cert for local use
				WritePrivateProfileString ( "LOCAL", "CERT", GetHWKey(), pINIPath );
			}

			// mark as a success
			bSuccess=true;

			// leave folder
			_chdir ( ".." );
		}

		// if TRIAL, 60DAY or FULL of main product
		if ( (*piIndexToInternalProduct)<=2 )
			CopyMainAppDLLSAcross ( *piIndexToInternalProduct );
	}
	else
	{
		// cannot find product match with TGC Online
		if ( g_pTGCWord && g_pFeedbackAreaString )
		{
			strcat ( g_pFeedbackAreaString, g_pTGCWord[29] );
			strcat ( g_pFeedbackAreaString, "\r\n" );
			strcat ( g_pFeedbackAreaString, "\r\n" );
		}
	}

	// result
	return bSuccess;
}

BOOL CTGCOnlineDlg::OnCommand(UINT nID, LPARAM lParam)
{
	bool bPromptUpdateOfNext=false;

	switch ( LOWORD ( nID ) )
	{
		case IDC_GETFREE :	// Run free email register module
		{
			// launch get free order key module
			ShellExecute(	NULL,
							"open",
							"TGCFree.exe",
							"",
							"",
							SW_SHOWDEFAULT);

//			// exit this module
//			PostQuitMessage(0);
		}
		break;

		case IDC_TRIAL :	// Link
		{
			// NEXT button
			CWnd* pControlWnd = GetDlgItem ( IDC_NEXT );
			SetDlgItemText ( IDC_DESCTEXT3, g_pTGCWord[4] );
			pControlWnd->EnableWindow ( TRUE );

			// Email And Password Entry
			SetDlgItemText ( IDC_ORDERKEYLABEL, g_pTGCWord[5] );
			SetDlgItemText ( IDC_HARDWAREKEYLABEL, g_pTGCWord[6] );
			SetDlgItemText ( IDC_HARDWAREKEY, "" );
			SetDlgItemText ( IDC_ORDERKEY, "" );
			GetDlgItem ( IDC_HARDWAREKEY )->EnableWindow ( TRUE );
			GetDlgItem ( IDC_ORDERKEY )->EnableWindow ( TRUE );
			GetDlgItem( IDC_ORDERKEY )->ShowWindow ( SW_SHOW );
			GetDlgItem( IDC_HARDWAREKEY )->ShowWindow ( SW_SHOW );
			GetDlgItem( IDC_REGISTERATSITE )->ShowWindow ( SW_SHOW );
			GetDlgItem( IDC_GETFREE )->ShowWindow ( SW_HIDE );
			SetDlgItemText ( IDC_FEEDBACKAREA, g_pTGCWord[7]);
			SetDlgItemText ( IDC_DESCTEXT2, g_pTGCWord[8]);

			// Free Version Activate
			g_dwFreeOrFullActivate=0;

			// Update Area
			bPromptUpdateOfNext=true;
			UpdateWindow();
		}
		break;

		case IDC_BUY :		// Link
		{
			// Email And Order Key Entry
			SetDlgItemText ( IDC_ORDERKEYLABEL, g_pTGCWord[5] );
			SetDlgItemText ( IDC_HARDWAREKEYLABEL, g_pTGCWord[9] );
			SetDlgItemText ( IDC_HARDWAREKEY, "" );
			SetDlgItemText ( IDC_ORDERKEY, "" );
			GetDlgItem ( IDC_HARDWAREKEY )->EnableWindow ( TRUE );
			GetDlgItem ( IDC_ORDERKEY )->EnableWindow ( TRUE );
			GetDlgItem( IDC_ORDERKEY )->ShowWindow ( SW_SHOW );
			GetDlgItem( IDC_HARDWAREKEY )->ShowWindow ( SW_SHOW );
			SetDlgItemText ( IDC_FEEDBACKAREA, g_pTGCWord[10]);
			SetDlgItemText ( IDC_DESCTEXT2, g_pTGCWord[11]);

			#ifdef FREECERTIFICATESVERSION
			SetDlgItemText ( IDC_HARDWAREKEY, "no@need.com" );
			SetDlgItemText ( IDC_ORDERKEY, "BYPASS:" );
			if ( g_pFeedbackAreaString ) delete g_pFeedbackAreaString;
			g_pFeedbackAreaString = new char [ 10240 ];
			strcpy ( g_pFeedbackAreaString, g_pTGCWord[12]);
			strcat ( g_pFeedbackAreaString, "\r\n");
			strcat ( g_pFeedbackAreaString, g_pTGCWord[13]);
			strcat ( g_pFeedbackAreaString, "\r\n");
			strcat ( g_pFeedbackAreaString, " 0, 1, 2 = DBPro Online\r\n");
			strcat ( g_pFeedbackAreaString, " 3, 4, 5 = Advanced Terrain\r\n");
			strcat ( g_pFeedbackAreaString, " 6, 7, 8 = System Enhancements\r\n");
			strcat ( g_pFeedbackAreaString, " 9,10,11 = Physics P\r\n");
			strcat ( g_pFeedbackAreaString, "12,13,14 = Physics C\r\n");
			strcat ( g_pFeedbackAreaString, "15,16,17 = Physics P + C\r\n");
			strcat ( g_pFeedbackAreaString, "18,19,20 = EZRotate\r\n");
			strcat ( g_pFeedbackAreaString, "21,22,23 = MEQON\r\n");
			strcat ( g_pFeedbackAreaString, "24,25,26 = LUA\r\n");
			strcat ( g_pFeedbackAreaString, "27,28,29 = TextureMax\r\n");
			strcat ( g_pFeedbackAreaString, "30,31,32 = Extends\r\n");
			strcat ( g_pFeedbackAreaString, "33,34,35 = DarkPHYSICS\r\n");
			strcat ( g_pFeedbackAreaString, "36,37,38 = DarkAI\r\n");
			strcat ( g_pFeedbackAreaString, "39,40,41 = DarkLIGHTS\r\n");
			strcat ( g_pFeedbackAreaString, "42,43,44 = StyX\r\n");
			strcat ( g_pFeedbackAreaString, "45,46,47 = ConvSEO\r\n");
			strcat ( g_pFeedbackAreaString, "48,49,50 = Enhancement Animation\r\n");
			strcat ( g_pFeedbackAreaString, "51,52,53 = DarkInk\r\n");
			strcat ( g_pFeedbackAreaString, "54,55,56 = GreenEar\r\n");
			strcat ( g_pFeedbackAreaString, "57,58,59 = DarkNet\r\n");
			strcat ( g_pFeedbackAreaString, "60,61,62 = DarkCLOUDS\r\n");
			strcat ( g_pFeedbackAreaString, "63,64,65 = DarkKFS\r\n");
			SetDlgItemText ( IDC_FEEDBACKAREA, g_pFeedbackAreaString );
			SetDlgItemText ( IDC_DESCTEXT2, "");
			#endif

			// ensure it does not interfere
			GetDlgItem ( IDC_HELPTEXT )->ShowWindow ( SW_HIDE );

			// Full Version Activate
			g_dwFreeOrFullActivate=1;

			// Update Area
			bPromptUpdateOfNext=true;
			UpdateWindow();
		}
		break;

		case IDC_BACK :	// BACK Button
		{
			// vars used
			CWnd* pControlWnd;

			#ifdef TGCVERIFIER
			// NEXT button active
			pControlWnd = GetDlgItem ( IDC_NEXT );
			pControlWnd->EnableWindow ( TRUE );
			// Ensure first page
			m_bFirstPage=false;
			// Restore text
			#ifdef TGCFREE
				SetDlgItemText ( IDC_FEEDBACKAREA, g_pTGCWord[102] );
			#else
				SetDlgItemText ( IDC_FEEDBACKAREA, g_pTGCWord[15] );
			#endif
			#else
			// initial feedback area text
			if ( g_pInstalledAndActiveProducts ) delete g_pInstalledAndActiveProducts;
			LPSTR pYourInstalled = g_pTGCWord[81];
			LPSTR pValidFor = g_pTGCWord[82];
			LPSTR pDays = g_pTGCWord[83];
			LPSTR pValidCert = g_pTGCWord[84];
			LPSTR pExpiredCert = g_pTGCWord[85];
			LPSTR pDBProNotActivated = g_pTGCWord[86];
			g_pInstalledAndActiveProducts = GenerateInstalledProducts( pYourInstalled, pValidFor, pDays, pValidCert, pExpiredCert, pDBProNotActivated );
			SetDlgItemText ( IDC_FEEDBACKAREA, g_pInstalledAndActiveProducts );
			#endif

			// clear backtrack button
			pControlWnd = GetDlgItem ( IDC_BACK );
			pControlWnd->EnableWindow ( false );

			// Clear upper area
			SetDlgItemText ( IDC_ORDERKEYLABEL, "" );
			SetDlgItemText ( IDC_HARDWAREKEYLABEL, "" );
			GetDlgItem ( IDC_HARDWAREKEY )->EnableWindow ( FALSE );
			GetDlgItem ( IDC_ORDERKEY )->EnableWindow ( FALSE );
			GetDlgItem( IDC_ORDERKEY )->ShowWindow ( SW_HIDE );
			GetDlgItem( IDC_HARDWAREKEY )->ShowWindow ( SW_HIDE );
			SetDlgItemText ( IDC_DESCTEXT2, "" );
			SetDlgItemText ( IDC_DESCTEXT3, "" );
		}
		break;

		case IDC_NEXT :	// NEXT Button
		{	
			#ifdef TGCSERIALCODE
			{
				// Get serial code text
				bool bValidSerialCode = false;
				LPSTR pSerialCodeString = new char [ 256 ];
				GetDlgItemText ( IDC_HARDWAREKEY, pSerialCodeString, 256 );
				if ( pSerialCodeString )
				{
					if ( strlen ( pSerialCodeString )==16 )
					{
						// determine valid or not
						bValidSerialCode = IsSerialCodeValid ( pSerialCodeString );
					}
				}

				// Serial Code Or Bust
				if ( bValidSerialCode )
				{
					// valid, go and create certificate for DBPro (special one for serial codes)
					SetDlgItemText ( IDC_FEEDBACKAREA, "..." );

					// Produce dummy certificate soon to be replaced with 'free' valid one
					int iIndexToInternalProduct = 2;
					LPSTR pResponseCertificate = new char [ 1 ];
					strcpy ( pResponseCertificate, "" );
					bool bSuccess = BuildTheCertificateFile ( pSerialCodeString, &pResponseCertificate, &iIndexToInternalProduct );
					if ( bSuccess ) { Sleep(1000); SetDlgItemText ( IDC_FEEDBACKAREA, g_pTGCWord[97] ); }
					GetDlgItem ( IDC_NEXT )->EnableWindow ( FALSE );
					GetDlgItem ( IDC_NEXT )->ShowWindow ( SW_HIDE );
					GetDlgItem ( IDC_HARDWAREKEY )->EnableWindow ( FALSE );
					SetDlgItemText ( IDCANCEL, "OK" );
					if ( pResponseCertificate )
					{
						delete pResponseCertificate;
						pResponseCertificate=NULL;
					}
				}
				else
				{
					// failed, reset edit box to try again
					SetDlgItemText ( IDC_HARDWAREKEY, "" );
					SetDlgItemText ( IDC_FEEDBACKAREA, g_pTGCWord[98] );
					GetDlgItem ( IDC_NEXT )->EnableWindow ( FALSE );
				}

				// free usages
				if ( pSerialCodeString )
				{
					delete pSerialCodeString;
					pSerialCodeString=NULL;
				}

				// done
				break;
			}
			#endif

			#ifdef TGCVERIFIER
			if ( m_bFirstPage==false )
			{
				// Instructions about verifier
				if ( g_pFeedbackAreaString ) delete g_pFeedbackAreaString;
				g_pFeedbackAreaString = new char [ 10240 ];
				#ifdef TGCFREE
					strcpy ( g_pFeedbackAreaString, g_pTGCWord[102] );
				#else
					strcpy ( g_pFeedbackAreaString, g_pTGCWord[15] );
				#endif
				SetDlgItemText ( IDC_FEEDBACKAREA, g_pFeedbackAreaString );

				// Email Required for info gathering
				SetDlgItemText ( IDC_ORDERKEYLABEL, g_pTGCWord[5] );
				SetDlgItemText ( IDC_ORDERKEY, "" );
				GetDlgItem ( IDC_HARDWAREKEY )->EnableWindow ( TRUE );
				GetDlgItem( IDC_HARDWAREKEY )->ShowWindow ( SW_SHOW );
				SetDlgItemText ( IDC_DESCTEXT2, g_pTGCWord[16]);

				// CD-Verifier Version Activate
				#ifdef TGCFREE
					g_dwFreeOrFullActivate=3;
				#else
					g_dwFreeOrFullActivate=2;
				#endif

				// Update Area
				bPromptUpdateOfNext=true;
				UpdateWindow();

				// exit now from page one
				m_bFirstPage=true;
				break;
			}
			#endif

			// Ensure it is always removed right away
			GetDlgItem( IDC_REGISTERATSITE )->ShowWindow ( SW_HIDE );
			GetDlgItem( IDC_GETFREE )->ShowWindow ( SW_HIDE );

			// verify or activation
			if ( g_dwFreeOrFullActivate==2 || g_dwFreeOrFullActivate==3 )
			{
				// feedback text for user prompt
				if ( g_pFeedbackAreaString ) delete g_pFeedbackAreaString;
				g_pFeedbackAreaString = new char [ 10240 ];

				// prompt user prior to check
				strcpy ( g_pFeedbackAreaString, "" );
				if ( g_dwFreeOrFullActivate==2 ) strcpy ( g_pFeedbackAreaString, g_pTGCWord[17]);
				strcat ( g_pFeedbackAreaString, "\r\n");
				SetDlgItemText ( IDC_FEEDBACKAREA, g_pFeedbackAreaString );
				UpdateWindow ();

				// verify that the CD is valid
				char Err[32];
				int count = 0;
				strcpy(Err,"0");
				int q = rand()%1000;
				int r = 0, s = 0;
				if ( g_dwFreeOrFullActivate==2 )
				{
					// CD VERIFIER (later replaced with free activation of compiler)
					r = rand()%1000;
					s = CheckForValidDBProCD ( q, r, rand()%1000, &count, Err );
				}
				if ( g_dwFreeOrFullActivate==3 )
				{
					// FREE COMPILER CERTIFICATE
				}
				if ( r==s )
				{
					// valid - this is where a hacker would replace JZ with unconditional JMP
					// so use condition above again when posting verification
					// we can count piracy hacks this way...
					// r-s : 0=valid  non-0=JMP hack used
					#ifdef TGCFREE
						strcpy ( g_pFeedbackAreaString, "" );
						strcat ( g_pFeedbackAreaString, "\r\n" );
					#else
						strcpy ( g_pFeedbackAreaString, g_pTGCWord[18]);
						strcat ( g_pFeedbackAreaString, "\r\n");
						strcat ( g_pFeedbackAreaString, g_pTGCWord[19]);
						strcat ( g_pFeedbackAreaString, "\r\n");
					#endif
					
					// extra data posted
					//if ( (r-s)==0 && q+3-count==0 )
					//	strcat ( g_pFeedbackAreaString, "Verifier not hacked.\r\n" );
					//else
					//	strcat ( g_pFeedbackAreaString, "Verifier hacked.\r\n" );

					// Hour glass
					SetCursor ( LoadCursor(NULL, IDC_WAIT) );

					// Send POST data to server
					LPSTR pActivationString = NULL;
					pActivationString = SendVerifyPostToServer("dbprocd",count,q,r,s);

					// If response returned, act on it
					LPSTR pResponseResponse = NULL;
					if ( pActivationString )
					{
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
								if ( strnicmp ( pPtrStart, "ERR: ", 5 )==NULL )
								{
									// status from response if error
									DWORD dwFieldDataOnly = dwLength-5;
									pResponseResponse = new char [ dwFieldDataOnly+1 ];
									memcpy ( pResponseResponse, pPtrStart+5, dwFieldDataOnly );
									pResponseResponse [ dwFieldDataOnly ] = 0;
									int iErrValue = atoi ( pResponseResponse );
									switch ( iErrValue )
									{
										case 1 : strcat ( g_pFeedbackAreaString, g_pTGCWord [ 120+iErrValue ] );	break;
										case 2 : strcat ( g_pFeedbackAreaString, g_pTGCWord [ 120+iErrValue ] );	break;
										case 3 : strcat ( g_pFeedbackAreaString, g_pTGCWord [ 120+iErrValue ] );	break;
										case 4 : strcat ( g_pFeedbackAreaString, g_pTGCWord [ 120+iErrValue ] );	break;
										case 5 : strcat ( g_pFeedbackAreaString, g_pTGCWord [ 120+iErrValue ] );	break;
										case 6 : strcat ( g_pFeedbackAreaString, g_pTGCWord [ 120+iErrValue ] );	break;
										case 7 : strcat ( g_pFeedbackAreaString, g_pTGCWord [ 120+iErrValue ] );	break;
										case 8 : strcat ( g_pFeedbackAreaString, g_pTGCWord [ 120+iErrValue ] );	break;
										case 9 : strcat ( g_pFeedbackAreaString, g_pTGCWord [ 120+iErrValue ] );	break;
										case 10 : strcat ( g_pFeedbackAreaString, g_pTGCWord [ 120+iErrValue ] );	break;
										case 11 : strcat ( g_pFeedbackAreaString, g_pTGCWord [ 120+iErrValue ] );	break;
										case 12 : strcat ( g_pFeedbackAreaString, g_pTGCWord [ 120+iErrValue ] );	break;
										case 13 : strcat ( g_pFeedbackAreaString, g_pTGCWord [ 120+iErrValue ] );	break;
										case 14 : strcat ( g_pFeedbackAreaString, g_pTGCWord [ 120+iErrValue ] );	break;
										case 17 : strcat ( g_pFeedbackAreaString, g_pTGCWord [ 120+iErrValue ] );	break;
										case 20 : strcat ( g_pFeedbackAreaString, g_pTGCWord [ 120+iErrValue ] );	break;
										case 21 : strcat ( g_pFeedbackAreaString, g_pTGCWord [ 120+iErrValue ] );	break;
										case 22 : strcat ( g_pFeedbackAreaString, g_pTGCWord [ 120+iErrValue ] );	break;
										case 23 : strcat ( g_pFeedbackAreaString, g_pTGCWord [ 120+iErrValue ] );	break;
										case 24 : strcat ( g_pFeedbackAreaString, g_pTGCWord [ 120+iErrValue ] );	break;
										case 50 : strcat ( g_pFeedbackAreaString, g_pTGCWord [ 120+iErrValue ] );	break;
										case 51 : strcat ( g_pFeedbackAreaString, g_pTGCWord [ 120+iErrValue ] );	break;
										case 53 : strcat ( g_pFeedbackAreaString, g_pTGCWord [ 120+iErrValue ] );	break;
										default : strcat ( g_pFeedbackAreaString, g_pTGCWord [ 154 ] );	break;
									}
								}
								// prepare of next line
								pPtrStart = pPtr+1;
							}
						}

						// success if no error
						if ( pResponseResponse==NULL )
						{
							// successful
							strcat ( g_pFeedbackAreaString, g_pTGCWord[20]);
							strcat ( g_pFeedbackAreaString, "\r\n" );

							// Button text change
							SetDlgItemText ( IDCANCEL, g_pTGCWord[21] );
						}
					}
					else
					{
						// no response at all
						strcat ( g_pFeedbackAreaString, g_pTGCWord[22]);
						strcat ( g_pFeedbackAreaString, "\r\n" );
						strcat ( g_pFeedbackAreaString, "\r\n" );
					}

					// Arrow again
					SetCursor ( LoadCursor(NULL, IDC_ARROW) );

					// free usages
					if ( pActivationString )
					{
						delete pActivationString;
						pActivationString=NULL;
					}
					if ( pResponseResponse )
					{
						delete pResponseResponse;
						pResponseResponse=NULL;
					}

					// completed task
					SetDlgItemText ( IDC_FEEDBACKAREA, g_pFeedbackAreaString );
					UpdateWindow ();
				}
				else
				{
					// not valid
					#ifdef TGCFREE
						strcpy ( g_pFeedbackAreaString, g_pTGCWord[104]);
					#else
						strcpy ( g_pFeedbackAreaString, g_pTGCWord[23]);
					#endif
					strcat ( g_pFeedbackAreaString, " (" );
					strcat ( g_pFeedbackAreaString, Err );
					strcat ( g_pFeedbackAreaString, ")" );
					strcat ( g_pFeedbackAreaString, "\r\n" );
					SetDlgItemText ( IDC_FEEDBACKAREA, g_pFeedbackAreaString );
					UpdateWindow ();
				}

				// clear field
				SetDlgItemText ( IDC_DESCTEXT3, "" );

				// restore buttons
				CWnd* pControlWnd = GetDlgItem ( IDC_BACK );
				pControlWnd->EnableWindow ( TRUE );
				pControlWnd = GetDlgItem ( IDC_NEXT );
				pControlWnd->EnableWindow ( FALSE );
			}
			else
			{
				// flag to record success
				bool bSuccess=false;

				// feedback text for user prompt
				if ( g_pFeedbackAreaString ) delete g_pFeedbackAreaString;
				g_pFeedbackAreaString = new char [ 10240 ];
				strcpy ( g_pFeedbackAreaString, g_pTGCWord[24] );
				strcat ( g_pFeedbackAreaString, "\r\n" );
				strcat ( g_pFeedbackAreaString, g_pTGCWord[25] );
				strcat ( g_pFeedbackAreaString, "\r\n" );

				// Clear upper area
				SetDlgItemText ( IDC_ORDERKEYLABEL, "" );
				SetDlgItemText ( IDC_HARDWAREKEYLABEL, "" );
				GetDlgItem ( IDC_HARDWAREKEY )->EnableWindow ( FALSE );
				GetDlgItem ( IDC_ORDERKEY )->EnableWindow ( FALSE );
				GetDlgItem( IDC_ORDERKEY )->ShowWindow ( SW_HIDE );
				GetDlgItem( IDC_HARDWAREKEY )->ShowWindow ( SW_HIDE );

				// Clear feedback area with sending prompt
				SetDlgItemText ( IDC_DESCTEXT3, "" );
				SetDlgItemText ( IDC_FEEDBACKAREA, g_pFeedbackAreaString );
				UpdateWindow();

				// data required from response
				LPSTR pResponseStatus = NULL;
				LPSTR pResponseResponse = NULL;
				LPSTR pResponseCertificate = NULL;
				LPSTR pResponseActivated = NULL;
				LPSTR pResponseTGCUser = NULL;
				LPSTR pResponseProduct = NULL;

				// First task condition
				if ( 1 )
				{
					// Hour glass
					SetCursor ( LoadCursor(NULL, IDC_WAIT) );

					// Send request to server
					bool bResponseReturnedButNoFieldsRecognised=true;
					LPSTR pActivationString = NULL;
					
					// Check if overridden with debug-certificate-maker
					#ifdef FREECERTIFICATESVERSION
	 				bBypassAllVerifyChecks = false;
					{
						pActivationString = new char[256];
						GetDlgItemText ( IDC_ORDERKEY, pActivationString, 256 );
	 					if ( strnicmp ( pActivationString, "BYPASS:", 7 )==NULL )
						{
							// create certificate
							bBypassAllVerifyChecks = true;
						}
						else
							delete pActivationString;
					}
					#else
					{
						// product to trial set by external file
						char curdir[_MAX_PATH];
						char curini[_MAX_PATH];
						getcwd(curdir,_MAX_PATH);
						strcpy(curini,curdir);
						strcat(curini,"\\TGCOnlineTrial.ini");
						char pProductName [ 256 ];
						GetPrivateProfileString("PRODUCT","TRIAL","dbpro",pProductName,256,curini);

						// REQUEST FROM SERVER
						pActivationString = SendRequestToServer(pProductName);
					}
					#endif

					// Clear feedback area
					strcpy ( g_pFeedbackAreaString, "" );
					SetDlgItemText ( IDC_FEEDBACKAREA, g_pFeedbackAreaString );
					UpdateWindow();

					// If response returned, act on it
					if ( pActivationString )
					{
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
								if ( strnicmp ( pPtrStart, "ERR: ", 5 )==NULL )
								{
									// status from response if error
									bResponseReturnedButNoFieldsRecognised=false;
									DWORD dwFieldDataOnly = dwLength-5;
									pResponseResponse = new char [ dwFieldDataOnly+1 ];
									memcpy ( pResponseResponse, pPtrStart+5, dwFieldDataOnly );
									pResponseResponse [ dwFieldDataOnly ] = 0;

									// feedback prompt for free activation ENGLISH!!
									int iErrValue = atoi ( pResponseResponse );
									if ( g_dwFreeOrFullActivate==0 )
									{
										switch ( iErrValue )
										{
											case 1 : strcat ( g_pFeedbackAreaString, "Machine Key (m) not supplied" );	break;
											case 2 : strcat ( g_pFeedbackAreaString, "Product ID (p) not supplied" );	break;
											case 3 : strcat ( g_pFeedbackAreaString, "Machine Key <> 32 chars" );	break;
											case 4 : strcat ( g_pFeedbackAreaString, "Product ID > 64 chars" );	break;
											case 5 : strcat ( g_pFeedbackAreaString, "Machine Key = Product ID (invalid possibility)" );	break;
											case 6 : strcat ( g_pFeedbackAreaString, "Machine Key has spaces in it (not allowed)" );	break;
											case 7 : strcat ( g_pFeedbackAreaString, "Product ID has spaces in it (not allowed)" );	break;
											case 8 : strcat ( g_pFeedbackAreaString, "SQL Error Saving the Certificate, try again later" );	break;
											case 9 : strcat ( g_pFeedbackAreaString, "SQL Error Retrieving Certificate Data, I've given up, sorry" );	break;
											case 10 : strcat ( g_pFeedbackAreaString, "The Machine Key has been banned from activating this Products Trial" );	break;
											case 11 : strcat ( g_pFeedbackAreaString, "The Machine Key has been deleted for this Product Trial" );	break;
											case 12 : strcat ( g_pFeedbackAreaString, "The Machine Key given is associated with a Fraudulent Order, banned" );	break;
											case 13 : strcat ( g_pFeedbackAreaString, "Verification Key Missing" );	break;
											case 14 : strcat ( g_pFeedbackAreaString, "Verification Key Invalid" );	break;
											case 15 : strcat ( g_pFeedbackAreaString, "Email Address given, but Password not set" );	break;
											case 16 : strcat ( g_pFeedbackAreaString, "Password given, but Email Address not set" );	break;
											case 17 : strcat ( g_pFeedbackAreaString, "System Error - the SQL Database Failed to Respond, Network Down?" );	break;
											case 18 : strcat ( g_pFeedbackAreaString, "Reserved" );	break;
											case 19 : strcat ( g_pFeedbackAreaString, "User does not meet Access Level required for this Product" );	break;
											case 50 : strcat ( g_pFeedbackAreaString, "The email address given was too long. Maximum of 100 characters" );	break;
											case 51 : strcat ( g_pFeedbackAreaString, "The email address given was too short. Minimum of 8 characters" );	break;
											case 52 : strcat ( g_pFeedbackAreaString, "Password was too short. Minimum of 5 characters" );	break;
											case 53 : strcat ( g_pFeedbackAreaString, "Invalid email address. Please check and re-try" );	break;
											case 54 : strcat ( g_pFeedbackAreaString, "Your email address was not found. Please check it is correct or perhaps you need to register an account?" );	break;
											case 55 : strcat ( g_pFeedbackAreaString, "This account has been deleted from our system. Please contact Support if you feel this was in error" );	break;
											case 56 : strcat ( g_pFeedbackAreaString, "This account has been banned from our system. Please contact Support if you feel this was in error" );	break;
											case 57 : strcat ( g_pFeedbackAreaString, "Password given does not match that on your account" );	break;

											default : strcat ( g_pFeedbackAreaString, "Unknown error" );	break;
										}
									}
									if ( g_dwFreeOrFullActivate==1 )
									{
										switch ( iErrValue )
										{
											case 1 : strcat ( g_pFeedbackAreaString, "Machine Key (m) not supplied" );	break;
											case 2 : strcat ( g_pFeedbackAreaString, "n/a" );	break;
											case 3 : strcat ( g_pFeedbackAreaString, "Machine Key <> 32 chars" );	break;
											case 4 : strcat ( g_pFeedbackAreaString, "Order Key <> 128 chars" );	break;
											case 5 : strcat ( g_pFeedbackAreaString, "n/a" );	break;
											case 6 : strcat ( g_pFeedbackAreaString, "Machine Key has spaces in it (not allowed)" );	break;
											case 7 : strcat ( g_pFeedbackAreaString, "n/a" );	break;
											case 8 : strcat ( g_pFeedbackAreaString, "SQL Error Saving the Certificate, try again later" );	break;
											case 9 : strcat ( g_pFeedbackAreaString, "SQL Error Retrieving Certificate Data, I've given up, sorry" );	break;
											case 10 : strcat ( g_pFeedbackAreaString, "The Machine Key has been banned from activating this Product" );	break;
											case 11 : strcat ( g_pFeedbackAreaString, "The Machine Key has been deleted for this Product" );	break;
											case 12 : strcat ( g_pFeedbackAreaString, "The Machine Key given is associated with a Fraudulent Order, banned." );	break;
											case 13 : strcat ( g_pFeedbackAreaString, "Verification Key Missing" );	break;
											case 14 : strcat ( g_pFeedbackAreaString, "Verification Key Invalid" );	break;
											case 15 : strcat ( g_pFeedbackAreaString, "Email Address missing" );	break;
											case 16 : strcat ( g_pFeedbackAreaString, "Order Key (k) not supplied" );	break;
											case 17 : strcat ( g_pFeedbackAreaString, "System Error - the SQL Database Failed to Respond, Network Down?" );	break;
											case 18 : strcat ( g_pFeedbackAreaString, "Order Key already ACTIVATED + Machine Key is DIFFERENT + Email Address is DIFFERENT" );	break;
											case 19 : strcat ( g_pFeedbackAreaString, "Order Key already ACTIVATED + Machine Key is DIFFERENT + Email Address MATCHES" );	break;
											case 20 : strcat ( g_pFeedbackAreaString, "Email Address given does NOT match that associated with the Order Key given" );	break;
											case 21 : strcat ( g_pFeedbackAreaString, "Order Key matches + Machine Key matches BUT Email Address given is DIFFERENT" );	break;
											case 22 : strcat ( g_pFeedbackAreaString, "The Order Key has been banned" );	break;
											case 23 : strcat ( g_pFeedbackAreaString, "The Order Key has been suspended" );	break;
											case 24 : strcat ( g_pFeedbackAreaString, "The Order Key given is associated with a Fraudulent Order, therefore banned." );	break;
											case 25 : strcat ( g_pFeedbackAreaString, "Order Key was NOT found in the certificates database, typo perhaps?" );	break;
											case 26 : strcat ( g_pFeedbackAreaString, "n/a" );	break;
											default : strcat ( g_pFeedbackAreaString, "Unknown error" );	break;
										}
									}
									strcat ( g_pFeedbackAreaString, "\r\n" );
								}
								if ( strnicmp ( pPtrStart, "CERTIFICATE: ", 13 )==NULL )
								{
									// certificate from response
									bResponseReturnedButNoFieldsRecognised=false;
									DWORD dwFieldDataOnly = dwLength-13;
									pResponseCertificate = new char [ dwFieldDataOnly+1 ];
									memcpy ( pResponseCertificate, pPtrStart+13, dwFieldDataOnly );
									pResponseCertificate [ dwFieldDataOnly ] = 0;
								}
								if ( strnicmp ( pPtrStart, "ACTIVATED: ", 11 )==NULL )
								{
									// activated from trial response
									bResponseReturnedButNoFieldsRecognised=false;
									DWORD dwFieldDataOnly = dwLength-11;
									pResponseActivated = new char [ dwFieldDataOnly+1 ];
									memcpy ( pResponseActivated, pPtrStart+11, dwFieldDataOnly );
									pResponseActivated [ dwFieldDataOnly ] = 0;
								}
								if ( strnicmp ( pPtrStart, "TGC-USER: ", 10 )==NULL )
								{
									// tgcuser flag from response
									bResponseReturnedButNoFieldsRecognised=false;
									DWORD dwFieldDataOnly = dwLength-10;
									pResponseTGCUser = new char [ dwFieldDataOnly+1 ];
									memcpy ( pResponseTGCUser, pPtrStart+10, dwFieldDataOnly );
									pResponseTGCUser [ dwFieldDataOnly ] = 0;
								}
								if ( strnicmp ( pPtrStart, "PRODUCT: ", 9 )==NULL )
								{
									// product id from response
									bResponseReturnedButNoFieldsRecognised=false;
									DWORD dwFieldDataOnly = dwLength-9;
									pResponseProduct = new char [ dwFieldDataOnly+1 ];
									memcpy ( pResponseProduct, pPtrStart+9, dwFieldDataOnly );
									pResponseProduct [ dwFieldDataOnly ] = 0;
								}
								#ifdef FREECERTIFICATESVERSION
								if ( strnicmp ( pPtrStart, "BYPASS:", 7 )==NULL )
								{
									// bypass code to copy bypass value into main creator code
									bResponseReturnedButNoFieldsRecognised=false;
									DWORD dwFieldDataOnly = dwLength-7;
									pResponseCertificate = new char [ dwFieldDataOnly+1 ];
									memcpy ( pResponseCertificate, pPtrStart+7, dwFieldDataOnly );
									pResponseCertificate [ dwFieldDataOnly ] = 0;
								}
								#endif

								// prepare of next line
								pPtrStart = pPtr+1;
							}
						}
					}

					// if certificate returned
					if ( pResponseCertificate )
					{
						// secret name for private product (between Server and Client)
						int iIndexToInternalProduct = FindProductName ( pResponseCertificate );

						// build the certificate file
						bSuccess = BuildTheCertificateFile ( "", &pResponseCertificate, &iIndexToInternalProduct );
					}
					else
					{
						// after a delay, could not get data back from server
						if ( pActivationString==NULL )
						{
							strcat ( g_pFeedbackAreaString, g_pTGCWord[30]);
							strcat ( g_pFeedbackAreaString, "\r\n" );
							strcat ( g_pFeedbackAreaString, "\r\n" );
						}
						else
						{
							if ( bResponseReturnedButNoFieldsRecognised==true )
							{
								strcat ( g_pFeedbackAreaString, g_pTGCWord[31]);
								strcat ( g_pFeedbackAreaString, "\r\n" );
								strcat ( g_pFeedbackAreaString, "\r\n" );
							}
						}
					}

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
					if ( pResponseActivated )
					{
						delete pResponseActivated;
						pResponseActivated=NULL;
					}
					if ( pResponseTGCUser )
					{
						delete pResponseTGCUser;
						pResponseTGCUser=NULL;
					}
					if ( pResponseProduct )
					{
						delete pResponseProduct;
						pResponseProduct=NULL;
					}

					// Arrow again
					SetCursor ( LoadCursor(NULL, IDC_ARROW) );

					// on return, report 
					SetDlgItemText ( IDC_FEEDBACKAREA, g_pFeedbackAreaString );

					// and allow backtrack to first screen (only if success)
					if ( bSuccess )
					{
						CWnd* pControlWnd = GetDlgItem ( IDC_BACK );
						pControlWnd->EnableWindow ( TRUE );
						pControlWnd = GetDlgItem ( IDC_NEXT );
						pControlWnd->EnableWindow ( FALSE );

						// disable free trial button if already present
						if ( AmIActive ( 0, NULL )==1 ) GetDlgItem ( IDC_TRIAL )->EnableWindow ( FALSE );

						// Button text change
						SetDlgItemText ( IDCANCEL, "Finish" );
					}
				}
				else
				{
					// Failed for whatever reason
					strcat ( g_pFeedbackAreaString, g_pTGCWord[32]);
					strcat ( g_pFeedbackAreaString, "\r\n");
					SetDlgItemText ( IDC_FEEDBACKAREA, g_pFeedbackAreaString );
					UpdateWindow ();
				}
			}
		}
		break;

		case IDCANCEL :	// Cancel Button
		{
			PostQuitMessage(0);
		}
		break;

		#ifdef TGCSERIALCODE
		// SerialCode System Ignores Events except hardwarekey(serialcode)
		case IDC_HARDWAREKEY :	// IDC_HARDWAREKEY Editbox
		{
			switch ( HIWORD ( nID ) )
			{
				case EN_CHANGE :
				{
					// Get current email key enetered
					if ( g_pEmailString ) delete g_pEmailString;
					g_pEmailString = new char [ 256 ];
					GetDlgItemText ( IDC_HARDWAREKEY, g_pEmailString, 256 );
					if ( g_pEmailString )
						if ( strlen ( g_pEmailString )==0 )
						{
							delete g_pEmailString;
							g_pEmailString=NULL;
						}

					// allow next button to be updated
					bPromptUpdateOfNext=true;
				}
			}
		}
		break;
		#else
		case IDC_ORDERKEY :	// ORDERKEY Editbox
		{
			switch ( HIWORD ( nID ) )
			{
				case EN_CHANGE :
				{
					// Get current order key entered
					if ( g_pOrderKeyString ) delete g_pOrderKeyString;
					g_pOrderKeyString = new char [ 256 ];
					GetDlgItemText ( IDC_ORDERKEY, g_pOrderKeyString, 256 );
					if ( g_pOrderKeyString )
						if ( strlen ( g_pOrderKeyString )==0 )
						{
							delete g_pOrderKeyString;
							g_pOrderKeyString=NULL;
						}

					// allow next button to be updated
					bPromptUpdateOfNext=true;
				}
				break;
			}
		}
		break;

		case IDC_HARDWAREKEY :	// IDC_HARDWAREKEY Editbox
		{
			switch ( HIWORD ( nID ) )
			{
				case EN_CHANGE :
				{
					// Get current email key enetered
					if ( g_pEmailString ) delete g_pEmailString;
					g_pEmailString = new char [ 256 ];
					GetDlgItemText ( IDC_HARDWAREKEY, g_pEmailString, 256 );
					if ( g_pEmailString )
						if ( strlen ( g_pEmailString )==0 )
						{
							delete g_pEmailString;
							g_pEmailString=NULL;
						}

					// allow next button to be updated
					bPromptUpdateOfNext=true;
				}
			}
		}
		break;

		case IDC_REGISTERATSITE :	// Visit TGC Website
		{
			// Ensure main window is normal
			::SetWindowPos ( m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );

			// Go to HTTP
			ShellExecute(	NULL,
							"open",
							"http://www.thegamecreators.com/?m=registration",
							"",
							"",
							SW_SHOWDEFAULT);
		}
		#endif
	}

	// allow next button to be updated
	if ( bPromptUpdateOfNext==true )
	{
		// NEXT button
		CWnd* pControlWnd = GetDlgItem ( IDC_NEXT );
		#ifdef TGCVERIFIER
		if ( g_dwFreeOrFullActivate==2 )
		{
			if ( g_pEmailString==NULL )
			{
				SetDlgItemText ( IDC_DESCTEXT3, g_pTGCWord[33]);
				pControlWnd->EnableWindow ( FALSE );
			}
			else
			{
				SetDlgItemText ( IDC_DESCTEXT3, g_pTGCWord[34] );
				pControlWnd->EnableWindow ( TRUE );
			}
		}
		if ( g_dwFreeOrFullActivate==3 )
		{
			if ( g_pEmailString==NULL )
			{
				SetDlgItemText ( IDC_DESCTEXT3, g_pTGCWord[33]);
				pControlWnd->EnableWindow ( FALSE );
			}
			else
			{
				SetDlgItemText ( IDC_DESCTEXT3, g_pTGCWord[103] );
				pControlWnd->EnableWindow ( TRUE );
			}
		}
		#else
		if ( g_dwFreeOrFullActivate==1 )
		{
			if ( g_pOrderKeyString && g_pEmailString )
			{
				SetDlgItemText ( IDC_DESCTEXT3, g_pTGCWord[35] );
				pControlWnd->EnableWindow ( TRUE );
			}
			else
			{
				SetDlgItemText ( IDC_DESCTEXT3, g_pTGCWord[36] );
				pControlWnd->EnableWindow ( FALSE );
			}
		}
		else
		{
			SetDlgItemText ( IDC_DESCTEXT3, g_pTGCWord[37] );
			pControlWnd->EnableWindow ( TRUE );
		}
		#endif
	}

	return CDialog::OnCommand(nID, lParam);
}

void CTGCOnlineDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CTGCOnlineDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTGCOnlineDlg::OnClose ( void )
{
	// free usages
	if ( g_pOrderKeyString )
	{
		delete g_pOrderKeyString;
		g_pOrderKeyString=NULL;
	}
	if ( g_pEmailString )
	{
		delete g_pEmailString;
		g_pEmailString=NULL;
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
}
