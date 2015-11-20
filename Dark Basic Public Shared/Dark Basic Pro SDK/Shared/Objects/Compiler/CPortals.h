
#ifndef _PROCESSPRT_H_
#define _PROCESSPRT_H_

#include "CompilerTypes.h"
#include "Common.h"

class cBSPTree;
class cBSPPortal;
class cCompiler;

#define PRT_ARRAY_THRESHOLD     100

class cProcessPRT
{
	// used to calculate portals

	private:
		// member variables
		sPRT			m_OptionSet;		// options for processing
		cCompiler*		m_pParent;			// compiler
		cBSPTree*		m_pTree;			// bsp tree
		sPRTStats*		m_pStats;			// stats

		// member functions
		bool            CheckDuplicatePortal ( cBSPPortal* pCheckPortal, int& iIndex );	// check for duplicate portals
		cBSPPortal*		ClipPortal           ( int iNode, cBSPPortal* pPortal );		// clip portals
		bool            FindLeaf             ( int iLeaf, int iNode );					// find a leaf
		int				ClassifyLeaf         ( int iLeaf, int iNode );					// classify a leaf
		void            RemovePortalFromList ( cBSPPortal* pRemovePortal );				// remove portal from list
		bool	        AddPortals           ( cBSPPortal* pPortalList );				// add portals

	public:
				 cProcessPRT ( );	// constructor
		virtual ~cProcessPRT ( );	// destructor

		bool	Process    ( cBSPTree*   pTree   );		// create portals
		void    SetOptions ( sPRT&		 Options );		// set options
		void    SetParent  ( cCompiler*  pParent );		// set compiler parent
		void    SetStats   ( sPRTStats*	 pStats  );		// set stats
};

#endif
