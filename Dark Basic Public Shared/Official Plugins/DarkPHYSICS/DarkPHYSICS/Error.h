
#ifndef _ERROR_H_
#define _ERROR_H_

#include "globals.h"

namespace PhysX
{
	class cErrorStream : public NxUserOutputStream
	{
		public:
			void				reportError				( NxErrorCode code, const char* szMessage, const char* szFile, int iLine );
			NxAssertResponse	reportAssertViolation	( const char* szMessage, const char* szFile, int iLine );
			void				print					( const char* szMessage );
	};

	extern cErrorStream m_ErrorStream;
};

bool dbPhyCheckScene ( void );

#endif _ERROR_H_