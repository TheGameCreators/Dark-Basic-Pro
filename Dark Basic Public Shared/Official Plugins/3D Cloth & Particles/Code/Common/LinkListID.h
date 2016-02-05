#ifndef DBPROPHYSICS_LINKLIST_SORTED_H
#define DBPROPHYSICS_LINKLIST_SORTED_H
//Linked list template with IDs in ascending order


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

template <class T> class LinkID
{
public:
	LinkID(int ID, T * ptr):id(ID),data(ptr),next(0),prev(0){}

	int		id;		// unique id of node
	T * data;

	LinkID<T> * next;
	LinkID<T> * prev;
};


template <class T> class LinkedList_SortID
{

public:
	LinkedList_SortID():startLink(0),iterLink(0),lastGetLink(0),numLinks(0){}

	~LinkedList_SortID()
	{
		deleteAndClear();
	}		

	bool Exists(int id)
	{
		LinkID<T> * curLink= GetLink(id);
		
		if(curLink && curLink->id==id) return true;
		return false;		
    }

	void Add(int id, T * ptr, bool overwrite=false)
	{
		if(id<1) throw "Can't use ID less than 1";

		LinkID<T> * curLink = GetLink(id);
		if(curLink==0)
		{
			if(startLink==0)
			{
				//Add first item
				assert(numLinks==0);
				startLink = new LinkID<T>(id,ptr);
			}
			else
			{
				//Insert at beginning of list
				LinkID<T> * newLink = new LinkID<T>(id,ptr);
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
			LinkID<T> * newLink = new LinkID<T>(id,ptr);
			newLink->prev=curLink;
			newLink->next=curLink->next;
			if(curLink->next!=0) curLink->next->prev=newLink;
			curLink->next=newLink;
			numLinks++;
		}
	}

	void Delete(int id) //Deletes node and data item
	{
		LinkID<T> * curLink = GetLink(id);
	
		if(curLink && curLink->id==id)
		{
			if(lastGetLink==curLink) lastGetLink=0;
			if(curLink->prev) curLink->prev->next=curLink->next;
			if(curLink->next) curLink->next->prev=curLink->prev;
			if(startLink==curLink) startLink=curLink->next;
			delete curLink->data;
			curLink->data=0;
			delete curLink;
			curLink=0;
			numLinks--;
		}
		else
		{
			throw "Can't delete";
		}
	}

	T * Remove(int id) //Only deletes node and just returns the data item
	{
		LinkID<T> * curLink = GetLink(id);
		T * temp;
	
		if(curLink && curLink->id==id)
		{
			if(lastGetLink==curLink) lastGetLink=0;
			if(curLink->prev) curLink->prev->next=curLink->next;
			if(curLink->next) curLink->next->prev=curLink->prev;
			if(startLink==curLink) startLink=curLink->next;
			temp = curLink->data;
			delete curLink;
			curLink=0;
			numLinks--;
			return temp;
		}
		else
		{
			throw "Can't delete";
		}
	}

	T * Get(int id)
	{
		LinkID<T> * curLink = GetLink(id);

		if(curLink && curLink->id==id)
		{
			return curLink->data;
		}
		return 0;
	}

	int Count()
	{
		#ifdef _DEBUG
			LinkID<T> * curLink=startLink;
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
		LinkID<T> * curLink=startLink;
		LinkID<T> * lastLink=curLink;
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
		startLink=0;
		numLinks=0;
	}

	void deleteAndClear()
	{
		LinkID<T> * curLink=startLink;
		LinkID<T> * lastLink=curLink;
		DEBUGSTART;
		while(curLink)
		{
			lastLink=curLink;
			curLink=curLink->next;
			delete lastLink->data;
			lastLink->data=0;
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

	T * getCurItem()
	{
		if(iterLink==0) return 0;
		return iterLink->data;
	}


	int getCurIndex()
	{
		if(iterLink==0) return -1;
		return iterLink->id;
	}

	int getLowestFreeID()
	{
		LinkID<T> * curLink=startLink;		
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

	LinkID<T> * GetLink(int id)
	{
		//If can't find link, then return item previous to where it should be
		//or zero if it should be at the beginning.

		if(lastGetLink)
		{
			if(lastGetLink->id==id) return lastGetLink;
		}

		LinkID<T> * curLink=startLink;
		LinkID<T> * lastLink=0;
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

	LinkID<T> * startLink;	
	LinkID<T> * iterLink;
	LinkID<T> * lastGetLink;
	int numLinks;
};


#endif