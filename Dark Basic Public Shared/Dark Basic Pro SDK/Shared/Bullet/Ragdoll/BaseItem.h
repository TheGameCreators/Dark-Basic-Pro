#pragma once
class BaseItem
{
public:
	BaseItem(int id);
	virtual ~BaseItem();
	int GetID() const;
	bool operator < (const BaseItem&) const; 
	bool operator > (const BaseItem&) const; 
	bool operator <= (const BaseItem&) const;
	bool operator >= (const BaseItem&) const; 
	bool operator == (const BaseItem&) const; 
	bool operator != (const BaseItem&) const; 
	static bool sortfnc ( const BaseItem* a, const BaseItem* b ) 
	{
			return *a < *b; 
	}
protected: 
	int m_id;

};

