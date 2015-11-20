
#ifndef _COBJECTS_H_
#define _COBJECTS_H_

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#include "CommonC.h"

//////////////////////////////////////////////////////////////////////////////////
// COMMAND FUNCTIONS /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK void RefreshMeshShortList ( sMesh* pMesh );

DARKSDK void Load				( SDK_LPSTR szFilename, int iID );
DARKSDK void Load				( SDK_LPSTR szFilename, int iID, int iDBProMode );
DARKSDK void Load				( SDK_LPSTR szFilename, int iID, int iDBProMode, int iDivideTextureSize );
DARKSDK void Save				( SDK_LPSTR szFilename, int iID );
DARKSDK void DeleteEx			( int iID );
DARKSDK void Set				( int iID, SDK_BOOL bWireframe, SDK_BOOL bTransparency, SDK_BOOL bCull );
DARKSDK void Set				( int iID, SDK_BOOL bWireframe, SDK_BOOL bTransparency, SDK_BOOL bCull, int iFilter );
DARKSDK void Set				( int iID, SDK_BOOL bWireframe, SDK_BOOL bTransparency, SDK_BOOL bCull, int iFilter, SDK_BOOL bLight );
DARKSDK void Set				( int iID, SDK_BOOL bWireframe, SDK_BOOL bTransparency, SDK_BOOL bCull, int iFilter, SDK_BOOL bLight, SDK_BOOL bFog );
DARKSDK void Set				( int iID, SDK_BOOL bWireframe, SDK_BOOL bTransparency, SDK_BOOL bCull, int iFilter, SDK_BOOL bLight, SDK_BOOL bFog, SDK_BOOL bAmbient );
DARKSDK void SetWireframe		( int iID, SDK_BOOL bFlag    );
DARKSDK void SetTransparency	( int iID, SDK_BOOL bFlag    );
DARKSDK void SetCull			( int iID, SDK_BOOL bCull    );
DARKSDK void SetFilter			( int iID, int  iFilter  );
DARKSDK void SetFilterStage		( int iID, int iStage, int iFilter );
DARKSDK void SetLight			( int iID, SDK_BOOL bLight   );
DARKSDK void SetFog				( int iID, SDK_BOOL bFog     );
DARKSDK void SetAmbient			( int iID, SDK_BOOL bAmbient );
DARKSDK void MakeObject			( int iID, int iMeshID, int iTextureID );
DARKSDK void MakeSphere			( int iID, float fRadius, int iRings, int iSegments );
DARKSDK void CloneShared		( int iDestinationID, int iSourceID, int iCloneSharedData );
DARKSDK void Clone				( int iDestinationID, int iSourceID );
DARKSDK void Instance			( int iDestinationID, int iSourceID );

DARKSDK void InitInstanceStamp	( int iDestinationID, int iX, int iY, int iZ, int iViewX, int iViewY, int iViewZ );
DARKSDK void FreeInstanceStamp	( int iID );
DARKSDK void InstanceStamp		( int iDestinationID, int iSourceInstanceID, int iAddRemoveFlag );

DARKSDK void MakeSphere			( int iID, float fRadius );
DARKSDK void MakeCube			( int iID, float iSize );
DARKSDK void MakeBox			( int iID, float fWidth, float fHeight, float fDepth );
DARKSDK void MakePyramid		( int iID, float fSize );
DARKSDK void MakeCylinder		( int iID, float fSize );
DARKSDK void MakeCone			( int iID, float fSize );
DARKSDK void MakePlane			( int iID, float fWidth, float fHeight );
DARKSDK void MakePlane			( int iID, float fWidth, float fHeight, int iFlag );
DARKSDK void MakeTriangle		( int iID, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3 );
DARKSDK void Append				( SDK_LPSTR pString, int iID, int iFrame );
DARKSDK void Play				( int iID );
DARKSDK void Play 				( int iID, int iStart );
DARKSDK void Play				( int iID, int iStart, int iEnd );
DARKSDK void Loop				( int iID );
DARKSDK void Loop				( int iID, int iStart );
DARKSDK void Loop				( int iID, int iStart, int iEnd );
DARKSDK void Stop				( int iID );
DARKSDK void AddVisibilityListMask ( int iID );
DARKSDK void Hide				( int iID );
DARKSDK void Show				( int iID );
DARKSDK void Color				( int iID, DWORD dwRGB );
DARKSDK void SetFrame			( int iID, int iFrame );
DARKSDK void SetFrameEx			( int iID, float fFrame );
DARKSDK void SetFrameEx			( int iID, float fFrame, int iRecalculateBounds );
DARKSDK void SetFrameEx			( int iID, int iLimbID, float fFrame, int iEnableOverride );
DARKSDK void SetSpeed			( int iID, int iSpeed );
DARKSDK void SetInterpolation	( int iID, int iJump );
DARKSDK void SetRotationXYZ		( int iID );
DARKSDK void SetRotationZYX		( int iID );
DARKSDK void SetRotationZXY		( int iID );
DARKSDK void GhostOn			( int iID );
DARKSDK void GhostOn			( int iID, int iFlag );
DARKSDK void GhostOff			( int iID );
DARKSDK void Fade				( int iID, float iPercentage );
DARKSDK void GlueToLimbEx		( int iSource, int iTarget, int iLimb, int iGlueMode );
DARKSDK void GlueToLimb			( int iSource, int iTarget, int iLimb );
DARKSDK void UnGlue				( int iID );
DARKSDK void LockOn				( int iID );
DARKSDK void LockOff			( int iID );
DARKSDK void DisableZDepth		( int iID );
DARKSDK void EnableZDepth		( int iID );
DARKSDK void DisableZRead		( int iID );
DARKSDK void EnableZRead		( int iID );
DARKSDK void DisableZWrite		( int iID );
DARKSDK void EnableZWrite		( int iID );
DARKSDK void SetObjectFOV		( int iID, float fRadius );
DARKSDK void Deletes			( int iFrom, int iTo );
DARKSDK void ClearObjectsOfTextureRef ( LPDIRECT3DTEXTURE9 pTextureRef );
DARKSDK void DisableZBias		( int iID );
DARKSDK void EnableZBias		( int iID, float fSlopeScale, float fDepth );
DARKSDK void ReverseObjectFrames ( int iID );

// Additional Color commands

DARKSDK void SetDiffuseMaterialEx		( int iID, DWORD dwRGB, int iMaterialOrVertexData );
DARKSDK void SetDiffuseMaterial			( int iID, DWORD dwRGB );
DARKSDK void SetAmbienceMaterial		( int iID, DWORD dwRGB );
DARKSDK void SetSpecularMaterial		( int iID, DWORD dwRGB );
DARKSDK void SetSpecularPower			( int iID, float fPower );
DARKSDK void SetSpecularMaterial		( int iID, DWORD dwRGB, float fPower );
DARKSDK void SetEmissiveMaterial		( int iID, DWORD dwRGB );
DARKSDK void SetArbitaryValue			( int iID, DWORD dwArbValue );

// Additional Euler/Freeflight commands
DARKSDK void FixPivot					( int iID );
DARKSDK void ResetPivot					( int iID );
DARKSDK void SetToObjectOrientation		( int iID, int iWhichID );
DARKSDK void SetToObjectOrientation		( int iID, int iWhichID, int iMode );
DARKSDK void SetToCameraOrientation		( int iID );

// Texture commands
DARKSDK void SetTexture					( int iID, int iImage );
DARKSDK void SetTextureStageEx			( int iID, int iStage, int iImage, int iDoNotSortTextures );
DARKSDK void SetTextureStage			( int iID, int iStage, int iImage );
DARKSDK void ScrollTexture				( int iID, int iStage, float fU, float fV );
DARKSDK void ScrollTexture				( int iID, float fU, float fV );
DARKSDK void ScaleTexture				( int iID, int iStage, float fU, float fV );
DARKSDK void ScaleTexture				( int iID, float fU, float fV );
DARKSDK void SetSmoothing				( int iID, float fAngle );
DARKSDK void SetNormals					( int iID );
DARKSDK void SetTextureMode				( int iID, int iMode, int iMipGeneration );
DARKSDK void SetTextureModeStage		( int iID, int iStage, int iMode, int iMipGeneration );
DARKSDK void SetLightMap				( int iID, int iImage );
DARKSDK void SetLightMap				( int iID, int iImage, int iAddDIffuseToStageZero );
DARKSDK void SetSphereMap				( int iID, int iSphereImage );
DARKSDK void SetCubeMap					( int iID, int i1, int i2, int i3, int i4, int i5, int i6 );
DARKSDK void SetDetailMap				( int iID, int iImage );
DARKSDK void SetBlendMap				( int iID, int iLimbNumber, int iStage, int iImage, int iTexCoordMode, int iMode, int iA, int iB, int iC, int iR );
DARKSDK void SetBlendMap				( int iID, int iStage, int iImage, int iTexCoordMode, int iMode, int iA, int iB, int iC, int iR );
DARKSDK void SetBlendMap				( int iID, int iStage, int iImage, int iTexCoordMode, int iMode );
DARKSDK void SetBlendMap				( int iID, int iImage, int iMode );
DARKSDK void SetTextureMD3              ( int iID, int iH0, int iH1, int iL0, int iL1, int iL2, int iU0 );

// 2790515 - can be called from external DLLs (blitzterrain to render to depth texture)
DARKSDK int  SwitchRenderTargetToDepthTexture ( int iID );

// Additional texture commands
DARKSDK void SetAlphaFactor				( int iID, float fPercentage );

// built-in shader effects
DARKSDK void SetBumpMap					( int iID, int iBumpMap );
DARKSDK void SetCartoonShadingOn		( int iID, int iStandardImage, int iEdgeImage );
DARKSDK void SetRainbowRenderingOn		( int iID, int iStandardImage );

// bolt-on effects
DARKSDK void SetEffectOn				( int iID, SDK_LPSTR pFilename, int iUseDefaultTextures );

// occlusion commands
DARKSDK void SetOcclusionMode			( int iMode );
DARKSDK void SetObjectOcclusion			( int iID, int iOcclusionShape, int iMeshOrLimbID, int iA, int iIsOccluder, int iDeleteFromOccluder );
DARKSDK int  GetObjectOcclusionValue	( int iID );

// FX effects commands
DARKSDK void LoadEffect					( SDK_LPSTR pFilename, int iEffectID, int iUseDefaultTextures );
DARKSDK void LoadEffectEx				( SDK_LPSTR pFilename, int iEffectID, int iUseDefaultTextures, int iDoNotGenerateExtraData );
DARKSDK void DeleteEffect				( int iEffectID );
DARKSDK void SetEffectToShadowMapping	( int iEffectID );
DARKSDK void SetEffectShadowMappingMode	( int iMode );
DARKSDK void RenderEffectShadowMapping	( int iEffectID );
DARKSDK void SetObjectEffectCore		( int iID, int iEffectID, int iEffectNoBoneID, int iForceCPUAnimationMode );
DARKSDK void SetObjectEffectCore		( int iID, int iEffectID, int iForceCPUAnimationMode );
DARKSDK void SetObjectEffect			( int iID, int iEffectID );
DARKSDK void SetLimbEffect				( int iID, int iLimbID, int iEffectID );
DARKSDK void PerformChecklistForEffectValues ( int iEffectID );
DARKSDK void PerformChecklistForEffectErrors ( void );
DARKSDK void PerformChecklistForEffectErrors ( int iEffectID );
DARKSDK void SetEffectTranspose			( int iEffectID, int iTransposeFlag );
DARKSDK int  GetEffectParameterIndex	( int iEffectID, SDK_LPSTR pConstantName );
DARKSDK void SetEffectConstantB			( int iEffectID, SDK_LPSTR pConstantName, int iParamIndex, int iValue );
DARKSDK void SetEffectConstantB			( int iEffectID, SDK_LPSTR pConstantName, int iValue );
DARKSDK void SetEffectConstantBEx		( int iEffectID, int iParamIndex, int iValue );
DARKSDK void SetEffectConstantI			( int iEffectID, SDK_LPSTR pConstantName, int iParamIndex, int iValue );
DARKSDK void SetEffectConstantI			( int iEffectID, SDK_LPSTR pConstantName, int iValue );
DARKSDK void SetEffectConstantIEx		( int iEffectID, int iParamIndex, int iValue );
DARKSDK void SetEffectConstantF			( int iEffectID, SDK_LPSTR pConstantName, int iParamIndex, float fValue );
DARKSDK void SetEffectConstantF			( int iEffectID, SDK_LPSTR pConstantName, float fValue );
DARKSDK void SetEffectConstantFEx		( int iEffectID, int iParamIndex, float fValue );
DARKSDK void SetEffectConstantV			( int iEffectID, SDK_LPSTR pConstantName, int iParamIndex, int iValue );
DARKSDK void SetEffectConstantV			( int iEffectID, SDK_LPSTR pConstantName, int iValue );
DARKSDK void SetEffectConstantVEx		( int iEffectID, int iParamIndex, int iValue );
DARKSDK void SetEffectConstantM			( int iEffectID, SDK_LPSTR pConstantName, int iParamIndex, int iValue );
DARKSDK void SetEffectConstantM			( int iEffectID, SDK_LPSTR pConstantName, int iValue );
DARKSDK void SetEffectConstantMEx		( int iEffectID, int iParamIndex, int iValue );
DARKSDK void SetEffectTechnique			( int iEffectID, SDK_LPSTR pTechniqueName );
DARKSDK void SetEffectLODTechnique		( int iEffectID, SDK_LPSTR pTechniqueName );

// new commands - need help additions too!
DARKSDK int GetEffectExist				( int iEffectID );
DARKSDK int GetObjectPolygonCount		( int iObjectNumber );
DARKSDK int GetObjectVertexCount		( int iObjectNumber );

// built-in stencil object effects
DARKSDK void SetShadowOnEx				( int iID, int iMesh, int iRange, int iUseShader );
DARKSDK void SetShadowOn				( int iID );
DARKSDK void SetShadowOff				( int iID );
DARKSDK void SetShadowPosition			( int iMode, float fX, float fY, float fZ );
DARKSDK void SetReflectionOn			( int iID );
DARKSDK void SetShadowColor				( int iRed, int iGreen, int iBlue, int iAlphaLevel );
DARKSDK void SetShadowShades			( int iShades );
DARKSDK void SetReflectionColor			( int iRed, int iGreen, int iBlue, int iAlphaLevel );
DARKSDK void SetShadingOff				( int iID );

// vertex and pixel shaders
DARKSDK void SetVertexShaderOn          ( int iID, int iShader );
DARKSDK void SetVertexShaderOff         ( int iID );
DARKSDK void CloneMeshToNewFormat       ( int iID, DWORD dwFVF );
DARKSDK void CloneMeshToNewFormat       ( int iID, DWORD dwFVF, DWORD dwEraseBones );
DARKSDK void SetVertexShaderStreamCount ( int iID, int iCount );
DARKSDK void SetVertexShaderStream      ( int iID, int iStreamPos, int iData, int iDataType );
DARKSDK void CreateVertexShaderFromFile ( int iID, SDK_LPSTR szFile );
DARKSDK void SetVertexShaderVector      ( int iID, DWORD dwRegister, int iVector, DWORD dwConstantCount );
DARKSDK void SetVertexShaderMatrix      ( int iID, DWORD dwRegister, int iMatrix, DWORD dwConstantCount );
DARKSDK void DeleteVertexShader			( int iShader );

DARKSDK void SetPixelShaderOn			( int iID, int iShader );
DARKSDK void SetPixelShaderOff			( int iID );
DARKSDK void CreatePixelShaderFromFile	( int iID, SDK_LPSTR szFile );
DARKSDK void DeletePixelShader			( int iShader );
DARKSDK void SetPixelShaderTexture		( int iShader, int iSlot, int iTexture );

// Animation
DARKSDK void SaveAnimation				( int iID, SDK_LPSTR pFilename );
DARKSDK void AppendAnimation			( int iID, SDK_LPSTR pFilename );
DARKSDK void ClearAllKeyFrames			( int iID );
DARKSDK void ClearKeyFrame				( int iID, int iFrame );
DARKSDK void SetKeyFrame				( int iID, int iFrame );

// Static
DARKSDK void CreateNodeTree				( float fX, float fY, float fZ );
DARKSDK void AddNodeTreeObject			( int iID, int iType, int iArbitaryValue, int iCastShadow, int iPortalBlocker );
DARKSDK void AddNodeTreeLimb            ( int iID, int iLimb, int iType, int iArbitaryValue, int iCastShadow, int iPortalBlocker );
DARKSDK void RemoveNodeTreeObject		( int iID );
DARKSDK void DeleteNodeTree				( void );
DARKSDK void SetNodeTreeWireframeOn		( void );
DARKSDK void SetNodeTreeWireframeOff	( void );
DARKSDK void MakeNodeTreeCollisionBox	( float fX1, float fY1, float fZ1, float fX2, float fY2, float fZ2 );
DARKSDK void SetNodeTreeTextureMode		( int iMode );
DARKSDK void DisableNodeTreeOcclusion	( void );
DARKSDK void EnableNodeTreeOcclusion	( void );
DARKSDK void SaveNodeTreeObjects		( SDK_LPSTR pFilename );
DARKSDK void SetNodeTreeEffectTechnique	( SDK_LPSTR pTechniqueName );
DARKSDK void LoadNodeTreeObjects		( SDK_LPSTR pFilename, int iDivideTextureSize );
DARKSDK void AttachObjectToNodeTree		( int iID );
DARKSDK void DetachObjectFromNodeTree	( int iID );
DARKSDK void SetNodeTreePortalsOn       ( void );
DARKSDK void SetNodeTreePortalsOff      ( void );
DARKSDK void SetNodeTreeCulling			( int iFlag );
DARKSDK void BuildNodeTreePortals       ( void );
DARKSDK void SetNodeTreeScorchTexture	( int iImageID, int iWidth, int iHeight );
DARKSDK void AddNodeTreeScorch			( float fSize, int iType );
DARKSDK void AddNodeTreeLight			( int iLightIndex, float fX, float fY, float fZ, float fRange );

// CSG Commands (CSG)
DARKSDK void PeformCSGUnion				( int iObjectA, int iObjectB );
DARKSDK void PeformCSGDifference		( int iObjectA, int iObjectB );
DARKSDK void PeformCSGIntersection		( int iObjectA, int iObjectB );
DARKSDK void PeformCSGClip				( int iObjectA, int iObjectB );
DARKSDK int ObjectBlocking				( int iID, float X1, float Y1, float Z1, float X2, float Y2, float Z2 );

// leeadd - 121008 - u70 - MEMBLOCK commands for custom data
DARKSDK void AddMemblockToObject		( int iMemblock, int iID );
DARKSDK void GetMemblockFromObject		( int iMemblock, int iID );
DARKSDK void DeleteMemblockFromObject	( int iID );
DARKSDK void SetObjectStatisticsInteger ( int iID, int iIndex, int dwValue );
DARKSDK int ObjectStatisticsInteger     ( int iID, int iIndex );

// Light Maps
DARKSDK void AddObjectToLightMapPool        ( int iID );
DARKSDK void AddLimbToLightMapPool          ( int iID, int iLimb );
DARKSDK void AddStaticObjectsToLightMapPool ( void );
DARKSDK void AddLightMapLight               ( float fX, float fY, float fZ, float fRadius, float fRed, float fGreen, float fBlue, float fBrightness, SDK_BOOL bCastShadow );
DARKSDK void FlushLightMapLights            ( void );
DARKSDK void CreateLightMaps                ( int iLMSize, int iLMQuality, SDK_LPSTR dwPathForLightMaps );

// Shadows
DARKSDK void SetGlobalShadowsOn			( void );
DARKSDK void SetGlobalShadowsOff		( void );

// Collision
DARKSDK void SetCollisionOn				( int iID );
DARKSDK void SetCollisionOff			( int iID );
DARKSDK void MakeCollisionBox			( int iID, float iX1, float iY1, float iZ1, float iX2, float iY2, float iZ2, int iRotatedBoxFlag );
DARKSDK void DeleteCollisionBox			( int iID );
DARKSDK void SetCollisionToSpheres		( int iID );
DARKSDK void SetCollisionToBoxes		( int iID );
DARKSDK void SetCollisionToPolygons		( int iID );
DARKSDK void SetGlobalCollisionOn		( void );
DARKSDK void SetGlobalCollisionOff		( void );
DARKSDK void SetSphereRadius			( int iID, float fRadius );
DARKSDK SDK_FLOAT IntersectObject		( int iObjectID, float fX, float fY, float fZ, float fNewX, float fNewY, float fNewZ );
DARKSDK int IntersectAll				( int iStart, int iEnd, float fX, float fY, float fZ, float fNewX, float fNewY, float fNewZ, int iIgnoreObjNo );
DARKSDK void SetObjectCollisionProperty ( int iObjectID, int iPropertyValue );
DARKSDK void AutomaticObjectCollision	( int iObjectID, float fRadius, int iResponse );
DARKSDK void AutomaticCameraCollision	( int iCameraID, float fRadius, int iResponse, int iStandGroundMode );
DARKSDK void AutomaticCameraCollision	( int iCameraID, float fRadius, int iResponse );
DARKSDK void ForceAutomaticEnd			( void );
DARKSDK void HideBounds					( int iID );
DARKSDK void ShowBoundsEx				( int iID, int iBoxOnly );
DARKSDK void ShowBounds					( int iID, int iLimb ); // mike 160505 - new function for specifying a limb bound box drae
DARKSDK void ShowBounds					( int iID );

// Limb
DARKSDK void PerformCheckListForLimbs	( int iID );
DARKSDK void PerformCheckListForOnscreenObjects ( int iMode );
DARKSDK void HideLimb					( int iID, int iLimbID );
DARKSDK void ShowLimb					( int iID, int iLimbID );
DARKSDK void OffsetLimb					( int iID, int iLimbID, float fX, float fY, float fZ );
DARKSDK void OffsetLimb					( int iID, int iLimbID, float fX, float fY, float fZ, int iBoundFlag );
DARKSDK void RotateLimb					( int iID, int iLimbID, float fX, float fY, float fZ );
DARKSDK void RotateLimb					( int iID, int iLimbID, float fX, float fY, float fZ, int iBoundFlag );
DARKSDK void ScaleLimb					( int iID, int iLimbID, float fX, float fY, float fZ );
DARKSDK void ScaleLimb					( int iID, int iLimbID, float fX, float fY, float fZ, int iBoundFlag );
DARKSDK void AddLimb					( int iID, int iLimbID, int iMeshID );
DARKSDK void RemoveLimb					( int iID, int iLimbID );
DARKSDK void LinkLimb					( int iID, int iParentID, int iLimbID );
DARKSDK void TextureLimb				( int iID, int iLimbID, int iImageID );
DARKSDK void TextureLimbStage			( int iID, int iLimbID, int iStage, int iImageID );
DARKSDK void ColorLimb					( int iID, int iLimbID, DWORD dwColor );
DARKSDK void ScrollLimbTexture			( int iID, int iLimbID, int iStage, float fU, float fV );
DARKSDK void ScrollLimbTexture			( int iID, int iLimbID, float fU, float fV );
DARKSDK void ScaleLimbTexture			( int iID, int iLimbID, int iStage, float fU, float fV );
DARKSDK void ScaleLimbTexture			( int iID, int iLimbID, float fU, float fV );
DARKSDK void SetLimbSmoothing			( int iID, int iLimbID, float fPercentage );
DARKSDK void SetLimbNormals				( int iID, int iLimbID );

DARKSDK void MakeObjectFromLimbEx		( int iNewID, int iSrcID, int iLimbID, int iCopyAllFromLimb );
DARKSDK void MakeObjectFromLimb			( int iID, int iSrcObj, int iLimbID );
DARKSDK void SetObjectLOD				( int iCurrentID, int iLODLevel, float fDistanceOfLOD );
DARKSDK void AddLODToObject				( int iCurrentID, int iLODModelID, int iLODLevel, float fDistanceOfLOD );

// Mesh
DARKSDK void  LoadMesh					( SDK_LPSTR pFilename, int iID );
DARKSDK void  DeleteMesh				( int iID );
DARKSDK void  SaveMesh					( SDK_LPSTR pFilename, int iMeshID );
DARKSDK void  ChangeMesh				( int iObjectID, int iLimbID, int iMeshID );
DARKSDK void  MakeMeshFromObject		( int iID, int iObjectID );
DARKSDK void  MakeMeshFromObject		( int iID, int iObjectID, int iIgnoreMode );
DARKSDK void  MakeMeshFromLimb			( int iMeshID, int iObjectID, int iLimbNumber );
DARKSDK void  ConvertMeshToVertexData	( int iMeshID );
DARKSDK void  ReduceMesh				( int iMeshID, int iBlockMode, int iNearMode, int iGX, int iGY, int iGZ );
DARKSDK void  MakeLODFromMesh			( int iMeshID, int iVertNum, int iNewMeshID );

// Mesh/Limb Manipulation 
DARKSDK void  LockVertexDataForLimbCore			( int iID, int iLimbID, int iReplaceOrUpdate );
DARKSDK void  LockVertexDataForLimb				( int iID, int iLimbID );
DARKSDK void  LockVertexDataForMesh				( int iID );
DARKSDK void  UnlockVertexData					( void );
DARKSDK void  SetVertexDataPosition				( int iVertex, float fX, float fY, float fZ );
DARKSDK void  SetVertexDataNormals				( int iVertex, float fNX, float fNY, float fNZ );
DARKSDK void  SetVertexDataDiffuse				( int iVertex, DWORD dwDiffuse );
DARKSDK void  SetVertexDataUV					( int iVertex, float fU, float fV );
DARKSDK void  SetVertexDataUV					( int iVertex, int iIndex, float fU, float fV );
DARKSDK void  SetVertexDataSize 				( int iVertex, float fSize );

DARKSDK void  SetIndexData						( int iIndex, int iValue );
DARKSDK int   GetIndexData						( int iIndex );

DARKSDK void  AddMeshToVertexData				( int iMeshID );
DARKSDK void  DeleteMeshFromVertexData			( int iVertex1, int iVertex2, int iIndex1, int iIndex2 );
DARKSDK void  PeformCSGUnionOnVertexData		( int iBrushMeshID );
DARKSDK void  PeformCSGDifferenceOnVertexData	( int iBrushMeshID );
DARKSDK void  PeformCSGIntersectionOnVertexData ( int iBrushMeshID );
DARKSDK int   GetVertexDataVertexCount			( void );
DARKSDK int   GetVertexDataIndexCount			( void );
DARKSDK SDK_FLOAT	GetVertexDataPositionX		( int iVertex );
DARKSDK SDK_FLOAT	GetVertexDataPositionY		( int iVertex );
DARKSDK SDK_FLOAT	GetVertexDataPositionZ		( int iVertex );
DARKSDK SDK_FLOAT	GetVertexDataNormalsX		( int iVertex );
DARKSDK SDK_FLOAT	GetVertexDataNormalsY		( int iVertex );
DARKSDK SDK_FLOAT	GetVertexDataNormalsZ		( int iVertex );
DARKSDK DWORD		GetVertexDataDiffuse		( int iVertex );
DARKSDK SDK_FLOAT	GetVertexDataU				( int iVertex );
DARKSDK SDK_FLOAT	GetVertexDataU				( int iVertex, int iIndex );
DARKSDK SDK_FLOAT	GetVertexDataV				( int iVertex );
DARKSDK SDK_FLOAT	GetVertexDataV				( int iVertex, int iIndex );
DARKSDK DWORD		GetVertexDataPtr			( void );

// Misc
DARKSDK void		SetFastBoundsCalculation	( int iMode );
DARKSDK void		SetMipmapMode				( int iMode );
DARKSDK void		FlushVideoMemory			( void );
DARKSDK void		DisableTNL					( void );
DARKSDK void		EnableTNL					( void );
DARKSDK void		Convert3DStoX				( DWORD pFilename1, DWORD pFilename2 );

// Pick Commands
DARKSDK int			PickScreenObjectEx			( int iX, int iY, int iObjectStart, int iObjectEnd, int iIgnoreCamera );
DARKSDK int			PickScreenObject			( int iX, int iY, int iObjectStart, int iObjectEnd );
DARKSDK void		PickScreen2D23D				( int iX, int iY, float fDistance );
DARKSDK SDK_FLOAT	GetPickDistance				( void );
DARKSDK SDK_FLOAT	GetPickVectorX				( void );
DARKSDK SDK_FLOAT	GetPickVectorY				( void );
DARKSDK SDK_FLOAT	GetPickVectorZ				( void );

// Emitters
DARKSDK void MakeEmitter     ( int iID, int iSize );
DARKSDK void GetEmitterData  ( int iID, BYTE** ppVertices, DWORD* pdwVertexCount, int** ppiDrawCount );
DARKSDK void UpdateEmitter   ( int iID );
DARKSDK void GetPositionData ( int iID, sPositionData** ppPosition );

DARKSDK void  ExcludeOn							( int iID );
DARKSDK void  ExcludeOff						( int iID );
DARKSDK void  SetGlobalObjectCreationMode	    ( int iMode );
DARKSDK void  ExcludeLimbOn						( int iID, int iLimbID );
DARKSDK void  ExcludeLimbOff					( int iID, int iLimbID );

// New construction commands for multiplayer nameplates

DARKSDK int MakeNewObjectPanel	( int iID , int iNumberOfCharacters );
DARKSDK void SetObjectPanelQuad	( int iID, int index, float fX, float fY, float fWidth, float fHeight, float fU1, float fV1, float fU2, float fV2 );
DARKSDK void FinishObjectPanel	( int iID, float fWidth, float fHeight );

//////////////////////////////////////////////////////////////////////////////////
// Expressions ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// Object
DARKSDK int GetExist    	  ( int iID );
DARKSDK int GetNumberOfFrames ( int iID );
DARKSDK SDK_FLOAT GetSizeEx  ( int iID, int iAccountForScale );
DARKSDK SDK_FLOAT GetXSizeEx ( int iID, int iAccountForScale );
DARKSDK SDK_FLOAT GetYSizeEx ( int iID, int iAccountForScale );
DARKSDK SDK_FLOAT GetZSizeEx ( int iID, int iAccountForScale );
DARKSDK SDK_FLOAT GetSize  ( int iID );
DARKSDK SDK_FLOAT GetXSize ( int iID );
DARKSDK SDK_FLOAT GetYSize ( int iID );
DARKSDK SDK_FLOAT GetZSize ( int iID );
DARKSDK SDK_FLOAT GetScaleX ( int iID );
DARKSDK SDK_FLOAT GetScaleY ( int iID );
DARKSDK SDK_FLOAT GetScaleZ ( int iID );
DARKSDK int GetVisible       ( int iID );
DARKSDK int GetPlaying       ( int iID );
DARKSDK int GetLooping       ( int iID );
DARKSDK SDK_FLOAT GetFrame         ( int iID );
DARKSDK SDK_FLOAT GetFrameEx       ( int iID, int iLimbID );
DARKSDK SDK_FLOAT GetSpeed         ( int iID );
DARKSDK SDK_FLOAT GetInterpolation ( int iID );
DARKSDK int GetScreenX       ( int iID );
DARKSDK int GetScreenY       ( int iID );
DARKSDK int GetInScreen      ( int iID );

DARKSDK SDK_FLOAT GetObjectCollisionRadius ( int iID );
DARKSDK SDK_FLOAT GetObjectCollisionCenterX ( int iID );
DARKSDK SDK_FLOAT GetObjectCollisionCenterY ( int iID );
DARKSDK SDK_FLOAT GetObjectCollisionCenterZ ( int iID );

// Collsion
DARKSDK int GetCollision ( int iObjectA, int iObjectB );
DARKSDK int GetHit		 ( int iObjectA, int iObjectB );
DARKSDK int GetLimbCollision ( int iObjectA, int iLimbA, int iObjectB, int iLimbB );
DARKSDK int GetLimbHit		 ( int iObjectA, int iLimbA, int iObjectB, int iLimbB );
DARKSDK SDK_FLOAT GetCollisionX			( void );
DARKSDK SDK_FLOAT GetCollisionY			( void );
DARKSDK SDK_FLOAT GetCollisionZ			( void );

// Static Expressions
DARKSDK int GetStaticHit ( float fOldX1, float fOldY1, float fOldZ1, float fOldX2, float fOldY2, float fOldZ2, float fNX1,   float fNY1,   float fNZ1,   float fNX2,   float fNY2,   float fNZ2   );
DARKSDK int GetStaticLineOfSight ( float fSx, float fSy, float fSz, float fDx, float fDy, float fDz, float fWidth, float fAccuracy );
DARKSDK int GetStaticRayCast ( float fSx, float fSy, float fSz, float fDx, float fDy, float fDz );
DARKSDK int GetStaticVolumeCast ( float fX, float fY, float fZ, float fNewX, float fNewY, float fNewZ, float fSize );
DARKSDK SDK_FLOAT GetStaticX ( void );
DARKSDK SDK_FLOAT GetStaticY ( void );
DARKSDK SDK_FLOAT GetStaticZ ( void );
DARKSDK int GetStaticFloor ( void );
DARKSDK int GetStaticColCount ( void );
DARKSDK int GetStaticColValue ( void );
DARKSDK SDK_FLOAT GetStaticLineOfSightX ( void );
DARKSDK SDK_FLOAT GetStaticLineOfSightY ( void );
DARKSDK SDK_FLOAT GetStaticLineOfSightZ ( void );

// Limb
DARKSDK int GetLimbExist ( int iID, int iLimbID );

DARKSDK SDK_FLOAT GetLimbOffsetX    ( int iID, int iLimbID );
DARKSDK SDK_FLOAT GetLimbOffsetY    ( int iID, int iLimbID );
DARKSDK SDK_FLOAT GetLimbOffsetZ    ( int iID, int iLimbID );
DARKSDK SDK_FLOAT GetLimbAngleX     ( int iID, int iLimbID );
DARKSDK SDK_FLOAT GetLimbAngleY     ( int iID, int iLimbID );
DARKSDK SDK_FLOAT GetLimbAngleZ     ( int iID, int iLimbID );
DARKSDK SDK_FLOAT GetLimbXPosition  ( int iID, int iLimbID );
DARKSDK SDK_FLOAT GetLimbYPosition  ( int iID, int iLimbID );
DARKSDK SDK_FLOAT GetLimbZPosition  ( int iID, int iLimbID );
DARKSDK SDK_FLOAT GetLimbXDirection ( int iID, int iLimbID );
DARKSDK SDK_FLOAT GetLimbYDirection ( int iID, int iLimbID );
DARKSDK SDK_FLOAT GetLimbZDirection ( int iID, int iLimbID );
DARKSDK int GetLimbTextureEx ( int iID, int iLimbID, int iTextureStage );
DARKSDK int GetLimbTexture ( int iID, int iLimbID );
DARKSDK int GetLimbTexturePtr ( int iID, int iLimbID );
DARKSDK int GetLimbVisible ( int iID, int iLimbID );
DARKSDK int GetLimbLink ( int iID, int iLimbID );
DARKSDK int GetLimbPolygonCount ( int iID, int iLimbID );
DARKSDK int GetMultiMaterialCount ( int iID );
DARKSDK SDK_LPSTR GetLimbTextureNameEx ( SDK_RETSTR int iID, int iLimbID, int iTextureStage );
DARKSDK SDK_LPSTR GetLimbTextureName ( SDK_RETSTR int iID, int iLimbID );

// Misc
DARKSDK int   GetMeshExist				( int iID );
DARKSDK int   GetAlphaBlending			( void );
DARKSDK int   GetBlending				( void );
DARKSDK int   GetFog					( void );
DARKSDK int   Get3DBLITAvailable		( void );
DARKSDK int   GetStatistic				( int iCode );
DARKSDK int   GetTNLAvailable			( void );

// new limb commands
DARKSDK SDK_LPSTR	GetLimbName					  ( DWORD pDestStr, int iID, int iLimbID );
DARKSDK int			GetLimbCount				  ( int iID );
DARKSDK SDK_FLOAT	GetLimbScaleX				  ( int iID, int iLimbID );
DARKSDK SDK_FLOAT	GetLimbScaleY				  ( int iID, int iLimbID );
DARKSDK SDK_FLOAT	GetLimbScaleZ				  ( int iID, int iLimbID );

// vertex shader expressions
DARKSDK SDK_BOOL	VertexShaderExist			  ( int iShader );
DARKSDK SDK_BOOL	PixelShaderExist			  ( int iShader );

// hardware information
DARKSDK int			GetDeviceType                 ( void );
DARKSDK SDK_BOOL	GetCalibrateGamma             ( void );
DARKSDK SDK_BOOL	GetRenderWindowed             ( void );
DARKSDK SDK_BOOL    GetFullScreenGamma            ( void );
DARKSDK SDK_BOOL    GetBlitSysOntoLocal           ( void );
DARKSDK SDK_BOOL    GetRenderAfterFlip            ( void );
DARKSDK SDK_BOOL    GetDrawPrimTLVertex           ( void );
DARKSDK SDK_BOOL    GetHWTransformAndLight        ( void );
DARKSDK SDK_BOOL    GetSeparateTextureMemories    ( void );
DARKSDK SDK_BOOL    GetTextureSystemMemory        ( void );
DARKSDK SDK_BOOL    GetTextureVideoMemory         ( void );
DARKSDK SDK_BOOL    GetTextureNonLocalVideoMemory ( void );
DARKSDK SDK_BOOL    GetTLVertexSystemMemory       ( void );
DARKSDK SDK_BOOL    GetTLVertexVideoMemory        ( void );
DARKSDK SDK_BOOL    GetClipAndScalePoints         ( void );
DARKSDK SDK_BOOL    GetClipTLVerts                ( void );
DARKSDK SDK_BOOL    GetColorWriteEnable           ( void );
DARKSDK SDK_BOOL    GetCullCCW                    ( void );
DARKSDK SDK_BOOL    GetCullCW                     ( void );
DARKSDK SDK_BOOL    GetAnisotropicFiltering       ( void );
DARKSDK SDK_BOOL    GetAntiAliasEdges             ( void );
DARKSDK SDK_BOOL    GetColorPerspective           ( void );
DARKSDK SDK_BOOL    GetDither                     ( void );
DARKSDK SDK_BOOL    GetFogRange                   ( void );
DARKSDK SDK_BOOL    GetFogTable                   ( void );
DARKSDK SDK_BOOL    GetFogVertex                  ( void );
DARKSDK SDK_BOOL    GetMipMapLODBias              ( void );
DARKSDK SDK_BOOL    GetWBuffer                    ( void );
DARKSDK SDK_BOOL    GetWFog                       ( void );
DARKSDK SDK_BOOL    GetZFog                       ( void );
DARKSDK SDK_BOOL    GetAlpha                      ( void );
DARKSDK SDK_BOOL    GetCubeMap                    ( void );
DARKSDK SDK_BOOL    GetMipCubeMap                 ( void );
DARKSDK SDK_BOOL    GetMipMap                     ( void );
DARKSDK SDK_BOOL    GetMipMapVolume               ( void );
DARKSDK SDK_BOOL    GetNonPowerOf2Textures        ( void );
DARKSDK SDK_BOOL    GetPerspective                ( void );
DARKSDK SDK_BOOL    GetProjected                  ( void );
DARKSDK SDK_BOOL    GetSquareOnly                 ( void );
DARKSDK SDK_BOOL    GetVolumeMap                  ( void );
DARKSDK SDK_BOOL    GetAlphaComparision           ( void );
DARKSDK SDK_BOOL    GetTexture                    ( void );
DARKSDK SDK_BOOL    GetZBuffer                    ( void );
DARKSDK int			GetMaxTextureWidth            ( void );
DARKSDK int			GetMaxTextureHeight           ( void );
DARKSDK int			GetMaxVolumeExtent            ( void );
DARKSDK SDK_FLOAT	GetVertexShaderVersion        ( void );
DARKSDK SDK_FLOAT	GetPixelShaderVersion         ( void );
DARKSDK int			GetMaxVertexShaderConstants   ( void );
DARKSDK SDK_FLOAT	GetMaxPixelShaderValue        ( void );
DARKSDK int			GetMaxLights                  ( void );

DARKSDK void			SetDeleteCallBack   ( int iID, ON_OBJECT_DELETE_CALLBACK pfn, int userData );
DARKSDK void			SetDisableTransform ( int iID, bool bTransform );
DARKSDK void			CreateMeshForObject ( int iID, DWORD dwFVF, DWORD dwVertexCount, DWORD dwIndexCount );
DARKSDK void			SetWorldMatrix ( int iID, D3DXMATRIX* pMatrix );
DARKSDK void			UpdatePositionStructure ( sPositionData* pPosition );
DARKSDK void			GetWorldMatrix ( int iID, int iLimb, D3DXMATRIX* pMatrix );
DARKSDK D3DXVECTOR3		GetCameraLook     ( void );
DARKSDK D3DXVECTOR3		GetCameraPosition ( void );
DARKSDK D3DXVECTOR3		GetCameraUp       ( void );
DARKSDK D3DXVECTOR3		GetCameraRight    ( void );
DARKSDK D3DXMATRIX		GetCameraMatrix   ( void );
DARKSDK sObject*		GetObject ( int iID );
DARKSDK void			GetUniverseMeshList ( vector < sMesh* > *pMeshList );

// mike - 300905 - command to update object bounds
DARKSDK void			CalculateObjectBounds ( int iID );
DARKSDK void			CalculateObjectBoundsEx ( int iID, int iOnlyUpdateFrames );

// mike - 230505 - need to be able to set mip map LOD bias on a per mesh basis
void		SetObjectMipMapLODBias	( int iID, float fBias );
void		SetObjectMipMapLODBias	( int iID, int iLimb, float fBias );

// lee - 2101006 - u63 - assists darkshader and related
DARKSDK void			SetMask	( int iID, int iMASK );

// lee - 140108 - x10 compat.
DARKSDK void SetMask			( int iID, int iMASK, int iShadowMASK );
DARKSDK void SetMask			( int iID, int iMASK, int iShadowMASK, int iCubeMapMASK );
DARKSDK void SetMask			( int iID, int iMASK, int iShadowMASK, int iCubeMapMASK, int iForeColorWipe );
DARKSDK void SetArrayMap		( int iID, int iStage, int i1, int i2, int i3, int i4, int i5, int i6, int i7, int i8, int i9, int i10 );
DARKSDK void SetArrayMapEx		( int iID, int iStage, int iSrcObject, int iSrcStage );
DARKSDK void Instance			( int iDestinationID, int iSourceID, int iInstanceValue );
DARKSDK void SetNodeTreeEffect	( int iEffectID );
DARKSDK void DrawSingle			( int iObjectID, int iCameraID );
DARKSDK void ResetStaticLights	( void );
DARKSDK void AddStaticLight		( int iIndex, float fX, float fY, float fZ, float fRange );
DARKSDK void UpdateStaticLights	( void );

// lee - 071108 - u71 - post processing screen commands (from SetupDLL)
DARKSDK void TextureScreen 				( int iStageIndex, int iImageID );
DARKSDK void SetScreenEffect			( int iEffectID );
DARKSDK int RenderQuad 					( int iActualRender );

DARKSDK void SetLegacyMode				( int iUseLegacy );

// DARKSDK FUNCTIONS -

#ifdef DARKSDK_COMPILE

	void		dbLoadObject							( char* szFilename, int iID );
	void		dbLoadObject							( char* szFilename, int iID, int iDBProMode );

	// mike - 181206 - expose full load object function for GDK
	void		dbLoadObject							( char* szFilename, int iID, int iDBProMode, int iDivideTextureSize );

	void		dbSaveObject							( char* szFilename, int iID );
	void		dbDeleteObject							( int iID );
	void		dbSetObject								( int iID, bool bWireframe, bool bTransparency, bool bCull );
	void		dbSetObject								( int iID, bool bWireframe, bool bTransparency, bool bCull, int iFilter );
	void		dbSetObject								( int iID, bool bWireframe, bool bTransparency, bool bCull, int iFilter, bool bLight );
	void		dbSetObject								( int iID, bool bWireframe, bool bTransparency, bool bCull, int iFilter, bool bLight, bool bFog );
	void		dbSetObject								( int iID, bool bWireframe, bool bTransparency, bool bCull, int iFilter, bool bLight, bool bFog, bool bAmbient );
	void		dbSetObjectWireframe					( int iID, bool bFlag    );
	void		dbSetObjectTransparency					( int iID, int iFlag    );
	void		dbSetObjectCull							( int iID, bool bCull    );
	void		dbSetObjectFilter						( int iID, int  iFilter  );
	void		dbSetObjectFilterStage					( int iID, int iStage, int iFilter );
	void		dbSetObjectLight						( int iID, bool bLight   );
	void		dbSetObjectFog							( int iID, bool bFog     );
	void		dbSetAmbient							( int iID, bool bAmbient );
	void		dbMakeObject							( int iID, int iMeshID, int iTextureID );
	void		dbMakeObjectSphere						( int iID, float fRadius, int iRings, int iSegments );
	void		dbCloneObject							( int iDestinationID, int iSourceID, int iCloneSharedData );
	void		dbCloneObject							( int iDestinationID, int iSourceID );
	void		dbInstanceObject						( int iDestinationID, int iSourceID );
	void		dbMakeObjectSphere						( int iID, float fRadius );
	void		dbMakeObjectCube						( int iID, float iSize );
	void		dbMakeObjectBox							( int iID, float fWidth, float fHeight, float fDepth );
	void		dbMakeObjectPyramid						( int iID, float fSize );
	void		dbMakeObjectCylinder					( int iID, float fSize );
	void		dbMakeObjectCone						( int iID, float fSize );
	void		dbMakeObjectPlane						( int iID, float fWidth, float fHeight );
	void		dbMakeObjectTriangle					( int iID, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3 );
	void		dbAppendObject							( char* pString, int iID, int iFrame );
	void		dbPlayObject							( int iID );
	void		dbPlayObject 							( int iID, int iStart );
	void		dbPlayObject							( int iID, int iStart, int iEnd );
	void		dbLoopObject							( int iID );
	void		dbLoopObject							( int iID, int iStart );
	void		dbLoopObject							( int iID, int iStart, int iEnd );
	void		dbStopObject							( int iID );
	void		dbHideObject							( int iID );
	void		dbShowObject							( int iID );
	void		dbColorObject							( int iID, DWORD dwRGB );
	void		dbSetObjectFrame						( int iID, int iFrame );
	void		dbSetObjectFrame						( int iID, float fFrame );
	void		dbSetObjectFrame						( int iID, float fFrame, int iRecalculateBounds );
	void		dbSetObjectFrame						( int iID, int iLimbID, float fFrame, int iEnableOverride );
	void		dbSetObjectSpeed						( int iID, int iSpeed );
	void		dbSetObjectInterpolation				( int iID, int iJump );
	void		dbSetObjectRotationXYZ					( int iID );
	void		dbSetObjectRotationZYX					( int iID );
	void		dbGhostObjectOn							( int iID );
	void		dbGhostObjectOn							( int iID, int iFlag );
	void		dbGhostObjectOff						( int iID );
	void		dbFadeObject							( int iID, float iPercentage );
	void		dbGlueObjectToLimb						( int iSource, int iTarget, int iLimb );
	void		dbGlueObjectToLimb						( int iSource, int iTarget, int iLimb, int iMode );
	void		dbUnGlueObject							( int iID );
	void		dbLockObjectOn							( int iID );
	void		dbLockObjectOff							( int iID );
	void		dbDisableObjectZDepth					( int iID );
	void		dbEnableObjectZDepth					( int iID );
	void		dbDisableObjectZRead					( int iID );
	void		dbEnableObjectZRead						( int iID );
	void		dbDisableObjectZWrite					( int iID );
	void		dbEnableObjectZWrite					( int iID );
	void		dbSetObjectFOV							( int iID, float fRadius );
	void		dbDeleteObjects							( int iFrom, int iTo );
	void		dbClearObjectsOfTextureRef				( LPDIRECT3DTEXTURE9 pTextureRef );
	void		dbDisableObjectBias						( int iID );
	void		dbEnableObjectZBias						( int iID, float fSlopeScale, float fDepth );

	void		dbSetObjectDiffuse						( int iID, DWORD dwRGB );
	void		dbSetObjectAmbience						( int iID, DWORD dwRGB );
	void		dbSetObjectSpecular						( int iID, DWORD dwRGB );
	void		dbSetObjectSpecularPower				( int iID, float fPower );
	void		dbSetObjectEmissive						( int iID, DWORD dwRGB );

	void		dbFixObjectPivot						( int iID );
	void		dbSetObjectToObjectOrientation			( int iID, int iWhichID );
	void		dbSetObjectToObjectOrientation			( int iID, int iWhichID, int iWhichLimbID );
	void		dbSetObjectToCameraOrientation			( int iID );

	void		dbTextureObject							( int iID, int iImage );
	void		dbTextureObject							( int iID, int iStage, int iImage );
	void		dbScrollObjectTexture					( int iID, float fU, float fV );
	void		dbScaleObjectTexture					( int iID, float fU, float fV );
	void		dbScaleObjectTexture					( int iID, int iStage, float fU, float fV );
	void		dbSetObjectSmoothing					( int iID, float fAngle );
	void		dbSetObjectNormals						( int iID );
	void		dbSetObjectTexture						( int iID, int iMode, int iMipGeneration );
	void		dbSetObjectTexture						( int iID, int iStage, int iMode, int iMipGeneration );
	void		dbSetLightMappingOn						( int iID, int iImage );
	void		dbSetSphereMappingOn					( int iID, int iSphereImage );
	void		dbSetCubeMappingOn						( int iID, int i1, int i2, int i3, int i4, int i5, int i6 );
	void		dbSetDetailMappingOn					( int iID, int iImage );
	void		dbSetBlendMappingOn						( int iID, int iImage, int iMode );
	void		dbSetBlendMappingOn						( int iID, int iStage, int iImage, int iTexCoordMode, int iMode );
	void		dbSetTextureMD3							( int iID, int iH0, int iH1, int iL0, int iL1, int iL2, int iU0 );

	void		dbSetAlphaMappingOn						( int iID, float fPercentage );

	void		dbSetBumpMappingOn						( int iID, int iBumpMap );
	void		dbSetCartoonShadingOn					( int iID, int iStandardImage, int iEdgeImage );
	void		dbSetRainbowShadingOn					( int iID, int iStandardImage );

	void		dbSetEffectOn							( int iID, char* pFilename, int iUseDefaultTextures );
	void		dbLoadEffect							( char* pFilename, int iEffectID, int iUseDefaultTextures );
	void		dbDeleteEffect							( int iEffectID );
	void		dbSetObjectEffect						( int iID, int iEffectID );
	void		dbSetLimbEffect							( int iID, int iLimbID, int iEffectID );
	void		dbPerformChecklistForEffectValues		( int iEffectID );
	void		dbPerformChecklistForEffectErrors		( void );
	void		dbPerformChecklistForEffectErrors		( int iEffectID );
	void		dbSetEffectTranspose					( int iEffectID, int iTransposeFlag );
	void		dbSetEffectConstantBoolean				( int iEffectID, char* pConstantName, int iValue );
	void		dbSetEffectConstantInteger				( int iEffectID, char* pConstantName, int iValue );
	void		dbSetEffectConstantFloat				( int iEffectID, char* pConstantName, float fValue );
	void		dbSetEffectConstantVector				( int iEffectID, char* pConstantName, int iValue );
	void		dbSetEffectConstantMatrix				( int iEffectID, char* pConstantName, int iValue );
	void		dbSetEffectTechnique					( int iEffectID, char* pTechniqueName );
	int			dbEffectExist							( int iEffectID );

	void		dbSetShadowShadingOn					( int iID );
	void		dbSetShadowShadingOn					( int iID, int iMesh, int iRange, int iUseShader );
	void		dbSetShadowClipping						( int iID, float fMin, float fMax );
	void		dbSetReflectionShadingOn				( int iID );
	void		dbSetGlobalShadowColor					( int iRed, int iGreen, int iBlue, int iAlphaLevel );
	void		dbSetGlobalShadowShades					( int iShades );
	void		dbSetGlobalReflectionColor				( int iRed, int iGreen, int iBlue, int iAlphaLevel );
	void		dbSetShadowShadingOff					( int iID );
	void		dbSetShadowPosition						( int iMode, float fX, float fY, float fZ );

	void		dbSetVertexShaderOn          			( int iID, int iShader );
	void		dbSetVertexShaderOff         			( int iID );
	void		dbConvertObjectFVF		       			( int iID, DWORD dwFVF );
	void		dbSetVertexShaderStreamCount 			( int iID, int iCount );
	void		dbSetVertexShaderStream      			( int iID, int iStreamPos, int iData, int iDataType );
	void		dbCreateVertexShaderFromFile 			( int iID, char* szFile );
	void		dbSetVertexShaderVector      			( int iID, DWORD dwRegister, int iVector, DWORD dwConstantCount );
	void		dbSetVertexShaderMatrix      			( int iID, DWORD dwRegister, int iMatrix, DWORD dwConstantCount );
	void		dbDeleteVertexShader					( int iShader );

	void		dbSetPixelShaderOn						( int iID, int iShader );
	void		dbSetPixelShaderOff						( int iID );
	void		dbCreatePixelShaderFromFile				( int iID, char* szFile );
	void		dbDeletePixelShader						( int iShader );
	void		dbSetPixelShaderTexture					( int iShader, int iSlot, int iTexture );

	void		dbSaveObjectAnimation					( int iID, char* pFilename );
	void		dbAppendObjectAnimation					( int iID, char* pFilename );
	void		dbClearAllObjectKeyFrames				( int iID );
	void		dbClearObjectKeyFrame					( int iID, int iFrame );
	void		dbSetObjectKeyFrame						( int iID, int iFrame );

	void		dbSetStaticUniverse						( float fX, float fY, float fZ );
	void		dbMakeStaticObject						( int iID, int iType, int iArbitaryValue, int iCastShadow, int iPortalBlocker );
	void		dbMakeStaticLimb						( int iID, int iLimb, int iType, int iArbitaryValue, int iCastShadow, int iPortalBlocker );
	void		dbDeleteStaticObject					( int iID );
	void		dbDeleteStaticObjects					( void );
	void		dbSetStaticObjectsWireframeOn			( void );
	void		dbSetStaticObjectsWireframeOff			( void );
	void		dbMakeStaticCollisionBox				( float fX1, float fY1, float fZ1, float fX2, float fY2, float fZ2 );
	void		dbSetSetStaticObjectsTexture			( int iMode );
	void		dbDisableStaticOcclusion				( void );
	void		dbEnableStaticOcclusion					( void );
	void		dbSaveStaticObjects						( char* pFilename );
	void		dbLoadStaticObjects						( char* pFilename, int iDivideTextureSize );
	void		dbAttachObjectToStatic					( int iID );
	void		dbDetachObjectFromStatic				( int iID );
	void		dbSetStaticPortalsOn       				( void );
	void		dbSetStaticPortalsOff					( void );
	void		dbBuildStaticPortals					( void );
	void		dbSetStaticScorch						( int iImageID, int iWidth, int iHeight );
	void		dbAddStaticScorch						( float fSize, int iType );
	void		dbAddStaticLight						( int iLightIndex, float fX, float fY, float fZ, float fRange );

	void		dbPeformCSGUnion						( int iObjectA, int iObjectB );
	void		dbPeformCSGDifference					( int iObjectA, int iObjectB );
	void		dbPeformCSGIntersection					( int iObjectA, int iObjectB );
	void		dbPeformCSGClip							( int iObjectA, int iObjectB );
	int			dbObjectBlocking						( int iID, float X1, float Y1, float Z1, float X2, float Y2, float Z2 );

	void		dbAddObjectToLightMapPool				( int iID );
	void		dbAddLimbToLightMapPool					( int iID, int iLimb );
	void		dbAddStaticObjectsToLightMapPool		( void );
	void		dbAddLightMapLight						( float fX, float fY, float fZ, float fRadius, float fRed, float fGreen, float fBlue, float fBrightness, bool bCastShadow );
	void		dbDeleteLightMapLights					( void );
	void		dbCreateLightMaps						( int iLMSize, int iLMQuality, char* dwPathForLightMaps );

	void		dbSetGlobalShadowsOn					( void );
	void		dbSetGlobalShadowsOff					( void );

	void		dbSetObjectCollisionOn					( int iID );
	void		dbSetObjectCollisionOff					( int iID );
	void		dbMakeObjectCollisionBox				( int iID, float iX1, float iY1, float iZ1, float iX2, float iY2, float iZ2, int iRotatedBoxFlag );
	void		dbDeleteObjectCollisionBox				( int iID );
	void		dbSetObjectCollisionToSpheres			( int iID );
	void		dbSetObjectCollisionToBoxes				( int iID );
	void		dbSetObjectCollisionToPolygons			( int iID );
	void		dbSetGlobalCollisionOn					( void );
	void		dbSetGlobalCollisionOff					( void );
	void		dbSetObjectRadius						( int iID, float fRadius );
	float		dbIntersectObject						( int iObjectID, float fX, float fY, float fZ, float fNewX, float fNewY, float fNewZ );
	void		dbAutomaticObjectCollision				( int iObjectID, float fRadius, int iResponse );
	void		dbAutomaticCameraCollision				( int iCameraID, float fRadius, int iResponse );
	void		dbCalculateAutomaticCollision			( void );
	void		dbHideObjectBounds						( int iID );
	void		dbShowObjectBounds						( int iID, int iBoxOnly );
	void		dbShowObjectBounds						( int iID );
	void		dbShowObjectLimbBounds					( int iID, int iLimb );

	void		dbPerformCheckListForObjectLimbs		( int iID );
	void		dbHideLimb								( int iID, int iLimbID );
	void		dbShowLimb								( int iID, int iLimbID );
	void		dbOffsetLimb							( int iID, int iLimbID, float fX, float fY, float fZ );
	void		dbOffsetLimb							( int iID, int iLimbID, float fX, float fY, float fZ, int iBoundFlag );
	void		dbRotateLimb							( int iID, int iLimbID, float fX, float fY, float fZ );
	void		dbRotateLimb							( int iID, int iLimbID, float fX, float fY, float fZ, int iBoundFlag );
	void		dbScaleLimb								( int iID, int iLimbID, float fX, float fY, float fZ );
	void		dbScaleLimb								( int iID, int iLimbID, float fX, float fY, float fZ, int iBoundFlag );
	void		dbAddLimb								( int iID, int iLimbID, int iMeshID );
	void		dbRemoveLimb							( int iID, int iLimbID );
	void		dbLinkLimb								( int iID, int iParentID, int iLimbID );
	void		dbTextureLimb							( int iID, int iLimbID, int iImageID );
	void		dbTextureLimb							( int iID, int iLimbID, int iStage, int iImageID );
	void		dbColorLimb								( int iID, int iLimbID, DWORD dwColor );
	void		dbScrollLimbTexture						( int iID, int iLimbID, float fU, float fV );
	void		dbScaleLimbTexture						( int iID, int iLimbID, float fU, float fV );
	void		dbSetLimbSmoothing						( int iID, int iLimbID, float fPercentage );
	void		dbSetLimbNormals						( int iID, int iLimbID );

	void		dbMakeObjectFromLimb					( int iID, int iSrcObj, int iLimbID );
	void		dbAddLODToObject						( int iCurrentID, int iLODModelID, int iLODLevel, float fDistanceOfLOD );

	void		dbLoadMesh								( char* pFilename, int iID );
	void		dbDeleteMesh							( int iID );
	void		dbSaveMesh								( char* pFilename, int iMeshID );
	void		dbChangeMesh							( int iObjectID, int iLimbID, int iMeshID );
	void		dbMakeMeshFromObject					( int iID, int iObjectID );
	void		dbReduceMesh							( int iMeshID, int iBlockMode, int iNearMode, int iGX, int iGY, int iGZ );
	void		dbMakeLODFromMesh						( int iMeshID, int iVertNum, int iNewMeshID );

	void		dbLockVertexDataForLimb					( int iID, int iLimbID, int iReplaceOrUpdate );
	void		dbLockVertexDataForLimb					( int iID, int iLimbID );
	void		dbLockVertexDataForMesh					( int iID );
	void		dbUnlockVertexData						( void );
	void		dbSetVertexDataPosition					( int iVertex, float fX, float fY, float fZ );
	void		dbSetVertexDataNormals					( int iVertex, float fNX, float fNY, float fNZ );
	void		dbSetVertexDataDiffuse					( int iVertex, DWORD dwDiffuse );
	void		dbSetVertexDataUV						( int iVertex, float fU, float fV );
	void		dbSetVertexDataSize 					( int iVertex, float fSize );
	void		dbSetIndexData							( int iIndex, int iValue );
	int			dbGetIndexData							( int iIndex );

	void		dbSetVertexDataUV						( int iVertex, int iIndex, float fU, float fV );

	void		dbAddMeshToVertexData					( int iMeshID );
	void		dbDeleteMeshFromVertexData				( int iVertex1, int iVertex2, int iIndex1, int iIndex2 );
	void		dbPeformCSGUnionOnVertexData			( int iBrushMeshID );
	void		dbPeformCSGDifferenceOnVertexData		( int iBrushMeshID );
	void		dbPeformCSGIntersectionOnVertexData		( int iBrushMeshID );
	int			dbGetVertexDataVertexCount				( void );
	int			dbGetVertexDataIndexCount				( void );
	float		dbGetVertexDataPositionX				( int iVertex );
	float		dbGetVertexDataPositionY				( int iVertex );
	float		dbGetVertexDataPositionZ				( int iVertex );
	float		dbGetVertexDataNormalsX					( int iVertex );
	float		dbGetVertexDataNormalsY					( int iVertex );
	float		dbGetVertexDataNormalsZ					( int iVertex );
	DWORD		dbGetVertexDataDiffuse					( int iVertex );
	float		dbGetVertexDataU						( int iVertex );
	float		dbGetVertexDataV						( int iVertex );
	float		dbGetVertexDataU						( int iVertex, int iIndex );
	float		dbGetVertexDataV						( int iVertex, int iIndex );

	void		dbSetMipmapMode							( int iMode );
	void		dbFlushVideoMemory						( void );
	void		dbDisableTNL							( void );
	void		dbEnableTNL								( void );
	void		dbConvert3DStoX							( char* pFilename1, char* pFilename2 );

	int			dbPickObject							( int iX, int iY, int iObjectStart, int iObjectEnd );
	void		dbPickScreen							( int iX, int iY, float fDistance );
	float		dbGetPickDistance						( void );
	float		dbGetPickVectorX						( void );
	float		dbGetPickVectorY						( void );
	float		dbGetPickVectorZ						( void );

	void		dbMakeEmitter							( int iID, int iSize );
	void		dbGetEmitterData  						( int iID, BYTE** ppVertices, DWORD* pdwVertexCount, int** ppiDrawCount );
	void		dbUpdateEmitter   						( int iID );
	void		dbGetPositionData 						( int iID, sPositionData** ppPosition );

	void		dbExcludeObjectOn 						( int iID );
	void		dbExcludeObjectOff						( int iID );

	int			db
		( int iID );
	int			dbTotalObjectFrames 					( int iID );
	float		dbObjectSize 							( int iID );
	float		dbObjectSizeX							( int iID );
	float		dbObjectSizeY							( int iID );
	float		dbObjectSizeZ							( int iID );
	float		dbObjectSizeX							( int iID, int iUseScaling );
	float		dbObjectSizeY							( int iID, int iUseScaling );
	float		dbObjectSizeZ							( int iID, int iUseScaling );
	int			dbObjectVisible    						( int iID );
	int			dbObjectPlaying    						( int iID );
	int			dbObjectLooping    						( int iID );
	float		dbObjectFrame          					( int iID );
	float		dbObjectSpeed          					( int iID );
	float		dbObjectInterpolation  					( int iID );
	int			dbObjectScreenX        					( int iID );
	int			dbObjectScreenY        					( int iID );
	int			dbObjectInScreen       					( int iID );

	float		dbObjectCollisionRadius					( int iID );
	float		dbObjectCollisionCenterX 				( int iID );
	float		dbObjectCollisionCenterY 				( int iID );
	float		dbObjectCollisionCenterZ 				( int iID );

	int			dbObjectCollision 						( int iObjectA, int iObjectB );
	int			dbObjectHit								( int iObjectA, int iObjectB );
	int			dbLimbCollision							( int iObjectA, int iLimbA, int iObjectB, int iLimbB );
	int			dbLimbHit								( int iObjectA, int iLimbA, int iObjectB, int iLimbB );
	int			dbGetStaticCollisionHit 				( float fOldX1, float fOldY1, float fOldZ1, float fOldX2, float fOldY2, float fOldZ2, float fNX1,   float fNY1,   float fNZ1,   float fNX2,   float fNY2,   float fNZ2   );
	int			dbStaticLineOfSight 					( float fSx, float fSy, float fSz, float fDx, float fDy, float fDz, float fWidth, float fAccuracy );
	int			dbStaticRayCast 						( float fSx, float fSy, float fSz, float fDx, float fDy, float fDz );
	int			dbStaticVolume	 						( float fX, float fY, float fZ, float fNewX, float fNewY, float fNewZ, float fSize );
	float		dbGetStaticCollisionX					( void );
	float		dbGetStaticCollisionY					( void );
	float		dbGetStaticCollisionZ					( void );
	int			dbGetStaticCollisionFloor 				( void );
	int			dbGetStaticCollisionCount				( void );
	int			dbGetStaticCollisionValue				( void );

	float		dbStaticLineOfSightX					( void );
	float		dbStaticLineOfSightY 					( void );
	float		dbStaticLineOfSightZ 					( void );
	float		dbGetObjectCollisionX					( void );
	float		dbGetObjectCollisionY					( void );
	float		dbGetObjectCollisionZ					( void );

	int			dbLimbExist 							( int iID, int iLimbID );

	float		dbLimbOffsetX    						( int iID, int iLimbID );
	float		dbLimbOffsetY    						( int iID, int iLimbID );
	float		dbLimbOffsetZ    						( int iID, int iLimbID );
	float		dbLimbAngleX     						( int iID, int iLimbID );
	float		dbLimbAngleY     						( int iID, int iLimbID );
	float		dbLimbAngleZ     						( int iID, int iLimbID );
	float		dbLimbPositionX  						( int iID, int iLimbID );
	float		dbLimbPositionY  						( int iID, int iLimbID );
	float		dbLimbPositionZ							( int iID, int iLimbID );
	float		dbLimbDirectionX 						( int iID, int iLimbID );
	float		dbLimbDirectionY 						( int iID, int iLimbID );
	float		dbLimbDirectionZ 						( int iID, int iLimbID );
	int			dbLimbTexture							( int iID, int iLimbID );
	int			dbLimbVisible							( int iID, int iLimbID );
	int			dbCheckLimbLink 						( int iID, int iLimbID );
	char*		dbLimbTextureName						( SDK_RETSTR int iID, int iLimbID );

	int			dbMeshExist								( int iID );
	int			dbAlphaBlendingAvailable				( void );
	int			dbFilteringAvailable					( void );
	int			dbFogAvailable							( void );
	int			dbGet3DBLITAvailable					( void );
	int			dbStatistic								( int iCode );
	int			dbGetTNLAvailable						( void );

	char*		dbLimbName						  		( DWORD pDestStr, int iID, int iLimbID );
	int			dbLimbCount					    		( int iID );
	float		dbLimbScaleX				    		( int iID, int iLimbID );
	float		dbLimbScaleY				    		( int iID, int iLimbID );
	float		dbLimbScaleZ				    		( int iID, int iLimbID );

	bool		dbVertexShaderExist						( int iShader );
	bool		dbPixelShaderExist						( int iShader );

	int			dbGetDeviceType							( void );
	bool		dbCalibrateGammaAvailable          		( void );
	bool		dbRenderWindowedAvailable          		( void );
	bool		dbFullScreenGammaAvailable         		( void );
	bool		dbBlitSysOntoLocalAvailable        		( void );
	bool		dbRenderAfterFlipAvailable         		( void );
	bool		dbTNLAvailable			           		( void );
	bool		dbGetHWTransformAndLight        		( void );
	bool		dbSeparateTextureMemoriesAvailable 		( void );
	bool		dbTextureSystemMemoryAvailable			( void );
	bool		dbTextureVideoMemoryAvailable      		( void );
	bool		dbNonLocalVideoMemoryAvailable	 		( void );
	bool		dbTLVertexSystemMemoryAvailable    		( void );
	bool		dbTLVertexVideoMemoryAvailable     		( void );
	bool		dbClipAndScalePointsAvailable      		( void );
	bool		dbClipTLVertsAvailable            		( void );
	bool		dbColorWriteEnableAvailable        		( void );
	bool		dbCullCCWAvailable                 		( void );
	bool		dbCullCWAvailable                  		( void );
	bool		dbAnisotropicFilteringAvailable    		( void );
	bool		dbAntiAliasAvailable             		( void );
	bool		dbColorPerspectiveAvailable        		( void );
	bool		dbDitherAvailable                 		( void );
	bool		dbFogRangeAvailable                		( void );
	bool		dbFogTableAvailable                		( void );
	bool		dbFogVertexAvailable               		( void );
	bool		dbMipMapLODBiasAvailable           		( void );
	bool		dbWBufferAvailable                 		( void );
	bool		dbWFogAvailable                    		( void );
	bool		dbZFogAvailable                    		( void );
	bool		dbAlphaAvailable                   		( void );
	bool		dbCubeMapAvailable                 		( void );
	bool		dbMipCubeMapAvailable              		( void );
	bool		dbMipMapAvailable                  		( void );
	bool		dbMipMapVolumeAvailable            		( void );
	bool		dbNonPowTexturesAvailable        		( void );
	bool		dbPerspectiveTexturesAvailable     		( void );
	bool		dbProjectedTexturesAvailable       		( void );
	bool		dbOnlySquareTexturesAvailable      		( void );
	bool		dbVolumeMapAvailable              		( void );
	bool		dbAlphaComparisionAvailable       		( void );
	bool		dbTextureAvailable              		( void );
	bool		dbZBufferAvailable                 		( void );
	int			dbGetMaximumTextureWidth           		( void );
	int			dbGetMaximumTextureHeight          		( void );
	int			dbGetMaximumVolumeExtent           		( void );
	float		dbGetMaximumVertexShaderVersion    		( void );
	float		dbGetMaximumPixelShaderVersion     		( void );
	int			dbGetMaximumVertexShaderConstants  		( void );
	float		dbGetMaximumPixelShaderValue       		( void );
	int			dbGetMaximumLights                 		( void );

	void		dbObjectSetDeleteCallBack   			( int iID, ON_OBJECT_DELETE_CALLBACK pfn, int userData );
	// mike - 220107 - update function name
	void		dbSetObjectDisableTransform 			( int iID, bool bTransform );
	void		dbCreateMeshForObject					( int iID, DWORD dwFVF, DWORD dwVertexCount, DWORD dwIndexCount );
	void		dbSetObjectWorldMatrix 					( int iID, D3DXMATRIX* pMatrix );
	void		dbUpdateObjectPositionStructure			( sPositionData* pPosition );
	void		dbGetObjectWorldMatrix 					( int iID, int iLimb, D3DXMATRIX* pMatrix );
	D3DXVECTOR3	dbGetObjectCameraLook					( void );
	D3DXVECTOR3	dbGetObjectCameraPosition				( void );
	D3DXVECTOR3	dbGetObjectCameraUp						( void );
	D3DXVECTOR3	dbGetObjectCameraRight					( void );
	D3DXMATRIX	dbGetObjectCameraMatrix					( void );
	sObject*	dbGetObject								( int iID );
	void		dbCalculateObjectBounds					( int iID );
	void		dbGetUniverseMeshList					( vector < sMesh* > *pMeshList );

	void		dbSetGlobalObjectCreationMode			( int iMode );

	// mike - 230505 - need to be able to set mip map LOD bias on a per mesh basis
	void		dbSetObjectMipMapLODBias				( int iID, float fBias );
	void		dbSetObjectMipMapLODBias				( int iID, int iLimb, float fBias );

	// lee - 300706 - GDK fixes
	void		dbSetObjectAmbient						( int iID, bool bAmbient );
	void		dbMakeObjectPlain						( int iID, float fWidth, float fHeight );
	void		dbFadeObject							( int iID, int iPercentage );
	void		dbSetObjectSmoothing					( int iID, int iPercentage );
	void		dbUnglueObject							( int iID );
	void		dbSetAlphaMappingOn						( int iID, int iPercentage );
	void		dbSetLimbSmoothing						( int iID, int iLimbID, int iPercentage );
	bool		dbAnistropicfilteringAvailable    		( void );
	bool		dbBlitSysToLocalAvailable        		( void );
	char*		dbLimbTextureName						( int iID, int iLimbID );

	// lee - 020906 - GDK tweaks
	void		dbPerformChecklistForObjectLimbs		( int iID );
	char*		dbLimbName								( int iID, int iLimbID );

	// lee - 081108 - U71 - new built-in quad for post process shading
	void		dbTextureScreen 						( int iStageIndex, int iImageID );
	void		dbSetScreenEffect						( int iEffectID );
	void		dbRenderQuad 							( void );

#endif

#endif _COBJECTS_H_
