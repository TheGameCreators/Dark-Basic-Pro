#ifndef __PPCGEKKO__

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include "NXU_File.h"
#include "NXU_string.h"

#ifdef _MSC_VER
#pragma warning(disable:4996) // Disabling stupid .NET deprecated warning.
#endif

namespace NXU
{

#define DEFAULT_BUFFER_SIZE 1000000
#define DEFAULT_GROW_SIZE   2000000


class NXU_FILE
{
public:
	NXU_FILE(const char *fname,const char *spec,void *mem,int len)
	{
		mMyAlloc = false;
		mRead = true; // default is read access.	
		mFph = 0;
		mData = (char *) mem;
		mLen  = len;
		mLoc  = 0;

		if ( spec && strcasecmp(spec,"wmem") == 0 )
		{
			mRead = false;
			if ( mem == 0 || len == 0 )
			{
				mData = new char[DEFAULT_BUFFER_SIZE];
				mLen  = DEFAULT_BUFFER_SIZE;
				mMyAlloc = true;
			}
		}
		if ( mData == 0 )
		{
			mFph = fopen(fname,spec);
		}

  	strncpy(mName,fname,512);
	}

  ~NXU_FILE(void)
  {
  	if ( mMyAlloc )
  	{
  		delete mData;
  	}
  	if ( mFph )
  	{
  		fclose(mFph);
  	}
  }

  size_t read(char *data,size_t size)
  {
  	int ret = 0;
  	if ( (mLoc+size) <= mLen )
  	{
  		memcpy(data, &mData[mLoc], size );
  		mLoc+=size;
  		ret = 1;
  	}
    return ret;
  }

  size_t write(const char *data,size_t size)
  {
  	size_t ret = 0;

		if ( (mLoc+size) >= mLen && mMyAlloc ) // grow it
		{
			size_t newLen = mLen+DEFAULT_GROW_SIZE;
			if ( size > newLen ) newLen = size+DEFAULT_GROW_SIZE;

			char *data = new char[newLen];
			memcpy(data,mData,mLoc);
			delete mData;
			mData = data;
			mLen  = newLen;
		}

  	if ( (mLoc+size) <= mLen )
  	{
  		memcpy(&mData[mLoc],data,size);
  		mLoc+=size;
  		ret = 1;
  	}
  	return ret;
  }

	size_t read(void *buffer,size_t size,int count)
	{
		size_t ret = 0;
		if ( mFph )
		{
			ret = (size_t) fread(buffer,size,count,mFph);
		}
		else
		{
			char *data = (char *)buffer;
			for (int i=0; i<count; i++)
			{
				if ( (mLoc+size) <= mLen )
				{
					read(data,size);
					data+=size;
					ret++;
				}
				else
				{
					break;
				}
			}
		}
		return ret;
	}

  size_t write(const void *buffer,size_t size,int count)
  {
  	size_t ret = 0;
  	if ( mFph )
  	{
  		ret = (int)fwrite(buffer,size,count,mFph);
  	}
  	else
  	{
  		const char *data = (const char *)buffer;

  		for (int i=0; i<count; i++)
  		{
    		if ( write(data,size) )
				{
    			data+=size;
    			ret++;
    		}
    		else
    		{
    			break;
    		}
  		}
  	}
  	return ret;
  }

  size_t writeString(const char *str)
  {
  	size_t ret = 0;
  	if ( str )
  	{
  		size_t len = strlen(str);
  		ret = write(str,len, 1 );
  	}
  	return ret;
  }


  int  flush(void)
  {
  	int ret = 0;
  	if ( mFph )
  	{
  		ret = fflush(mFph);
  	}
  	return ret;
  }


  size_t seek(size_t loc,int mode)
  {
  	size_t ret = 0;
  	if ( mFph )
  	{
		assert(loc < 0xffffffff); // should use different fseek for 64 bit systems (_fseeki64 ?) 
  		ret = (size_t)fseek(mFph,(long)loc,mode);
  	}
  	else
  	{
  		if ( mode == SEEK_SET )
  		{
  			if ( loc <= mLen )
  			{
  				mLoc = loc;
  				ret = 1;
  			}
  		}
  		else if ( mode == SEEK_END )
  		{
  			mLoc = mLen;
  		}
  		else
  		{
  			assert(0);
  		}
  	}
  	return ret;
  }

  size_t tell(void)
  {
  	size_t ret = 0;
  	if ( mFph )
  	{
  		ret = ftell(mFph);
  	}
  	else
  	{
  		ret = mLoc;
  	}
  	return ret;
  }

  size_t myputc(char c)
  {
  	size_t ret = 0;
  	if ( mFph )
  	{
  		ret = fputc(c,mFph);
  	}
  	else
  	{
  		ret = write(&c,1);
  	}
  	return ret;
  }

  int eof(void)
  {
  	int ret = 0;
  	if ( mFph )
  	{
  		ret = feof(mFph);
  	}
  	else
  	{
  		if ( mLoc >= mLen )
  			ret = 1;
  	}
  	return ret;
  }

  int  error(void)
  {
  	int ret = 0;
  	if ( mFph )
  	{
  		ret = ferror(mFph);
  	}
  	return ret;
  }


  FILE 	*mFph;
  char  *mData;
  size_t mLen;
  size_t mLoc;
  bool   mRead;
  char   mName[512];
  bool   mMyAlloc;

};

NXU_FILE * nxu_fopen(const char *fname,const char *spec,void *mem,int len)
{
	NXU_FILE *ret = 0;

	ret = new NXU_FILE(fname,spec,mem,len);

	if ( mem == 0 && ret->mData == 0)
  { 
  	if ( ret->mFph == 0 )
  	{
  		delete ret;
  		ret = 0;
  	}
  }

	return ret;
}

void       nxu_fclose(NXU_FILE *file)
{
	delete file;
}

size_t     nxu_fread(void *buffer,size_t size,int count,NXU_FILE *fph)
{
	size_t ret = 0;
	if ( fph )
	{
		ret = fph->read(buffer,size,count);
	}
	return ret;
}

size_t     nxu_fwrite(const void *buffer,size_t size,int count,NXU_FILE *fph)
{
	size_t ret = 0;
	if ( fph )
	{
		ret = fph->write(buffer,size,count);
	}
	return ret;
}

size_t      nxu_fprintf(NXU_FILE *fph,const char *fmt,...)
{
	size_t ret = 0;

	char buffer[2048];

    va_list ap;
	va_start(ap, fmt);
	vsprintf(buffer, fmt, ap);
	va_end(ap);

	if ( fph )
	{
		ret = fph->writeString(buffer);
	}

	return ret;
}


int        nxu_fflush(NXU_FILE *fph)
{
	int ret = 0;
	if ( fph )
	{
		ret = fph->flush();
	}
	return ret;
}


size_t     nxu_fseek(NXU_FILE *fph,long loc,int mode)
{
	size_t ret = 0;
	if ( fph )
	{
		ret = fph->seek(loc,mode);
	}
	return ret;
}

size_t     nxu_ftell(NXU_FILE *fph)
{
	size_t ret = 0;
	if ( fph )
	{
		ret = fph->tell();
	}
	return ret;
}

size_t     nxu_fputc(char c,NXU_FILE *fph)
{
	size_t ret = 0;
	if ( fph )
	{
		ret = fph->myputc(c);
	}
	return ret;
}

size_t     nxu_fputs(const char *str,NXU_FILE *fph)
{
	size_t ret = 0;
	if ( fph )
	{
		ret = fph->writeString(str);
	}
	return ret;
}

int        nxu_feof(NXU_FILE *fph)
{
	int ret = 0;
	if ( fph )
	{
		ret = fph->eof();
	}
	return ret;
}

int        nxu_ferror(NXU_FILE *fph)
{
	int ret = 0;
	if ( fph )
	{
		ret = fph->error();
	}
	return ret;
}

void *     nxu_getMemBuffer(NXU_FILE *fph,size_t &outputLength)
{
	outputLength = 0;
	void * ret = 0;
	if ( fph )
	{
		ret = fph->mData;
		outputLength = fph->mLoc;
		fph->mMyAlloc = false;
	}
	return ret;
}

}; // end of NXU namespace

#endif // !__PPCGEKKO__
