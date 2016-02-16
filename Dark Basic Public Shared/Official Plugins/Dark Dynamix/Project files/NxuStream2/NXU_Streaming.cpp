#include <stdio.h>
#include "NxPhysics.h"
#include "NXU_Streaming.h"

namespace	NXU
{

UserStream::UserStream(const char	*filename, bool	load): fp(NULL)
{
	fp = nxu_fopen(filename, load	?	"rb" : "wb");
}

UserStream::~UserStream()
{
	if (fp)
	{
		nxu_fclose(fp);
	}
}

// Loading API
NxU8 UserStream::readByte()const
{
	NxU8 b;
#ifdef _DEBUG
	size_t r = nxu_fread(&b, sizeof(NxU8), 1,	fp);
	NX_ASSERT(r);
#else
	nxu_fread(&b, sizeof(NxU8), 1,	fp);
#endif
	return b;
}

NxU16	UserStream::readWord()const
{
	NxU16	w;
#ifdef _DEBUG
	size_t r = nxu_fread(&w, sizeof(NxU16),	1, fp);
	NX_ASSERT(r);
#else
	nxu_fread(&w, sizeof(NxU16),	1, fp);
#endif
	return w;
}

NxU32	UserStream::readDword()const
{
	NxU32	d;
#ifdef _DEBUG
	size_t r = nxu_fread(&d, sizeof(NxU32),	1, fp);
	NX_ASSERT(r);
#else
	nxu_fread(&d, sizeof(NxU32),	1, fp);	
#endif
	return d;
}

float	UserStream::readFloat()const
{
	NxReal f;
#ifdef _DEBUG
	size_t r = nxu_fread(&f, sizeof(NxReal), 1,	fp);
	NX_ASSERT(r);
#else
	nxu_fread(&f, sizeof(NxReal), 1,	fp);
#endif
	return f;
}

double UserStream::readDouble()const
{
	NxF64	f;
#ifdef _DEBUG
	size_t r = nxu_fread(&f, sizeof(NxF64),	1, fp);
	NX_ASSERT(r);
#else
	nxu_fread(&f, sizeof(NxF64),	1, fp);
#endif
	return f;
}

void UserStream::readBuffer(void *buffer,	NxU32	size)const
{
#ifdef _DEBUG
	size_t w = nxu_fread(buffer, size, 1,	fp);
	NX_ASSERT(w);
#else
	nxu_fread(buffer, size, 1,	fp);
#endif
}

// Saving	API
NxStream &UserStream::storeByte(NxU8 b)
{
#ifdef _DEBUG
	size_t w = nxu_fwrite(&b,	sizeof(NxU8),	1, fp);
	NX_ASSERT(w);
#else
	nxu_fwrite(&b,	sizeof(NxU8),	1, fp);
#endif
	return	*this;
}

NxStream &UserStream::storeWord(NxU16	w)
{
#ifdef _DEBUG
	size_t ww	=	nxu_fwrite(&w, sizeof(NxU16),	1, fp);
	NX_ASSERT(ww);
#else
	nxu_fwrite(&w, sizeof(NxU16),	1, fp);
#endif
	return	*this;
}

NxStream &UserStream::storeDword(NxU32 d)
{
#ifdef _DEBUG
	size_t w = nxu_fwrite(&d,	sizeof(NxU32), 1,	fp);
	NX_ASSERT(w);
#else
	nxu_fwrite(&d,	sizeof(NxU32), 1,	fp);
#endif
	return	*this;
}

NxStream &UserStream::storeFloat(NxReal	f)
{
#ifdef _DEBUG
	size_t w = nxu_fwrite(&f,	sizeof(NxReal),	1, fp);
	NX_ASSERT(w);
#else
	nxu_fwrite(&f,	sizeof(NxReal),	1, fp);
#endif
	return	*this;
}

NxStream &UserStream::storeDouble(NxF64	f)
{
#ifdef _DEBUG
	size_t w = nxu_fwrite(&f,	sizeof(NxF64), 1,	fp);
	NX_ASSERT(w);
#else
	nxu_fwrite(&f,	sizeof(NxF64), 1,	fp);
#endif
	return	*this;
}

NxStream &UserStream::storeBuffer(const	void *buffer,	NxU32	size)
{
#ifdef _DEBUG
	size_t w = nxu_fwrite(buffer,	size,	1, fp);
	NX_ASSERT(w);
#else
	nxu_fwrite(buffer,	size,	1, fp);
#endif
	return	*this;
}




MemoryWriteBuffer::MemoryWriteBuffer():	currentSize(0),	maxSize(0),	data(NULL){}

MemoryWriteBuffer::~MemoryWriteBuffer()
{
	NX_DELETE_ARRAY(data);
}

void MemoryWriteBuffer::clear()
{
	currentSize	=	0;
}

NxStream &MemoryWriteBuffer::storeByte(NxU8	b)
{
	storeBuffer(&b,	sizeof(NxU8));
	return	*this;
}
NxStream &MemoryWriteBuffer::storeWord(NxU16 w)
{
	storeBuffer(&w,	sizeof(NxU16));
	return	*this;
}
NxStream &MemoryWriteBuffer::storeDword(NxU32	d)
{
	storeBuffer(&d,	sizeof(NxU32));
	return	*this;
}
NxStream &MemoryWriteBuffer::storeFloat(NxReal f)
{
	storeBuffer(&f,	sizeof(NxReal));
	return	*this;
}
NxStream &MemoryWriteBuffer::storeDouble(NxF64 f)
{
	storeBuffer(&f,	sizeof(NxF64));
	return	*this;
}
NxStream &MemoryWriteBuffer::storeBuffer(const void	*buffer, NxU32 size)
{
	NxU32	expectedSize = currentSize + size;
	if (expectedSize > maxSize)
	{
		maxSize	=	expectedSize + 4096;

		NxU8 *newData	=	new	NxU8[maxSize];
		NX_ASSERT(newData	!= NULL);

		if (data)
		{
			memcpy(newData,	data,	currentSize);
			delete []	data;
		}
		data = newData;
	}
	memcpy(data	+	currentSize, buffer, size);
	currentSize	+= size;
	return	*this;
}


MemoryReadBuffer::MemoryReadBuffer(const NxU8	*data):	buffer(data){}

MemoryReadBuffer::~MemoryReadBuffer()
{
	// We	don't	own	the	data =>	no delete
}

NxU8 MemoryReadBuffer::readByte()const
{
	NxU8 b;
	memcpy(&b, buffer, sizeof(NxU8));
	buffer +=	sizeof(NxU8);
	return b;
}

NxU16	MemoryReadBuffer::readWord()const
{
	NxU16	w;
	memcpy(&w, buffer, sizeof(NxU16));
	buffer +=	sizeof(NxU16);
	return w;
}

NxU32	MemoryReadBuffer::readDword()const
{
	NxU32	d;
	memcpy(&d, buffer, sizeof(NxU32));
	buffer +=	sizeof(NxU32);
	return d;
}

float	MemoryReadBuffer::readFloat()const
{
	float	f;
	memcpy(&f, buffer, sizeof(float));
	buffer +=	sizeof(float);
	return f;
}

double MemoryReadBuffer::readDouble()const
{
	double f;
	memcpy(&f, buffer, sizeof(double));
	buffer +=	sizeof(double);
	return f;
}

void MemoryReadBuffer::readBuffer(void *dest,	NxU32	size)const
{
	memcpy(dest, buffer, size);
	buffer +=	size;
}

ImportMemoryWriteBuffer::ImportMemoryWriteBuffer():	currentSize(0),	maxSize(0),	data(NULL){}

ImportMemoryWriteBuffer::~ImportMemoryWriteBuffer()
{
	if (data !=	NULL)
	{
		delete []	data;
		data = NULL;
	}
}

void ImportMemoryWriteBuffer::clear()
{
	currentSize	=	0;
}

NxStream &ImportMemoryWriteBuffer::storeByte(NxU8	b)
{
	storeBuffer(&b,	sizeof(NxU8));
	return	*this;
}

NxStream &ImportMemoryWriteBuffer::storeWord(NxU16 w)
{
	storeBuffer(&w,	sizeof(NxU16));
	return	*this;
}

NxStream &ImportMemoryWriteBuffer::storeDword(NxU32	d)
{
	storeBuffer(&d,	sizeof(NxU32));
	return	*this;
}

NxStream &ImportMemoryWriteBuffer::storeFloat(NxReal f)
{
	storeBuffer(&f,	sizeof(NxReal));
	return	*this;
}

NxStream &ImportMemoryWriteBuffer::storeDouble(NxF64 f)
{
	storeBuffer(&f,	sizeof(NxF64));
	return	*this;
}

NxStream &ImportMemoryWriteBuffer::storeBuffer(const void	*buffer, NxU32 size)
{
	NxU32	expectedSize = currentSize + size;
	if (expectedSize > maxSize)
	{
		maxSize	=	expectedSize + 4096;
		NxU8 *newData	=	new	NxU8[maxSize];
		NX_ASSERT(newData	!= NULL);

		if (data)
		{
			memcpy(newData,	data,	currentSize);
			delete []	data;
		}
		data = newData;
	}
	memcpy(data	+	currentSize, buffer, size);
	currentSize	+= size;
	return	*this;
}


ImportMemoryReadBuffer::ImportMemoryReadBuffer(const NxU8	*data):	buffer(data){}

ImportMemoryReadBuffer::~ImportMemoryReadBuffer()
{
	// We	don't	own	the	data =>	no delete
}

NxU8 ImportMemoryReadBuffer::readByte()const
{
	NxU8 b;
	memcpy(&b, buffer, sizeof(NxU8));
	buffer +=	sizeof(NxU8);
	return b;
}

NxU16	ImportMemoryReadBuffer::readWord()const
{
	NxU16	w;
	memcpy(&w, buffer, sizeof(NxU16));
	buffer +=	sizeof(NxU16);
	return w;
}

NxU32	ImportMemoryReadBuffer::readDword()const
{
	NxU32	d;
	memcpy(&d, buffer, sizeof(NxU32));
	buffer +=	sizeof(NxU32);
	return d;
}

float	ImportMemoryReadBuffer::readFloat()const
{
	float	f;
	memcpy(&f, buffer, sizeof(float));
	buffer +=	sizeof(float);
	return f;
}

double ImportMemoryReadBuffer::readDouble()const
{
	double f;
	memcpy(&f, buffer, sizeof(double));
	buffer +=	sizeof(double);
	return f;
}

void ImportMemoryReadBuffer::readBuffer(void *dest,	NxU32	size)const
{
	memcpy(dest, buffer, size);
	buffer +=	size;
}



};
