#ifndef NXU_SCHEMA_TYPES_H

#define NXU_SCHEMA_TYPES_H

#include "NxSimpleTypes.h"

namespace NXU
{

class NxTri
{
public:
  NxTri(NxU32 _a,NxU32 _b,NxU32 _c)
  {
  	a = _a;
  	b = _b;
  	c = _c;
  }
  NxTri(void)
  {
  	a = 0;
  	b = 0;
  	c = 0;
  }
  NxU32	a;
  NxU32 b;
  NxU32 c;
};

class NxTetra
{
public:
  NxTetra(NxU32 _a,NxU32 _b,NxU32 _c,NxU32 _d)
  {
  	a = _a;
  	b = _b;
  	c = _c;
  	d = _d;
  }
  NxTetra(void)
  {
  	a = 0;
  	b = 0;
  	c = 0;
  	d = 0;
  }
  NxU32	a;
  NxU32 b;
  NxU32 c;
  NxU32 d;
};


}

#endif
