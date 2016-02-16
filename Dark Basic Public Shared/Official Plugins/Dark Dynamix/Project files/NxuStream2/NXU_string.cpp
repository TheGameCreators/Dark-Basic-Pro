#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#ifdef _MSC_VER
#pragma warning(disable:4996) // Disabling stupid .NET deprecated warning.
#endif

#include "NXU_string.h"
#include "NXU_helper.h"

NXU_errorReport	*gErrorReport	=	0;

namespace	NXU
{

bool gUseClothActiveState=false;
bool gUseSoftBodyActiveState=false;
bool gSaveBigEndian=false;
bool gProcessorBigEndian=false;

struct StringList
{
	StringList *mNextString;
	const	char *mString;
};

static StringList	*gRoot = 0;
static int gStringCount	=	0;
static int gStringMem	=	0;

struct InstanceList
{
	InstanceList *mNextInstance;
	const	char *mName;
	void *mInstance;
};

static InstanceList	*gInstanceRoot = 0;
static int gInstanceCount	=	0;
static int gInstanceMem	=	0;

const	char *getGlobalString(const	char *str)
{
	if (str	== 0)
	{
		return 0;
	}
	int	l	=	(int)strlen(str);
	if (l	== 0)
	{
		return 0;
	}

	if (gRoot)
	{
		StringList *s	=	gRoot;

		while	(s)
		{
			if (strcmp(s->mString, str)	== 0)
			{
				return s->mString;
			}
			s	=	s->mNextString;
		}
	}


	l	=	l	+	1+sizeof(StringList);
	gStringCount++;
	gStringMem +=	l;

	char *ret	=	new	char[l];

	StringList *s	=	(StringList*)ret;
	s++;
	ret	=	(char*)s;
	s--;

	s->mNextString = gRoot;
	s->mString = ret;
	gRoot	=	s;

	strcpy(ret,	str);

	return ret;
}

void releaseGlobalStrings(void)
{
	StringList *s	=	gRoot;
	while	(s)
	{
		StringList *next = s->mNextString;
		char *c	=	(char*)s;
		delete c;
		s	=	next;
	}
	gRoot	=	0;
	gStringCount = 0;
	gStringMem = 0;
}


void *findInstance(const char	*str)	// find	a	previously created instance	of an	entity by	name.	null if	not	found.
{
	void *ret	=	0;

	InstanceList *s	=	gInstanceRoot;

	while	(s)
	{
		if (strcmp(s->mName, str)	== 0)
		{
			ret	=	s->mInstance;
			break;
		}
		s	=	s->mNextInstance;
	}

	return ret;
}

void setInstance(const char	*str,	void *instance)	// set a named association with	this instance.
{
	void *mem	=	findInstance(str);

	assert(mem ==	0);	// this	should never happen?

	if (mem	== 0)
	{
		str	=	getGlobalString(str);	// convert it	to a global	string.

		gInstanceCount++;
		gInstanceMem +=	sizeof(InstanceList);

		InstanceList *il = new InstanceList;

		il->mNextInstance	=	gInstanceRoot;
		gInstanceRoot	=	il;
		il->mName	=	str;
		il->mInstance	=	instance;
	}

}

void releaseGlobalInstances(void)	// release the global	instance table
{
	InstanceList *s	=	gInstanceRoot;

	while	(s)
	{
		InstanceList *next = s->mNextInstance;
		char *c	=	(char*)s;
		delete c;
		s	=	next;
	}

	gInstanceRoot	=	0;
	gInstanceCount = 0;
	gInstanceMem = 0;
}



void reportError(const char *fmt, ... )
{
	if ( gErrorReport )
	{
		char wbuff[2048];
		va_list ap;
		va_start(ap, fmt);
		vsprintf(wbuff, fmt, ap);
		va_end(ap);
		gErrorReport->NXU_errorMessage(true,wbuff);
	}
}

void reportWarning(const char *fmt, ... )
{
	if ( gErrorReport )
	{
		char wbuff[2048];
		va_list ap;
		va_start(ap, fmt);
		vsprintf(wbuff, fmt, ap);
		va_end(ap);
		gErrorReport->NXU_errorMessage(false,wbuff);
	}
}


};
