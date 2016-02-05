#ifndef DBPROPHYSICS_SMARTPTR_H
#define DBPROPHYSICS_SMARTPTR_H

//A smart pointer template

//Also includes a central registry (only if _DEBUG is defined)
//which the smart pointer checks on its construction to make
//sure it's not creating a reference to the same object as 
//another pointer.
//(which would mean the object would get deleted twice or more)

#include "stdafx.h"

#ifdef _DEBUG

class _SmartPtrRegister
{
public:
	static _SmartPtrRegister& SmartPtrRegister()
	{
		static _SmartPtrRegister inst;
		return inst;
	}

	~_SmartPtrRegister()
	{
		if(num>0)
		{
			OutputDebugString("SmartPtrRegister :: References still in existence!\n");
			for(unsigned int i=0;i<num;i++)
			{
				delete ptrList[i];
				ptrList[i]=0;
			}
		}

		delete[] ptrList;
		ptrList=0;
	}

	void Add(void * ptr,int * refCount)
	{		
		if(ptr==0) return;

		unsigned int pos;
		if(Find(&pos,ptr,refCount))
		{
			OutputDebugString("SmartPtrRegister :: Pointer already exists!\n");
			throw "Pointer already exists!";
		}
		if(num==size)
		{
			PtrRecord ** temp = new PtrRecord*[size*2];			
			memcpy(temp,ptrList,size*sizeof(PtrRecord*));
			delete[] ptrList;			
			ptrList = temp;
			size*=2;
		}
		ptrList[num]=new PtrRecord;
		ptrList[num]->ptr=ptr;
		ptrList[num]->refCount=refCount;
		num++;
	}

	void Remove(void * ptr,int * refCount)
	{
		if(ptr==0) return;

		unsigned int pos;
		bool result = Find(&pos,ptr,refCount);
		if(result)
		{
			assert((*(ptrList[pos]->refCount))==0);
			delete ptrList[pos];
			ptrList[pos]=0;
			memmove(&(ptrList[pos]),&(ptrList[pos+1]),(num-pos-1)*sizeof(PtrRecord*));
			num--;
		}
		else
		{
			OutputDebugString("SmartPtrRegister :: Couldn't find ptr to remove\n");
			throw "Couldn't find ptr to remove";
		}		
	}

	bool Find(unsigned int * loc,void * ptr,int * refCount)
	{
		for(unsigned int i=0;i<num;i++)
		{
			if(ptrList[i]->ptr==ptr)
			{
				assert(ptrList[i]->refCount=refCount);
				(*loc)=i;
				return true;
			}
		}
		return false;
	}

private:
	_SmartPtrRegister():size(2048),ptrList(0)
	{		
		ptrList = new PtrRecord*[size];		
	}

	struct PtrRecord
	{
		PtrRecord():ptr(0),refCount(0){}
		void * ptr;
		int * refCount;
	};


	unsigned int size;
	unsigned int num;
	PtrRecord ** ptrList;	
};

#define PTRREGISTER_ADD(ptr,ref) _SmartPtrRegister::SmartPtrRegister().Add(ptr,ref)
#define PTRREGISTER_REMOVE(ptr,ref) _SmartPtrRegister::SmartPtrRegister().Remove(ptr,ref)

#else

#define PTRREGISTER_ADD(ptr,ref)
#define PTRREGISTER_REMOVE(ptr,ref)

#endif


template <class T> class SmartPtr
{

public:	
	friend class SmartPtr; //Needed for the member function templates to access private members	

	SmartPtr(T* ptr=0)
	{
		refCount = new int;
		instPtr=ptr;
		(*refCount)=1;

		PTRREGISTER_ADD(instPtr,refCount);
	}
	
	SmartPtr(const SmartPtr& rhs)
	{		
		instPtr=rhs.instPtr;
		refCount=rhs.refCount;
		(*refCount)++;				
	}	


	
	// mike - have to comment out
	//A generic copy constructor
	/*
	template <class newType> SmartPtr(const SmartPtr<newType>& rhs)
	{		
		instPtr=rhs.instPtr;
		refCount=rhs.refCount;
		(*refCount)++;				
	}
	*/

	~SmartPtr()
	{
		assert(refCount);
		assert((*refCount)>0);

		(*refCount)--;				
		if((*refCount)==0)
		{
			PTRREGISTER_REMOVE(instPtr,refCount);

			delete instPtr;
			instPtr=0;
			delete refCount;			
			refCount=0;
		}			
	}	

	SmartPtr& operator=(T* rhs)
	{
		assert(refCount);
		assert((*refCount)>0);
		
		(*refCount)--;
		if((*refCount)==0)
		{
			PTRREGISTER_REMOVE(instPtr,refCount);

			delete instPtr;
			instPtr=0;
			delete refCount;				
			refCount=0;
		}
		refCount=new int;		
		instPtr=rhs;
		(*refCount)=1;

		PTRREGISTER_ADD(instPtr,refCount);
		return *this;
	}

	SmartPtr& operator=(SmartPtr& rhs)
	{
		assert(refCount);
		assert((*refCount)>0);
		
		if(instPtr==rhs.instPtr) return *this;

		(*refCount)--;
		if((*refCount)==0)
		{
			PTRREGISTER_REMOVE(instPtr,refCount);

			delete refCount;
			refCount=0;
			delete instPtr;				
			instPtr=0;
		}
		instPtr=rhs.instPtr;
		refCount=rhs.refCount;
		(*refCount)++;
		return *this;
	}

	
	// mike - have to comment out
	/*
	template <class newType> SmartPtr& operator=(SmartPtr<newType>& rhs)
	{
		assert(refCount);
		assert((*refCount)>0);
		
		//Are we pointing at the same thing?
		if(instPtr==rhs.instPtr) return *this;

		(*refCount)--;
		if((*refCount)==0)
		{
			PTRREGISTER_REMOVE(instPtr,refCount);

			delete refCount;
			refCount=0;
			delete instPtr;				
			instPtr=0;
		}
		instPtr=rhs.instPtr;
		refCount=rhs.refCount;
		(*refCount)++;		
		return *this;		
	}
	*/


	T* operator->() const
	{
		assert(refCount);
		assert((*refCount)>0);
		return instPtr;
	}

	T& operator*() const
	{
		assert(refCount);
		assert((*refCount)>0);
		return *instPtr;
	}

	//Safest way to check for equality without allowing
	//someone to compare pointers of different types.
	//(unfortunately it will also stop smart pointers of
	//derived classes being compared to bases)
	struct nestedClassForComparisons{};
	operator nestedClassForComparisons*()
	{		
		return reinterpret_cast<nestedClassForComparisons*>(instPtr);
	}

	// leechange - 040306 - u60 - this operator does not compile in 2005, and is not needed
	//operator ==(nestedClassForComparisons* rhs)
	//{
	//	return (rhs==reinterpret_cast<nestedClassForComparisons*>(instPtr));
	//}


private:
	T* instPtr;
	int * refCount;	
};


#endif