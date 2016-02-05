#ifdef WIN32 
	#define NOMINMAX
	#include <windows.h>
#elif LINUX
	#include "string.h"
#endif

#include <stdio.h>
#include "NxPhysics.h"
#include "UserAllocator.h"

#define MEMBLOCKSTART 64
static const size_t g_pointerMarker = (size_t)0xbadbad00deadbabeLL;


UserAllocator::UserAllocator() : mNbAllocatedBytes(0), mHighWaterMark(0), mTotalNbAllocs(0), mNbAllocs(0), mNbReallocs(0)
{
	//we need a lock so that block list handling etc is thread safe.

#if defined(WIN32) || defined(_XBOX)
	InitializeCriticalSection(&allocatorLock);
#endif

	mMemBlockList = NULL;

#ifdef _DEBUG
	//Initialize the Memory blocks list (DEBUG mode only)
	mMemBlockList = (size_t*)::malloc(MEMBLOCKSTART*sizeof(size_t));
	memset(mMemBlockList, 0, MEMBLOCKSTART*sizeof(size_t));
	mMemBlockListSize	= MEMBLOCKSTART;
	mMemBlockFirstFree	= 0;
	mMemBlockUsed		= 0;
#endif
}

UserAllocator::~UserAllocator()
{
	if(mNbAllocatedBytes)	printf("Memory leak detected: %d bytes non released\n", mNbAllocatedBytes);
	if(mNbAllocs)			printf("Remaining allocs: %d\n", mNbAllocs);
	printf("Nb alloc: %d\n", mTotalNbAllocs);
	printf("Nb realloc: %d\n", mNbReallocs);
	printf("High water mark: %d Kb\n", mHighWaterMark/1024);

#ifdef _DEBUG
	// Scanning for memory leaks
	if(mMemBlockList && mMemBlockUsed)
	{
		NxU32 NbLeaks = 0;
		printf("\n\n  ICE Message Memory leaks detected :\n\n");
		NxU32 used = mMemBlockUsed;
		for(NxU32 i=0, j=0; i<used; i++, j++)
			{
			while(!mMemBlockList[j]) j++;
			size_t* cur = (size_t*)mMemBlockList[j];
			printf(" Address 0x%.8X, %d bytes (%s), allocated in: %s(%d):\n\n", cur+6, cur[1], (const char*)cur[5], (const char*)cur[2], cur[3]);
			NbLeaks++;
		//			Free(cur+4);
			}
		printf("\n  Dump complete (%d leaks)\n\n", NbLeaks);
	}
	
	// Free the Memory Block list
	::free(mMemBlockList);
	mMemBlockList = NULL;
#endif

#if defined(WIN32) || defined(_XBOX)
	DeleteCriticalSection(&allocatorLock);
#endif
}

void UserAllocator::reset()
{
	LockAlloc();

	mNbAllocatedBytes	= 0;
	mHighWaterMark		= 0;
	mNbAllocs			= 0;

	UnlockAlloc();
}

void* UserAllocator::malloc(size_t size)
{
	printf("Obsolete code called!\n");
	return NULL;
}

void* UserAllocator::malloc(size_t size, NxMemoryType type)
{
#ifdef _DEBUG
	return mallocDEBUG(size, NULL, 0, "Undefined", type);
#endif

	if(size == 0)
	{
		printf("Warning: trying to allocate 0 bytes\n");
		return NULL;
	}

	LockAlloc();

	void* ptr = (void*)::malloc(size+2*sizeof(size_t));
	mTotalNbAllocs++;
	mNbAllocs++;

	size_t* blockStart = (size_t*)ptr;
	blockStart[0] = g_pointerMarker;
	blockStart[1] = size;

	mNbAllocatedBytes+=size;
	if(mNbAllocatedBytes>mHighWaterMark) mHighWaterMark = mNbAllocatedBytes;

	UnlockAlloc();

	return ((size_t*)ptr)+2;
}

void* UserAllocator::mallocDEBUG(size_t size, const char* file, int line)
{
	printf("Obsolete code called!\n");
	return NULL;
}

void* UserAllocator::mallocDEBUG(size_t size, const char* file, int line, const char* className, NxMemoryType type)
{
#ifndef _DEBUG
	return malloc(size, type);
#endif
	if(size == 0)
	{
		printf("Warning: trying to allocate 0 bytes\n");
		return NULL;
	}

	LockAlloc();

	void* ptr = (void*)::malloc(size+6*sizeof(size_t));
	mTotalNbAllocs++;
	mNbAllocs++;

	size_t* blockStart = (size_t*)ptr;
	blockStart[0] = g_pointerMarker;
	blockStart[1] = size;
	blockStart[2] = (size_t)file;
	blockStart[3] = line;

	mNbAllocatedBytes+=size;
	if(mNbAllocatedBytes>mHighWaterMark) mHighWaterMark = mNbAllocatedBytes;

	// Insert the allocated block in the debug memory block list
	if(mMemBlockList)
	{
		
		mMemBlockList[mMemBlockFirstFree] = (size_t)ptr;
		blockStart[4] = mMemBlockFirstFree;
		mMemBlockUsed++;
		
		if(mMemBlockUsed==mMemBlockListSize)
		{
			size_t* tps = (size_t*)::malloc((mMemBlockListSize+MEMBLOCKSTART)*sizeof(size_t));
			memcpy(tps, mMemBlockList, mMemBlockListSize*sizeof(size_t));
			memset((tps+mMemBlockListSize), 0, MEMBLOCKSTART*sizeof(size_t));
			::free(mMemBlockList);
			mMemBlockList = tps;
			mMemBlockFirstFree = mMemBlockListSize-1;
			mMemBlockListSize += MEMBLOCKSTART;
		}
		
		while(mMemBlockList[++mMemBlockFirstFree] && (mMemBlockFirstFree<mMemBlockListSize));
		
		if(mMemBlockFirstFree==mMemBlockListSize)
		{
			mMemBlockFirstFree = (size_t)-1;
			while(mMemBlockList[++mMemBlockFirstFree] && (mMemBlockFirstFree<mMemBlockListSize));
		}
	}
	else
	{
		blockStart[4] = 0;
	}
	
	blockStart[5] = (size_t)className;

	UnlockAlloc();

	return ((size_t*)ptr)+6;
}

void* UserAllocator::realloc(void* memory, size_t size)
	{
//	return ::realloc(memory, size);

	if(memory == NULL)
	{
		printf("Warning: trying to realloc null pointer\n");
		return NULL;
	}

	if(size == 0)
	{
		printf("Warning: trying to realloc 0 bytes\n");
	}

#ifdef _DEBUG

	LockAlloc();

	size_t* ptr = ((size_t*)memory)-6;
	if(ptr[0]!=g_pointerMarker)
		{
		printf("Error: realloc unknown memory!!\n");
		}
	mNbAllocatedBytes -= ptr[1];
	mNbAllocatedBytes += size;

	if(mNbAllocatedBytes>mHighWaterMark)	mHighWaterMark = mNbAllocatedBytes;

	void* ptr2 = ::realloc(ptr, size+6*sizeof(size_t));
	mNbReallocs++;
	*(((size_t*)ptr2)+1) = size;
	if(ptr==ptr2)
		{
		UnlockAlloc();
		return memory;
		}

	*(((size_t*)ptr2)) = g_pointerMarker;
	*(((size_t*)ptr2)+1) = size;
	*(((size_t*)ptr2)+2) = 0;	// File
	*(((size_t*)ptr2)+3) = 0;	// Line

	NxU32* blockStart = (NxU32*)ptr2;

	// Insert the allocated block in the debug memory block list
	if(mMemBlockList)
	{
		mMemBlockList[mMemBlockFirstFree] = (size_t)ptr;
		blockStart[4] = mMemBlockFirstFree;
		mMemBlockUsed++;
		if(mMemBlockUsed==mMemBlockListSize)
		{
			size_t* tps = (size_t*)::malloc((mMemBlockListSize+MEMBLOCKSTART)*sizeof(size_t));
			memcpy(tps, mMemBlockList, mMemBlockListSize*sizeof(size_t));
			memset((tps+mMemBlockListSize), 0, MEMBLOCKSTART*sizeof(size_t));
			::free(mMemBlockList);
			mMemBlockList = tps;
			mMemBlockFirstFree = mMemBlockListSize-1;
			mMemBlockListSize += MEMBLOCKSTART;
		}
		
		while(mMemBlockList[++mMemBlockFirstFree] && (mMemBlockFirstFree<mMemBlockListSize));
		
		if(mMemBlockFirstFree==mMemBlockListSize)
		{
			mMemBlockFirstFree = (size_t)-1;
			while(mMemBlockList[++mMemBlockFirstFree] && (mMemBlockFirstFree<mMemBlockListSize));
		}
	}
	else
	{
		blockStart[4] = 0;
	}
	
	blockStart[5] = 0;	// Classname

	UnlockAlloc();

	return ((size_t*)ptr2)+6;

#else
	
	LockAlloc();

	size_t* ptr = ((size_t*)memory)-2;
	if(ptr[0]!=g_pointerMarker)
	{
		printf("Error: realloc unknown memory!!\n");
	}
	
	mNbAllocatedBytes -= ptr[1];
	mNbAllocatedBytes += size;

	if(mNbAllocatedBytes>mHighWaterMark)	mHighWaterMark = mNbAllocatedBytes;

	void* ptr2 = ::realloc(ptr, size+2*sizeof(size_t));
	mNbReallocs++;
	
	*(((size_t*)ptr2)+1) = size;
	if(ptr == ptr2) 
		{
		UnlockAlloc();
		return memory;
		}

	*(((size_t*)ptr2)) = g_pointerMarker;
	
	UnlockAlloc();

	return ((size_t*)ptr2)+2;
#endif

}

void UserAllocator::free(void* memory)
{
	if(memory == NULL)
	{
		printf("Warning: trying to free null pointer\n");
		return;
	}

	LockAlloc();

#ifdef _DEBUG
	
	

	size_t* ptr = ((size_t*)memory)-6;
	if(ptr[0] != g_pointerMarker)
	{
		printf("Error: free unknown memory!!\n");
	}
	mNbAllocatedBytes -= ptr[1];
	mNbAllocs--;

	const char* File = (const char*)ptr[2];
	size_t Line = ptr[3];
	size_t MemBlockFirstFree = ptr[4];

	// Remove the block from the Memory block list
	if(mMemBlockList)
	{
		mMemBlockList[MemBlockFirstFree] = 0;
		mMemBlockUsed--;
	}
#else
	size_t* ptr = ((size_t*)memory)-2;
	if(ptr[0] != g_pointerMarker)
	{
		printf("Error: free unknown memory!!\n");
	}
	
	mNbAllocatedBytes -= ptr[1];
	
	if(mNbAllocatedBytes < 0)
	{
		printf("Oops (%d)\n", ptr[1]);
	}
	
	mNbAllocs--;
#endif

	ptr[0]=g_pointerMarker;
	ptr[1]=0;
	::free(ptr);

	UnlockAlloc();
}
