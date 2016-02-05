#ifndef DBPROPHYSICS_LINKLIST_H
#define DBPROPHYSICS_LINKLIST_H


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

template <class T> class Link
{
public:
	Link(T* ptr):data(ptr),next(0),prev(0){}

	T* data;
	Link<T> * next;
	Link<T> * prev;
};


template <class T> class LinkedList
{

public:


	LinkedList():startLink(0),iterLink(0),numLinks(0){}

	~LinkedList()
	{
		Clear();
	}		

	void Add(T* ptr)
	{		
		if(startLink==0)
		{			
            startLink = new Link<T>(ptr);
			numLinks++;
		}
		else
		{
			//Insert link at beginning
			Link<T> * newLink = new Link<T>(ptr);			
			newLink->next=startLink;
			startLink->prev=newLink;			
			startLink=newLink;
			numLinks++;
		}
	}

	void addAfter(T* ptr, Link<T> * addAfterLink)
	{		
		if(!addAfterLink) 
		{
			Add(ptr);
			return;
		}
		Link<T> * newLink = new Link<T>(ptr);			
		newLink->prev=addAfterLink;
		newLink->next=addAfterLink->next;
		if(addAfterLink->next) addAfterLink->next->prev=newLink;
		addAfterLink->next=newLink;		
		numLinks++;
	}

	void Delete(Link<T> * curLink)
	{
		if(curLink)
		{
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

	void AddLink(Link<T> * curLink)
	{
		if(startLink==0)
		{
			assert(startLink==0);
            startLink = curLink;
			numLinks++;
		}
		else
		{
			//Insert link at beginning			
			curLink->prev=0;
			curLink->next=startLink;
			startLink->prev=curLink;			
			startLink=curLink;
			numLinks++;
		}
	}

	void RemoveLink(Link<T> * curLink)
	{
		if(curLink)
		{
			if(iterLink==curLink)	//We might be iterating through the list, so rewind to previous link or zero
			{
				if(curLink->prev) iterLink=curLink->prev;
				else iterLink=0;
			}

			if(curLink->prev) curLink->prev->next=curLink->next;
			if(curLink->next) curLink->next->prev=curLink->prev;
			if(startLink==curLink) startLink=curLink->next;
			curLink->prev=0;
			curLink->next=0;
			numLinks--;
		}
		else
		{
			throw "Can't delete";
		}
	}


	int Count()
	{
		#ifdef _DEBUG
			Link<T> * curLink=startLink;
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
		Link<T> * curLink=startLink;
		Link<T> * lastLink=curLink;
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
		Link<T> * curLink=startLink;
		Link<T> * lastLink=curLink;
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
		if(iterLink==0) iterLink=startLink;	//First link might have been removed using RemoveLink()
		else iterLink=iterLink->next;
		if(iterLink==0) return false;
		return true;
	}

	T* getCurObject()
	{
		if(iterLink==0) return 0;
		return iterLink->data;
	}

	Link<T>* getCurLink()
	{		
		return iterLink;
	}

private:
	Link<T> * startLink;	
	Link<T> * iterLink;	
	int numLinks;
};


#endif