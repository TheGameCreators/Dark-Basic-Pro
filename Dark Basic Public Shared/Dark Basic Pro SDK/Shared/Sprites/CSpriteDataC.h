#ifndef _CSPRITEDATA_H_
#define _CSPRITEDATA_H_

struct VERTEX2D
{
	float	x;
	float	y;
	float	z;

	float	rhw;

	DWORD	color;

	float	tu;
	float	tv;
};

enum eAnimType
{
	SEPARATE_IMAGES,
	ONE_IMAGE,
};

#define D3DFVF_VERTEX2D ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

struct tagSpriteData
{
	float					fX;				// x position float for MOVE accuracy
	float					fY;				// y position float for MOVE accuracy

	int						iAlpha;
	int						iRed;
	int						iGreen;
	int						iBlue;

	bool					bUpdateVertices;

	int						iXOffset;
	int						iYOffset;

	int						iWidth;
	int						iHeight;
	float					fClipU;
	float					fClipV;

	float					fXScale;
	float					fYScale;
	bool					bTransparent;
	bool					bGhostMode;
	int						iXSize;
	int						iYSize;

	float					fAngle;

	int						iPriority;

	LPDIRECT3DTEXTURE9		lpTexture;		// pointer to texture data
	int						iImage;			// image id

	bool					bVisible;
	bool					bFlipped;
	bool					bMirrored;

	// collision support data
	int						iHitoverlapstore;

	// animation data
	bool					bIsAnim;
	eAnimType				eAnimType;		// anim type - separate images or one image
	int						iStart;			// start image
	int						iEnd;			// end image
	DWORD					dwLastTime;		// last time recorded
	DWORD					dwCurrentTime;	// current time
	int						iDelay;			// delay between images

	// animation data for use with frames on 1 image
	int						iFrameWidth;
	int						iFrameHeight;
	int						iFrameCount;
	int						iFrameAcross;
	int						iFrameDown;
	int						iFrame;

	// mike - 220604 - frame animation addition
	int						iLastFrame;
	
	// internal data
	VERTEX2D				lpVertices [ 4 ];	// transformed vertex data
};

#endif _CSPRITEDATA_H_
