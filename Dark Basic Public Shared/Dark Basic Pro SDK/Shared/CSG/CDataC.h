#ifndef _CDATA_H_
#define _CDATA_H_

#define WIN32_LEAN_AND_MEAN 
#include <windows.h>

struct link
{
	int		id;		// id of node
	VOID*	data;	// data in node

	link*	next;	// pointer to next link
	link*	prev;	// pointer to previous link
};

class CData
{
	private:
		
	public:
		link*	m_current;	// current link
		link*	m_start;	// pointer to start
		int		m_count;	// number of nodes

		CData  ( );
		~CData ( );

		//void  Add        ( VOID* pbData, int iOverwrite, int iSort );			// add a node
		void  Add        ( int num, VOID* pbData, int iOverwrite, int iSort );	// add a node
		void  Delete     ( int iID );											// delete
		VOID* Get        ( int iID );											// get the data
		int	  Count      ( void    );											// count the nodes
		void  DeleteAll  ( void    );											// delete all elements
};

#endif _CDATA_