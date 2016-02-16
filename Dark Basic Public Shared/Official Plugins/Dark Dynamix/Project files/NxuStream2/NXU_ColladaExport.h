#ifndef	NXU_COLLADA_EXPORT_H

#define	NXU_COLLADA_EXPORT_H


#include "NXU_File.h"

namespace	NXU
{
class	NxuPhysicsCollection;

bool saveColladaFile(NxuPhysicsCollection	*c,	NXU_FILE *fph, const char	*asset_name);

}


#endif
