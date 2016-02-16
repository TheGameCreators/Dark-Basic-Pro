#ifndef	NXU_COLLADA_IMPORT_H

#define	NXU_COLLADA_IMPORT_H


namespace	NXU
{

class	NxuPhysicsCollection;

NxuPhysicsCollection *colladaImport(const	char *fname,void *mem,int len);

}


#endif
