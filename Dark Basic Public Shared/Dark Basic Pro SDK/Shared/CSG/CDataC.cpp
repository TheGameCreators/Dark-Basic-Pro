#include "cdatac.h"

CData::CData ( )
{
	// constructor
	m_current	= NULL;	// point current to nothing
	m_start		= NULL;	// point start to nothing
	m_count	= 0;		// set to 0
}

CData::~CData ( )
{
	// destructor
	link* get = m_start;	// pointer to start of list
	link* select;			// to store currently selected node

	// delete any existing nodes
	for ( int temp = 0; temp < m_count; temp++ )
	{
		select	= get;			// store pointer to current node
		get		= get->next;	// move pointer to next node

		// delete the node if it exists
		if ( select )
			delete select;
	}	
}

/*
void CData::Add ( VOID* pbData, int iOverwrite, int iSort )
{
	m_count
}
*/

void CData::Add ( int num, VOID* pbData, int iOverwrite, int iSort )
{
	// the default behaviour of adding nodes is to update
	// any nodes where the id alreay exists, setting the 
	// iOverwrite flag on will add a new node to the list

	// possible options for adding nodes -
	//		* can the memory be allocated?
	//		* does a node with the given id already exist, replace it?
	//		* where should the id go? resort the list if necessary
	link* newlink = new link;	// create a new link

	if ( newlink == NULL )
		return;

	// check if the node needs to be inserted between
	// some other existing nodes (basically sort the list)
	// e.g. list data = 1,2,4,5, insert 3 list = 1,2,3,4,5

	// get a pointer to the start of the list
	link* get = m_start;
	int iFound = 0;

	int iStart = 0;
	
	// get a pointer to the location
	for ( int temp = 0; temp < m_count; temp++ )
	{
		if ( get->id && get->next )
		{
			// see if the number needs to be inserted
			// in the middle of the list
			if ( get->id < num && get->next->id > num )
			{
				iFound = 1;
				break;
			}
		}

		// see if number exists
		if ( get->id )
		{
			if ( get->id > num )
				iStart++;
		}
		
		// id already exists, update
		// the data
		if ( !iOverwrite )
		{
			if ( get->id == num )
			{
				get->data = pbData;
				return;
			}
		}

		if ( get->next )
			get = get->next;
		else
			continue;
	}
	
	// check for insertion at start
	if ( iStart == m_count && m_start != NULL )
		iFound = 2;

	// HACK
	iFound = 0;

	switch ( iFound )
	{
		case 0:
		{
			// insert at end of list
			newlink->id	  = num;		// set the data
			newlink->data = pbData;		// store pointer to data
			
			newlink->prev = m_current;	// set the previous link
			newlink->next = NULL;		// make sure next initially points to nothing
				
			// when we have more than 1 node
			// set the previous nodes next link
			// to the newly created link
			if ( m_count > 0 )
				newlink->prev->next = newlink;

			// store the current link
			m_current = newlink;
				
			// for when the list is first created
			if ( m_count == 0 )
			{
				m_start = newlink;		// get a pointer to the start node
				newlink->next = NULL;	// next will point to nothing
			}
			
			// increment the link count
			m_count++;
		}
		break;

		case 1:
		{
			// insert in middle of list
			newlink->prev		= get;			// prev should point to found node
			newlink->next		= get->next;	// point next to the node after found node
			newlink->next->prev	= newlink;		// set the node after this one to point to it
			get->next			= newlink;		// make found node point to this
					
			newlink->id			= num;			// set id
			newlink->data		= pbData;		// store pointer to data

			// increment the link count
			m_count++;

			return;
		}
		break;

		case 2:
		{
			// insert at start of list
			m_current = newlink;

			newlink->id        = num;
			newlink->data      = pbData;

			newlink->next      = m_start;
			newlink->prev      = NULL;

			m_start->prev      = newlink;
			m_start            = newlink;

			m_count++;

			return;
		}
		break;
	}
}

void CData::Delete ( int iID )
{	
	// possible options for deleting nodes -
	//		* does the id exist?
	//		* is the node at the end of the list?
	//		* is the node at the start of the list?
	//		* is the node the only node in the list?
	//		* is the node enclosed by 2 other nodes?

	// check where the node is, recreation of 
	// broken links is dependant on this

	// get a pointer to the start of the list
	link* get  = m_start;
	int	iFound = 0;

	// get a pointer to the location
	for ( int temp = 0; temp < m_count; temp++ )
	{
		if ( get->id == iID )
		{
			iFound = 1;
			break;
		}

		get = get->next;
	}

	// if the id was not found
	// then quit out of function
	if ( !iFound )
		return;

	// node is at the end of the list
	if ( get->next == NULL && get->prev )
	{
		get->prev->next = NULL;	// set next pointer to null

		// add
		//m_current = m_start;

		

		delete get;				// delete the node
		m_count--;				// decrement link count

		/////////////////
		// get a pointer to the start of the list
		link* check = m_start;

		// get a pointer to the location
		for ( temp = 0; temp < m_count; temp++ )
		{
			//if ( check->id == iID )
			//	return check->data;

			if ( check->next )
				check = check->next;
		}

		m_current = check;
		/////////////////

		return;					// quit out of function
	}

	// node is at the start of the list
	if ( get->prev == NULL && get->next )
	{
		get->next->prev = NULL;	// set the next link nodes prev to null
		m_start = get->next;		// get the new start pos of list
				
		delete get;				// delete the node
		m_count--;				// decrement link count
		return;					// quit out of function
	}

	// the only node in the list
	if ( get->prev == NULL && get->next == NULL )
	{
		m_current		= NULL;	// point current to nothing
		m_start		= NULL;	// point start to nothing

		delete get;
		m_count = 0;
		return;
	}

	// node is enclosed by 2 other nodes (1 on each side of it)
	if ( get->next && get->prev )
	{
		get->prev->next = get->next;	// recreate the next link
		get->next->prev = get->prev;	// recreate the prev link

		delete get;				// delete the node
		m_count--;			// decrement link count
		return;					// quit out of function
	}
}

VOID* CData::Get ( int iID )
{
	// value to return
	int temp = NULL;

	// get a pointer to the start of the list
	link* check = m_start;

	// get a pointer to the location
	for ( temp = 0; temp < m_count; temp++ )
	{
		if ( check->id == iID )
			return check->data;

		check = check->next;
	}

	return NULL;
}

int	CData::Count ( void )
{
	return m_count;
}

void CData::DeleteAll ( void )
{
	// destructor
	link* get = m_start;	// pointer to start of list
	link* select;			// to store currently selected node

	// delete any existing nodes
	for (int temp = 0; temp < m_count; temp++)
	{
		select	= get;			// store pointer to current node
		get		= get->next;	// move pointer to next node

		// delete the node if it exists
		if (select)
			delete select;
	}
	
	m_current	= NULL;	// point current to nothing
	m_start		= NULL;	// point start to nothing
	m_count	= 0;		// set to 0
}