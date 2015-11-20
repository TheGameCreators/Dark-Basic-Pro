/*
            File : DBPro.hpp
   DBPro release : 6.7+
*/

#ifndef INCLUDE_GUARD_DBPro_hpp
#define INCLUDE_GUARD_DBPro_hpp

// Version of DBPro that this interface is based upon
#define DBPRO__INTERFACE_VERSION 1074

// Set up all compiler-specific stuff
///#include <Support/Compiler.hpp>

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

// Load the DBPro Globstruct and Object structures
#include ".\..\..\..\..\Core\Globstruct.h"
#include ".\..\..\..\..\Camera\ccameradatac.h"
//FHB: added these from Limb tools DLL

#include ".\..\..\..\..\DBOFormat\DBOData.h"
#include ".\..\..\..\..\DBOFormat\DBOFile.h"
#include ".\..\..\..\..\DBOFormat\DBOBlock.h"

// Include special DBPro-specific types
///#include <Support/DBProTypes.hpp>

// Load all DBPro DLL prototypes
#include <DBPro/Animation.hpp>
#include <DBPro/Basic2D.hpp>
#include <DBPro/Basic3D.hpp>
#include <DBPro/Bitmap.hpp>
#include <DBPro/Camera.hpp>
#include <DBPro/Core.hpp>
#include <DBPro/File.hpp>
#include <DBPro/FTP.hpp>
#include <DBPro/GameFX.hpp>
#include <DBPro/Image.hpp>
#include <DBPro/Input.hpp>
#include <DBPro/Light.hpp>
#include <DBPro/LODTerrain.hpp>
#include <DBPro/Matrix.hpp>
#include <DBPro/Memblock.hpp>
#include <DBPro/Multiplayer.hpp>
#include <DBPro/MultiplayerPlus.hpp>
#include <DBPro/Music.hpp>
#include <DBPro/ODE.hpp>
#include <DBPro/Particle.hpp>
#include <DBPro/Setup.hpp>
#include <DBPro/Sound.hpp>
#include <DBPro/Sprite.hpp>
#include <DBPro/System.hpp>
#include <DBPro/Terrain.hpp>
#include <DBPro/Text.hpp>
#include <DBPro/Vector.hpp>
#include <DBPro/World3D.hpp>

// Load extra user functions
///#include <Support/Core.hpp>
///#include <Support/Arrays.hpp>
///#include <Support/SimpleHelp.hpp>
///#include <Support/Checklist.hpp>
///#include <Support/ErrorHandler.hpp>

namespace DBPro
{
	int FindFreeObject ( void );
	int FindFreeMesh ( void );
	float ObjectScaleX ( int );
	float ObjectScaleY ( int );
	float ObjectScaleZ ( int );
	void ReportError ( LPCSTR pErr1, LPSTR pErr2 );
}

#endif

