//
// Decompress ADPCM to PCM WAV Header
//

// Includes
#include <windows.h>
#include <dsetup.h>
#include <dsound.h>
#include <mmreg.h>
#include <msacm.h>
#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <math.h>
#pragma comment(lib, "winmm")
#pragma comment(lib, "msacm32")

// Function Headers
LPBYTE WaveDecompress(LPBYTE lpSrc, LPWAVEFORMATEX lpwfxSrc, DWORD dwSizeSrc, LPWAVEFORMATEX lpwfxDst, LPDWORD lpdwSizeDst);
