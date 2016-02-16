#include "DGDKPlugin.h"
#include "NETCScene.h"
#include "NETCConstruction.h"
#include "NETCDebug.h"


/**
* The main Physics class, you only need to create one instance
* of this class to control all physics functionality. This is accomplished
* via its instance variables - oCore, oConstruction, oScene etc.
*/
public ref class CPhysics : public CDGDKPlugin
{
public:

    CCore ^oCore;
	CConstruction ^oConstruction;
	CScene ^oScene;
    CDebug ^oDebug;

	CPhysics( CDGDKGlobals^ oGlobals )
		: CDGDKPlugin( oGlobals )
	{ 
		#pragma push_macro("new")
		#undef new

		oScene = gcnew CScene(oGlobals);
	    oCore = gcnew CCore(oGlobals);//, oScene);
		oConstruction = gcnew CConstruction(oGlobals);
		oDebug = gcnew CDebug(oGlobals);

		#pragma pop_macro("new")
	}
};
