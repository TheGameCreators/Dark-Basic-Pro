#include "stdafx.h"
#include "thread.h"

#pragma once

class CAdvertThread : public Thread
{
	public:
		CAdvertThread();
		virtual ~CAdvertThread();

	public:
		unsigned Run( );
		bool IsCertificateFree ( LPSTR pCertificate );
		bool IsFreeVersion();

};
