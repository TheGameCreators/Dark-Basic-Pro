
//
// DBOMesh Functions Header
//

//////////////////////////////////////////////////////////////////////////////////
// DBOFORMAT COMMON ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#include "DBOFormat.h"
#include "..\global.h"

// Workhorse functions

void ResetMaterial						( D3DMATERIAL9* pMaterial );

// Mesh Setting Functions

DARKSDK void SetWireframe				( sMesh* pMesh, bool bWireframe );
DARKSDK void SetTransparency			( sMesh* pMesh, bool bTransparency );
DARKSDK void SetAlphaTest				( sMesh* pMesh, DWORD dwAlphaValue );
DARKSDK void SetCullCWCCW				( sMesh* pMesh, int iCullMode );
DARKSDK void SetCull					( sMesh* pMesh, bool bCull );
DARKSDK void SetFilter					( sMesh* pMesh, int iFilter );
DARKSDK void SetFilter					( sMesh* pMesh, int iStage, int iFilter );
DARKSDK void SetLight					( sMesh* pMesh, bool bLight );
DARKSDK void SetFog						( sMesh* pMesh, bool bFog );
DARKSDK void SetAmbient					( sMesh* pMesh, bool bAmbient );
DARKSDK void SetZRead					( sMesh* pMesh, bool bZRead );
DARKSDK void SetZWrite					( sMesh* pMesh, bool bZWrite );
DARKSDK void SetZBias					( sMesh* pMesh, bool bZBias, float fSlopeScale, float fDepth );

// Mesh Component Data Functions

DARKSDK void SetDiffuseEx				( sMesh* pMesh, DWORD dwRGB );
DARKSDK void SetDiffuse					( sMesh* pMesh, float fPercentage );
DARKSDK void ScrollTexture				( sMesh* pMesh, int iStage, float fU, float fV );
DARKSDK void ScrollTexture				( sMesh* pMesh, float fU, float fV );
DARKSDK void ScaleTexture				( sMesh* pMesh, int iStage, float fU, float fV );
DARKSDK void ScaleTexture				( sMesh* pMesh, float fU, float fV );
DARKSDK void SmoothNormals				( sMesh* pMesh, float fAngle );
DARKSDK void GenerateNormals			( sMesh* pMesh );
DARKSDK void ReduceMeshPolygons			( sMesh* pMesh, int iBlockMode, int iNearMode, int iGX, int iGY, int iGZ );
DARKSDK int  CheckIfMeshSolid			( sMesh* pMesh, int iGX, int iGY, int iGZ );
DARKSDK bool CheckIfMeshBlocking		( sMesh* pMesh, float X1, float Y1, float Z1, float X2, float Y2, float Z2 );

// Mesh VertexData Functions

DARKSDK void  SetPositionData			( sMesh* pMesh, int iCurrentVertex, float fX, float fY, float fZ );
DARKSDK void  SetNormalsData			( sMesh* pMesh, int iCurrentVertex, float fNX, float fNY, float fNZ );
DARKSDK void  SetDiffuseData			( sMesh* pMesh, int iCurrentVertex, DWORD dwDiffuse );
DARKSDK void  SetUVData					( sMesh* pMesh, int iCurrentVertex, float fU, float fV );
DARKSDK bool  AddMeshToData				( sMesh* pMesh, sMesh* pMeshToAdd );
DARKSDK bool  DeleteMeshFromData		( sMesh* pMesh, int iVertex1, int iVertex2, int iIndex1, int iIndex2 );
DARKSDK int   GetVertexCount			( sMesh* pMesh );
DARKSDK int   GetIndexCount				( sMesh* pMesh );
DARKSDK float GetDataPositionX			( sMesh* pMesh, int iCurrentVertex );
DARKSDK float GetDataPositionY			( sMesh* pMesh, int iCurrentVertex );
DARKSDK float GetDataPositionZ			( sMesh* pMesh, int iCurrentVertex );
DARKSDK float GetDataNormalsX			( sMesh* pMesh, int iCurrentVertex );
DARKSDK float GetDataNormalsY			( sMesh* pMesh, int iCurrentVertex );
DARKSDK float GetDataNormalsZ			( sMesh* pMesh, int iCurrentVertex );
DARKSDK DWORD GetDataDiffuse			( sMesh* pMesh, int iCurrentVertex );
DARKSDK float GetDataU					( sMesh* pMesh, int iCurrentVertex );
DARKSDK float GetDataV					( sMesh* pMesh, int iCurrentVertex );
DARKSDK float GetDataU					( sMesh* pMesh, int iCurrentVertex, int iIndex );
DARKSDK float GetDataV					( sMesh* pMesh, int iCurrentVertex, int iIndex );

// Mesh Visual Functions

DARKSDK void  Hide						( sMesh* pMesh );
DARKSDK void  Show						( sMesh* pMesh );
DARKSDK void  SetGhost					( sMesh* pMesh, bool bGhost, int iGhostMode );
DARKSDK void  SetTextureMode			( sMesh* pMesh, int iMode, int iMipMode );
DARKSDK void  SetTextureMode			( sMesh* pMesh, int iStage, int iMode, int iMipMode );

// Mesh Texture Functions

int   LoadOrFindTextureAsImage	( LPSTR pTextureName, LPSTR TexturePath );
int   LoadOrFindTextureAsImage	( LPSTR pTextureName, LPSTR TexturePath, int iDivideTextureSize );
void  LoadInternalTextures		( sMesh* pMesh, LPSTR pPath, int iDBProMode, int iDivideTextureSize, LPSTR pOptionalLightmapNoReduce );
void  LoadInternalTextures		( sMesh* pMesh, LPSTR pPath, int iDBProMode, int iDivideTextureSize );
void  LoadInternalTextures		( sMesh* pMesh, LPSTR pPath, int iDBProMode );
void  FreeInternalTextures      ( sMesh* pMesh );
void  CloneInternalTextures		( sMesh* pMeshDest, sMesh* pMeshSrc );
void  CopyMeshSettings			( sMesh* pDestMesh, sMesh* pSrcMesh );
void  SetBaseTexture			( sMesh* pMesh, int iStage, int iImage );
void  SetBaseTextureStage		( sMesh* pMesh, int iStage, int iImage );
void  SetMultiTexture			( sMesh* pMesh, int iStage, DWORD dwBlendMode, DWORD dwTexCoordMode, int iImage );
void  SetCubeTexture			( sMesh* pMesh, int iStage, LPDIRECT3DCUBETEXTURE9 pCubeTexture );
void  SetBaseColor				( sMesh* pMesh, DWORD dwRGB );
void  SetAlphaOverride			( sMesh* pMesh, float fPercentage );
void  SetDiffuseMaterial		( sMesh* pMesh, DWORD dwRGB );
void  SetAmbienceMaterial		( sMesh* pMesh, DWORD dwRGB );
void  SetSpecularMaterial		( sMesh* pMesh, DWORD dwRGB );
void  SetEmissiveMaterial		( sMesh* pMesh, DWORD dwRGB );
void  SetSpecularPower			( sMesh* pMesh, float fPower );
void  RemoveTextureRefFromMesh  ( sMesh* pMesh, LPDIRECT3DTEXTURE9 pTextureRef );

// Mesh Shader Functions

DARKSDK void  SetBumpTexture			( sMesh* pMesh, int iImage, int iGenerateNormalMap );
DARKSDK void  SetCartoonTexture			( sMesh* pMesh, int iImage, int iEdge );
DARKSDK void  SetRainbowTexture			( sMesh* pMesh, int iImage );

// Mesh Effect Functions

DARKSDK bool  SetSpecialEffect			( sMesh* pMesh, cSpecialEffect* pEffectObj, bool bChangeMesh );
DARKSDK bool  SetSpecialEffect			( sMesh* pMesh, cSpecialEffect* pEffectObj );

// Mesh Custom Shader Functions

DARKSDK void  ConvertToFVF				( sMesh* pMesh, DWORD dwFVF );
DARKSDK void  CombineSubsetPolygonsInMesh ( sMesh* pMesh );
DARKSDK void  SetCustomShader			( sMesh* pMesh, LPDIRECT3DVERTEXSHADER9 pVertexShader, LPDIRECT3DVERTEXDECLARATION9 pVertexDec, DWORD dwStagesRequired );
DARKSDK void  SetNoShader				( sMesh* pMesh );

// Mesh Custom Pixel Shader Functions

DARKSDK void  SetCustomPixelShader		( sMesh* pMesh, LPDIRECT3DPIXELSHADER9 pPixelShader );
DARKSDK void  SetNoPixelShader			( sMesh* pMesh );

// Mesh Animation Functions

//DARKSDK bool  AnimateBoneMesh			( sObject* pObject, sFrame* pFrame );
bool  AnimateBoneMesh					( sObject* pObject, sFrame* pFrame );
bool  AnimateBoneMesh					( sObject* pObject, sFrame* pFrame, sMesh* pMesh );
void  AnimateBoneMeshBONE				( sObject* pObject, sFrame* pFrame, sMesh* pMesh );

void  ResetVertexDataInMeshPerMesh		( sMesh* pMesh );
void  CollectOriginalVertexData			( sMesh* pMesh );
void  ResetVertexDataInMesh				( sObject* pObject );
void  UpdateVertexDataInMesh			( sObject* pObject );

// Mesh Shadow Functions

DARKSDK bool  CreateShadowMesh ( sMesh* pMesh, sMesh** pShadowMesh, int iUseShader );
DARKSDK bool  CastShadowGeometryToShadowMesh ( D3DXVECTOR3 vecLightPos, sMesh* pCasterMesh, float fShadowRange, sMesh** ppShadowMesh, DWORD* pdwStartAtVertex, D3DXMATRIX* pmatWorld, D3DXMATRIX* pmatBoneMatrix );
DARKSDK bool  UpdateShadowMesh ( float fX, float fY, float fZ, float fRange, sMesh* pMesh, sMesh* pShadowMesh, D3DXMATRIX* pmatWorld, D3DXMATRIX* pmatCombined );

// Mesh Construction Functions

DARKSDK bool  MakeMeshPlain				( bool bCreateNew, sMesh* pMesh, float fWidth, float fHeight, DWORD dwFVF, DWORD dwColor );
DARKSDK bool  MakeMeshPlainEx			( bool bCreateNew, sMesh* pMesh, float fWidth, float fHeight, DWORD dwFVF, DWORD dwColor );
DARKSDK bool  MakeMeshBox				( bool bCreateNew, sMesh* pMesh, float fWidth1, float fHeight1, float fDepth1, float fWidth2, float fHeight2, float fDepth2, DWORD dwFVF, DWORD dwColor );
DARKSDK bool  MakeMeshPyramid			( bool bCreateNew, sMesh* pMesh, float fSize, DWORD dwFVF, DWORD dwColor );
DARKSDK bool  MakeMeshSphere			( bool bCreateNew, sMesh* pMesh, D3DXVECTOR3 vecCenter, float fRadius, int iRings, int iSegments, DWORD dwFVF, DWORD dwColor );
DARKSDK bool  MakeMeshFromOtherMesh		( bool bCreateNew, sMesh* pMesh, sMesh* pOtherMesh, D3DXMATRIX* pmatWorld );
DARKSDK bool  MakeMeshFromOtherMesh		( bool bCreateNew, sMesh* pMesh, sMesh* pOtherMesh, D3DXMATRIX* pmatWorld, DWORD dwIndexCount, DWORD dwVertexCount );
DARKSDK bool  MakeLODMeshFromMesh		( sMesh* pMeshIn, int iVertexNum, sMesh** ppMeshOut );

