//
// Decompress ADPCM to PCM WAV
//

// Include
#include "ADPCMtoWAV.h"

// Function Code
LPBYTE WaveDecompress(LPBYTE lpSrc, LPWAVEFORMATEX lpwfxSrc, DWORD dwSizeSrc, LPWAVEFORMATEX lpwfxDst, LPDWORD lpdwSizeDst)
{
	HACMSTREAM has;
	ACMSTREAMHEADER ash;
	LPBYTE lpDst;
	MMRESULT mmr;

	/* Setup the output format to the default PCM */
	memset(lpwfxDst, 0, sizeof(WAVEFORMATEX));
	lpwfxDst->wFormatTag = WAVE_FORMAT_PCM;

	/* Ask the ACM to suggest a compatible format */
	if ((mmr = acmFormatSuggest(NULL, lpwfxSrc, lpwfxDst, sizeof(WAVEFORMATEX), ACM_FORMATSUGGESTF_WFORMATTAG)) != 0) 
	{
		switch (mmr) {
			case MMSYSERR_INVALFLAG:
				puts("acmFormatSuggest(): At least one flag is invalid!");
				break;
			case MMSYSERR_INVALHANDLE:
				puts("acmFormatSuggest(): The specified handle is invalid!");
				break;
			case MMSYSERR_INVALPARAM:
				puts("acmFormatSuggest(): At least one parameter is invalid!");
				break;
			default:
				puts("acmFormatSuggest() failed!");
		}
		return NULL;
	}

  	/* Open an ACM data conversion stream */
	if ((mmr = acmStreamOpen(&has, NULL, lpwfxSrc, lpwfxDst, NULL, 0, 0, ACM_STREAMOPENF_NONREALTIME)) != 0) 
	{
		switch (mmr) {
			case ACMERR_NOTPOSSIBLE:
				puts("acmStreamOpen(): The requested operation cannot be performed!");
				break;
			case MMSYSERR_INVALFLAG:
				puts("acmStreamOpen(): At least one flag is invalid!");
				break;
			case MMSYSERR_INVALHANDLE:
				puts("acmStreamOpen(): The specified handle is invalid!");
				break;
			case MMSYSERR_INVALPARAM:
				puts("acmStreamOpen(): At least one parameter is invalid!");
				break;
			case MMSYSERR_NOMEM:
				puts("acmStreamOpen(): The system is unable to allocate resources!");
				break;
			default:
				puts("acmStreamOpen() failed!");
		}
		return NULL;
	}
	/* Determine output buffer size */
	if (acmStreamSize(has, dwSizeSrc, lpdwSizeDst, ACM_STREAMSIZEF_SOURCE))
	{
		acmStreamClose(has, 0);
		puts("acmStreamSize() failed!");
		return NULL;
	}
	/* Allocate the buffer */
	lpDst = (LPBYTE)malloc(*lpdwSizeDst);
	if (lpDst == NULL) {
		acmStreamClose(has, 0);
		puts("malloc() failed!");
		return NULL;
	}
	/* Prepare the stream header */
	memset(&ash, 0, sizeof(ash));
	ash.cbStruct = sizeof(ash);
	ash.pbSrc = lpSrc;
	ash.cbSrcLength = dwSizeSrc;
	ash.pbDst = lpDst;
	ash.cbDstLength = *lpdwSizeDst;
	if (acmStreamPrepareHeader(has, &ash, 0)) {
		free(lpDst);
		acmStreamClose(has, 0);
		puts("acmStreamPrepareHeader() failed!");
		return NULL;
	}
  	/* Start converting the data */
	if (acmStreamConvert(has, &ash, 0)) {
		acmStreamUnprepareHeader(has, &ash, 0);
		free(lpDst);
		acmStreamClose(has, 0);
		puts("acmStreamConvert() failed!");
		return NULL;
	}
	/* Unprepare the header */
	acmStreamUnprepareHeader(has, &ash, 0);
	/* Close the ACM stream */
	acmStreamClose(has, 0);
	/* return the exact destination buffer usage size */
	*lpdwSizeDst = ash.cbDstLengthUsed;

	// return raw data
	return lpDst;
}
