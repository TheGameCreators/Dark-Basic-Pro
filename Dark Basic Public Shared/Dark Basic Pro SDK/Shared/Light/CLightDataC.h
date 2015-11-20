#ifndef _CLIGHTDATA_H_
#define _CLIGHTDATA_H_

enum eLightType
{
    eDirectionalLight = 1,
    ePointLight = 2,
    eSpotLight = 3,

    // Original light type was int, force this enum to the same size
    eLightType_Force_DWORD = 0x7FFFFFFF,
};

struct tagLightData
{
	D3DLIGHT9	light;

	eLightType	eType;
	bool		bEnable;
	float		fRange;
	DWORD		dwColor;
};

#endif _CLIGHTDATA_H_
