// Encryptor.h: interface for the CEncryptor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENCRYPTOR_H__5BAF9135_98D8_4A41_BC9D_98EFF500133A__INCLUDED_)
#define AFX_ENCRYPTOR_H__5BAF9135_98D8_4A41_BC9D_98EFF500133A__INCLUDED_

#include "windows.h"

class CEncryptor  
{
	public:

		CEncryptor(DWORD dwUniqueKeyValue);
		virtual ~CEncryptor();

		void EncryptFileUniqueValue(LPSTR pKey);
		bool EncryptFileData(LPSTR filebuffer, DWORD filebuffersize, bool bEncryptIfTrue);
		DWORD GetUniqueKey(void) { return m_dwUniqueKey; }
		void SetUniqueKey(DWORD dwValue) { m_dwUniqueKey=dwValue; }

	private:

		DWORD m_dwUniqueKey;
};

#endif // !defined(AFX_ENCRYPTOR_H__5BAF9135_98D8_4A41_BC9D_98EFF500133A__INCLUDED_)
