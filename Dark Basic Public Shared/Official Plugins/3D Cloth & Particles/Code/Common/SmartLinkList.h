#ifndef DBPROPHYSICS_SMARTPTRLINKLIST_SORTED_H
#define DBPROPHYSICS_SMARTPTRLINKLIST_SORTED_H
//Smart pointer based linked list template with IDs in ascending order


//This just keeps an eye on the number of links
//in the list to make sure it's the same as numLinks
//(only if _DEBUG is defined)
#ifdef _DEBUG
	#define DEBUGSTART int __count=0;
	#define DEBUGCOUNT __count++;
	#define DEBUGCHECK assert(__count==numLinks);
#else
	#define DEBUGSTART
	#define DEBUGCOUNT
	#define DEBUGCHECK
#endif



template <class T> class SmartLink
{
public:
	SmartLink(int ID, SmartPtr<T>& ptr):id(ID),data(ptr),next(0),prev(0){}

	int		id;		// unique id of node
	SmartPtr<T> data;

	SmartLink<T> * next;
	SmartLink<T> * prev;
};


template <class T> class SmartLinkedList_SortID
{
public:
	SmartLinkedList_SortID():startLink(0),iterLink(0),lastGetLink(0),numLinks(0){}

	~SmartLinkedList_SortID()
	{
		Clear();
	}		

	bool Exists(int id)
	{
		SmartLink<T> * curLink= GetLink(id);
		
		if(curLink && curLink->id==id) return true;
		return false;		
    }

	void Add(int id, SmartPtr<T>& ptr, bool overwrite=false)
	{
		if(id<1) throw "Can't use ID less than 1";

		SmartLink<T> * curLink = GetLink(id);
		if(curLink==0)
		{
			if(startLink==0)
			{
				//Add first item
				assert(numLinks==0);
				startLink = new SmartLink<T>(id,ptr);
			}
			else
			{
				//Insert at beginning of list
				SmartLink<T> * newLink = new SmartLink<T>(id,ptr);
				newLink->prev=0;
				newLink->next=startLink;
				startLink->prev=newLink;
				startLink=newLink;
			}
			numLinks++;
		}
		else if(curLink->id==id)
		{
			if(overwrite)
			{
				curLink->data=ptr;
			}
			else
			{
				throw "Can't overwrite";
			}
		}
		else
		{
			SmartLink<T> * newLink = new SmartLink<T>(id,ptr);
			newLink->prev=curLink;
			newLink->next=curLink->next;
			if(curLink->next!=0) curLink->next->prev=newLink;
			curLink->next=newLink;
			numLinks++;
		}
	}

	void Delete(int id)
	{
		SmartLink<T> * curLink = GetLink(id);
	
		if(curLink && curLink->id==id)
		{
			if(lastGetLink==curLink) lastGetLink=0;
			if(curLink->prev) curLink->prev->next=curLink->next;
			if(curLink->next) curLink->next->prev=curLink->prev;
			if(startLink==curLink) startLink=curLink->next;
			delete curLink;
			curLink=0;
			numLinks--;
		}
		else
		{
			throw "Can't delete";
		}
	}

	SmartPtr<T> Get(int id)
	{
		SmartLink<T> * curLink = GetLink(id);

		if(curLink && curLink->id==id)
		{
			return curLink->data;
		}
		return 0;
	}

	int Count()
	{
		#ifdef _DEBUG
			SmartLink<T> * curLink=startLink;
			DEBUGSTART;
			while(curLink)
			{
				curLink=curLink->next;
				DEBUGCOUNT;
			}
			DEBUGCHECK;	
		#endif	
		return numLinks;
	}

	void Clear()
	{
		SmartLink<T> * curLink=startLink;
		SmartLink<T> * lastLink=curLink;
		DEBUGSTART;
		while(curLink)
		{
			lastLink=curLink;
			curLink=curLink->next;
			delete lastLink;
			lastLink=0;
			DEBUGCOUNT;
		}
		DEBUGCHECK;
		lastGetLink=0;
		startLink=0;
		numLinks=0;
	}

	bool gotoFirstItem()
	{
		iterLink=startLink;
		if(iterLink==0) return false;
		return true;
	}

	bool gotoNextItem()
	{
		iterLink=iterLink->next;
		if(iterLink==0) return false;
		return true;
	}

	//We could pass a pointer to the smart pointer, but it
	//would get very confusing with dereferencing etc. so we'll 
	//pass by value - not particularly efficient, but safe.
	SmartPtr<T> getCurObject()
	{
		if(iterLink==0) return 0;
		return iterLink->data;
	}

	//Must be very careful using this. It circumvents the 
	//whole safe pointer system, but is very handy for 
	//quickly calling an objects member functions without
	//the overhead of passing and copying smart pointers.
	T* getCurItem_WARNING() 
	{
		if(iterLink==0) return 0;
		return &(*(iterLink->data)); //Yuck I know!!
	}


	int getCurIndex()
	{
		if(iterLink==0) return -1;
		return iterLink->id;
	}

	int getLowestFreeID()
	{
		SmartLink<T> * curLink=startLink;		
		int curID=1;
		while(curLink)
		{
			if(curLink->id=curID)
			{
				curID++;
				curLink=curLink->next;
			}
			else
			{
				return curID;
			}
		}		
		return curID;
	}

private:

	SmartLink<T> * GetLink(int id)
	{
		//If can't find link, then return item previous to where it should be
		//or zero if it should be at the beginning.

		if(lastGetLink)
		{
			if(lastGetLink->id==id) return lastGetLink;
		}

		SmartLink<T> * curLink=startLink;
		SmartLink<T> * lastLink=0;
		DEBUGSTART;
		while(curLink)
		{
			if(curLink->id==id)
			{
				lastGetLink=curLink;
				return curLink;
			}
				
			if(id<curLink->id)
			{
				lastGetLink=lastLink;
				return lastLink;
			}
			lastLink=curLink;
			curLink=curLink->next;
			DEBUGCOUNT;
		}
		DEBUGCHECK;
		lastGetLink=lastLink;
		return lastLink;
	}

	SmartLink<T> * startLink;	
	SmartLink<T> * iterLink;
	SmartLink<T> * lastGetLink;
	int numLinks;
};


#endif