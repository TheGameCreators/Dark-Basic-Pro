#ifndef LMPOLYGROUP_H
#define LMPOLYGROUP_H

#include "Thread.h"

class LMPoly;
class Lumel;
class CollisionTree;
class Light;
class LMTexture;
class Point;

class LMPolyGroup //: public Thread
{
	
private:

	static int iPixelBorder;
	static Point *pRandomPoints;
	static float *pRandomDist;
	static int iIterations;
	static float fAmbientDistance;
	static bool bAmbientOcclusion;
	static int iAmbientPattern;

	const Light* pLocalLightList;
	const CollisionTree* pLocalColTree;
	int iLocalBlur;

	float fSizeU;
	float fSizeV;
	float fOrigSizeU;
	float fOrigSizeV;

	float fMinU;
	float fMinV;
	float fOrigMinU;
	float fOrigMinV;

	int iSizeU;
	int iSizeV;

	int iStartU;
	int iStartV;

	Lumel **ppLumel;		//stores the final pixel colour over all lights
	Lumel **ppColour;		//stores the pixel colour for the current light
	float **ppShadow;		//stores the shadow for the current light, multiplied by colour before adding
	//float **ppShadowBlur;	//stores the shadow whilst blurring

	bool GetPoint( float fPosU, float fPosV, float* fPosX, float* fPosY, float* fPosZ );
	bool GetPoint( int iPosU, int iPosV, float* fPosX, float* fPosY, float* fPosZ ); //converts texture UV into world XYZ position
	void GetNormal( float fPosX, float fPosY, float fPosZ, float* fNormX, float* fNormY, float* fNormZ ); //converts texture UV into world XYZ normal
	void ExtrapolateNormal( float fPosX, float fPosY, float fPosZ, float* fNormX, float* fNormY, float* fNormZ ); //for points outside polygons

	void GetFaceNormal( int iPosU, int iPosV, float *fNormX, float *fNormY, float *fNormZ );

	//unsigned int Run( );

public:

	static int iMode;
	static float fShadowPower;
	static float fCurvedBoost;
	static float fMaxCurvedBoostSize;

	static void SetAmbientOcclusionOn( int iterations, float fRayDist, int iPattern );
	static void SetAmbientOcclusionOff( );

	float fQuality;

	LMPoly *pPolyList;
	LMPolyGroup *pNextGroup;

	LMPolyGroup( );
	~LMPolyGroup( );

	static void SetPixelBorder( int iNewBorder ) { iPixelBorder = iNewBorder; }

	void AddPoly( LMPoly *pNewPoly );
	void SetStartPoint( int u, int v );
	void Scale( float fQuality );

	int GetScaledSizeU( );
	int GetScaledSizeV( );

	float GetSizeU( );
	float GetSizeV( );

	void CalculatePositionRadius( float *pPosX, float *pPosY, float *pPosZ, float *pSqrRadius );

	bool PointInGroup( int iU, int iV );
	bool Joined( LMPoly *pOtherPoly );
	LMPoly* GetClosestPoint( int iU, int iV, float *pPosU, float *pPosV );

	float GetUsedSpace( ); //0.0 for empty, 1.0 for 100% usage
	float GetNewUsedSpace( LMPoly *pNewPoly ); //returns how much space would be used if the poly was included

	void SetLocalValues( const Light *pLightList, const CollisionTree *pColTree, int iBlur );

	void CalculateLight( const Light *pLightList, const CollisionTree *pColTree, int iBlur );
	void ApplyToTexture( LMTexture *pTexture );
	void CalculateTexUV( int iTexSizeU, int iTexSizeV );
	unsigned long GetColour( int u, int v );
	void GetColour( int u, int v, float* red, float* green, float* blue );

};

#endif
