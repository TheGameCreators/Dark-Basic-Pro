
#include "LMGlobal.h"

#include "LMPoly.h"
#include <math.h>

//#include <windows.h>
//#include <stdio.h>

LMPoly::LMPoly( )
{
	d = 0;
	pNextPoly = 0;

	pU1 = 0; pV1 = 0;
	pU2 = 0; pV2 = 0;
	pU3 = 0; pV3 = 0;

	//iStartU = 0;
	//iStartV = 0;
	fMinU = 0;
	fMinV = 0;
//	iSizeU = 0;
//	iSizeV = 0;
	fQuality = 1;

	fArea = -1;
}

LMPoly::~LMPoly( )
{
	
}

void LMPoly::SetVertices( Point *v1, Point *v2, Point *v3 )
{
	vert1[0] = v1->x; vert1[1] = v1->y; vert1[2] = v1->z;
	vert2[0] = v2->x; vert2[1] = v2->y; vert2[2] = v2->z;
	vert3[0] = v3->x; vert3[1] = v3->y; vert3[2] = v3->z;

	iUIndex = 0;
	iVIndex = 1;

	fArea = -1;
}

void LMPoly::SetNormal( Vector *n )
{
	normal[0] = n->x;
	normal[1] = n->y;
	normal[2] = n->z;
}

void LMPoly::Flatten( )
{
	float fMax = fabs( normal[0] );
	iUIndex = 2;
	iVIndex = 1;

	if ( fabs( normal[1] ) > fMax ) 
	{ 
		fMax = fabs( normal[1] ); 
		iUIndex = 0;
		iVIndex = 2;
	}
	if ( fabs( normal[2] ) > fMax ) 
	{ 
		fMax = fabs( normal[2] ); 
		iUIndex = 0;
		iVIndex = 1;
	}
	
	float fMaxU, fMaxV;

	fMinU = vert1[iUIndex]; fMaxU = vert1[iUIndex];
	if ( vert2[iUIndex] < fMinU ) fMinU = vert2[iUIndex];
	if ( vert3[iUIndex] < fMinU ) fMinU = vert3[iUIndex];
	if ( vert2[iUIndex] > fMaxU ) fMaxU = vert2[iUIndex];
	if ( vert3[iUIndex] > fMaxU ) fMaxU = vert3[iUIndex];
	
	fMinV = vert1[iVIndex]; fMaxV = vert1[iVIndex];
	if ( vert2[iVIndex] < fMinV ) fMinV = vert2[iVIndex];
	if ( vert3[iVIndex] < fMinV ) fMinV = vert3[iVIndex];
	if ( vert2[iVIndex] > fMaxV ) fMaxV = vert2[iVIndex];
	if ( vert3[iVIndex] > fMaxV ) fMaxV = vert3[iVIndex];

	cw = ( (vert2[iVIndex]-vert1[iVIndex])*(vert3[iUIndex]-vert1[iUIndex]) - (vert2[iUIndex]-vert1[iUIndex])*(vert3[iVIndex]-vert1[iVIndex]) ) > 0 ? 1 : -1;

	fSizeU = fMaxU - fMinU;
	fSizeV = fMaxV - fMinV;
}

float LMPoly::GetMinU( )
{
	return fMinU;
}

float LMPoly::GetMinV( )
{
	return fMinV;
}

float LMPoly::GetSizeU( )
{
	return fSizeU;
}

float LMPoly::GetSizeV( )
{
	return fSizeV;
}

void LMPoly::GetAvgPoint( float *pPosX, float *pPosY, float *pPosZ )
{
	*pPosX = ( vert1[0] + vert2[0] + vert3[0] ) / 3.0f;
	*pPosY = ( vert1[1] + vert2[1] + vert3[1] ) / 3.0f;
	*pPosZ = ( vert1[2] + vert2[2] + vert3[2] ) / 3.0f;
}

float LMPoly::GetMaxRadius( float fPosX, float fPosY, float fPosZ )
{
	float fDiffX = vert1[0] - fPosX;
	float fDiffY = vert1[1] - fPosY;
	float fDiffZ = vert1[2] - fPosZ;
	float fMaxRadius = fDiffX*fDiffX + fDiffY*fDiffY + fDiffZ*fDiffZ;

	fDiffX = vert2[0] - fPosX;
	fDiffY = vert2[1] - fPosY;
	fDiffZ = vert2[2] - fPosZ;
	float fNewRadius = fDiffX*fDiffX + fDiffY*fDiffY + fDiffZ*fDiffZ;
	if ( fNewRadius > fMaxRadius ) fMaxRadius = fNewRadius;

	fDiffX = vert3[0] - fPosX;
	fDiffY = vert3[1] - fPosY;
	fDiffZ = vert3[2] - fPosZ;
	fNewRadius = fDiffX*fDiffX + fDiffY*fDiffY + fDiffZ*fDiffZ;
	if ( fNewRadius > fMaxRadius ) fMaxRadius = fNewRadius;

	return fMaxRadius;
}

bool LMPoly::GetPoint( float fPosU, float fPosV, float* pPosX, float* pPosY, float* pPosZ )
{
	float fDiffU = fPosU - vert1[iUIndex];
	float fDiffV = fPosV - vert1[iVIndex];

	int iWIndex = 0;
	if ( iUIndex==0 )
	{
		iWIndex = iVIndex==2 ? 1 : 2;
	}

	float fPosW = ( -d - normal[iUIndex]*fPosU - normal[iVIndex]*fPosV ) / normal[iWIndex];

	switch( iWIndex )
	{
		case 0: 
		{
			*pPosX = fPosW;
			*pPosY = fPosV;
			*pPosZ = fPosU;
			return true;
		} break;

		case 1: 
		{
			*pPosX = fPosU;
			*pPosY = fPosW;
			*pPosZ = fPosV;
			return true;
		} break;

		case 2: 
		{
			*pPosX = fPosU;
			*pPosY = fPosV;
			*pPosZ = fPosW;
			return true;
		} break;
	}

	return false;
}

void LMPoly::GetNormal( float fPosX, float fPosY, float fPosZ, float* pNormX, float* pNormY, float* pNormZ )
{
	//Note: for curved surfaces interpolate normal between vertices (LMCurvedPoly)
	
	*pNormX = normal[0];
	*pNormY = normal[1];
	*pNormZ = normal[2];
}

void LMPoly::ExtrapolateNormal( float fPosX, float fPosY, float fPosZ, float* pNormX, float* pNormY, float* pNormZ )
{
	*pNormX = normal[0];
	*pNormY = normal[1];
	*pNormZ = normal[2];
}

void LMPoly::GetFaceNormal( float *pNormX, float *pNormY, float *pNormZ )
{
	*pNormX = normal[0];
	*pNormY = normal[1];
	*pNormZ = normal[2];
}

bool LMPoly::PointInPoly( float fU, float fV )
{
	if ( ((vert2[iVIndex]-vert1[iVIndex])*(fU-vert1[iUIndex]) - (vert2[iUIndex]-vert1[iUIndex])*(fV-vert1[iVIndex]))*cw < -0.0000001f ) return false;
	if ( ((vert3[iVIndex]-vert2[iVIndex])*(fU-vert2[iUIndex]) - (vert3[iUIndex]-vert2[iUIndex])*(fV-vert2[iVIndex]))*cw < -0.0000001f ) return false;
	if ( ((vert1[iVIndex]-vert3[iVIndex])*(fU-vert3[iUIndex]) - (vert1[iUIndex]-vert3[iUIndex])*(fV-vert3[iVIndex]))*cw < -0.0000001f ) return false;
	return true;
}

bool LMPoly::PointInPoly( float fX, float fY, float fZ )
{
	float fU = fX;
	if ( iUIndex == 1 ) fU = fY;
	if ( iUIndex == 2 ) fU = fZ;
	
	float fV = fX;
	if ( iVIndex == 1 ) fV = fY;
	if ( iVIndex == 2 ) fV = fZ;

	return PointInPoly( fU, fV );
}

float LMPoly::SqrDistToPoint( float fU, float fV, float *pPosU, float *pPosV )
{
	float fDiffU = vert2[iUIndex]-vert1[iUIndex];
	float fDiffV = vert2[iVIndex]-vert1[iVIndex];
	float fDist = (fDiffV*(fU-vert1[iUIndex]) - fDiffU*(fV-vert1[iVIndex]))*cw;
	
	if ( fDist < 0 ) 
	{
		float fDist2 = ( fDiffU*(fU-vert1[iUIndex]) + fDiffV*(fV-vert1[iVIndex]) ) / ( fDiffU*fDiffU + fDiffV*fDiffV );
		if ( fDist2 >= 1.0f )
		{
			fDiffU = vert2[iUIndex];
			fDiffV = vert2[iVIndex];
		}
		else if ( fDist2 <= 0.0f )
		{
			fDiffU = vert1[iUIndex];
			fDiffV = vert1[iVIndex];
		}
		else
		{
			fDiffU = vert1[iUIndex] + fDiffU*fDist2;
			fDiffV = vert1[iVIndex] + fDiffV*fDist2;
		}

		if ( pPosU ) *pPosU = fDiffU;
		if ( pPosV ) *pPosV = fDiffV;
		return (fDiffU-fU)*(fDiffU-fU) + (fDiffV-fV)*(fDiffV-fV);
	}

	fDiffU = vert3[iUIndex]-vert2[iUIndex];
	fDiffV = vert3[iVIndex]-vert2[iVIndex];
	fDist = (fDiffV*(fU-vert2[iUIndex]) - fDiffU*(fV-vert2[iVIndex]))*cw;
	
	if ( fDist < 0 ) 
	{
		float fDist2 = ( fDiffU*(fU-vert2[iUIndex]) + fDiffV*(fV-vert2[iVIndex]) ) / ( fDiffU*fDiffU + fDiffV*fDiffV );
		if ( fDist2 >= 1.0f )
		{
			fDiffU = vert3[iUIndex];
			fDiffV = vert3[iVIndex];
		}
		else if ( fDist2 <= 0.0f )
		{
			fDiffU = vert2[iUIndex];
			fDiffV = vert2[iVIndex];
		}
		else
		{
			fDiffU = vert2[iUIndex] + fDiffU*fDist2;
			fDiffV = vert2[iVIndex] + fDiffV*fDist2;
		}

		if ( pPosU ) *pPosU = fDiffU;
		if ( pPosV ) *pPosV = fDiffV;
		return (fDiffU-fU)*(fDiffU-fU) + (fDiffV-fV)*(fDiffV-fV);
	}

	fDiffU = vert1[iUIndex]-vert3[iUIndex];
	fDiffV = vert1[iVIndex]-vert3[iVIndex];
	fDist = (fDiffV*(fU-vert3[iUIndex]) - fDiffU*(fV-vert3[iVIndex]))*cw;
	
	if ( fDist < 0 ) 
	{
		float fDist2 = ( fDiffU*(fU-vert3[iUIndex]) + fDiffV*(fV-vert3[iVIndex]) ) / ( fDiffU*fDiffU + fDiffV*fDiffV );
		if ( fDist2 >= 1.0f )
		{
			fDiffU = vert1[iUIndex];
			fDiffV = vert1[iVIndex];
		}
		else if ( fDist2 <= 0.0f )
		{
			fDiffU = vert3[iUIndex];
			fDiffV = vert3[iVIndex];
		}
		else
		{
			fDiffU = vert3[iUIndex] + fDiffU*fDist2;
			fDiffV = vert3[iVIndex] + fDiffV*fDist2;
		}

		if ( pPosU ) *pPosU = fDiffU;
		if ( pPosV ) *pPosV = fDiffV;
		return (fDiffU-fU)*(fDiffU-fU) + (fDiffV-fV)*(fDiffV-fV);
	}

	if ( pPosU ) *pPosU = fU;
	if ( pPosV ) *pPosV = fV;
	return 0.0f;
}

float LMPoly::SqrDistToPoint( float fX, float fY, float fZ, float *pPosU, float *pPosV )
{
	float fU = fX;
	if ( iUIndex == 1 ) fU = fY;
	if ( iUIndex == 2 ) fU = fZ;
	
	float fV = fX;
	if ( iVIndex == 1 ) fV = fY;
	if ( iVIndex == 2 ) fV = fZ;

	return SqrDistToPoint( fU, fV, pPosU, pPosV );
}

float LMPoly::GetArea( )
{
	if ( fArea < 0 ) CalculateArea( );
	return fArea;
}

void LMPoly::CalculateArea( )
{
	float fDiffU1 = vert2[iUIndex] - vert1[iUIndex];
	float fDiffV1 = vert2[iVIndex] - vert1[iVIndex];
	float fDist1 = fDiffU1*fDiffU1 + fDiffV1*fDiffV1;

	float fDiffU2 = vert3[iUIndex] - vert1[iUIndex];
	float fDiffV2 = vert3[iVIndex] - vert1[iVIndex];

	float fDotP = ( fDiffU1*fDiffU2 + fDiffV1*fDiffV2 ) / fDist1;

	fDiffU1 = fDiffU2 - fDotP*fDiffU1;
	fDiffV1 = fDiffV2 - fDotP*fDiffV1;
	float fDistT = fDiffU1*fDiffU1 + fDiffV1*fDiffV1;

	fArea = ( sqrt (fDistT) * sqrt (fDist1) ) / 2.0f;
}

bool LMPoly::Joined( LMPoly* pOtherPoly )
{
	if ( iUIndex != pOtherPoly->iUIndex || iVIndex != pOtherPoly->iVIndex ) return false;
	if ( cw != pOtherPoly->cw ) return false;
	if ( normal[0]*pOtherPoly->normal[0] + normal[1]*pOtherPoly->normal[1] + normal[2]*pOtherPoly->normal[2] < 0.99995f ) return false;
	
	int iCount = 0;
	float fMinDist = 0.001f;

	float fDiffX = fabs( vert1[0] - pOtherPoly->vert1[0] );
	float fDiffY = fabs( vert1[1] - pOtherPoly->vert1[1] );
	float fDiffZ = fabs( vert1[2] - pOtherPoly->vert1[2] );
	if ( fDiffX*fDiffX + fDiffY*fDiffY + fDiffZ*fDiffZ < fMinDist ) iCount++;
	else
	{
		fDiffX = fabs( vert1[0] - pOtherPoly->vert2[0] );
		fDiffY = fabs( vert1[1] - pOtherPoly->vert2[1] );
		fDiffZ = fabs( vert1[2] - pOtherPoly->vert2[2] );
		if ( fDiffX*fDiffX + fDiffY*fDiffY + fDiffZ*fDiffZ < fMinDist ) iCount++;
		else
		{
			fDiffX = fabs( vert1[0] - pOtherPoly->vert3[0] );
			fDiffY = fabs( vert1[1] - pOtherPoly->vert3[1] );
			fDiffZ = fabs( vert1[2] - pOtherPoly->vert3[2] );
			if ( fDiffX*fDiffX + fDiffY*fDiffY + fDiffZ*fDiffZ < fMinDist ) iCount++;
		}
	}

	fDiffX = fabs( vert2[0] - pOtherPoly->vert1[0] );
	fDiffY = fabs( vert2[1] - pOtherPoly->vert1[1] );
	fDiffZ = fabs( vert2[2] - pOtherPoly->vert1[2] );
	if ( fDiffX*fDiffX + fDiffY*fDiffY + fDiffZ*fDiffZ < fMinDist ) iCount++;
	else
	{
		fDiffX = fabs( vert2[0] - pOtherPoly->vert2[0] );
		fDiffY = fabs( vert2[1] - pOtherPoly->vert2[1] );
		fDiffZ = fabs( vert2[2] - pOtherPoly->vert2[2] );
		if ( fDiffX*fDiffX + fDiffY*fDiffY + fDiffZ*fDiffZ < fMinDist ) iCount++;
		else
		{
			fDiffX = fabs( vert2[0] - pOtherPoly->vert3[0] );
			fDiffY = fabs( vert2[1] - pOtherPoly->vert3[1] );
			fDiffZ = fabs( vert2[2] - pOtherPoly->vert3[2] );
			if ( fDiffX*fDiffX + fDiffY*fDiffY + fDiffZ*fDiffZ < fMinDist ) iCount++;
		}
	}

	fDiffX = fabs( vert3[0] - pOtherPoly->vert1[0] );
	fDiffY = fabs( vert3[1] - pOtherPoly->vert1[1] );
	fDiffZ = fabs( vert3[2] - pOtherPoly->vert1[2] );
	if ( fDiffX*fDiffX + fDiffY*fDiffY + fDiffZ*fDiffZ < fMinDist ) iCount++;
	else
	{
		fDiffX = fabs( vert3[0] - pOtherPoly->vert2[0] );
		fDiffY = fabs( vert3[1] - pOtherPoly->vert2[1] );
		fDiffZ = fabs( vert3[2] - pOtherPoly->vert2[2] );
		if ( fDiffX*fDiffX + fDiffY*fDiffY + fDiffZ*fDiffZ < fMinDist ) iCount++;
		else
		{
			fDiffX = fabs( vert3[0] - pOtherPoly->vert3[0] );
			fDiffY = fabs( vert3[1] - pOtherPoly->vert3[1] );
			fDiffZ = fabs( vert3[2] - pOtherPoly->vert3[2] );
			if ( fDiffX*fDiffX + fDiffY*fDiffY + fDiffZ*fDiffZ < fMinDist ) iCount++;
		}
	}

	if ( iCount == 2 ) return true;
	else return false;
}

void LMPoly::CalculateTexUV( int iStartU, int iStartV, float fMinU2, float fMinV2, int iTexSizeU, int iTexSizeV )
{
	float fDiffU = (vert1[iUIndex] - fMinU2) / fQuality;
	float fDiffV = (vert1[iVIndex] - fMinV2) / fQuality;

	*pU1 = (fDiffU + iStartU + 0.0f) / iTexSizeU;
	*pV1 = (fDiffV + iStartV + 0.0f) / iTexSizeV;

	fDiffU = (vert2[iUIndex] - fMinU2) / fQuality;
	fDiffV = (vert2[iVIndex] - fMinV2) / fQuality;

	*pU2 = (fDiffU + iStartU + 0.0f) / iTexSizeU;
	*pV2 = (fDiffV + iStartV + 0.0f) / iTexSizeV;

	fDiffU = (vert3[iUIndex] - fMinU2) / fQuality;
	fDiffV = (vert3[iVIndex] - fMinV2) / fQuality;

	*pU3 = (fDiffU + iStartU + 0.0f) / iTexSizeU;
	*pV3 = (fDiffV + iStartV + 0.0f) / iTexSizeV;
}

void LMCurvedPoly::SetVertexNormals( Vector *nv1, Vector *nv2, Vector *nv3 )
{
	normalv1[0] = nv1->x;
	normalv1[1] = nv1->y;
	normalv1[2] = nv1->z;

	normalv2[0] = nv2->x;
	normalv2[1] = nv2->y;
	normalv2[2] = nv2->z;

	normalv3[0] = nv3->x;
	normalv3[1] = nv3->y;
	normalv3[2] = nv3->z;
}

void LMCurvedPoly::GetNormal( float fPosX, float fPosY, float fPosZ, float *fNormX, float *fNormY, float *fNormZ )
{
	float fDiffX = vert1[0] - fPosX;
	float fDiffY = vert1[1] - fPosY;
	float fDiffZ = vert1[2] - fPosZ;
	float fDist1 = fDiffX*fDiffX + fDiffY*fDiffY + fDiffZ*fDiffZ;

	fDiffX = vert2[0] - fPosX;
	fDiffY = vert2[1] - fPosY;
	fDiffZ = vert2[2] - fPosZ;
	float fDist2 = fDiffX*fDiffX + fDiffY*fDiffY + fDiffZ*fDiffZ;

	fDiffX = vert3[0] - fPosX;
	fDiffY = vert3[1] - fPosY;
	fDiffZ = vert3[2] - fPosZ;
	float fDist3 = fDiffX*fDiffX + fDiffY*fDiffY + fDiffZ*fDiffZ;

	float* pVertA = vert1;
	float* pVertB = vert2;
	float* pVertC = vert3;
	float* fNormalA = normalv1;
	float* fNormalB = normalv2;
	float* fNormalC = normalv3;

	if ( fDist2 >= fDist1 && fDist2 >= fDist3 )
	{
		pVertA = vert2; pVertB = vert3; pVertC = vert1;
		fNormalA = normalv2;
		fNormalB = normalv3;
		fNormalC = normalv1;
	}

	if ( fDist3 >= fDist1 && fDist3 >= fDist2 )
	{
		pVertA = vert3; pVertB = vert1; pVertC = vert2;
		fNormalA = normalv3;
		fNormalB = normalv1;
		fNormalC = normalv2;
	}

	Vector BCNormal;
	fDiffX = pVertC[0]-pVertB[0];
	fDiffY = pVertC[1]-pVertB[1];
	fDiffZ = pVertC[2]-pVertB[2];
	BCNormal.set( fDiffX, fDiffY, fDiffZ );
	BCNormal = BCNormal.crossProduct( normal[0], normal[1], normal[2] ); //(B->C) x (face normal) = BC edge normal
	//BCNormal.normalise( );

	//fDist1 = BCNormal.x*pvertB[0] + BCNormal.y*pvertB[1] + BCNormal.z*pvertB[2];
	fDist1 = BCNormal.x*(pVertA[0]-pVertB[0]) + BCNormal.y*(pVertA[1]-pVertB[1]) + BCNormal.z*(pVertA[2]-pVertB[2]);
	fDist2 = (pVertA[0] - fPosX)*BCNormal.x + (pVertA[1] - fPosY)*BCNormal.y + (pVertA[2] - fPosZ)*BCNormal.z;
	fDist1 = fDist1 / fDist2;

	Point intersect;
	intersect.x = pVertA[0] + (fPosX - pVertA[0])*fDist1;
	intersect.y = pVertA[1] + (fPosY - pVertA[1])*fDist1;
	intersect.z = pVertA[2] + (fPosZ - pVertA[2])*fDist1;

	fDist1 = (intersect.x - pVertB[0])*fDiffX + (intersect.y - pVertB[1])*fDiffY + (intersect.z - pVertB[2])*fDiffZ;
	fDist1 /= ( fDiffX*fDiffX + fDiffY*fDiffY + fDiffZ*fDiffZ );

	float fNormalBCX, fNormalBCY, fNormalBCZ;

	if ( fDist1 > 2 ) fDist1 = 2;
	if ( fDist1 < -1 ) fDist1 = -1;

	if ( fDist1 > 1 )
	{
		fDist1 = 2 - fDist1;
		fDist2 = fNormalC[0]*fNormalC[0] + fNormalC[1]*fNormalC[1] + fNormalC[2]*fNormalC[2];
		fDist2 = 2*( fNormalB[0]*fNormalC[0] + fNormalB[1]*fNormalC[1] + fNormalB[2]*fNormalC[2] ) / fDist2;
		fNormalBCX = (fNormalC[0]*fDist2 - fNormalB[0])*(1-fDist1) + fNormalC[0]*(fDist1);
		fNormalBCY = (fNormalC[1]*fDist2 - fNormalB[1])*(1-fDist1) + fNormalC[1]*(fDist1);
		fNormalBCZ = (fNormalC[2]*fDist2 - fNormalB[2])*(1-fDist1) + fNormalC[2]*(fDist1);
	}
	else if ( fDist1 < 0 )
	{
		fDist1 = -fDist1;
		fDist2 = fNormalB[0]*fNormalB[0] + fNormalB[1]*fNormalB[1] + fNormalB[2]*fNormalB[2];
		fDist2 = 2*( fNormalB[0]*fNormalC[0] + fNormalB[1]*fNormalC[1] + fNormalB[2]*fNormalC[2] ) / fDist2;
		fNormalBCX = fNormalB[0]*(1-fDist1) + (fNormalB[0]*fDist2 - fNormalC[0])*(fDist1);
		fNormalBCY = fNormalB[1]*(1-fDist1) + (fNormalB[1]*fDist2 - fNormalC[1])*(fDist1);
		fNormalBCZ = fNormalB[2]*(1-fDist1) + (fNormalB[2]*fDist2 - fNormalC[2])*(fDist1);
	}
	else
	{
		//interpolate along edge B->C
		fNormalBCX = fNormalB[0]*(1-fDist1) + fNormalC[0]*(fDist1);
		fNormalBCY = fNormalB[1]*(1-fDist1) + fNormalC[1]*(fDist1);
		fNormalBCZ = fNormalB[2]*(1-fDist1) + fNormalC[2]*(fDist1);
	}

	fDist1 = sqrt( fNormalBCX*fNormalBCX + fNormalBCY*fNormalBCY + fNormalBCZ*fNormalBCZ );
	fNormalBCX /= fDist1;
	fNormalBCY /= fDist1;
	fNormalBCZ /= fDist1;

	fDiffX = intersect.x - pVertA[0];
	fDiffY = intersect.y - pVertA[1];
	fDiffZ = intersect.z - pVertA[2];
	fDist1 = (fPosX - pVertA[0])*fDiffX + (fPosY - pVertA[1])*fDiffY + (fPosZ - pVertA[2])*fDiffZ;
	fDist1 /= ( fDiffX*fDiffX + fDiffY*fDiffY + fDiffZ*fDiffZ );

	if ( fDist1 > 2 ) fDist1 = 2;
	if ( fDist1 < -1 ) fDist1 = -1;

	if ( fDist1 > 1 )
	{
		fDist1 = 2 - fDist1;
		fDist2 = fNormalBCX*fNormalBCX + fNormalBCY*fNormalBCY + fNormalBCZ*fNormalBCZ;
		fDist2 = 2*( fNormalA[0]*fNormalBCX + fNormalA[1]*fNormalBCY + fNormalA[2]*fNormalBCZ ) / fDist2;
		fDiffX = (fNormalBCX*fDist2 - fNormalA[0])*(1-fDist1) + fNormalBCX*(fDist1);
		fDiffY = (fNormalBCY*fDist2 - fNormalA[1])*(1-fDist1) + fNormalBCY*(fDist1);
		fDiffZ = (fNormalBCZ*fDist2 - fNormalA[2])*(1-fDist1) + fNormalBCZ*(fDist1);
	}
	else if ( fDist1 < 0 )
	{
		fDist1 = -fDist1;
		fDist2 = fNormalA[0]*fNormalA[0] + fNormalA[1]*fNormalA[1] + fNormalA[2]*fNormalA[2];
		fDist2 = 2*( fNormalA[0]*fNormalBCX + fNormalA[1]*fNormalBCY + fNormalA[2]*fNormalBCZ ) / fDist2;
		fDiffX = fNormalA[0]*(1-fDist1) + (fNormalA[0]*fDist2 - fNormalBCX)*(fDist1);
		fDiffY = fNormalA[1]*(1-fDist1) + (fNormalA[1]*fDist2 - fNormalBCY)*(fDist1);
		fDiffZ = fNormalA[2]*(1-fDist1) + (fNormalA[2]*fDist2 - fNormalBCZ)*(fDist1);
	}
	else
	{
		//interpolate between Vert A and edge BC
		fDiffX = fNormalA[0]*(1-fDist1) + fNormalBCX*(fDist1);
		fDiffY = fNormalA[1]*(1-fDist1) + fNormalBCY*(fDist1);
		fDiffZ = fNormalA[2]*(1-fDist1) + fNormalBCZ*(fDist1);
	}

	fDist1 = sqrt( fDiffX*fDiffX + fDiffY*fDiffY + fDiffZ*fDiffZ );

	*fNormX = fDiffX / fDist1;
	*fNormY = fDiffY / fDist1;
	*fNormZ = fDiffZ / fDist1;
}

void LMCurvedPoly::ExtrapolateNormal( float fPosX, float fPosY, float fPosZ, float *fNormX, float *fNormY, float *fNormZ )
{
	GetNormal( fPosX, fPosY, fPosZ, fNormX, fNormY, fNormZ );
}