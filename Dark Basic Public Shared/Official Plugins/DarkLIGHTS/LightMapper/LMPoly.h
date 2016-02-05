#ifndef LMPOLY_H
#define LMPOLY_H

#include "LMGlobal.h"

#include "Point.h"
#include "Vector.h"

class LMPoly
{

protected:

	float fMinU;
	float fMinV;
	float fSizeU;
	float fSizeV;

	float vert1[3];
	float vert2[3];
	float vert3[3];
	float normal[3];

	int iUIndex;
	int iVIndex;

	float fArea;

	//int iStartU;
	//int iStartV;
	//int iSizeU;
	//int iSizeV;
	
public:

	float d;
	int cw;

	DWORD colour;
	
	float *pU1, *pV1;
	float *pU2, *pV2;
	float *pU3, *pV3;

	float fQuality;

	LMPoly *pNextPoly;

	LMPoly( );
	virtual ~LMPoly( );

	virtual int GetType( ) { return 0; }
	virtual bool IsCurved( ) { return false; }

	void SetVertices( Point* v1, Point* v2, Point* v3 );
	void SetNormal( Vector* n );
	void SetStartPoint( int u, int v );

	void Flatten( );
	//void Scale( float fQuality );

	float GetMinU( );
	float GetMinV( );
	float GetSizeU( );
	float GetSizeV( );

	void GetAvgPoint( float *pPosX, float *pPosY, float *pPosZ );
	float GetMaxRadius( float fPosX, float fPosY, float fPosZ );

	bool GetPoint( float fPosU, float fPosV, float* fPosX, float* fPosY, float* fPosZ ); //converts texture UV into world XYZ position
	virtual void GetNormal( float fPosX, float fPosY, float fPosZ, float* fNormX, float* fNormY, float* fNormZ ); //converts world XYZ into world XYZ normal, point assumed on plane
	virtual void ExtrapolateNormal( float fPosX, float fPosY, float fPosZ, float* fNormX, float* fNormY, float* fNormZ );

	void GetFaceNormal( float *fNormX, float *fNormY, float *fNormZ );

	bool PointInPoly( float fU, float fV );
	bool PointInPoly( float fX, float fY, float fZ );
	float SqrDistToPoint( float fU, float fV, float *pPosU, float *pPosV );
	float SqrDistToPoint( float fX, float fY, float fZ, float *pPosU, float *pPosV );

	float GetArea( );
	void CalculateArea( );
	bool Joined( LMPoly* pOtherPoly );

	void CalculateTexUV( int iStartU, int iStartV, float fMinU, float fMinV, int iTexSizeU, int iTexSizeV );
};

class LMCurvedPoly : public LMPoly
{

private:

	float normalv1[3];
	float normalv2[3];
	float normalv3[3];

public:

	LMCurvedPoly( ) { }
	~LMCurvedPoly( ) { }

	int GetType( ) { return 1; }
	bool IsCurved( ) { return true; }

	void SetVertexNormals( Vector *nv1, Vector *nv2, Vector *nv3 );
	void GetNormal( float fPosX, float fPosY, float fPosZ, float* fNormX, float* fNormY, float* fNormZ );
	void ExtrapolateNormal( float fPosX, float fPosY, float fPosZ, float* fNormX, float* fNormY, float* fNormZ );
};


#endif