// File Name:	IWRsdk.h		
// Description:	Function prototypes for iWear tracking support.
#define IWEARDRV_EXPLICIT
#include <iweardrv.h>
#define IWRSTDRV_EXPLICIT
#include <iwrstdrv.h>

// StereoModes
enum { IWR_MONO_MODE = 0, IWR_STEREO_MODE };
//-----------------------------------------------------------------------------
// Useful Conversion constants
//-----------------------------------------------------------------------------
#define IWR_PI					3.141593f
#define IWR_RAWTODEG			180.0f / 32768.0f
#define IWR_RAWTORAD			IWR_PI / 32768.0f
// Function return defines.
#define IWR_OK					0 // Ok 2 go with tracker driver.
#define IWR_NO_TRACKER			1 // Tracker Driver is NOT installed.
#define IWR_OFFLINE				2 // Tracker driver installed, yet appears to be offline or not responding.
#define IWR_TRACKER_CORRUPT		3 // Tracker driver installed, and missing exports.
#define IWR_NOTRACKER_INSTANCE	4 // Tracker driver did not open properly.
#define IWR_NO_STEREO			5 // Stereo driver not loaded.
#define IWR_STEREO_CORRUPT		6 // Stereo driver exports incorrect.
#define IWR_FILTER_NOSUPPORT	7 // No internal Driver filtering supported.
// Function prototyping.
#ifdef __cplusplus
 	extern "C" long IWRLoadDll( void );
	extern "C" long IWRFreeDll( void );
#else
	extern long IWRLoadDll( void );
	extern long IWRFreeDll( void );
#endif

HMODULE IWRTrackerInstance=NULL;
HMODULE IWRStereoInstance=NULL;
