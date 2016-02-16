#include "UserAllocator.h"
#include <stdio.h>                                  //printf

//UserAllocator::UserAllocator(void)
//{
//}
//
//UserAllocator::~UserAllocator(void)
//{
//}



#define MEMBLOCKSTART		64

UserAllocator::UserAllocator() : mNbAllocatedBytes(0), mHighWaterMark(0), mTotalNbAllocs(0), mNbAllocs(0), mNbReallocs(0)
{
    mMemBlockList = NULL;

#ifdef _DEBUG
    // Initialize the Memory blocks list (DEBUG mode only)
    mMemBlockList = (NxU32*)::malloc(MEMBLOCKSTART*4);
    memset(mMemBlockList, 0, MEMBLOCKSTART*4);
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
            NxU32* cur = (NxU32*)mMemBlockList[j];
            printf(" Address 0x%.8X, %d bytes (%s), allocated in: %s(%d):\n\n", cur+6, cur[1], (const char*)cur[5], (const char*)cur[2], cur[3]);
            NbLeaks++;
            // Free(cur+4);
        }
        printf("\n  Dump complete (%d leaks)\n\n", NbLeaks);
    }
    // Free the Memory Block list
    ::free(mMemBlockList);
    mMemBlockList = NULL;
#endif
}

void UserAllocator::reset()
{
    mNbAllocatedBytes	= 0;
    mHighWaterMark		= 0;
    mNbAllocs			= 0;
}

void* UserAllocator::malloc(NxU32 size)
{
#ifdef _DEBUG
    return mallocDEBUG(size, NULL, 0, "Undefined", NX_MEMORY_PERSISTENT);
#endif

    if(!size)
    {
        printf("Warning: trying to allocate 0 bytes\n");
        return NULL;
    }

    void* ptr = (void*)::malloc(size+8);
    mTotalNbAllocs++;
    mNbAllocs++;

    NxU32* blockStart = (NxU32*)ptr;
    blockStart[0] = 0xDeadBabe;
    blockStart[1] = size;

    mNbAllocatedBytes+=size;
    if(mNbAllocatedBytes>mHighWaterMark)	mHighWaterMark = mNbAllocatedBytes;

    return ((NxU32*)ptr)+2;
#if 0
    printf("Obsolete code called!\n");
    return NULL;
#endif  // 0
}

void* UserAllocator::malloc(NxU32 size, NxMemoryType type)
{
// return ::malloc(size);

#ifdef _DEBUG
    return mallocDEBUG(size, NULL, 0, "Undefined", type);
#endif

    if(!size)
    {
        printf("Warning: trying to allocate 0 bytes\n");
        return NULL;
    }

    void* ptr = (void*)::malloc(size+8);
    mTotalNbAllocs++;
    mNbAllocs++;

    NxU32* blockStart = (NxU32*)ptr;
    blockStart[0] = 0xDeadBabe;
    blockStart[1] = size;

    mNbAllocatedBytes+=size;
    if(mNbAllocatedBytes>mHighWaterMark)	mHighWaterMark = mNbAllocatedBytes;

    return ((NxU32*)ptr)+2;
}

void* UserAllocator::mallocDEBUG(NxU32 size, const char* file, int line)
{
    printf("Obsolete code called!\n");
    return NULL;
}

void* UserAllocator::mallocDEBUG(NxU32 size, const char* file, int line, const char* className, NxMemoryType type)
{

#ifdef _DEBUG

    if(!size)
    {
        printf("Warning: trying to allocate 0 bytes\n");
        return NULL;
    }

    void* ptr = (void*)::malloc(size+24);
    mTotalNbAllocs++;
    mNbAllocs++;

    NxU32* blockStart = (NxU32*)ptr;
    blockStart[0] = 0xDeadBabe;
    blockStart[1] = size;
    blockStart[2] = (NxU32)file;
    blockStart[3] = line;

    mNbAllocatedBytes+=size;
    if(mNbAllocatedBytes>mHighWaterMark)	mHighWaterMark = mNbAllocatedBytes;

    // Insert the allocated block in the debug memory block list
    if(mMemBlockList)
    {
        mMemBlockList[mMemBlockFirstFree] = (NxU32)ptr;
        blockStart[4] = mMemBlockFirstFree;
        mMemBlockUsed++;
        if(mMemBlockUsed==mMemBlockListSize)
        {
            NxU32* tps = (NxU32*)::malloc((mMemBlockListSize+MEMBLOCKSTART)*4);
            memcpy(tps, mMemBlockList, mMemBlockListSize*4);
            memset((tps+mMemBlockListSize), 0, MEMBLOCKSTART*4);
            ::free(mMemBlockList);
            mMemBlockList = tps;
            mMemBlockFirstFree = mMemBlockListSize-1;
            mMemBlockListSize += MEMBLOCKSTART;
        }
        while (mMemBlockList[++mMemBlockFirstFree] && (mMemBlockFirstFree<mMemBlockListSize));
        if(mMemBlockFirstFree==mMemBlockListSize)
        {
            mMemBlockFirstFree = (NxU32)-1;
            while(mMemBlockList[++mMemBlockFirstFree] && (mMemBlockFirstFree<mMemBlockListSize));
        }
    }
    else
    {
        blockStart[4] = 0;
    }
    blockStart[5] = (NxU32)className;

    return ((NxU32*)ptr)+6;
#else

//     printf("Error: mallocDEBUG has been called in release!\n");
//     NX_ASSERT(0);//Don't use debug malloc for release mode code!
//     return 0;
		
    return malloc(size, type);

#endif
}

void* UserAllocator::realloc(void* memory, NxU32 size)
{
//     return ::realloc(memory, size);

    if(!memory)
    {
        printf("Warning: trying to realloc null pointer\n");
        return NULL;
    }

    if(!size)
    {
        printf("Warning: trying to realloc 0 bytes\n");
    }

#ifdef _DEBUG

    NxU32* ptr = ((NxU32*)memory)-6;
    if(ptr[0]!=0xDeadBabe)
    {
        printf("Error: realloc unknown memory!!\n");
    }
    mNbAllocatedBytes -= ptr[1];
    mNbAllocatedBytes += size;

    if(mNbAllocatedBytes>mHighWaterMark)	mHighWaterMark = mNbAllocatedBytes;

    void* ptr2 = ::realloc(ptr, size+24);
    mNbReallocs++;
    *(((NxU32*)ptr2)+1) = size;
    if(ptr==ptr2)
    {
        return memory;
    }

    *(((NxU32*)ptr2)) = 0xDeadBabe;
    *(((NxU32*)ptr2)+1) = size;
    *(((NxU32*)ptr2)+2) = 0;	// File
    *(((NxU32*)ptr2)+3) = 0;	// Line

    NxU32* blockStart = (NxU32*)ptr2;

    // Insert the allocated block in the debug memory block list
    if(mMemBlockList)
    {
        mMemBlockList[mMemBlockFirstFree] = (NxU32)ptr;
        blockStart[4] = mMemBlockFirstFree;
        mMemBlockUsed++;
        if(mMemBlockUsed==mMemBlockListSize)
        {
            NxU32* tps = (NxU32*)::malloc((mMemBlockListSize+MEMBLOCKSTART)*4);
            memcpy(tps, mMemBlockList, mMemBlockListSize*4);
            memset((tps+mMemBlockListSize), 0, MEMBLOCKSTART*4);
            ::free(mMemBlockList);
            mMemBlockList = tps;
            mMemBlockFirstFree = mMemBlockListSize-1;
            mMemBlockListSize += MEMBLOCKSTART;
        }
        while (mMemBlockList[++mMemBlockFirstFree] && (mMemBlockFirstFree<mMemBlockListSize));
        if(mMemBlockFirstFree==mMemBlockListSize)
        {
            mMemBlockFirstFree = (NxU32)-1;
            while(mMemBlockList[++mMemBlockFirstFree] && (mMemBlockFirstFree<mMemBlockListSize));
        }
    }
    else
    {
        blockStart[4] = 0;
    }
    blockStart[5] = 0;	// Classname

    return ((NxU32*)ptr2)+6;

#else
    NxU32* ptr = ((NxU32*)memory)-2;
    if(ptr[0]!=0xDeadBabe)
    {
        printf("Error: realloc unknown memory!!\n");
    }
    mNbAllocatedBytes -= ptr[1];
    mNbAllocatedBytes += size;

    if(mNbAllocatedBytes>mHighWaterMark)	mHighWaterMark = mNbAllocatedBytes;

    void* ptr2 = ::realloc(ptr, size+8);
    mNbReallocs++;
    *(((NxU32*)ptr2)+1) = size;
    if(ptr==ptr2)	return memory;

    *(((NxU32*)ptr2)) = 0xDeadBabe;
    return ((NxU32*)ptr2)+2;
#endif
}

void UserAllocator::free(void* memory)
{
//     ::free(memory);	return;

    if(!memory)
    {
        printf("Warning: trying to free null pointer\n");
        return;
    }

#ifdef _DEBUG
    NxU32* ptr = ((NxU32*)memory)-6;
    if(ptr[0]!=0xDeadBabe)
    {
        printf("Error: free unknown memory!!\n");
    }
    mNbAllocatedBytes -= ptr[1];
    mNbAllocs--;

    NxU32 MemBlockFirstFree = ptr[4];
    NxU32 Line = ptr[3];
    const char* File = (const char*)ptr[2];

    // Remove the block from the Memory block list
    if(mMemBlockList)
    {
        mMemBlockList[MemBlockFirstFree] = 0;
        mMemBlockUsed--;
    }

    ptr[0]=0xDeadDead;
    ptr[1]=0;
    ::free(ptr);
#else
    NxU32* ptr = ((NxU32*)memory)-2;
    if(ptr[0]!=0xDeadBabe)
    {
        printf("Error: free unknown memory!!\n");
    }
    mNbAllocatedBytes -= ptr[1];
    if(mNbAllocatedBytes<0)
    {
        printf("Oops (%d)\n", ptr[1]);
    }
    mNbAllocs--;
    ptr[0]=0xDeadDead;
    ptr[1]=0;
    ::free(ptr);
#endif
}