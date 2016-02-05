#ifndef LMTEXTURE_H
#define LMTEXTURE_H

#include "d3dx9.h"

class Lumel;
class LMObject;
class LMTexNode;
class Light;
class CollisionTree;

//#include "Thread.h"

class LMTexture //: public Thread
{

private:
	
	int iSizeU, iSizeV;
	Lumel **ppTexLumel;			//the pixels of this texture
	bool bEmpty;

	LMTexNode* pNodeSpace;

	char pFilename[256];
	IDirect3DTexture9 *pTexture;

public:
	IDirect3DTexture9 *pTextureDDS;

private:
	DWORD *pPixels;
	int iPitch;
	UINT iWidth;
	UINT iHeight;

public:

	LMTexture *pNextLMTex;		//the next light map in the list, should get progressively less filled.
	//Light* pLocalLightList;
	//CollisionTree* pLocalColTree;
	
	LMTexture( int sizeU, int sizeV );
	~LMTexture( );

	bool IsEmpty( );

	int GetSizeU( );
	int GetSizeV( );

	bool AddLMObject( LMObject* pLMObject );
	//void CalculateLight ( Light* pLightList, CollisionTree* pColTree );

	void SetLumel( int u, int v, float red, float green, float blue );

	void CopyToTexture( );
	void SaveTexture( char* pFilename );
	IDirect3DTexture9* GetDXTextureRef( );
	char* GetFilename( );

	//unsigned int Run( );
};

#endif