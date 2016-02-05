
////////////////////////////////////////////////////////////////////
// INFORMATION /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

/*
	EAX COMMANDS
*/

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// DEFINES AND INCLUDES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

#define DARKSDK	__declspec ( dllexport )
#include "stdafx.h"
#include "globstruct.h"
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>
#include "core.h"
#include <stdio.h>
#include "eax.h"
#include "eaxman.h"


LPKSPROPERTYSET			g_lpPropertySet = NULL;
IDirectSound3DBuffer8*  g_pBuffer       = NULL;
EAXBUFFERPROPERTIES		g_EAX;
EAXLISTENERPROPERTIES	g_EAXListener;
DWORD					g_dwReceived    = 0;
DWORD					g_dwFlagsA      = 0;
DWORD					g_dwFlagsB      = 0;
D3DXVECTOR3				g_vecCone		= D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );
DWORD					g_dwIn			= 0;
DWORD					g_dwOut			= 0;
float					g_fMin			= 0.0f;
float					g_fMax			= 0.0f;
long					g_lVolume		= 0.0f;
DWORD					g_dwSupport		= 0;

enum eEAXFlags
{
	eNone									= 0,

	eEAXLISTENERFLAGS_DECAYTIMESCALE		= 0x00000001,
	eEAXLISTENERFLAGS_REFLECTIONSSCALE      = 0x00000002,
	eEAXLISTENERFLAGS_REFLECTIONSDELAYSCALE = 0x00000004,
	eEAXLISTENERFLAGS_REVERBSCALE           = 0x00000008,
	eEAXLISTENERFLAGS_REVERBDELAYSCALE      = 0x00000010,
	eEAXLISTENERFLAGS_DECAYHFLIMIT          = 0x00000020,

	eEAXBUFFERFLAGS_DIRECTHFAUTO			= 0x00000001,
	eEAXBUFFERFLAGS_ROOMAUTO				= 0x00000002,
	eEAXBUFFERFLAGS_ROOMHFAUTO				= 0x00000004
};

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// FUNCTIONS ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

DARKSDK void  PerformCheckListForEAXEffects ( void );
DARKSDK void  ApplyPresetEAXEffect          ( int iSound, int iEffect );

DARKSDK void  SetEAXRoom                    ( int iSound, int   iValue );
DARKSDK void  SetEAXRoomHF                  ( int iSound, int   iValue );
DARKSDK void  SetEAXRoomRolloff             ( int iSound, float fValue );
DARKSDK void  SetEAXDecayTime               ( int iSound, float fValue );
DARKSDK void  SetEAXDecayHFRatio            ( int iSound, float fValue );
DARKSDK void  SetEAXReflections             ( int iSound, int   iValue );
DARKSDK void  SetEAXReflectionsDelay        ( int iSound, float fValue );
DARKSDK void  SetEAXReverb                  ( int iSound, int   iValue );
DARKSDK void  SetEAXReverbDelay             ( int iSound, float fValue );
DARKSDK void  SetEAXEnvironment             ( int iSound, int   iValue );
DARKSDK void  SetEAXEnvironmentSize         ( int iSound, float fValue );
DARKSDK void  SetEAXEnvironmentDiffusion    ( int iSound, float fValue );
DARKSDK void  SetEAXAirAbsorption           ( int iSound, float fValue );
DARKSDK void  SetEAXFlags                   ( int iSound, int   iValue );

DARKSDK void  SetEAXScaleDecayTime          ( int iSound, int   iMode  );
DARKSDK void  SetEAXScaleReflections        ( int iSound, int   iMode  );
DARKSDK void  SetEAXScaleReflectionsDelay   ( int iSound, int   iMode  );
DARKSDK void  SetEAXScaleReverb             ( int iSound, int   iMode  );
DARKSDK void  SetEAXScaleReverbDelay        ( int iSound, int   iMode  );
DARKSDK void  SetEAXClipDecayHF             ( int iSound, int   iMode  );

DARKSDK void  SetEAXSourceDirect            ( int iSound, int   iValue );
DARKSDK void  SetEAXSourceDirectHF          ( int iSound, int   iValue );
DARKSDK void  SetEAXSourceRoom              ( int iSound, int   iValue );
DARKSDK void  SetEAXSourceRoomHF            ( int iSound, float fValue );
DARKSDK void  SetEAXSourceRolloff           ( int iSound, float fValue );
DARKSDK void  SetEAXSourceOutside           ( int iSound, int   iValue );
DARKSDK void  SetEAXSourceAbsorption        ( int iSound, float fValue );
DARKSDK void  SetEAXSourceFlags             ( int iSound, int   iValue );
DARKSDK void  SetEAXSourceObstruction       ( int iSound, int   iValue );
DARKSDK void  SetEAXSourceObstructionLF     ( int iSound, float fValue );
DARKSDK void  SetEAXSourceOcclusion         ( int iSound, int   iValue );
DARKSDK void  SetEAXSourceOcclusionLF       ( int iSound, float fValue );
DARKSDK void  SetEAXSourceOcclusionRoom     ( int iSound, float fValue );

DARKSDK void  SetEAXSourceAffectDirectHF    ( int iSound, int   iMode  );
DARKSDK void  SetEAXSourceAffectRoom        ( int iSound, int   iMode  );
DARKSDK void  SetEAXSourceAffectRoomHF      ( int iSound, int   iMode  );

DARKSDK void  SetEAXConeOrientationX        ( int iSound, float fValue );
DARKSDK void  SetEAXConeOrientationY        ( int iSound, float fValue );
DARKSDK void  SetEAXConeOrientationZ        ( int iSound, float fValue );
DARKSDK void  SetEAXConeInsideAngle         ( int iSound, int   iValue );
DARKSDK void  SetEAXConeOutsideAngle        ( int iSound, int   iValue );
DARKSDK void  SetEAXConeOutsideVolume       ( int iSound, int   iValue );
DARKSDK void  SetEAXMinDistance             ( int iSound, float fValue );
DARKSDK void  SetEAXMaxDistance             ( int iSound, float fValue );

DARKSDK int   GetEAX                        ( void );

DARKSDK int   GetEAXRoom                    ( int iSound );
DARKSDK int   GetEAXRoomHF                  ( int iSound );
DARKSDK DWORD GetEAXRoomRolloff             ( int iSound );
DARKSDK DWORD GetEAXDecayTime               ( int iSound );
DARKSDK DWORD GetEAXDecayHFRatio            ( int iSound );
DARKSDK int   GetEAXReflections             ( int iSound );
DARKSDK DWORD GetEAXReflectionsDelay        ( int iSound );
DARKSDK int   GetEAXReverb                  ( int iSound );
DARKSDK DWORD GetEAXReverbDelay             ( int iSound );
DARKSDK int   GetEAXEnvironment             ( int iSound );
DARKSDK DWORD GetEAXEnvironmentSize         ( int iSound );
DARKSDK DWORD GetEAXEnvironmentDiffusion    ( int iSound );
DARKSDK DWORD GetEAXAirAbsorption           ( int iSound );
DARKSDK int   GetEAXFlags                   ( int iSound );

DARKSDK int   GetEAXSourceDirect            ( int iSound );
DARKSDK int   GetEAXSourceDirectHF          ( int iSound );
DARKSDK int   GetEAXSourceRoom              ( int iSound );
DARKSDK DWORD GetEAXSourceRoomHF            ( int iSound );
DARKSDK DWORD GetEAXSourceRolloff           ( int iSound );
DARKSDK int   GetEAXSourceOutside           ( int iSound );
DARKSDK DWORD GetEAXSourceAbsorption        ( int iSound );
DARKSDK int   GetEAXSourceFlags             ( int iSound );
DARKSDK int   GetEAXSourceObstruction       ( int iSound );
DARKSDK DWORD GetEAXSourceObstructionLF     ( int iSound );
DARKSDK int   GetEAXSourceOcclusion         ( int iSound );
DARKSDK DWORD GetEAXSourceOcclusionLF       ( int iSound );
DARKSDK DWORD GetEAXSourceOcclusionRoom     ( int iSound );

DARKSDK DWORD GetEAXConeOrientationX        ( int iSound );
DARKSDK DWORD GetEAXConeOrientationY        ( int iSound );
DARKSDK DWORD GetEAXConeOrientationZ        ( int iSound );
DARKSDK int   GetEAXConeInsideAngle         ( int iSound );
DARKSDK int   GetEAXConeOutsideAngle        ( int iSound );
DARKSDK int   GetEAXConeOutsideVolume       ( int iSound );
DARKSDK DWORD GetEAXMinDistance             ( int iSound );
DARKSDK DWORD GetEAXMaxDistance             ( int iSound );

		void  GetData                       ( int iSound );
		void  ClearData					    ( void );
		void  SetProperty				    ( int iSound, DSPROPERTY_EAX_LISTENERPROPERTY property, int iType, int iValue, float fValue, eEAXFlags flags, DSPROPERTY_EAX_BUFFERPROPERTY );
		void* GetEAXData                    ( int iSound, int iData );
		void* GetConeData                   ( int iSound, int iData );

/*
	// export names for string table -

		PERFORM CHECKLIST FOR EAX EFFECTS%0%?PerformCheckListForEAXEffects@@YAXXZ%
		APPLY PRESET EAX EFFECT%LL%?ApplyPresetEAXEffect@@YAXHH@Z%Sound, Effect

		SET EAX ROOM%LL%?SetEAXRoom@@YAXHH@Z%Sound, Value
		SET EAX ROOM HF%LL%?SetEAXRoomHF@@YAXHH@Z%Sound, Value
		SET EAX ROOM ROLLOFF%LF%?SetEAXRoomRolloff@@YAXHM@Z%Sound, Value
		SET EAX DECAY TIME%LF%?SetEAXDecayTime@@YAXHM@Z%Sound, Value
		SET EAX DECAY HF RATIO%LF%?SetEAXDecayHFRatio@@YAXHM@Z%Sound, Value
		SET EAX REFLECTIONS%LL%?SetEAXReflections@@YAXHH@Z%Sound, Value
		SET EAX REFLECTIONS DELAY%LF%?SetEAXReflectionsDelay@@YAXHM@Z%Sound, Value
		SET EAX REVERB%LL%?SetEAXReverb@@YAXHH@Z%Sound, Value
		SET EAX REVERB DELAY%LF%?SetEAXReverbDelay@@YAXHM@Z%Sound, Value
		SET EAX ENVIRONMENT%LL%?SetEAXEnvironment@@YAXHH@Z%Sound, Value
		SET EAX ENVIRONMENT SIZE%LF%?SetEAXEnvironmentSize@@YAXHM@Z%Sound, Value
		SET EAX ENVIRONMENT DIFFUSION%LF%?SetEAXEnvironmentDiffusion@@YAXHM@Z%Sound, Value
		SET EAX AIR ABSORPTION%LF%?SetEAXAirAbsorption@@YAXHM@Z%Sound, Value
		SET EAX FLAGS%LL%?SetEAXFlags@@YAXHH@Z%Sound, Value

		SET EAX SCALE DECAY TIME%LL%?SetEAXScaleDecayTime@@YAXHH@Z%Sound, Mode
		SET EAX SCALE REFLECTIONS%LL%?SetEAXScaleReflections@@YAXHH@Z%Sound, Mode
		SET EAX SCALE REFLECTIONS DELAY%LL%?SetEAXScaleReflectionsDelay@@YAXHH@Z%Sound, Mode
		SET EAX SCALE REVERB%LL%?SetEAXScaleReverb@@YAXHH@Z%Sound, Mode
		SET EAX SCALE REVERB DELAY%LL%?SetEAXScaleReverbDelay@@YAXHH@Z%Sound, Mode
		SET EAX CLIP DECAY HF%LL%?SetEAXClipDecayHF@@YAXHH@Z%Sound, Mode

		SET EAX SOURCE DIRECT%LL%?SetEAXSourceDirect@@YAXHH@Z%Sound, Value
		SET EAX SOURCE DIRECT HF%LL%?SetEAXSourceDirectHF@@YAXHH@Z%Sound, Value
		SET EAX SOURCE ROOM%LL%?SetEAXSourceRoom@@YAXHH@Z%Sound, Value
		SET EAX SOURCE ROLLOFF%LF%?SetEAXSourceRolloff@@YAXHM@Z%Sound, Value
		SET EAX SOURCE OUTSIDE%LL%?SetEAXSourceOutside@@YAXHH@Z%Sound, Value
		SET EAX SOURCE ABSORPTION%LF%?SetEAXSourceAbsorption@@YAXHM@Z%Sound, Value
		SET EAX SOURCE FLAGS%LL%?SetEAXSourceFlags@@YAXHH@Z%Sound, Value
		SET EAX SOURCE OBSTRUCTION%LL%?SetEAXSourceObstruction@@YAXHH@Z%Sound, Value
		SET EAX SOURCE OBSTRUCTION LF%LF%?SetEAXSourceObstructionLF@@YAXHM@Z%Sound, Value
		SET EAX SOURCE OCCLUSION%LL%?SetEAXSourceOcclusion@@YAXHH@Z%Sound, Value
		SET EAX SOURCE OCCLUSION LF%LF%?SetEAXSourceOcclusionLF@@YAXHM@Z%Sound, Value
		SET EAX SOURCE OCCLUSION ROOM%LF%?SetEAXSourceOcclusionRoom@@YAXHM@Z%Sound, Value

  		SET EAX SOURCE AFFECT DIRECT HF%LL%?SetEAXSourceAffectDirectHF@@YAXHH@Z%Sound, Mode
		SET EAX SOURCE AFFECT ROOM%LL%?SetEAXSourceAffectRoom@@YAXHH@Z%Sound, Mode
		SET EAX SOURCE AFFECT ROOM HF%LL%?SetEAXSourceAffectRoomHF@@YAXHH@Z%Sound, Mode

		SET EAX CONE ORIENTATION X%LF%?SetEAXConeOrientationX@@YAXHM@Z%Sound, Value
		SET EAX CONE ORIENTATION Y%LF%?SetEAXConeOrientationY@@YAXHM@Z%Sound, Value
		SET EAX CONE ORIENTATION Z%LF%?SetEAXConeOrientationZ@@YAXHM@Z%Sound, Value
		SET EAX CONE INSIDE ANGLE%LL%?SetEAXConeInsideAngle@@YAXHH@Z%Sound, Value
		SET EAX CONE OUTSIDE ANGLE%LL%?SetEAXConeOutsideAngle@@YAXHH@Z%Sound, Value
		SET EAX CONE OUTSIDE VOLUME%LL%?SetEAXConeOutsideVolume@@YAXHH@Z%Sound, Value
		SET EAX MIN DISTANCE%LF%?SetEAXMinDistance@@YAXHM@Z%Sound, Value
		SET EAX MAX DISTANCE%LF%?SetEAXMaxDistance@@YAXHM@Z%Sound, Value

		GET EAX[%L%?GetEAX@@YAHXZ%

		GET EAX ROOM[%LL%?GetEAXRoom@@YAHH@Z%
		GET EAX ROOM HF[%LL%?GetEAXRoomHF@@YAHH@Z%
		GET EAX ROOM ROLLOFF[%FL%?GetEAXRoomRolloff@@YAKH@Z%
		GET EAX ROOM DECAY TIME[%FL%?GetEAXDecayTime@@YAKH@Z%
		GET EAX DECAY HF RATIO[%FL%?GetEAXDecayHFRatio@@YAKH@Z%
		GET EAX REFLECTIONS[%LL%?GetEAXReflections@@YAHH@Z%
		GET EAX REFLECTIONS DELAY[%FL%?GetEAXReflectionsDelay@@YAKH@Z%
		GET EAX REVERB[%LL%?GetEAXReverb@@YAHH@Z%
		GET EAX REVERB DELAY[%FL%?GetEAXReverbDelay@@YAKH@Z%
		GET EAX ENVIRONMENT[%LL%?GetEAXEnvironment@@YAHH@Z%
		GET EAX ENVIRONMENT SIZE[%FL%?GetEAXEnvironmentSize@@YAKH@Z%
		GET EAX ENVIRONMENT DIFFUSION[%FL%?GetEAXEnvironmentDiffusion@@YAKH@Z%
		GET EAX AIR ABSORPTION[%FL%?GetEAXAirAbsorption@@YAKH@Z%
		GET EAX FLAGS[%LL%?GetEAXFlags@@YAHH@Z%

		GET EAX SOURCE DIRECT[%LL%?GetEAXSourceDirect@@YAHH@Z%
		GET EAX SOURCE DIRECT HF[%LL%?GetEAXSourceDirectHF@@YAHH@Z%
		GET EAX SOURCE ROOM[%LL%?GetEAXSourceRoom@@YAHH@Z%
		GET EAX SOURCE ROOM HF[%FL%?GetEAXSourceRoomHF@@YAKH@Z%
		GET EAX SOURCE ROLLOFF[%FL%?GetEAXSourceRolloff@@YAKH@Z%
		GET EAX SOURCE OUTSIDE[%LL%?GetEAXSourceOutside@@YAHH@Z%
		GET EAX SOURCE ABSORPTION[%FL%?GetEAXSourceAbsorption@@YAKH@Z%
		GET EAX SOURCE FLAGS[%LL%?GetEAXSourceFlags@@YAHH@Z%
		GET EAX SOURCE OBSTRUCTION[%LL%?GetEAXSourceObstruction@@YAHH@Z%
		GET EAX SOURCE OBSTRUCTION LF[%FL%?GetEAXSourceObstructionLF@@YAKH@Z%
		GET EAX SOURCE OCCLUSION[%LL%?GetEAXSourceOcclusion@@YAHH@Z%
		GET EAX SOURCE OCCLUSION LF[%FL%?GetEAXSourceOcclusionLF@@YAKH@Z%
		GET EAX SOURCE OCCLUSION ROOM[%FL%?GetEAXSourceOcclusionRoom@@YAKH@Z%

		GET EAX CONE ORIENTATION X[%FL%?GetEAXConeOrientationX@@YAKH@Z%
		GET EAX CONE ORIENTATION Y[%FL%?GetEAXConeOrientationY@@YAKH@Z%
		GET EAX CONE ORIENTATION Z[%FL%?GetEAXConeOrientationZ@@YAKH@Z%
		GET EAX CONE INSIDE ANGLE[%LL%?GetEAXConeInsideAngle@@YAHH@Z%
		GET EAX CONE OUTSIDE ANGLE[%LL%?GetEAXConeOutsideAngle@@YAHH@Z%
		GET EAX CONE OUTSIDE VOLUME[%LL%?GetEAXConeOutsideVolume@@YAHH@Z%
		GET EAX MIN DISTANCE[%FL%?GetEAXMinDistance@@YAKH@Z%
		GET EAX MAX DISTANCE[%FL%?GetEAXMaxDistance@@YAKH@Z%
*/

void PerformCheckListForEAXEffects ( void )
{
	// Macro to prevent free DLL command
	

	if ( !g_pGlob )
	{
		Error ( 1 );
		return;
	}

	char szPresetEffect [ 26 ] [ 255 ] =	{
												"Generic",
												"Padded Cell",
												"Room",
												"Bathroom",
												"Livingroom",
												"Stone Room",
												"Auditorium",
												"Concert Hall",
												"Cave",
												"Arena",
												"Hangar",
												"Carpeted Hallway",
												"Hallway",
												"Stone Corridor",
												"Alley",
												"Forest",
												"City",
												"Mountains",
												"Quarry",
												"Plain",
												"Parkinglot",
												"Sewer Pipe",
												"Under Water",
												"Drugged",
												"Dizzy",
												"Psychotic"
											};

	g_pGlob->checklistexists     = true;
	g_pGlob->checklisthasvalues  = true;
	g_pGlob->checklisthasstrings = true;
	g_pGlob->checklistqty        = 26;

	for ( int i = 0; i < 26; i++ )
	{
		GlobExpandChecklist ( i, 255 );

		strcpy ( g_pGlob->checklist [ i ].string, szPresetEffect [ i ] );
	}
}

void GetData ( int iSound )
{
	// get the data we need for an operation

	// access sound pointer
	g_pBuffer = g_pGetSoundBuffer ( iSound );

	// ensure pointer is valid
	if ( !g_pBuffer )
	{
		Error ( 4 );
		return;
	}
	
	// see if we can get the property set interface
	if ( FAILED ( g_pBuffer->QueryInterface ( IID_IKsPropertySet, ( LPVOID* ) &g_lpPropertySet ) ) )
	{
		Error ( 6 );
		return;
	}

	// get listener flags
	if ( FAILED ( g_lpPropertySet->Get ( DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_FLAGS, NULL, 0, &g_dwFlagsA, sizeof ( DWORD ), &g_dwReceived ) ) )
	{
		Error ( 6 );
		return;
	}

	// get buffer flags
	if ( FAILED ( g_lpPropertySet->Get ( DSPROPSETID_EAX_BufferProperties, DSPROPERTY_EAXBUFFER_FLAGS, NULL, 0, &g_dwFlagsB, sizeof ( DWORD ), &g_dwReceived ) ) )
	{
		Error ( 6 );
		return;
	}

	// get listener structure
	if ( FAILED ( g_lpPropertySet->Get ( DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ALLPARAMETERS, NULL, 0, &g_EAXListener, sizeof ( EAXLISTENERPROPERTIES ), &g_dwReceived ) ) )
	{
		Error ( 6 );
		return;
	}

	// get buffer structure
	if ( FAILED ( g_lpPropertySet->Get ( DSPROPSETID_EAX_BufferProperties, DSPROPERTY_EAXBUFFER_ALLPARAMETERS, NULL, 0, &g_EAX, sizeof ( EAXLISTENERPROPERTIES ), &g_dwReceived ) ) )
	{
		Error ( 6 );
		return;
	}
}

void ClearData ( void )
{
	SAFE_RELEASE ( g_lpPropertySet );
}

void ApplyPresetEAXEffect ( int iSound, int iEffect )
{
	// Macro to prevent free DLL command
	

	iEffect--;

	DWORD dwValue = iEffect;

	GetData ( iSound );

	// set default properties
	g_EAX.lOutsideVolumeHF      = EAXBUFFER_DEFAULTOUTSIDEVOLUMEHF;
	g_EAX.lDirect               = EAXBUFFER_DEFAULTDIRECT;
	g_EAX.lDirectHF             = EAXBUFFER_DEFAULTDIRECTHF;
	g_EAX.lRoom                 = EAXBUFFER_DEFAULTROOM;
	g_EAX.lRoomHF               = EAXBUFFER_DEFAULTROOMHF;
	g_EAX.flRoomRolloffFactor   = EAXBUFFER_DEFAULTROOMROLLOFFFACTOR;
	g_EAX.flAirAbsorptionFactor = EAXBUFFER_DEFAULTAIRABSORPTIONFACTOR;
	g_EAX.lObstruction          = EAXBUFFER_DEFAULTOBSTRUCTION;
	g_EAX.flObstructionLFRatio  = EAXBUFFER_DEFAULTOBSTRUCTIONLFRATIO;
	g_EAX.lOcclusion            = EAXBUFFER_DEFAULTOCCLUSION;
	g_EAX.flOcclusionLFRatio    = EAXBUFFER_DEFAULTOCCLUSIONLFRATIO;
	g_EAX.flOcclusionRoomRatio  = EAXBUFFER_DEFAULTOCCLUSIONROOMRATIO;
	g_EAX.dwFlags               = EAXBUFFERFLAGS_DIRECTHFAUTO | EAXBUFFERFLAGS_ROOMAUTO | EAXBUFFERFLAGS_ROOMHFAUTO;

	if ( !g_lpPropertySet )
	{
		Error ( 6 );
		return;
	}

	// set effect properties with structure
	if ( FAILED ( g_lpPropertySet->Set ( DSPROPSETID_EAX_BufferProperties, DSPROPERTY_EAXBUFFER_ALLPARAMETERS, NULL, 0, &g_EAX, sizeof ( EAXBUFFERPROPERTIES ) ) ) )
	{
		Error ( 7 );
		return;
	}

	// now select the effect to use
	if ( FAILED ( g_lpPropertySet->Set ( DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ENVIRONMENT, NULL, 0, &dwValue, sizeof ( DWORD ) ) ) )
	{
		Error ( 7 );
		return;
	}
}

void SetProperty ( 
					int								iSound,
					DSPROPERTY_EAX_LISTENERPROPERTY property,
					int								iType,
					int								iValue,
					float							fValue,
					eEAXFlags						flags,
					DSPROPERTY_EAX_BUFFERPROPERTY	buffer
				 )
{
	// sets a property

	// modes -
	//
	//	0	set a listener property with an integer
	//	1	set a listener property with a float
	//	2	sets a listener flag
	//	3	sets a buffer property with an integer
	//	4	sets a buffer property with a float
	//	5	sets a buffer flag

	GetData ( iSound );

	if ( !g_lpPropertySet )
	{
		Error ( 6 );
		return;
	}

	if ( iType == 0 )
		g_lpPropertySet->Set ( DSPROPSETID_EAX_ListenerProperties, property, NULL, 0, &iValue, sizeof ( LONG ) );
	
	if ( iType == 1 )
		g_lpPropertySet->Set ( DSPROPSETID_EAX_ListenerProperties, property, NULL, 0, &fValue, sizeof ( float ) );

	if ( iType == 2 )
	{
		g_dwFlagsA &= ( 0xFFFFFFFF ^ flags );
	
		if ( iValue )
			g_dwFlagsA |= flags;

		g_lpPropertySet->Set ( DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_FLAGS, NULL, 0, &g_dwFlagsA, sizeof ( DWORD ) );
	}

	if ( iType == 3 )
		g_lpPropertySet->Set ( DSPROPSETID_EAX_BufferProperties, buffer, NULL, 0, &iValue, sizeof ( LONG ) );

	if ( iType == 4 )
		g_lpPropertySet->Set ( DSPROPSETID_EAX_BufferProperties, buffer, NULL, 0, &fValue, sizeof ( float ) );

	if ( iType == 5 )
	{
		g_dwFlagsB &= ( 0xFFFFFFFF ^ flags );
	
		if ( iValue )
			g_dwFlagsB |= flags;

		g_lpPropertySet->Set ( DSPROPSETID_EAX_BufferProperties, DSPROPERTY_EAXLISTENER_FLAGS, NULL, 0, &g_dwFlagsB, sizeof ( DWORD ) );
	}
	
	ClearData ( );
}

void SetEAXRoom ( int iSound, int iValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_ROOM, 0, iValue, 0.0f, eNone, DSPROPERTY_EAXBUFFER_NONE );
}

void SetEAXRoomHF ( int iSound, int iValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_ROOMHF, 0, iValue, 0.0f, eNone, DSPROPERTY_EAXBUFFER_NONE );
}

void SetEAXRoomRolloff ( int iSound, float fValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_ROOMROLLOFFFACTOR, 1, 0, fValue, eNone, DSPROPERTY_EAXBUFFER_NONE );
}

void SetEAXDecayTime ( int iSound, float fValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_DECAYTIME, 1, 0, fValue, eNone, DSPROPERTY_EAXBUFFER_NONE );
}

void SetEAXDecayHFRatio ( int iSound, float fValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_DECAYHFRATIO, 1, 0, fValue, eNone, DSPROPERTY_EAXBUFFER_NONE );
}

void SetEAXReflections ( int iSound, int iValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_REFLECTIONS, 0, iValue, 0.0f, eNone, DSPROPERTY_EAXBUFFER_NONE );
}

void SetEAXReflectionsDelay ( int iSound, float fValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_REFLECTIONSDELAY, 1, 0, fValue, eNone, DSPROPERTY_EAXBUFFER_NONE );
}

void SetEAXReverb ( int iSound, int iValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_REVERB, 0, iValue, 0.0f, eNone, DSPROPERTY_EAXBUFFER_NONE );
}

void SetEAXReverbDelay ( int iSound, float fValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_REVERBDELAY, 1, 0, fValue, eNone, DSPROPERTY_EAXBUFFER_NONE );
}

void SetEAXEnvironment ( int iSound, int iValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_ENVIRONMENT, 0, iValue, 0.0f, eNone, DSPROPERTY_EAXBUFFER_NONE );
}

void SetEAXEnvironmentSize ( int iSound, float fValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_ENVIRONMENTSIZE, 1, 0, fValue, eNone, DSPROPERTY_EAXBUFFER_NONE );
}

void SetEAXEnvironmentDiffusion ( int iSound, float fValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_ENVIRONMENTDIFFUSION, 1, 0, fValue, eNone, DSPROPERTY_EAXBUFFER_NONE );
}

void SetEAXAirAbsorption ( int iSound, float fValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_AIRABSORPTIONHF, 1, 0, fValue, eNone, DSPROPERTY_EAXBUFFER_NONE );
}

void SetEAXFlags ( int iSound, int iValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_FLAGS, 0, iValue, 0.0f, eNone, DSPROPERTY_EAXBUFFER_NONE );
}

void SetEAXScaleDecayTime ( int iSound, int iMode )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_NONE, 2, iMode, 0.0f, eEAXLISTENERFLAGS_DECAYTIMESCALE, DSPROPERTY_EAXBUFFER_NONE );
}

void SetEAXScaleReflections ( int iSound, int iMode  )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_NONE, 2, iMode, 0.0f, eEAXLISTENERFLAGS_REFLECTIONSSCALE, DSPROPERTY_EAXBUFFER_NONE );
}

void SetEAXScaleReflectionsDelay ( int iSound, int iMode )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_NONE, 2, iMode, 0.0f, eEAXLISTENERFLAGS_REFLECTIONSDELAYSCALE, DSPROPERTY_EAXBUFFER_NONE );
}

void SetEAXScaleReverb ( int iSound, int iMode )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_NONE, 2, iMode, 0.0f, eEAXLISTENERFLAGS_REVERBSCALE, DSPROPERTY_EAXBUFFER_NONE );
}

void SetEAXScaleReverbDelay ( int iSound, int iMode )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_NONE, 2, iMode, 0.0f, eEAXLISTENERFLAGS_REVERBDELAYSCALE, DSPROPERTY_EAXBUFFER_NONE );
}

void SetEAXClipDecayHF ( int iSound, int iMode )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_NONE, 2, iMode, 0.0f, eEAXLISTENERFLAGS_DECAYHFLIMIT, DSPROPERTY_EAXBUFFER_NONE );
}

void SetEAXSourceDirect ( int iSound, int iValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_NONE, 3, iValue, 0.0f, eNone, DSPROPERTY_EAXBUFFER_DIRECT );
}

void SetEAXSourceDirectHF ( int iSound, int iValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_NONE, 3, iValue, 0.0f, eNone, DSPROPERTY_EAXBUFFER_DIRECTHF );
}

void SetEAXSourceRoom ( int iSound, int iValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_NONE, 3, iValue, 0.0f, eNone, DSPROPERTY_EAXBUFFER_ROOM );
}

void SetEAXSourceRoomHF ( int iSound, float fValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_NONE, 4, 0, fValue, eNone, DSPROPERTY_EAXBUFFER_ROOMHF );
}

void SetEAXSourceRolloff ( int iSound, float fValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_NONE, 4, 0, fValue, eNone, DSPROPERTY_EAXBUFFER_ROOMROLLOFFFACTOR );
}

void SetEAXSourceOutside ( int iSound, int iValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_NONE, 3, iValue, 0.0f, eNone, DSPROPERTY_EAXBUFFER_OUTSIDEVOLUMEHF );
}

void SetEAXSourceAbsorption ( int iSound, float fValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_NONE, 4, 0, fValue, eNone, DSPROPERTY_EAXBUFFER_AIRABSORPTIONFACTOR );
}

void SetEAXSourceFlags ( int iSound, int iValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_NONE, 3, iValue, 0.0f, eNone, DSPROPERTY_EAXBUFFER_FLAGS );
}

void SetEAXSourceObstruction ( int iSound, int iValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_NONE, 3, iValue, 0.0f, eNone, DSPROPERTY_EAXBUFFER_OBSTRUCTION );
}

void SetEAXSourceObstructionLF ( int iSound, float fValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_NONE, 4, 0, fValue, eNone, DSPROPERTY_EAXBUFFER_OBSTRUCTIONLFRATIO );
}

void SetEAXSourceOcclusion ( int iSound, int iValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_NONE, 3, iValue, 0.0f, eNone, DSPROPERTY_EAXBUFFER_OCCLUSION );
}

void SetEAXSourceOcclusionLF ( int iSound, float fValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_NONE, 4, 0, fValue, eNone, DSPROPERTY_EAXBUFFER_OCCLUSIONLFRATIO );
}

void SetEAXSourceOcclusionRoom ( int iSound, float fValue )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_NONE, 4, 0, fValue, eNone, DSPROPERTY_EAXBUFFER_OCCLUSIONROOMRATIO );
}

void SetEAXSourceAffectDirectHF ( int iSound, int iMode )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_NONE, 5, iMode, 0.0f, eEAXBUFFERFLAGS_DIRECTHFAUTO, DSPROPERTY_EAXBUFFER_NONE );
}

void SetEAXSourceAffectRoom ( int iSound, int iMode )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_NONE, 5, iMode, 0.0f, eEAXBUFFERFLAGS_ROOMAUTO, DSPROPERTY_EAXBUFFER_NONE );
}

void SetEAXSourceAffectRoomHF ( int iSound, int iMode )
{
	
	SetProperty ( iSound, DSPROPERTY_EAXLISTENER_NONE, 5, iMode, 0.0f, eEAXBUFFERFLAGS_ROOMHFAUTO, DSPROPERTY_EAXBUFFER_NONE );
}

void SetCone ( int iSound, int iType, float fX, float fY, float fZ, int iValue, float fValue )
{
	
	GetData ( iSound );

	if ( iType < 3 )
	{
		D3DXVECTOR3 vec;

		g_pBuffer->GetConeOrientation ( &vec );

		if ( iType == 0 ) vec.x = fX;
		if ( iType == 1 ) vec.y = fY;
		if ( iType == 2 ) vec.z = fZ;

		g_pBuffer->SetConeOrientation ( vec.x, vec.y, vec.z, DS3D_IMMEDIATE );
	}

	if ( iType == 3 || iType == 4 )
	{
		DWORD dwIn, dwOut;

		g_pBuffer->GetConeAngles ( &dwIn, &dwOut );

		if ( iType == 3 ) dwIn  = iValue;
		if ( iType == 4 ) dwOut = iValue;
		
		g_pBuffer->SetConeAngles ( dwIn, dwOut, DS3D_IMMEDIATE );
	}

	if ( iType == 5 )
		g_pBuffer->SetConeOutsideVolume ( iValue, DS3D_IMMEDIATE );

	if ( iType == 6 )
		g_pBuffer->SetMinDistance ( fValue, DS3D_IMMEDIATE );

	if ( iType == 7 )
		g_pBuffer->SetMaxDistance ( fValue, DS3D_IMMEDIATE );

	ClearData ( );
}

void SetEAXConeOrientationX ( int iSound, float fValue )
{
	SetCone ( iSound, 0, fValue, 0.0f, 0.0f, 0, 0.0f );
}

void SetEAXConeOrientationY ( int iSound, float fValue )
{
	SetCone ( iSound, 1, 0.0f, fValue, 0.0f, 0, 0.0f );
}

void SetEAXConeOrientationZ ( int iSound, float fValue )
{
	SetCone ( iSound, 2, 0.0f, 0.0f, fValue, 0, 0.0f );
}

void SetEAXConeInsideAngle ( int iSound, int iValue )
{
	SetCone ( iSound, 3, 0.0f, 0.0f, 0.0f, iValue, 0.0f );	
}

void SetEAXConeOutsideAngle ( int iSound, int iValue )
{
	SetCone ( iSound, 4, 0.0f, 0.0f, 0.0f, iValue, 0.0f );
}

void SetEAXConeOutsideVolume ( int iSound, int iValue )
{
	SetCone ( iSound, 5, 0.0f, 0.0f, 0.0f, iValue, 0.0f );
}

void SetEAXMinDistance ( int iSound, float fValue )
{
	SetCone ( iSound, 6, 0.0f, 0.0f, 0.0f, 0, fValue );
}

void SetEAXMaxDistance ( int iSound, float fValue )
{
	SetCone ( iSound, 7, 0.0f, 0.0f, 0.0f, 0, fValue );
}

void* GetEAXData ( int iSound, int iData )
{
	GetData ( iSound );

	switch ( iData )
	{
		case  0: return ( void* ) &g_EAXListener.lRoom;
		case  1: return ( void* ) &g_EAXListener.lRoomHF;
		case  2: return ( void* ) &g_EAXListener.flRoomRolloffFactor;
		case  3: return ( void* ) &g_EAXListener.flDecayTime;
		case  4: return ( void* ) &g_EAXListener.flDecayHFRatio;
		case  5: return ( void* ) &g_EAXListener.lReflections;
		case  6: return ( void* ) &g_EAXListener.flReflectionsDelay;
		case  7: return ( void* ) &g_EAXListener.lReverb;
		case  8: return ( void* ) &g_EAXListener.flReverbDelay;
		case  9: return ( void* ) &g_EAXListener.dwEnvironment;
		case 10: return ( void* ) &g_EAXListener.flEnvironmentSize;
		case 11: return ( void* ) &g_EAXListener.flEnvironmentDiffusion;
		case 12: return ( void* ) &g_EAXListener.flAirAbsorptionHF;
		case 13: return ( void* ) &g_EAXListener.dwFlags;

		case 14: return ( void* ) &g_EAX.lDirect;
		case 15: return ( void* ) &g_EAX.lDirectHF;
		case 16: return ( void* ) &g_EAX.lRoom;
		case 17: return ( void* ) &g_EAX.lRoomHF;
		case 18: return ( void* ) &g_EAX.flRoomRolloffFactor;
		case 19: return ( void* ) &g_EAX.lObstruction;
		case 20: return ( void* ) &g_EAX.flObstructionLFRatio;
		case 21: return ( void* ) &g_EAX.lOcclusion;
		case 22: return ( void* ) &g_EAX.flOcclusionLFRatio;
		case 23: return ( void* ) &g_EAX.flOcclusionRoomRatio;
		case 24: return ( void* ) &g_EAX.lOutsideVolumeHF;
		case 25: return ( void* ) &g_EAX.flAirAbsorptionFactor;
		case 26: return ( void* ) &g_EAX.dwFlags;
	}

	return NULL;
}

bool QuerySupport ( ULONG ulQuery )
{
	if ( !g_lpPropertySet )
		return false;

	ULONG ulSupport = 0;
	HRESULT hr = g_lpPropertySet->QuerySupport ( DSPROPSETID_EAX_ListenerProperties, ulQuery, &ulSupport );

	if ( FAILED ( hr ) )
		return false;

	if ( ( ulSupport&( KSPROPERTY_SUPPORT_GET | KSPROPERTY_SUPPORT_SET ) ) == ( KSPROPERTY_SUPPORT_GET | KSPROPERTY_SUPPORT_SET ) )
	{
		g_dwSupport |= ( DWORD ) ( 1 << ulQuery );
		return true;
	}

	return false;
}

int GetEAX ( void )
{
	g_dwSupport = 0;

	if ( !QuerySupport ( DSPROPERTY_EAXLISTENER_NONE                 ) ) return 0;
	if ( !QuerySupport ( DSPROPERTY_EAXLISTENER_ALLPARAMETERS        ) ) return 0;
	if ( !QuerySupport ( DSPROPERTY_EAXLISTENER_ROOM                 ) ) return 0;
	if ( !QuerySupport ( DSPROPERTY_EAXLISTENER_ROOMHF               ) ) return 0;
	if ( !QuerySupport ( DSPROPERTY_EAXLISTENER_ROOMROLLOFFFACTOR    ) ) return 0;
	if ( !QuerySupport ( DSPROPERTY_EAXLISTENER_DECAYTIME            ) ) return 0;
	if ( !QuerySupport ( DSPROPERTY_EAXLISTENER_DECAYHFRATIO         ) ) return 0;
	if ( !QuerySupport ( DSPROPERTY_EAXLISTENER_REFLECTIONS          ) ) return 0;
	if ( !QuerySupport ( DSPROPERTY_EAXLISTENER_REFLECTIONSDELAY     ) ) return 0;
	if ( !QuerySupport ( DSPROPERTY_EAXLISTENER_REVERB               ) ) return 0;
	if ( !QuerySupport ( DSPROPERTY_EAXLISTENER_REVERBDELAY          ) ) return 0;
	if ( !QuerySupport ( DSPROPERTY_EAXLISTENER_ENVIRONMENT          ) ) return 0;
	if ( !QuerySupport ( DSPROPERTY_EAXLISTENER_ENVIRONMENTSIZE      ) ) return 0;
	if ( !QuerySupport ( DSPROPERTY_EAXLISTENER_ENVIRONMENTDIFFUSION ) ) return 0;
	if ( !QuerySupport ( DSPROPERTY_EAXLISTENER_AIRABSORPTIONHF      ) ) return 0;
	if ( !QuerySupport ( DSPROPERTY_EAXLISTENER_FLAGS                ) ) return 0;

	return 1;
}

int GetEAXRoom ( int iSound )
{
	return *( int* ) GetEAXData ( iSound, 0 );
}

int GetEAXRoomHF ( int iSound )
{
	return *( int* ) GetEAXData ( iSound, 1 );
}

DWORD GetEAXRoomRolloff ( int iSound )
{
	return *( DWORD* ) GetEAXData ( iSound, 2 );
}

DWORD GetEAXDecayTime ( int iSound )
{
	return *( DWORD* ) GetEAXData ( iSound, 3 );
}

DWORD GetEAXDecayHFRatio ( int iSound )
{
	return *( DWORD* ) GetEAXData ( iSound, 4 );
}

int GetEAXReflections ( int iSound )
{
	return *( int* ) GetEAXData ( iSound, 5 );
}

DWORD GetEAXReflectionsDelay ( int iSound )
{
	return *( DWORD* ) GetEAXData ( iSound, 6 );
}

int GetEAXReverb ( int iSound )
{
	return *( int* ) GetEAXData ( iSound, 7 );
}

DWORD GetEAXReverbDelay ( int iSound )
{
	return *( DWORD* ) GetEAXData ( iSound, 8 );
}

int GetEAXEnvironment ( int iSound )
{
	return *( int* ) GetEAXData ( iSound, 9 );
}

DWORD GetEAXEnvironmentSize ( int iSound )
{
	return *( DWORD* ) GetEAXData ( iSound, 10 );
}

DWORD GetEAXEnvironmentDiffusion ( int iSound )
{
	return *( DWORD* ) GetEAXData ( iSound, 11 );
}

DWORD GetEAXAirAbsorption ( int iSound )
{
	return *( DWORD* ) GetEAXData ( iSound, 12 );
}

int GetEAXFlags ( int iSound )
{
	return *( int* ) GetEAXData ( iSound, 13 );
}

int GetEAXSourceDirect ( int iSound )
{
	return *( int* ) GetEAXData ( iSound, 14 );
}

int GetEAXSourceDirectHF ( int iSound )
{
	return *( int* ) GetEAXData ( iSound, 15 );
}

int GetEAXSourceRoom ( int iSound )
{
	return *( int* ) GetEAXData ( iSound, 16 );
}

DWORD GetEAXSourceRoomHF ( int iSound )
{
	return *( DWORD* ) GetEAXData ( iSound, 17 );
}

DWORD GetEAXSourceRolloff ( int iSound )
{
	return *( DWORD* ) GetEAXData ( iSound, 18 );
}

int GetEAXSourceOutside ( int iSound )
{
	return *( int* ) GetEAXData ( iSound, 19 );
}

DWORD GetEAXSourceAbsorption ( int iSound )
{
	return *( DWORD* ) GetEAXData ( iSound, 20 );
}

int GetEAXSourceFlags ( int iSound )
{
	return *( int* ) GetEAXData ( iSound, 21 );
}

int GetEAXSourceObstruction ( int iSound )
{
	return *( int* ) GetEAXData ( iSound, 22 );
}

DWORD GetEAXSourceObstructionLF ( int iSound )
{
	return *( DWORD* ) GetEAXData ( iSound, 23 );
}

int GetEAXSourceOcclusion ( int iSound )
{
	return *( int* ) GetEAXData ( iSound, 24 );
}

DWORD GetEAXSourceOcclusionLF ( int iSound )
{
	return *( DWORD* ) GetEAXData ( iSound, 25 );
}

DWORD GetEAXSourceOcclusionRoom ( int iSound )
{
	return *( DWORD* ) GetEAXData ( iSound, 26 );
}

void* GetConeData ( int iSound, int iData )
{
	GetData ( iSound );

	g_pBuffer->GetConeOrientation   ( &g_vecCone );
	g_pBuffer->GetConeAngles        ( &g_dwIn, &g_dwOut );
	g_pBuffer->GetMinDistance       ( &g_fMin );
	g_pBuffer->GetMaxDistance       ( &g_fMax );
	g_pBuffer->GetConeOutsideVolume ( &g_lVolume );

	switch ( iData )
	{
		case 0: return ( void* ) &g_vecCone.x;
		case 1: return ( void* ) &g_vecCone.y;
		case 2: return ( void* ) &g_vecCone.z;
		case 3: return ( void* ) &g_dwIn;
		case 4: return ( void* ) &g_dwOut;
		case 5: return ( void* ) &g_lVolume;
		case 6: return ( void* ) &g_fMin;
		case 7: return ( void* ) &g_fMax;
	}
	
	return NULL;
}

DWORD GetEAXConeOrientationX ( int iSound )
{
	return *( DWORD* ) GetConeData ( iSound, 0 );
}

DWORD GetEAXConeOrientationY ( int iSound )
{
	return *( DWORD* ) GetConeData ( iSound, 1 );
}

DWORD GetEAXConeOrientationZ ( int iSound )
{
	return *( DWORD* ) GetConeData ( iSound, 2 );
}

int GetEAXConeInsideAngle ( int iSound )
{
	return *( int* ) GetConeData ( iSound, 3 );
}

int GetEAXConeOutsideAngle ( int iSound )
{
	return *( int* ) GetConeData ( iSound, 4 );
}

int GetEAXConeOutsideVolume ( int iSound )
{
	return *( int* ) GetConeData ( iSound, 5 );
}

DWORD GetEAXMinDistance ( int iSound )
{
	return *( DWORD* ) GetConeData ( iSound, 6 );
}

DWORD GetEAXMaxDistance ( int iSound )
{
	return *( DWORD* ) GetConeData ( iSound, 7 );
}