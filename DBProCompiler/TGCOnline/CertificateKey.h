//
// Hardware Key Header
//

// Include
#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable : 4996)
#include "windows.h"
#include "direct.h"
#include "io.h"

// Defines
#define InternalProductListQTY 26
#define InternalProductListMax InternalProductListQTY*3

// Externals
extern int g_iLastProductIndexCode;
extern char g_pHardwareHashString [ 256 ];
extern LPSTR g_pProductList[];
extern LPSTR g_pProductDescList[];
extern LPSTR g_pProductIcon[];
extern LPSTR g_pProductIconLink[];
extern LPSTR g_pProductDLLNames[];
extern DWORD g_pProductCodes[];

// DLL global calls
typedef char* ( *TMD5ChecksumGetMD5 )  ( BYTE*, UINT );
typedef void ( *TMD5ChecksumFreeMD5 )  ( char* );
extern TMD5ChecksumGetMD5				gCMD5ChecksumGetMD5;
extern TMD5ChecksumFreeMD5				gCMD5ChecksumFreeMD5;
extern void								InitMD5DLL ( void );
extern void								FreeMD5DLL ( void );

// Internal Functions
bool WriteToRegistry(char* PerfmonNamesKey, char* key, LPSTR Datavalue);
int GetCurrentDay ( void );

// Functions for HWKEY handling
void GenerateHWKey ( void );
void ReadLocalHWKey ( void );

// Functions for TGCOnline
int FindProductName ( LPSTR pResponseCertificate );
int FindIndexOfPluginByDLLName ( LPSTR pDLLName );
LPSTR GenerateCertificateForProduct ( int iIndex );
LPSTR BuildProductKey ( int iIndex );
LPSTR GenerateInstalledProductsEx ( LPSTR pYourInstalled, LPSTR pValidFor, LPSTR pDays, LPSTR pValidCert, LPSTR pExpiredCert, LPSTR pDBProNotActivated, LPSTR pNotInstalled );
LPSTR GenerateInstalledProducts ( LPSTR pYourInstalled, LPSTR pValidFor, LPSTR pDays, LPSTR pValidCert, LPSTR pExpiredCert, LPSTR pDBProNotActivated );
LPSTR GenerateMangledDate ( int iDay );

// Functions for TGCProtectedApp
int AmIActive ( int iProductIndex, LPSTR pDaysLeft );
LPSTR GetHWKey ( void );

