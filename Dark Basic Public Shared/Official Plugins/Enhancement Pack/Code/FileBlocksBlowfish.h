// _THE BlowFishEnc ENCRYPTION ALGORITHM_
// by Bruce Schneier
// Revised code--3/20/94
// Converted to C++ class 5/96, Jim Conger
// Updated to support FileEncryption Utility by Nir Dremer, 9/02
#pragma once

#include "stdafx.h"

//#include "../FileEnc/Encryption_i.h"

class EncryptionInterface
{
public:
	EncryptionInterface(const char *pwd) { strncpy(_encryptionKey, pwd, 99); _encryptionKey[99] = 0; }
	virtual ~EncryptionInterface() { ZeroMemory(_encryptionKey, 100); }
	virtual DWORD encryptStream(const char *plain, const DWORD size, char *cipher) = 0;
	virtual DWORD decryptStream(const char *cipher, const DWORD size, char *plain) = 0;
protected:
    char _encryptionKey[100];	
};


class BlowFishEnc : public EncryptionInterface
{
public:
	BlowFishEnc(const char *pwd);
	~BlowFishEnc();

	DWORD encryptStream(const char *plain, const DWORD size, char *cipher);
	DWORD decryptStream(const char *cipher, const DWORD size, char *plain);

private:
	DWORD	GetOutputLength(DWORD lInputLong);

	DWORD 	*PArray;
	DWORD	(*SBoxes)[256];
	void 	BlowFishEnc_encipher(DWORD *xl, DWORD *xr);
	void 	BlowFishEnc_decipher(DWORD *xl, DWORD *xr);
};

union aword {
	DWORD dword;
	BYTE byte [4];
	struct {
	unsigned int byte3:8;
	unsigned int byte2:8;
	unsigned int byte1:8;
	unsigned int byte0:8;
	} w;
};
