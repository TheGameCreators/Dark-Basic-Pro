
/////////////////////////////////////////////////////////////////////////////////////
// ENGINE SET UP COMMANDS ///////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

DARKSDK void			dbPhyEnableDebug		( void );
DARKSDK void			dbPhyStart				( void );
DARKSDK void			dbPhyStart				( int iSimulationType, int iSimulationSceneType, int iSimulationFlags );
DARKSDK void			dbPhyEnd				( void );
DARKSDK void			dbPhyClear				( void );
DARKSDK int				dbPhysX					( void );

DARKSDK void			dbPhyMakeScene			( int iIndex, int iSimulationType, int iSimulationSceneType, int iSimulationFlags );
DARKSDK void			dbPhySetCurrentScene	( int iIndex );

DARKSDK void			dbPhySetGroundPlane		( int iState );

DARKSDK void			dbPhySetFluidRendererParticleRadius ( float v );
DARKSDK void			dbPhySetFluidRendererParticleSurfaceScale ( float v );
DARKSDK void			dbPhySetFluidRendererDensityThreshold ( float v );
DARKSDK void			dbPhySetFluidRendererFluidColor ( float r, float g, float b, float a );
DARKSDK void			dbPhySetFluidRendererFluidColorFalloff ( float r, float g, float b, float a );
DARKSDK void			dbPhySetFluidRendererColorFalloffScale ( float v );
DARKSDK void			dbPhySetFluidRendererSpecularColor ( float r, float g, float b, float a );
DARKSDK void			dbPhySetFluidRendererShininess ( float v );
DARKSDK void			dbPhySetFluidRendererFresnelColor ( float r, float g, float b, float a );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// INTERNAL ENGINE SET UP COMMANDS //////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

DARKSDK int				GetNumDependencies		( void );
DARKSDK const char*		GetDependencyID			( int n );

void dbPhySetupFunctionPointers ( void );
void dbPhySetupSDK				( void );
void dbPhySetupScene			( int iID );
void dbPhySetupDefaultSettings	( void );
void dbPhyStartProcess			( int iID );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////