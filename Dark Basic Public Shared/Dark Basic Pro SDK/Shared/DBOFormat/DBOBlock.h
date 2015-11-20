
//
// DBOBlock Functions Header
//

#ifndef _DBOBLOCK_H_
#define _DBOBLOCK_H_

//////////////////////////////////////////////////////////////////////////////////////
// COMMON INCLUDES ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>

#include "..\global.h"

//#define DARKSDK __declspec ( dllexport )


//////////////////////////////////////////////////////////////////////////////////////
// DBOFORMAT INCLUDE /////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

#include "DBOData.h"
#include "stdio.h"

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

// Version Control from U6 onwards:
// 0001 - Pre-U6 version
// 1060 - U1.060 version 
// 1062 - U1.062 version 
#define	DBO_VERSION_NUMBER					1062

//////////////////////////////////////////////////////////////////////////////////////
// DEFINES ///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

#define DBOBLOCK_ROOT_FRAME					1
#define DBOBLOCK_ROOT_ANIMATIONSET			2

#define	DBOBLOCK_FRAME_NAME					101
#define	DBOBLOCK_FRAME_MATRIX				102
#define	DBOBLOCK_FRAME_MESH					103
#define	DBOBLOCK_FRAME_CHILD				104
#define	DBOBLOCK_FRAME_SIBLING				105
#define	DBOBLOCK_FRAME_OFFSET				106
#define	DBOBLOCK_FRAME_ROTATION				107
#define	DBOBLOCK_FRAME_SCALE				108

#define	DBOBLOCK_MESH_FVF					111
#define	DBOBLOCK_MESH_FVFSIZE				112
#define	DBOBLOCK_MESH_VERTEXCOUNT			113
#define	DBOBLOCK_MESH_INDEXCOUNT			114
#define	DBOBLOCK_MESH_VERTEXDATA			115
#define	DBOBLOCK_MESH_INDEXDATA				116
#define	DBOBLOCK_MESH_PRIMTYPE				117
#define	DBOBLOCK_MESH_DRAWVERTCOUNT			118
#define	DBOBLOCK_MESH_DRAWPRIMCOUNT			119
#define DBOBLOCK_MESH_VERTEXDEC				120

#define	DBOBLOCK_MESH_BONECOUNT				121
#define	DBOBLOCK_MESH_BONESDATA				122
#define	DBOBLOCK_MESH_USEMATERIAL			125
#define	DBOBLOCK_MESH_MATERIAL				126
#define	DBOBLOCK_MESH_TEXTURECOUNT			127
#define	DBOBLOCK_MESH_TEXTURES				128
#define	DBOBLOCK_MESH_WIREFRAME				129
#define	DBOBLOCK_MESH_LIGHT					130
#define	DBOBLOCK_MESH_CULL					131
#define	DBOBLOCK_MESH_FOG					132
#define	DBOBLOCK_MESH_AMBIENT				133
#define	DBOBLOCK_MESH_TRANSPARENCY			134
#define	DBOBLOCK_MESH_GHOST					135
#define	DBOBLOCK_MESH_GHOSTMODE				136

#define	DBOBLOCK_MESH_LINKED				137
#define	DBOBLOCK_MESH_SUBFRAMES				138
#define DBOBLOCK_MESH_FXEFFECTNAME			154
#define DBOBLOCK_MESH_ARBITARYVALUE			155

#define DBOBLOCK_MESH_ZBIASFLAG				156
#define DBOBLOCK_MESH_ZBIASSLOPE			157
#define DBOBLOCK_MESH_ZBIASDEPTH			158
#define DBOBLOCK_MESH_ZREAD					159
#define DBOBLOCK_MESH_ZWRITE				160
#define DBOBLOCK_MESH_ALPHATESTVALUE		166

#define	DBOBLOCK_MESH_USEMULTIMAT			123
#define	DBOBLOCK_MESH_MULTIMATCOUNT			124
#define	DBOBLOCK_MESH_MULTIMAT				139
#define DBOBLOCK_MESH_VISIBLE				140

#define	DBOBLOCK_TEXTURE_NAME				141
#define	DBOBLOCK_TEXTURE_STAGE				142
#define	DBOBLOCK_TEXTURE_BMODE				143
#define	DBOBLOCK_TEXTURE_BARG1				144
#define	DBOBLOCK_TEXTURE_BARG2				145
#define	DBOBLOCK_TEXTURE_ADDRU				146
#define	DBOBLOCK_TEXTURE_ADDRV				147
#define	DBOBLOCK_TEXTURE_MAG				148
#define	DBOBLOCK_TEXTURE_MIN				149
#define	DBOBLOCK_TEXTURE_MIP				150
#define	DBOBLOCK_TEXTURE_TCMODE				151
#define	DBOBLOCK_TEXTURE_PRIMST				152
#define	DBOBLOCK_TEXTURE_PRIMCN				153

#define	DBOBLOCK_MULTIMAT_NAME				161
#define	DBOBLOCK_MULTIMAT_MATERIAL			162
#define	DBOBLOCK_MULTIMAT_START				163
#define	DBOBLOCK_MULTIMAT_COUNT				164
#define	DBOBLOCK_MULTIMAT_POLY				165

#define DBOBLOCK_ANIMSET_NAME				201
#define DBOBLOCK_ANIMSET_LENGTH				202
#define DBOBLOCK_ANIMSET_DATA				203

#define DBOBLOCK_ANIM_NAME					211
#define DBOBLOCK_ANIM_NUMPOSKEYS			212
#define DBOBLOCK_ANIM_POSDATA				213
#define DBOBLOCK_ANIM_NUMROTKEYS			214
#define DBOBLOCK_ANIM_ROTDATA				215
#define DBOBLOCK_ANIM_NUMSCALEKEYS			216
#define DBOBLOCK_ANIM_SCALEDATA				217
#define DBOBLOCK_ANIM_NUMMATRIXKEYS			218
#define DBOBLOCK_ANIM_MATRIXDATA			219
#define DBOBLOCK_ANIM_NEXT					220

#define DBOBLOCK_BONES_NAME					301
#define DBOBLOCK_BONES_NUMINFLUENCES		302
#define DBOBLOCK_BONES_VERTICES				303
#define DBOBLOCK_BONES_WEIGHTS				304
#define DBOBLOCK_BONES_TRANSLATIONMATRIX	305

#define	DBOBLOCK_FRAME_BONETYPE				400
#define	DBOBLOCK_FRAME_BONEOFFSETLIST		401
#define	DBOBLOCK_FRAME_BONEOFFSETLISTSIZE	402
#define	DBOBLOCK_FRAME_BONEMATRIXSIZEA		403
#define	DBOBLOCK_FRAME_BONEMATRIXSIZEB		404
#define	DBOBLOCK_FRAME_BONEMATRIXLIST		405
#define	DBOBLOCK_FRAME_BONEMATRIXDATA		406

// 280305 - used when objects want to store custom data for example when
//        - they save and want to save out this data
#define	DBOBLOCK_OBJECT_CUSTOMDATA			406

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS /////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

DARKSDK bool DBOConvertObjectToBlock	( sObject* pObject, DWORD* ppBlock, DWORD* pdwBlockSize );
DARKSDK bool DBOConvertBlockToObject	( DWORD pBlock, DWORD dwBlockSize, sObject** ppObject );
DARKSDK bool DBOLoadBlockFile			( LPSTR pFilename, DWORD* ppBlock, DWORD* pdwSize );
DARKSDK bool DBOSaveBlockFile			( LPSTR pFilename, DWORD pBlock, DWORD dwSize );
DARKSDK bool ConstructObject            ( sObject** ppObject, LPSTR* ppBlock );

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

#endif _DBOBLOCK_H_
