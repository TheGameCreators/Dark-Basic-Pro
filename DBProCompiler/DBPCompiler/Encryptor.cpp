// Encryptor.cpp: implementation of the CEncryptor class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "Encryptor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEncryptor::CEncryptor(DWORD dwUniqueKeyValue)
{
	m_dwUniqueKey=dwUniqueKeyValue;
}

CEncryptor::~CEncryptor()
{

}

bool CEncryptor::EncryptFileData(LPSTR filebuffer, DWORD filebuffersize, bool bEncryptIfTrue)
{
	DWORD dwUniqueKeyValue = m_dwUniqueKey;
	if(dwUniqueKeyValue>0)
	{
		// New key string from DWORD
		char pNewKey[10];
		for(DWORD n=0; n<8; n++)
		{
			unsigned char cChar = (unsigned char)((dwUniqueKeyValue/(1+n)) % 255);
			if(cChar<32) cChar=32;
			pNewKey[n] = cChar;
		}
		pNewKey[9]=0;

		// Key Index
		DWORD dwKeyIndex=0;
		DWORD dwKeyMax=strlen(pNewKey);

		// Encrupt Data using Key (mess the file data up)
		DWORD dwSpan = filebuffersize/1024;

		// Change a byte at each step point
		LPSTR pPtr = filebuffer;
		LPSTR pPtrEnd = filebuffer + filebuffersize;
		while(pPtr<pPtrEnd)
		{
			// Key Modifier (not so predictable method)
			DWORD dwActualIndex = (DWORD)(dwKeyIndex/2)*2;
			DWORD dwKeyValue = pNewKey[dwActualIndex];
			int iKeyData = dwKeyValue;
			if(((float)dwKeyIndex/3.0)==(DWORD)dwKeyIndex/3) iKeyData*=-1;
			iKeyData = abs(iKeyData) % 32;

			// Replacement keycode thats reliable
			iKeyData = dwUniqueKeyValue % 64;

			// Modify byte (true=encrypt)
			for(int r=0; r<iKeyData; r++)
			{
				if(bEncryptIfTrue)
				{
					if((unsigned char)*(pPtr)==255)
						*(pPtr)=0;
					else
						*(pPtr)+=1;
				}
				else
				{
					if((unsigned char)*(pPtr)==0)
						*(pPtr)=(unsigned char)255;
					else
						*(pPtr)-=1;
				}
			}

			// Next step in data
			pPtr+=dwSpan;

			// Move key index
			dwKeyIndex++;
			if(dwKeyIndex>=dwKeyMax) dwKeyIndex=0;
		}
	}

	// Complete
	return true;
}