//
// STEREOSCOPICS HEADER
//

// Includes
//#include ".\..\data\cdatac.h"
#include "CCameraDatac.h"

// Prototypes
void StereoscopicConvertLeftRightToBackFront ( int iMode, LPDIRECT3DTEXTURE9 pLeft, LPDIRECT3DTEXTURE9 pRight, LPDIRECT3DSURFACE9 pBack, LPDIRECT3DSURFACE9 pFront );
void StereoscopicConvertLeftToGreySideBySide ( int iMode, LPDIRECT3DTEXTURE9 pLeft, LPDIRECT3DSURFACE9 pGrey, LPDIRECT3DSURFACE9 pSideBySide );
void FreeStereoscopicResources ( void );