#ifndef _CSG_H_
#define _CSG_H_

#include <d3dx9.h>
#include <stdio.h>					
#include <d3d9.h>
#include <d3dx9.h>
#include <mmsystem.h>
#include <time.h>

#define SCREENWIDTH				800
#define SCREENHEIGHT			600
#define VIEWMODE_FIRSTPERSON	0
#define VIEWMODE_TOPDOWN		1

#define CSG_NONE				0
#define CSG_DIFFERENCE			1
#define CSG_INTERSECTION		2
#define CSG_UNION				3

#define D3DFVF_VERTEX ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 )

const FLOAT g_PI       =  3.14159265358979323846f;
const FLOAT g_DEGTORAD =  0.01745329251994329547f;
//const FLOAT g_EPSILON  =  0.001f;
//const FLOAT g_EPSILON  =  0.01f;
//const FLOAT g_EPSILON  =  0.1f;
extern FLOAT g_EPSILON;

struct D3DVERTEX
{
    float		x, y, z;
	D3DXVECTOR3 Normal;
	float		tu , tv;
};

struct POLYGON
{
	D3DVERTEX*	Vertices;
	WORD*		Indices;
	WORD		VertexCount;
	WORD		IndexCount;
	D3DXVECTOR3 Normal;
	int			TextureIndex;
};

struct BBOX
{
	D3DXVECTOR3 Min;
	D3DXVECTOR3 Max;
};

struct BRUSH
{
	BBOX		Bounds;
	POLYGON*	Faces;
	ULONG		FaceCount;
	D3DXMATRIX	Matrix;
	void*		BSPTree;
};

extern LPDIRECT3DTEXTURE9*		lpTextureSurface;
extern LPDIRECT3DDEVICE9		lpDevice;
extern BOOL						g_bCompileFinal;
extern BOOL						g_bSafeTransform;
extern BOOL						g_bWireFrame;
extern int						g_iLastTexture;
extern int						g_iCSGBrush;
extern int						g_iCSGMode;	
extern int						NumberOfTextures;

D3DXVECTOR3 CalculatePolyNormal  ( D3DVERTEX v1, D3DVERTEX v2, D3DVERTEX v3, D3DXMATRIX* Matrix );
BOOL		EpsilonCompareVector ( D3DXVECTOR3 Vec1, D3DXVECTOR3 Vec2 ) ;


#endif // _CSG_H_
