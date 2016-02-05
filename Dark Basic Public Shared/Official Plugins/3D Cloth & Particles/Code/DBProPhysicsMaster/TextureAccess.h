#ifndef DBPROPHYSICSMASTER_TEXTURE_ACCESS_H
#define DBPROPHYSICSMASTER_TEXTURE_ACCESS_H

typedef LPDIRECT3DTEXTURE9	( *DLL_GETTEXTUREPOINTER_FP) ( int );

class TextureAccess
{
	friend class PhysicsManager;
public:
	TextureAccess(int textureID):id(textureID),texture(0),pixels(0){}
	bool getTexture();
	void releaseTexture();

	int Width(){if(texture) return textureInfo.Width; else return -1;}
	int Height(){if(texture) return textureInfo.Height; else return -1;}

	unsigned long getPixel(unsigned int x, unsigned int y)
	{
		if(x<0 || x>=textureInfo.Width || y<0 || y>=textureInfo.Height) return 0;
		if(pixels) return pixels[x + y*d3dlr.Pitch/4]; 
		return 0;
	}
	int getRed(unsigned int x, unsigned int y)
	{
		if(x<0 || x>=textureInfo.Width || y<0 || y>=textureInfo.Height) return -1;
		if(pixels) return (int)(((pixels[x + y*d3dlr.Pitch/4])>>16) & 0xff); 
		return -1;
	}
	int getGreen(unsigned int x, unsigned int y)
	{
		if(x<0 || x>=textureInfo.Width || y<0 || y>=textureInfo.Height) return -1;
		if(pixels) return (int)(((pixels[x + y*d3dlr.Pitch/4])>>8) & 0xff); 
		return -1;
	}
	int getBlue(unsigned int x, unsigned int y)
	{
		if(x<0 || x>=textureInfo.Width || y<0 || y>=textureInfo.Height) return -1;
		if(pixels) return (int)((pixels[x + y*d3dlr.Pitch/4]) & 0xff); 
		return -1;
	}

	bool isRedAboveHalf(unsigned int x, unsigned int y)
	{
		if(x<0 || x>=textureInfo.Width || y<0 || y>=textureInfo.Height) return false;
		if( (((pixels[x + y*d3dlr.Pitch/4])>>16) & 0xff) > 127) return true; 
		return false;
	}

private:
	int id;
	LPDIRECT3DTEXTURE9 texture;
	D3DLOCKED_RECT	d3dlr;
	unsigned long *	pixels;
	D3DSURFACE_DESC textureInfo;

	static DLL_GETTEXTUREPOINTER_FP	DLL_GetTexturePointer;
};


#endif