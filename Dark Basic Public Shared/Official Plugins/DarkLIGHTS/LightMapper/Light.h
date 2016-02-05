#ifndef LIGHT_H
#define LIGHT_H

//template for all lights
class Light
{

protected:

	float fRed, fGreen, fBlue;

public:

	static float fAmbientR;
	static float fAmbientG;
	static float fAmbientB;

	Light *pNextLight;

	Light( );
	~Light( );

	//return the origin of the light for ray casting
	virtual void GetOrgin( float posX, float posY, float posZ, float* pOriginPosX, float* pOriginPosY, float* pOriginPosZ ) const = 0;

	//all lights must be able to return the amount of light falling on any given point
	virtual bool GetColorFromPoint( float posX, float posY, float posZ, float* pRed, float* pGreen, float* pBlue, float *pDist ) const = 0;

	virtual bool GetInRange( float posX, float posY, float posZ ) const = 0;
	virtual bool GetInRange( float posX, float posY, float posZ, float fRadius2 ) const = 0;
	virtual Light* Clone( ) = 0;

};

//-----------------------------------------

class PointLight : public Light
{
	
protected:

	float fPosX, fPosY, fPosZ, fRadius, fAttenuation, fAttenuation2;

public:

	PointLight( float posX, float posY, float posZ, float radius, float attenuation, float attenuation2, float red, float green, float blue );
	~PointLight( );

	void GetOrgin( float posX, float posY, float posZ, float* pOriginPosX, float* pOriginPosY, float* pOriginPosZ ) const;
	bool GetColorFromPoint( float posX, float posY, float posZ, float* pRed, float* pGreen, float* pBlue, float *pDist ) const;
	
	bool GetInRange( float posX, float posY, float posZ ) const;
	bool GetInRange( float posX, float posY, float posZ, float fRadius2 ) const;

	Light* Clone( );

};

inline bool PointLight::GetInRange( float posX, float posY, float posZ ) const
{
	return ( (posX-fPosX)*(posX-fPosX) + (posY-fPosY)*(posY-fPosY) + (posZ-fPosZ)*(posZ-fPosZ) < fRadius*fRadius );
}

inline bool PointLight::GetInRange( float posX, float posY, float posZ, float fRadius2 ) const 
{
	return ( (posX-fPosX)*(posX-fPosX) + (posY-fPosY)*(posY-fPosY) + (posZ-fPosZ)*(posZ-fPosZ) < (fRadius+fRadius2)*(fRadius+fRadius2) );
}

inline Light* PointLight::Clone( )
{
	 Light* pNewLight = new PointLight( fPosX, fPosY, fPosZ, fRadius, fAttenuation, fAttenuation2, fRed, fGreen, fBlue );
	 return pNewLight;
}

//-----------------------------------------

class DirLight : public Light
{
	
protected:

	float fDirX, fDirY, fDirZ;

public:

	DirLight( float dirX, float dirY, float dirZ, float red, float green, float blue );
	DirLight( float dirX, float dirY, float dirZ, float red, float green, float blue, bool bNormalised );
	~DirLight( );

	void GetOrgin( float posX, float posY, float posZ, float* pOriginPosX, float* pOriginPosY, float* pOriginPosZ ) const;
	bool GetColorFromPoint( float posX, float posY, float posZ, float* pRed, float* pGreen, float* pBlue, float *pDist ) const;
	bool GetInRange( float posX, float posY, float posZ ) const { return true; }
	bool GetInRange( float posX, float posY, float posZ, float fRadius2 ) const { return true; }

	Light* Clone( );

};

inline Light* DirLight::Clone( )
{
	 Light* pNewLight = new DirLight( fDirX, fDirY, fDirZ, fRed, fGreen, fBlue, true );
	 return pNewLight;
}

//-----------------------------------------

class SpotLight : public Light
{
	
protected:

	float fPosX, fPosY, fPosZ, fDirX, fDirY, fDirZ, fRange, fAng1, fAng2;

public:

	SpotLight( float posX, float posY, float posZ, float dirX, float dirY, float dirZ, float ang1, float ang2, float range, float red, float green, float blue );
	SpotLight( float posX, float posY, float posZ, float dirX, float dirY, float dirZ, float ang1, float ang2, float range, float red, float green, float blue, bool bNormalised );
	~SpotLight( );

	void GetOrgin( float posX, float posY, float posZ, float* pOriginPosX, float* pOriginPosY, float* pOriginPosZ ) const;
	bool GetColorFromPoint( float posX, float posY, float posZ, float* pRed, float* pGreen, float* pBlue, float *pDist ) const;
	bool GetInRange( float posX, float posY, float posZ ) const;
	bool GetInRange( float posX, float posY, float posZ, float fRadius2 ) const;

	Light* Clone( );

};

inline bool SpotLight::GetInRange( float posX, float posY, float posZ ) const 
{
	float fDiffX = posX-fPosX;
	float fDiffY = posY-fPosY;
	float fDiffZ = posZ-fPosZ;
	if ( fDiffX*fDiffX + fDiffY*fDiffY + fDiffZ*fDiffZ > fRange*fRange ) return false;
	if ( fAng2 < 90 && fDiffX*fDirX + fDiffY*fDirY + fDiffZ*fDirZ < 0 ) return false;

	return true;
}

inline bool SpotLight::GetInRange( float posX, float posY, float posZ, float fRadius2 ) const 
{
	return ( (posX-fPosX)*(posX-fPosX) + (posY-fPosY)*(posY-fPosY) + (posZ-fPosZ)*(posZ-fPosZ) < (fRange+fRadius2)*(fRange+fRadius2) );
}

inline Light* SpotLight::Clone( )
{
	 Light* pNewLight = new SpotLight( fPosX, fPosY, fPosZ, fDirX, fDirY, fDirZ, fAng1, fAng2, fRange, fRed, fGreen, fBlue, true );
	 return pNewLight;
}

#endif