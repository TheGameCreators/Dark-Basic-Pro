
#include "cportals.h"
#include "ccompiler.h"
#include "cbsptree.h"
#include "cbounds.h"
#include "cplane.h"

cProcessPRT::cProcessPRT ( )
{
	// clear everything out
    m_pParent     = NULL;
    m_pStats      = NULL;
}

cProcessPRT::~cProcessPRT ( )
{
    
}

void cProcessPRT::SetOptions ( sPRT& Options )
{
	// set options
	m_OptionSet = Options;
}

void cProcessPRT::SetStats ( sPRTStats* pStats )
{
	// set stats
	m_pStats = pStats;
}

bool cProcessPRT::Process ( cBSPTree* pTree )
{
	// create portals
	cBounds3		PortalBounds;			// portal bounds
    cBSPPortal*		pInitialPortal = NULL;	// initial portal
    cBSPNode*		pCurrentNode   = NULL;	// node
    cBSPNode*		pRootNode      = NULL;	// root node
    cPlane3*		pNodePlane     = NULL;	// plane
    cBSPPortal*		pPortalList    = NULL;	// list of portals

	// check tree is valid
    if ( !pTree )
		return false;

	// store pointer to tree
    m_pTree = pTree;

	// use a try catch block to trap any errors, works well
	// if anything goes wrong
    try 
    {
		// do we need to fill in stats
		if ( m_pStats )
        {
			// save node count
            m_pStats->iNodeCount = m_pTree->GetNodeCount ( );
        }
		
		// get the first root node
		if ( ! ( pRootNode = m_pTree->GetNode ( 0 ) ) )
			throw;

		// go through each node in the tree
		for ( int iNode = 0; iNode < pTree->GetNodeCount ( ); iNode++ )
        {
			// get the current node
            if ( ! ( pCurrentNode = m_pTree->GetNode ( iNode ) ) )
				throw;

			// get the plane of the current node
            if ( ! ( pNodePlane = m_pTree->GetPlane ( pCurrentNode->m_iPlane ) ) )
				throw;
            
			// create a new portal
			if ( ! ( pInitialPortal = cBSPTree::AllocBSPPortal ( ) ) )
				throw;
            
			// see if we need to do a full compile
            if ( m_OptionSet.bFullCompile )
				PortalBounds = pRootNode->m_Bounds;
			else
				PortalBounds = pCurrentNode->m_Bounds;
			
			// generate portal from plane
            pInitialPortal->GenerateFromPlane ( *pNodePlane, PortalBounds );

			// save the owner node
            pInitialPortal->m_iOwnerNode = iNode;

			// clip initial portal
            pPortalList = ClipPortal ( 0, pInitialPortal );

			// set pointer to null
            pInitialPortal = NULL;
            
			// do we have portals
            if ( pPortalList )
            {
				// add portal to list
                if ( ! ( AddPortals ( pPortalList ) ) )
					throw;
            }
		}

		// update stats if necessary
        if ( m_pStats )
        {
			// save portal count
            m_pStats->iPortalCount = m_pTree->GetPortalCount ( );
        }
    }
	catch ( ... ) 
    {
		// if something goes wrong we end up here, so delete
		// previously allocate memory

        if ( pInitialPortal )
			delete pInitialPortal;
        
		// go back
        return false;
	}

	// went okay
    return true;
}

cBSPPortal* cProcessPRT::ClipPortal ( int iNode, cBSPPortal* pPortal )
{
	// clip a portal
	cBSPPortal* pPortalList      = NULL;	// portal list
	cBSPPortal* pFrontPortalList = NULL;	// front portals
	cBSPPortal* pBackPortalList  = NULL;	// back portals
	cBSPPortal* pIterator        = NULL;	// temp pointer
	cBSPPortal* pFrontSplit      = NULL;	// front split
	cBSPPortal* pBackSplit       = NULL;	// back split
    cBSPNode*	pCurrentNode     = NULL;	// current node
    cPlane3*	pCurrentPlane    = NULL;	// current plane
	int			iOwnerPos        = 0;		// owner
	int			iLeafIndex       = 0;		// leaf index

	// see if portal and tree are valid
    if ( !pPortal || !m_pTree )
		throw;

	// get the node
    if ( ! ( pCurrentNode = m_pTree->GetNode ( iNode ) ) )
		throw;

	// and now get the plane
    if ( ! ( pCurrentPlane = m_pTree->GetPlane ( pCurrentNode->m_iPlane ) ) )
		throw;

	// classify this polygon
    switch ( pCurrentPlane->ClassifyPoly ( pPortal->m_pVertices, pPortal->m_dwVertexCount, sizeof ( cVertex ) ) )
    {
		case CP_ONPLANE:
		{
			// the portal has to be sent down both sides of the tree
			// and tracked, send it down front first but do not delete
			// any parts that end up in solid space - ignore them

			if ( pCurrentNode->m_iFront < 0 ) 
            {
				// store leaf index and owner position
				iLeafIndex = abs ( pCurrentNode->m_iFront + 1 );
                iOwnerPos  = ClassifyLeaf ( iLeafIndex, pPortal->m_iOwnerNode );

				// does portal have owner
                if ( iOwnerPos != NO_OWNER )
                {
                    pPortal->m_iLeafOwner [ iOwnerPos] = iLeafIndex;	// save leaf index
				    pPortal->m_ucLeafCount++;							// increment leaf count
				    pPortal->m_pNextPortal = NULL;						// set next portal to null
				    pPortal->m_pPrevPortal = NULL;						// set previous portal to null
				    pFrontPortalList	   = pPortal;					// save portal
                }
                else
                {
					// delete portal
                    delete pPortal;
                    return NULL;
                }
			}
            else 
            {
				// send the portal down the front list and get returned a list of
				// portals that survived the front tree
				pFrontPortalList = ClipPortal ( pCurrentNode->m_iFront, pPortal );
			}

			// see if front list is valid
			if ( pFrontPortalList == NULL )
				return NULL;

			// make sure back index is valid
            if ( pCurrentNode->m_iBack < 0 )
				return pFrontPortalList;

			// go through all portals in front list
			while ( pFrontPortalList != NULL )
            {	
				// get next portal
				cBSPPortal* pNextPortal = pFrontPortalList->m_pNextPortal;

				// get back portal
				pBackPortalList	= NULL;
				pBackPortalList	= ClipPortal ( pCurrentNode->m_iBack, pFrontPortalList );

				// if back portal list is valid
				if ( pBackPortalList != NULL )
                {
					// save pointer
					pIterator = pBackPortalList;

					// go through until end portal
                    while ( pIterator->m_pNextPortal != NULL )
						pIterator = pIterator->m_pNextPortal;
					
					// save next portal
					pIterator->m_pNextPortal = pPortalList;

					// store previous portal
					if ( pPortalList != NULL )
						pPortalList->m_pPrevPortal = pIterator;

					// save pointer to back list
					pPortalList = pBackPortalList;
                }

				// pointer to next portal
				pFrontPortalList = pNextPortal;
			}
			
			// return portal list
            return pPortalList;
		}

		case CP_FRONT:
		{
			// either send it down the front tree or add it to the portal 
			// list because it has come out in empty space
			if ( pCurrentNode->m_iFront < 0 )
            {
				// store leaf index and owner position
                iLeafIndex = abs ( pCurrentNode->m_iFront + 1 );
                iOwnerPos  = ClassifyLeaf ( iLeafIndex, pPortal->m_iOwnerNode );

				// does leaf have owner
                if ( iOwnerPos != NO_OWNER )
                {
                    pPortal->m_iLeafOwner [ iOwnerPos ] = iLeafIndex;	// leaf index
				    pPortal->m_ucLeafCount++;							// increment leaf count
				    pPortal->m_pNextPortal	= NULL;						// next pointer to null
					pPortal->m_pPrevPortal	= NULL;						// previous pointer to null
					return pPortal;										// return this portal
                } 
                else 
                {
					// delete this portal and return
                    delete pPortal;
                    return NULL;
                }
			}
            else 
            {
				// clip these portals
                pPortalList = ClipPortal ( pCurrentNode->m_iFront, pPortal );
				return pPortalList;	
            }
		}
		break;

		case CP_BACK:
		{
			// either send it down the back tree or delete it if no back tree exists
			// because it means it has come out in solid space
            if ( pCurrentNode->m_iBack < 0 )
            {
				// delete and return
                delete pPortal;
                return NULL;
			} 
            else
            {
				// clip portals
                pPortalList = ClipPortal ( pCurrentNode->m_iBack, pPortal );
                return pPortalList;
			}
		}
        break;

		case CP_SPANNING:
		{
			// spanning the plane - we need to split

			// create front split
            if ( ! ( pFrontSplit = cBSPTree::AllocBSPPortal ( ) ) )
				throw;

			// create back split
            if ( ! ( pBackSplit = cBSPTree::AllocBSPPortal ( ) ) )
			{
				delete pFrontSplit;
				throw;
			}
			
			// split planes
            if ( FAILED ( pPortal->Split ( *pCurrentPlane, pFrontSplit, pBackSplit ) ) )
            {  
                delete pFrontSplit;
				delete pBackSplit;
                throw;
            }
			
			// delete portal
            delete pPortal;
			pPortal = NULL;
	
			// do we have a front node
			if ( pCurrentNode->m_iFront < 0 ) 
            {
				// push into leaf - same as before
                iLeafIndex = abs ( pCurrentNode->m_iFront + 1 );
                iOwnerPos  = ClassifyLeaf ( iLeafIndex, pFrontSplit->m_iOwnerNode );

                if ( iOwnerPos != NO_OWNER )
                {
                    pFrontSplit->m_iLeafOwner [ iOwnerPos ] = iLeafIndex;
				    pFrontSplit->m_ucLeafCount++;
				    pFrontSplit->m_pPrevPortal = NULL;
				    pFrontSplit->m_pNextPortal = NULL;
				    pFrontPortalList           = pFrontSplit;
                } 
                else 
                {
					// delete front split pointer
                    delete pFrontSplit;
                }
			} 
            else
            {
				// clip front portals
				pFrontPortalList = ClipPortal ( pCurrentNode->m_iFront, pFrontSplit );
			}
	
			// back split is in solid space
			if ( pCurrentNode->m_iBack < 0 ) 
            {
				// delete back split
                delete pBackSplit;
				pBackSplit = NULL;
			} 
            else
            {
				// now clip back split portals
				pBackPortalList = ClipPortal ( pCurrentNode->m_iBack, pBackSplit );
			}

			// find the end of the list and attach it to front back list
			if ( pFrontPortalList != NULL )
            {
				// store pointer to front list
				pIterator = pFrontPortalList;

				// move through until end
				while ( pIterator->m_pNextPortal != NULL )
					pIterator = pIterator->m_pNextPortal;
				
				// if back list is valid store new pointers
				if ( pBackPortalList != NULL )
                {
					pIterator->m_pNextPortal	   = pBackPortalList;
					pBackPortalList->m_pPrevPortal = pIterator;
				}

				// return front list
                return pFrontPortalList;
			}
            else
            {
				// delete back list
				if ( pBackPortalList != NULL )
					return pBackPortalList;

                return NULL;
			}
			
			return NULL;
		}
	}

	return NULL;
}

bool cProcessPRT::AddPortals ( cBSPPortal* pPortalList )
{
	// if the fragments are valid portals, then they are added to the final portal list

	// local variables
	int				iPortalIndex = 0;		// portal index
    cBSPPortal*		pIterator    = NULL;	// temporary pointer
	cBSPPortal*		pNext        = NULL;	// next portal
    cBSPLeaf*		pLeaf        = NULL;	// leaf

	// see if portal list is valid
    if ( ! pPortalList )
		return false;

	// save pointer to portal list
    pIterator = pPortalList;

	// go through each portal
    while ( pIterator != NULL )
    {
		// is portal valid
		if ( pIterator->m_ucLeafCount != 2 )
        {
			// portal is invalid - remove from list
            pNext     = pIterator->m_pNextPortal;
			RemovePortalFromList ( pIterator );
			pIterator = pNext;
		} 
        else 
        {
			// get rid of duplicate portals
            if ( CheckDuplicatePortal ( pIterator, iPortalIndex ) )
            {
				pNext      = pIterator->m_pNextPortal;
				RemovePortalFromList ( pIterator );
				pIterator  = pNext;
			} 
            else 
            {
				// this portal is valid so add it
                if ( iPortalIndex == m_pTree->GetPortalCount ( ) )
                {
                    for ( int i = 0; i < 2; i++ )
                    {
						// get leaves
                        pLeaf = m_pTree->GetLeaf ( pIterator->m_iLeafOwner [ i ] );

						// is leaf valid
                        if ( ! pLeaf )
							return false;

						// now add portal
                        pLeaf->AddPortal ( m_pTree->GetPortalCount ( ) );
                    }

					// increase the portal count
                    m_pTree->IncreasePortalCount ( );
				}

				// set this portal
                m_pTree->SetPortal ( iPortalIndex, pIterator );

				// now move to next portal
                pIterator = pIterator->m_pNextPortal;
			}
		}
	}
	
	// all went well so return
    return true;
}

void cProcessPRT::RemovePortalFromList ( cBSPPortal* pRemovePortal )
{
	// remove a portal from the list

	// local variables
	cBSPPortal* pPortal     = pRemovePortal;	// portal to remove
	cBSPPortal* pPrevPortal = NULL;				// previous portal
	cBSPPortal* pNextPortal = NULL;				// next portal
	
	// is there a previous portal
    if ( pRemovePortal->m_pPrevPortal != NULL )
    {
		// store previous portal
        pPrevPortal = pRemovePortal->m_pPrevPortal;

		// is next portal valid
		if ( pRemovePortal->m_pNextPortal != NULL )
        {
			pPrevPortal->m_pNextPortal = pRemovePortal->m_pNextPortal;
        }
        else
        {
			pPrevPortal->m_pNextPortal = NULL;
		}
	}

	// check next portal
	if ( pRemovePortal->m_pNextPortal != NULL )
    {
		// is next portal valid
        pNextPortal = pRemovePortal->m_pNextPortal;

		// get pointer
		if ( pRemovePortal->m_pPrevPortal != NULL )
        {
			pNextPortal->m_pPrevPortal = pRemovePortal->m_pPrevPortal;
        }
        else
        {
			pNextPortal->m_pPrevPortal = NULL;
		}
	}

	// get rid of this portal
    delete pPortal;
}

bool cProcessPRT::CheckDuplicatePortal ( cBSPPortal* pCheckPortal, int& iIndex )
{
	// this is used to check the current portal with the portal already 
	// sharing the same leaves in tree, it is only accepted if the portal is
	// larger than the one that is ready occupying this place in the master 
	// portal array

	int			iCheckPortalLeaf1 = pCheckPortal->m_iLeafOwner [ 0 ];	// first leaf owner
	int			iCheckPortalLeaf2 = pCheckPortal->m_iLeafOwner [ 1 ];	// second leaf owner
	int			iPALeaf1 = 0;
	int			iPALeaf2 = 0;

	// bounds
    cBounds3	Bounds1;
	cBounds3	Bounds2;
    cBSPPortal* pCurrentPortal = NULL;
	
	// we need to go through each portal
	for ( int i = 0; i < m_pTree->GetPortalCount ( ); i++ )
    {
		// get current portal and leaf owners
        pCurrentPortal = m_pTree->GetPortal ( i );
		iPALeaf1       = pCurrentPortal->m_iLeafOwner [ 0 ];
		iPALeaf2       = pCurrentPortal->m_iLeafOwner [ 1 ];

		// are they the same
		if ( ( iCheckPortalLeaf1 == iPALeaf1 && iCheckPortalLeaf2 == iPALeaf2 ) || 
			 ( iCheckPortalLeaf1 == iPALeaf2 && iCheckPortalLeaf2 == iPALeaf1 ) )
        {
			// get the bounds of the portals
			Bounds1.CalculateFromPolygon ( pCheckPortal->m_pVertices,   pCheckPortal->m_dwVertexCount,   sizeof ( cVertex ) );
            Bounds2.CalculateFromPolygon ( pCurrentPortal->m_pVertices, pCurrentPortal->m_dwVertexCount, sizeof ( cVertex ) );
            
			// store size
			float fNewSize = Bounds1.GetDimensions ( ).SquareLength ( );
			float fOldSize = Bounds2.GetDimensions ( ).SquareLength ( );
 
			// is portal larger
            if ( fNewSize > fOldSize )
            {
				// delete it
                delete pCurrentPortal;
				iIndex = i;
				return false;
			} 
            else 
            {
				// already in array
				return true;
			}
		}
	}
	
	// save index
	iIndex = m_pTree->GetPortalCount ( );
	
	// finished
	return false;
}

bool cProcessPRT::FindLeaf ( int iLeaf, int iNode )
{
	// determines if the leaf passed is somewhere below the specified node

	// current node
	cBSPNode* pCurrentNode = NULL;
    
	// is tree valid
    if ( !m_pTree )
		throw;

	// get node
    if ( ! ( pCurrentNode = m_pTree->GetNode ( iNode ) ) )
		throw;

	// is node a leaf
	if ( pCurrentNode->m_iFront < 0 ) 
    {
        if ( abs ( pCurrentNode->m_iFront + 1 ) == iLeaf )
			return true;
    } 
    else 
    {
        if ( FindLeaf ( iLeaf, pCurrentNode->m_iFront ) )
			return true;
    }

	// go down the back side
    if ( pCurrentNode->m_iBack >= 0 ) 
    {
        if ( FindLeaf ( iLeaf, pCurrentNode->m_iBack ) )
			return true;
    }

    return false;
}

int cProcessPRT::ClassifyLeaf ( int iLeaf, int iNode )
{
	// classifies a leaf against this node, leaf must be
	// somewhere relative to the node passed

	cBSPNode* pCurrentNode = NULL;
    
	// is tree valid
    if ( !m_pTree )
		throw;

	// get node
    if ( ! ( pCurrentNode = m_pTree->GetNode ( iNode ) ) )
		throw;

	// is the front leaf
    if ( pCurrentNode->m_iFront < 0 )
    {
        if ( abs ( pCurrentNode->m_iFront + 1 ) == iLeaf )
			return FRONT_OWNER;
    } 
    else 
    { 
        if ( FindLeaf ( iLeaf, pCurrentNode->m_iFront ) )
			return FRONT_OWNER;
    }

	// go down the back
    if ( pCurrentNode->m_iBack >= 0 )
    {
        if ( FindLeaf ( iLeaf, pCurrentNode->m_iBack ) )
			return BACK_OWNER;
    }
    
    return NO_OWNER;
}