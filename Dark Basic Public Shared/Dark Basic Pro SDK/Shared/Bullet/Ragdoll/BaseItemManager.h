#pragma once

#include "BaseItem.h"
#include "LinearMath\btAlignedObjectArray.h"

class BaseItemSortPredicate
{
	public:
		bool operator() ( const BaseItem* a, const BaseItem* b ) const
		{
			 return *a < *b;
		}
};

class BaseItemManager
{
public:
	BaseItemManager();
	virtual ~BaseItemManager();
	
	void Dispose();
	void Sort();
	int GetFreeID();
	int Find(int id);

protected:
	void AddItem(BaseItem* item);
	BaseItem* GetItem(int id); 
	bool RemoveItem(int id);
	//
	btAlignedObjectArray<BaseItem*> m_data; 
};

