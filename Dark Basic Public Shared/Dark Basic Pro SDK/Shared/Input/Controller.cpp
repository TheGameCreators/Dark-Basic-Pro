#define _CRT_SECURE_NO_DEPRECATE
#include "Controller.h"

// Module functions
static BOOL CALLBACK EnumObjectsCallback( const LPDIDEVICEOBJECTINSTANCE lpdidoi, LPVOID pContext )
{
    CController* Controller = (CController*)(pContext);

    if (lpdidoi->guidType == GUID_XAxis)
        Controller->HasX = true;
    else if (lpdidoi->guidType == GUID_YAxis)
        Controller->HasY = true;
    else if (lpdidoi->guidType == GUID_ZAxis)
        Controller->HasZ = true;
    else if (lpdidoi->guidType == GUID_RxAxis)
        Controller->HasRx = true;
    else if (lpdidoi->guidType == GUID_RyAxis)
        Controller->HasRy = true;
    else if (lpdidoi->guidType == GUID_RzAxis)
        Controller->HasRz = true;
    else if (lpdidoi->guidType == GUID_Slider)
        ++Controller->SliderCount;
    else if (lpdidoi->guidType == GUID_Button)
        ++Controller->ButtonCount;
    else if (lpdidoi->guidType == GUID_POV)
        ++Controller->PovCount;

    return DIENUM_CONTINUE;
}

static BOOL CALLBACK DIEnumEffectsProc(LPCDIEFFECTINFO pei, LPVOID pv)
{
	*((GUID *)pv) = pei->guid;
	return DIENUM_STOP;
}


// Object functions
CController::CController()
{
    lpDevice = NULL;

    for (int i = 0; i < EFF_MAX; ++i)
    {
        lpdiEffect[i] = NULL;
    }

    ResetDetail();
}

CController::~CController()
{
}

void CController::ResetDetail()
{
    bHasForceFeedback = false;
    sName[0] = 0;
    HasX = false;
    HasY = false;
    HasZ = false;
    HasRx = false;
    HasRy = false;
    HasRz = false;
    SliderCount = 0;
    ButtonCount = 0;
    PovCount = 0;
    Deadzone = 0;
}

void CController::Release()
{
    Acquire(NULL);
}

void CController::Acquire(LPDIRECTINPUTDEVICE8 lpNewDevice)
{
    if (lpDevice)
    {
        for (int i = 0; i < EFF_MAX; ++i)
        {
            if (lpdiEffect[i])
            {
                lpdiEffect[i]->Release();
                lpdiEffect[i] = NULL;
            }
        }
	    lpDevice->Unacquire(); 
	    lpDevice->Release();
    }

    lpDevice = lpNewDevice;
    ResetDetail();
    
    if (lpDevice)
    {
        lpDevice->Acquire();
        lpDevice->EnumObjects( (LPDIENUMDEVICEOBJECTSCALLBACK)EnumObjectsCallback, (LPVOID)this, DIDFT_ALL );
    }
}

void CController::Poll( DIJOYSTATE2& ControlDeviceBuffer )
{
    if (lpDevice)
    {
	    HRESULT hr = lpDevice->Poll(); 
	    if(FAILED(hr)) 
	    {
		    hr = lpDevice->Acquire();
	    }
	    if(SUCCEEDED(hr)) 
	    {
		    hr = lpDevice->GetDeviceState(sizeof(DIJOYSTATE2), &ControlDeviceBuffer);
            return;
	    }
    }

    memset(&ControlDeviceBuffer, 0, sizeof(ControlDeviceBuffer));
}

void CController::SetName(char* sNewName)
{
    strncpy(sName, sNewName, sizeof(sName));
    sName[ sizeof(sName)-1] = 0;
}

bool CController::ShootEffect(int magnitude, int delay, bool bStart)
{
    if (!lpDevice)
        return false;
	
	// Zero indicates infinite duration
	if(delay==0)
		delay=INFINITE;
	else
		delay=(DWORD) ((0.05+(delay/1000.0)) * DI_SECONDS);

	DWORD		    dwAxes[2] = { DIJOFS_X, DIJOFS_Y };
	LONG		    lDirection[2] = { 0 * DI_DEGREES, 0 }; 

    DICONSTANTFORCE	diConstantForce;            // type-specific parameters
    diConstantForce.lMagnitude = magnitude*100;

	DIEFFECT diEffect;                          // general parameters
	ZeroMemory(&diEffect, sizeof(DIEFFECT));
	diEffect.dwSize = sizeof(DIEFFECT); 
	diEffect.dwFlags = DIEFF_POLAR | DIEFF_OBJECTOFFSETS; 
	diEffect.dwDuration = delay;
 	diEffect.dwSamplePeriod = 0;                // = default 
	diEffect.dwGain = DI_FFNOMINALMAX;          // no scaling
	diEffect.dwTriggerButton = DIEB_NOTRIGGER;
	diEffect.dwTriggerRepeatInterval = 0;      
	diEffect.cAxes = 2; 
	diEffect.rgdwAxes = dwAxes; 
	diEffect.rglDirection = &lDirection[0]; 
	diEffect.lpEnvelope = 0; 
	diEffect.cbTypeSpecificParams = sizeof(diConstantForce);
	diEffect.lpvTypeSpecificParams = &diConstantForce;  

    HRESULT		hRes;
	if(lpdiEffect[EFF_SHOOT]) 
		hRes=lpdiEffect[EFF_SHOOT]->SetParameters(&diEffect, DIEP_DIRECTION | DIEP_DURATION | DIEP_TYPESPECIFICPARAMS | DIEP_START);
	else
		hRes=lpDevice->CreateEffect(GUID_ConstantForce, &diEffect, &lpdiEffect[EFF_SHOOT], NULL);

	return true;
}

bool CController::ChainsawEffect(int magnitude, int delay, bool bStart)
{
    if (!lpDevice)
        return false;

	// Zero indicates infinite duration
	if(delay==0)
		delay=INFINITE;
	else
		delay=(DWORD) ((0.05+(delay/1000.0)) * DI_SECONDS);

	// Enumeration of Effect
	GUID	guidEffect; 
	HRESULT hRes = lpDevice->EnumEffects( (LPDIENUMEFFECTSCALLBACK)DIEnumEffectsProc, &guidEffect, DIEFT_PERIODIC);
	if(FAILED(hRes))
		return false;	
 
	DWORD		dwAxes[2] = { DIJOFS_X, DIJOFS_Y };
	LONG		lDirection[2] = { 0, 0 }; 
	
	DIPERIODIC	diPeriodic;      // type-specific parameters
	diPeriodic.dwMagnitude = magnitude*100;
	diPeriodic.lOffset = 0; 
	diPeriodic.dwPhase = 0;
	diPeriodic.dwPeriod = (DWORD) (0.05 * DI_SECONDS); 

    DIEFFECT	diEffect;        // general parameters
	ZeroMemory(&diEffect, sizeof(DIEFFECT));
	diEffect.dwSize = sizeof(DIEFFECT); 
	diEffect.dwFlags = DIEFF_POLAR | DIEFF_OBJECTOFFSETS; 
	diEffect.dwDuration = delay;
 	diEffect.dwSamplePeriod = 0;               // = default 
	diEffect.dwGain = DI_FFNOMINALMAX;         // no scaling
	diEffect.dwTriggerButton = DIEB_NOTRIGGER;
	diEffect.dwTriggerRepeatInterval = 0;      
	diEffect.cAxes = 2; 
	diEffect.rgdwAxes = dwAxes; 
	diEffect.rglDirection = &lDirection[0]; 
	diEffect.lpEnvelope = 0;
	diEffect.cbTypeSpecificParams = sizeof(diPeriodic);
	diEffect.lpvTypeSpecificParams = &diPeriodic;  
 
	// Chainsaw
	if(lpdiEffect[EFF_CHAINSAW]) 
		hRes=lpdiEffect[EFF_CHAINSAW]->SetParameters(&diEffect, DIEP_DIRECTION | DIEP_DURATION | DIEP_TYPESPECIFICPARAMS | DIEP_START);
	else
		hRes=lpDevice->CreateEffect(GUID_ConstantForce, &diEffect, &lpdiEffect[EFF_CHAINSAW], NULL);

	if(hRes!=DI_OK && hRes!=S_FALSE)
		return false;

	if(bStart)
	{
		lpdiEffect[EFF_CHAINSAW]->Start(1,0);
		if(hRes!=DI_OK)
		{
			if(lpdiEffect[EFF_CHAINSAW]) lpdiEffect[EFF_CHAINSAW]->Release();
			hRes=lpDevice->CreateEffect(guidEffect , &diEffect, &lpdiEffect[EFF_CHAINSAW], NULL);
			lpdiEffect[EFF_CHAINSAW]->Start(1,0);
		}
	}

	return true;
}

bool CController::ImpactEffect(int magnitude, int delay, bool bStart)
{
    if (!lpDevice)
        return false;

	// Zero indicates infinite duration
	if(delay==0)
		delay=INFINITE;
	else
		delay=(DWORD) ((0.05+(delay/1000.0)) * DI_SECONDS);

	// Enumeration of Effect
	GUID	guidEffect; 
	HRESULT hRes = lpDevice->EnumEffects( (LPDIENUMEFFECTSCALLBACK)DIEnumEffectsProc, &guidEffect, DIEFT_PERIODIC);
	if(FAILED(hRes))
		return false;	
 
	DWORD		dwAxes[2] = { DIJOFS_X, DIJOFS_Y };
	LONG		lDirection[2] = { 0, 0 }; 
	
	DIPERIODIC	diPeriodic;      // type-specific parameters
	diPeriodic.dwMagnitude = magnitude*100;
	diPeriodic.lOffset = 0; 
	diPeriodic.dwPhase = 0;
	diPeriodic.dwPeriod = (DWORD) (0.1 * DI_SECONDS); 

    DIEFFECT	diEffect;        // general parameters
	ZeroMemory(&diEffect, sizeof(DIEFFECT));
	diEffect.dwSize = sizeof(DIEFFECT); 
	diEffect.dwFlags = DIEFF_POLAR | DIEFF_OBJECTOFFSETS; 
	diEffect.dwDuration = delay;
 	diEffect.dwSamplePeriod = 0;               // = default 
	diEffect.dwGain = DI_FFNOMINALMAX;         // no scaling
	diEffect.dwTriggerButton = DIEB_NOTRIGGER;
	diEffect.dwTriggerRepeatInterval = 0;      
	diEffect.cAxes = 2; 
	diEffect.rgdwAxes = dwAxes; 
	diEffect.rglDirection = &lDirection[0]; 
	diEffect.lpEnvelope = 0; 
	diEffect.cbTypeSpecificParams = sizeof(diPeriodic);
	diEffect.lpvTypeSpecificParams = &diPeriodic;  

	// Impact
	if(lpdiEffect[EFF_IMPACT])
		hRes=lpdiEffect[EFF_IMPACT]->SetParameters(&diEffect,	DIEP_DURATION | DIEP_TYPESPECIFICPARAMS | DIEP_START);
	else
		hRes=lpDevice->CreateEffect(guidEffect, &diEffect, &lpdiEffect[EFF_IMPACT], NULL);

	if(hRes!=DI_OK && hRes!=S_FALSE)
		return false;

	if(bStart)
	{
		lpdiEffect[EFF_IMPACT]->Start(1,0);
		if(hRes!=DI_OK)
		{
			if(lpdiEffect[EFF_IMPACT]) lpdiEffect[EFF_IMPACT]->Release();
			hRes=lpDevice->CreateEffect(guidEffect, &diEffect, &lpdiEffect[EFF_IMPACT], NULL);
			lpdiEffect[EFF_IMPACT]->Start(1,0);
		}
	}

	return true;
}

bool CController::DirectionEffect(short id, int magnitude, bool bStart)
{
    if (!lpDevice)
        return false;

	DWORD		dwAxes[2] = { DIJOFS_X, DIJOFS_Y };
	LONG		lDirection[2] = { 0 * DI_DEGREES, 0 }; 

    DICONSTANTFORCE	diConstantForce;      // type-specific parameters
	diConstantForce.lMagnitude       = magnitude*100;

    DIEFFECT	diEffect;        // general parameters
	ZeroMemory(&diEffect, sizeof(DIEFFECT));
	diEffect.dwSize = sizeof(DIEFFECT); 
    diEffect.dwFlags                 = DIEFF_POLAR | DIEFF_OBJECTOFFSETS;
    diEffect.dwDuration              = (DWORD) (0.1 * DI_SECONDS);
    diEffect.dwSamplePeriod          = 0;
    diEffect.dwGain                  = DI_FFNOMINALMAX;
    diEffect.dwTriggerButton         = DIEB_NOTRIGGER;
    diEffect.dwTriggerRepeatInterval = 0;
    diEffect.cAxes                   = 2;
    diEffect.rgdwAxes                = dwAxes;
    diEffect.rglDirection            = &lDirection[0];
    diEffect.lpEnvelope              = 0;
    diEffect.cbTypeSpecificParams    = sizeof(DICONSTANTFORCE);
    diEffect.lpvTypeSpecificParams   = &diConstantForce;
    diEffect.dwStartDelay            = 0;

    E_ForceType ForceType = EFF_UP;

    switch (id)
    {
    case 1:
		lDirection[0] = 180 * DI_DEGREES;
        ForceType = EFF_UP;
        break;
    case 2:
		lDirection[0] = 0 * DI_DEGREES; 
        ForceType = EFF_DOWN;
        break;
    case 3:
		lDirection[0] = 90 * DI_DEGREES; 
        ForceType = EFF_LEFT;
        break;
    case 4:
		lDirection[0] = 270 * DI_DEGREES;
        ForceType = EFF_RIGHT;
        break;
    default:
        return false;
    }

	HRESULT	hRes;
	if(lpdiEffect[ForceType])
		hRes=lpdiEffect[ForceType]->SetParameters(&diEffect, DIEP_DIRECTION | DIEP_TYPESPECIFICPARAMS | DIEP_START);
	else
		hRes=lpDevice->CreateEffect(GUID_ConstantForce, &diEffect, &lpdiEffect[ForceType], NULL);

	if(hRes!=DI_OK && hRes!=S_FALSE)
		return false;

	if(bStart)
	{
		lpdiEffect[ForceType]->Start(1,0);
		if(hRes!=DI_OK)
		{
			if(lpdiEffect[ForceType]) lpdiEffect[ForceType]->Release();
			hRes=lpDevice->CreateEffect(GUID_ConstantForce, &diEffect, &lpdiEffect[ForceType], NULL);
			lpdiEffect[ForceType]->Start(1,0);
		}
	}
	return true;
}

bool CController::AngleEffect(int magnitude, int angle, int delay, bool bStart)
{	
    if (!lpDevice)
        return false;

	// Zero indicates infinite duration
	if(delay==0)
		delay=INFINITE;
	else
		delay=(DWORD) ((0.05 + (delay/1000)) * DI_SECONDS);

	DWORD		dwAxes[2] = { DIJOFS_X, DIJOFS_Y };
	LONG		lDirection[2] = { angle * DI_DEGREES, 0 }; 

	DICONSTANTFORCE	diConstantForce;      // type-specific parameters
	diConstantForce.lMagnitude       = magnitude*100;

    DIEFFECT	diEffect;        // general parameters
	ZeroMemory(&diEffect, sizeof(DIEFFECT));
	diEffect.dwSize = sizeof(DIEFFECT); 
    diEffect.dwFlags                 = DIEFF_POLAR | DIEFF_OBJECTOFFSETS;
    diEffect.dwDuration              = (DWORD)delay;
    diEffect.dwSamplePeriod          = 0;
    diEffect.dwGain                  = DI_FFNOMINALMAX;
    diEffect.dwTriggerButton         = DIEB_NOTRIGGER;
    diEffect.dwTriggerRepeatInterval = 0;
    diEffect.cAxes                   = 2;
    diEffect.rgdwAxes                = dwAxes;
    diEffect.rglDirection            = &lDirection[0];
    diEffect.lpEnvelope              = 0;
    diEffect.cbTypeSpecificParams    = sizeof(DICONSTANTFORCE);
    diEffect.lpvTypeSpecificParams   = &diConstantForce;
    diEffect.dwStartDelay            = 0;

	HRESULT	hRes;
    if(lpdiEffect[EFF_ANGLE])
		hRes=lpdiEffect[EFF_ANGLE]->SetParameters(&diEffect,	DIEP_DURATION | DIEP_DIRECTION | DIEP_TYPESPECIFICPARAMS | DIEP_START);
	else
		hRes=lpDevice->CreateEffect(GUID_ConstantForce, &diEffect, &lpdiEffect[EFF_ANGLE], NULL);

	if(hRes!=DI_OK && hRes!=S_FALSE)
		return false;

	if(bStart)
	{
		lpdiEffect[EFF_ANGLE]->Start(1,0);
		if(hRes!=DI_OK)
		{
			if(lpdiEffect[EFF_ANGLE]) lpdiEffect[EFF_ANGLE]->Release();
			lpDevice->CreateEffect(GUID_ConstantForce, &diEffect, &lpdiEffect[EFF_ANGLE], NULL);
			lpdiEffect[EFF_ANGLE]->Start(1,0);
		}
	}

	return true;
}

bool CController::WaterEffect(int magnitude, int delay, bool bStart)
{
    if (!lpDevice)
        return false;

	// Zero indicates infinite duration
	if(delay==0)
		delay=INFINITE;
	else
		delay=(DWORD) ((0.05+(delay/1000.0)) * DI_SECONDS);

	DWORD		dwAxes[2] = { DIJOFS_X, DIJOFS_Y };
	LONG		lDirection[2] = { 1, 1 }; 
	
	DICONDITION	diCondition;      // type-specific parameters
	diCondition.lOffset = 0;
	diCondition.lPositiveCoefficient = (5000/100)*magnitude; 
	diCondition.lNegativeCoefficient = 0;
	diCondition.dwPositiveSaturation = 10000;
	diCondition.dwNegativeSaturation = 0;
	diCondition.lDeadBand = 0;

    DIEFFECT	diEffect;        // general parameters
	ZeroMemory(&diEffect, sizeof(DIEFFECT));
	diEffect.dwSize = sizeof(DIEFFECT); 
	diEffect.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS; 
	diEffect.dwDuration = delay;
 	diEffect.dwSamplePeriod = 0;               // = default 
	diEffect.dwGain = DI_FFNOMINALMAX;         // no scaling
	diEffect.dwTriggerButton = DIEB_NOTRIGGER;
	diEffect.dwTriggerRepeatInterval = 0;      
	diEffect.cAxes = 2; 
	diEffect.rgdwAxes = dwAxes; 
	diEffect.rglDirection = &lDirection[0]; 
	diEffect.lpEnvelope = 0; 
	diEffect.cbTypeSpecificParams = sizeof(diCondition);
	diEffect.lpvTypeSpecificParams = &diCondition;  

	HRESULT	hRes;
	if(lpdiEffect[EFF_WATER1]) 
		hRes=lpdiEffect[EFF_WATER1]->SetParameters(&diEffect,	DIEP_DURATION | DIEP_TYPESPECIFICPARAMS | DIEP_START);
	else
		hRes=lpDevice->CreateEffect(GUID_Damper, &diEffect, &lpdiEffect[EFF_WATER1], NULL);

	if(hRes!=DI_OK && hRes!=S_FALSE)
		return false;

	lDirection[0] = -1;
	lDirection[1] = -1;
	if(lpdiEffect[EFF_WATER2]) 
		hRes=lpdiEffect[EFF_WATER2]->SetParameters(&diEffect,	DIEP_DURATION | DIEP_TYPESPECIFICPARAMS | DIEP_START);
	else
		hRes=lpDevice->CreateEffect(GUID_Damper, &diEffect, &lpdiEffect[EFF_WATER2], NULL);

	if(hRes!=DI_OK && hRes!=S_FALSE)
		return false;

	if(bStart)
	{
		lpdiEffect[EFF_WATER1]->Start(1,0);
		if(hRes!=DI_OK)
		{
			if(lpdiEffect[EFF_WATER1]) lpdiEffect[EFF_WATER1]->Release();
			lpDevice->CreateEffect(GUID_Damper, &diEffect, &lpdiEffect[EFF_WATER1], NULL);
			lpdiEffect[EFF_WATER1]->Start(1,0);
		}
		lpdiEffect[EFF_WATER2]->Start(1,0);
		if(hRes!=DI_OK)
		{
			if(lpdiEffect[EFF_WATER2]) lpdiEffect[EFF_WATER2]->Release();
			lpDevice->CreateEffect(GUID_Damper, &diEffect, &lpdiEffect[EFF_WATER2], NULL);
			lpdiEffect[EFF_WATER2]->Start(1,0);
		}
	}

	return true;
}

void CController::StopEffect()
{
    for (int i = 0; i < EFF_MAX; ++i)
    {
        if (lpdiEffect[i])
        {
            lpdiEffect[i]->Stop();
        }
    }
}

void CController::AutoCenter(bool AutoCenterOn)
{
	DIPROPDWORD DIPropAutoCenter; 
	ZeroMemory(&DIPropAutoCenter, sizeof(DIPROPHEADER));
	DIPropAutoCenter.diph.dwSize = sizeof(DIPropAutoCenter);
	DIPropAutoCenter.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	DIPropAutoCenter.diph.dwObj = 0;
	DIPropAutoCenter.diph.dwHow = DIPH_DEVICE;
    DIPropAutoCenter.dwData = (AutoCenterOn ? DIPROPAUTOCENTER_ON : DIPROPAUTOCENTER_OFF); 
	lpDevice->Unacquire();
	lpDevice->SetProperty(DIPROP_AUTOCENTER, &DIPropAutoCenter.diph);
	lpDevice->Acquire();
}

