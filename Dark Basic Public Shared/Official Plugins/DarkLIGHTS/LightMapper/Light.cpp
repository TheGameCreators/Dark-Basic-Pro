#include "Light.h"

#include <math.h>

//ambient light to fake radiosity
float Light::fAmbientR = 0;
float Light::fAmbientG = 0;
float Light::fAmbientB = 0;

Light::Light( )
{

}

Light::~Light( )
{
	
}

PointLight::PointLight( float posX, float posY, float posZ, float radius, float attenuation, float attenuation2, float red, float green, float blue )
{
	fPosX = posX;
	fPosY = posY;
	fPosZ = posZ;

	fAttenuation = attenuation;
	fAttenuation2 = attenuation2;

	if ( radius < 0.001f ) radius = 0.001f;
	fRadius = radius;

	fRed = red;
	fGreen = green;
	fBlue = blue;
}

PointLight::~PointLight( )
{
	
}

bool PointLight::GetColorFromPoint( float posX, float posY, float posZ, float* pRed, float* pGreen, float* pBlue, float *pDist ) const
{
	float fDist = (posX-fPosX)*(posX-fPosX) + (posY-fPosY)*(posY-fPosY) + (posZ-fPosZ)*(posZ-fPosZ);
	float fFade;

	if ( fDist > fRadius*fRadius )
	{
		*pRed = 0; *pGreen = 0; *pBlue = 0;
		return false;
	}

	//fFade = -0.5f*log(fDist/(fRadius));
	//fFade = 1 / (1 + fAttenuation*fDist);

	*pDist = sqrt( fDist );

	//squared attenuation
	fFade = 1 / (1 + fAttenuation2*fDist);
	
	if ( fAttenuation > 0 ) fFade = fFade * (-(*pDist)/fAttenuation + 1 );
	if ( fFade > 1.0f ) fFade = 1.0f;
	if ( fFade < 0.0f ) fFade = 0.0f;

	*pRed = fRed*fFade;
	*pGreen = fGreen*fFade;
	*pBlue = fBlue*fFade;

	return true;
}

void PointLight::GetOrgin(float posX, float posY, float posZ, float *pOriginPosX, float *pOriginPosY, float *pOriginPosZ) const
{
	*pOriginPosX = fPosX;
	*pOriginPosY = fPosY;
	*pOriginPosZ = fPosZ;
}

DirLight::DirLight( float dirX, float dirY, float dirZ, float red, float green, float blue )
{
	float fDist = sqrt ( dirX*dirX + dirY*dirY + dirZ*dirZ );
	if ( fDist < 0.000001f )
	{
		fDirX = 0; fDirY = -1; fDirZ = 0;
	}
	else
	{
		fDirX = dirX/fDist;
		fDirY = dirY/fDist;
		fDirZ = dirZ/fDist;
	}

	fRed = red;
	fGreen = green;
	fBlue = blue;
}

DirLight::DirLight( float dirX, float dirY, float dirZ, float red, float green, float blue, bool bNormalised )
{
	if ( bNormalised )
	{
		fDirX = dirX;
		fDirY = dirY;
		fDirZ = dirZ;
	}
	else 
	{
		float fDist = sqrt ( dirX*dirX + dirY*dirY + dirZ*dirZ );
		if ( fDist < 0.000001f )
		{
			fDirX = 0; fDirY = -1; fDirZ = 0;
		}
		else
		{
			fDirX = dirX/fDist;
			fDirY = dirY/fDist;
			fDirZ = dirZ/fDist;
		}
	}
	
	fRed = red;
	fGreen = green;
	fBlue = blue;
}

DirLight::~DirLight( )
{
	
}

bool DirLight::GetColorFromPoint( float posX, float posY, float posZ, float *pRed, float *pGreen, float *pBlue, float *pDist ) const
{
	//no attenuation
	*pRed = fRed;
	*pGreen = fGreen;
	*pBlue = fBlue;
	*pDist = 173205.0807569f;
	return true;
}

void DirLight::GetOrgin(float posX, float posY, float posZ, float *pOriginPosX, float *pOriginPosY, float *pOriginPosZ) const
{
	//choose a parallel point very far away
	*pOriginPosX = posX - fDirX*100000.0f;
	*pOriginPosY = posY - fDirY*100000.0f;
	*pOriginPosZ = posZ - fDirZ*100000.0f;
}

SpotLight::SpotLight(float posX, float posY, float posZ, float dirX, float dirY, float dirZ, float ang1, float ang2, float range, float red, float green, float blue)
{
	fPosX = posX;
	fPosY = posY;
	fPosZ = posZ;

	float fDist = sqrt ( dirX*dirX + dirY*dirY + dirZ*dirZ );
	if ( fDist < 0.000001f )
	{
		fDirX = 0; fDirY = -1; fDirZ = 0;
	}
	else
	{
		fDirX = dirX/fDist;
		fDirY = dirY/fDist;
		fDirZ = dirZ/fDist;
	}

	if ( range < 0.0001f ) range = 0.0001f;
	fRange = range;

	if ( ang2 > 360 ) ang2 = 360;
	if ( ang1 < 0 ) ang1 = 0;
	if ( ang1 > ang2 ) ang1 = ang2;
	fAng1 = ang1/2.0f;
	fAng2 = ang2/2.0f;

	fRed = red;
	fGreen = green;
	fBlue = blue;
}

SpotLight::SpotLight(float posX, float posY, float posZ, float dirX, float dirY, float dirZ, float ang1, float ang2, float range, float red, float green, float blue, bool bNormalised)
{
	fPosX = posX;
	fPosY = posY;
	fPosZ = posZ;

	if ( bNormalised )
	{
		fDirX = dirX;
		fDirY = dirY;
		fDirZ = dirZ;

		fAng1 = ang1;
		fAng2 = ang2;
	}
	else
	{
		float fDist = sqrt ( dirX*dirX + dirY*dirY + dirZ*dirZ );
		if ( fDist < 0.000001f )
		{
			fDirX = 0; fDirY = -1; fDirZ = 0;
		}
		else
		{
			fDirX = dirX/fDist;
			fDirY = dirY/fDist;
			fDirZ = dirZ/fDist;
		}

		if ( ang2 > 360 ) ang2 = 360;
		if ( ang1 < 0 ) ang1 = 0;
		if ( ang2 < ang1 ) ang2 = ang1;

		fAng1 = ang1/2.0f;
		fAng2 = ang2/2.0f;
	}

	if ( range < 0.0001f ) range = 0.0001f;
	fRange = range;
	
	fRed = red;
	fGreen = green;
	fBlue = blue;
}

SpotLight::~SpotLight( )
{
	
}

bool SpotLight::GetColorFromPoint( float posX, float posY, float posZ, float *pRed, float *pGreen, float *pBlue, float *pDist ) const
{
	float fDiffX = posX-fPosX;
	float fDiffY = posY-fPosY;
	float fDiffZ = posZ-fPosZ;
	float fDist = fDiffX*fDiffX + fDiffY*fDiffY + fDiffZ*fDiffZ;
	if ( fDist > fRange*fRange )
	{
		//no light hits point
		*pRed = 0; *pGreen = 0; *pBlue = 0;
		return false;
	}

	*pDist = sqrt( fDist );

	float fDotP = ( fDiffX*fDirX + fDiffY*fDirY + fDiffZ*fDirZ ) / (*pDist);
	float fAngle = acos ( fDotP ) * 57.295779513f;

	if ( fAngle > fAng2 )
	{
		//no light hits point
		*pRed = 0; *pGreen = 0; *pBlue = 0;
		return false;
	}

	float fAngFallOff = 1.0f;
	if ( fAngle > fAng1 )
	{
		fAngFallOff = fAng2 - fAng1;
		fAngFallOff = ( fAng2 - fAngle ) / fAngFallOff;
	}

	//full light + attenuation + angle fall off
	//fDist = ( 1.0f - fDist/(fRange*fRange) ) * fAngFallOff;
	if ( fDist < 0.000001f ) fDist = fAngFallOff;
	else
	{
		//fDist = -0.5f*log(fDist/(fRange*fRange));
		fDist = 1 / (1 + 16.0f*fDist/(fRange*fRange));
		if ( fDist > 1.0f ) fDist = 1.0f;
		if ( fDist < 0.0f ) fDist = 0.0f;
		fDist *= fAngFallOff;
	}

	*pRed = fRed*fDist;
	*pGreen = fGreen*fDist;
	*pBlue = fBlue*fDist;

	return true;
}

void SpotLight::GetOrgin( float posX, float posY, float posZ, float *pOriginPosX, float *pOriginPosY, float *pOriginPosZ ) const
{
	*pOriginPosX = fPosX;
	*pOriginPosY = fPosY;
	*pOriginPosZ = fPosZ;
}