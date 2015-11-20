//#include "StdAfx.h"
#include "BaseItem.h"

BaseItem::BaseItem(int id)
{
	m_id = id;
}

BaseItem::~BaseItem()
{
}

int BaseItem::GetID() const
{
	return m_id;
}

bool BaseItem::operator< (const BaseItem&c) const{ return GetID() < c.GetID(); }
bool BaseItem::operator> (const BaseItem& c) const{ return GetID() > c.GetID(); }
bool BaseItem::operator<= (const BaseItem& c) const{ return GetID() <= c.GetID(); }
bool BaseItem::operator>= (const BaseItem& c) const{ return GetID() >= c.GetID(); }
bool BaseItem::operator== (const BaseItem& c) const{ return GetID() == c.GetID(); }
bool BaseItem::operator!= (const BaseItem& c) const{ return GetID() != c.GetID(); }