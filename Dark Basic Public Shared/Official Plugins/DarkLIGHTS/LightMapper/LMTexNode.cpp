
#include "LMGlobal.h"
#include "LMTexNode.h"

LMTexNode::LMTexNode( )
{
	iStartU = 0; iStartV = 0;
	iSizeU = 0; iSizeV = 0;

	bFilled = false;
	bFinalised = false;

	pLeftNode = 0;
	pRightNode = 0;
}

LMTexNode::LMTexNode( int startU, int startV, int sizeU, int sizeV )
{
	iStartU = startU;
	iStartV = startV;
	iSizeU = sizeU;
	iSizeV = sizeV;

	bFilled = false;
	bFinalised = false;

	pLeftNode = 0;
	pRightNode = 0;
}

LMTexNode::~LMTexNode( )
{
	if ( pLeftNode ) delete pLeftNode;
	if ( pRightNode ) delete pRightNode;
}

int LMTexNode::GetStartU( )
{
	return iStartU;
}

int LMTexNode::GetStartV( )
{
	return iStartV;
}

void LMTexNode::Finalise( )
{
	bFinalised = true;

	if ( bFilled )
	{
		pLeftNode->Finalise( );
		pRightNode->Finalise( );
	}
}

bool LMTexNode::IsFinalised( )
{
	return bFinalised;
}

bool LMTexNode::BackTrack( )
{
	if ( bFilled )
	{
		if ( !bFinalised )
		{
			delete pLeftNode;
			delete pRightNode;
			return true;
		}
		else
		{
			if ( !pLeftNode->IsFinalised( ) || !pRightNode->IsFinalised( ) ) 
			{ 
				delete pLeftNode; 
				delete pRightNode;
				pLeftNode = 0; 
				pRightNode = 0;
				bFilled = false; 
			}

			return false;
		}
	}
	else
	{
		return !bFinalised;
	}
}

LMTexNode* LMTexNode::AddPoly( int polySizeU, int polySizeV )
{
	if ( bFilled )
	{
		LMTexNode *pFoundNode = pLeftNode->AddPoly ( polySizeU, polySizeV );

		if ( pFoundNode ) return pFoundNode;
		else return pRightNode->AddPoly ( polySizeU, polySizeV );
	}
	else
	{
		if ( iSizeU <= 0 || iSizeV <= 0 ) return 0;

		if ( polySizeU > iSizeU || polySizeV > iSizeV ) return 0;
		else 
		{
			if ( polySizeU/(float)iSizeU > polySizeV/(float)iSizeV )
			{
				pLeftNode = new LMTexNode( iStartU + polySizeU, iStartV, iSizeU - polySizeU, polySizeV );
				pRightNode = new LMTexNode( iStartU, iStartV + polySizeV, iSizeU, iSizeV - polySizeV );
			}
			else
			{
				pLeftNode = new LMTexNode( iStartU, iStartV + polySizeV, polySizeU, iSizeV - polySizeV );
				pRightNode = new LMTexNode( iStartU + polySizeU, iStartV, iSizeU - polySizeU, iSizeV );
			}

			bFilled = true;

			return this;
		}
	}
}