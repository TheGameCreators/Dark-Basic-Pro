#include "stdafx.h"

DLL_GETTEXTUREPOINTER_FP TextureAccess::DLL_GetTexturePointer=0;

bool TextureAccess::getTexture()
{
	// get pointer to texture
	texture = DLL_GetTexturePointer(id);
	if(!texture) return false;
	
	// texture info
	texture->GetLevelDesc(0,&textureInfo);
	texture->LockRect(0, &d3dlr, 0, 0);
	pixels = (unsigned long *)d3dlr.pBits;
	return true;
}


void TextureAccess::releaseTexture()
{
	texture->UnlockRect(0);	
}

