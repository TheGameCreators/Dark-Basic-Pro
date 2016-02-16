#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#ifdef _MSC_VER
#pragma warning(disable:4996) // Disabling stupid .NET deprecated warning.
#endif

#include <NxPhysics.h>
#include "NXU_SchemaStream.h"
#include "NXU_schema.h"
#include "NXU_File.h"
#include "NXU_string.h"
#include "NXU_Asc2Bin.h"

namespace NXU
{

#define DEBUG_WARNING 0

bool gSaveDefaults=true;

static char filterString[2048];

static bool isSep(unsigned char c)
{
	bool ret = false;

  if ( c < 32 || c >= 126 || c == '%' || c == 34 || c == 39 ) ret = true;

  return ret;
}

static bool isSep(const char *c)
{
	bool ret = false;
	while ( *c )
	{
		if ( isSep( (unsigned char) *c++ ) )
		{
			ret = true;
			break;
		}
	}
	return ret;
}

static char hex(char c)
{
	if ( c < 10 )
		c = '0'+c;
	else
		c = 'A'+(c-10);
	return c;
}

static const char *filterOutputString(const char *str)
{
	const char *ret = str;

  if ( isSep(str) )
  {
		int len = (int)strlen(str);
		if ( len < 1024 )
		{
			char *dest = filterString;
			while ( *str )
			{
				char c = *str++;
				if ( isSep((unsigned char)c) )
				{
					*dest++ = '%';
					*dest++ = hex(c>>4);
					*dest++ = hex(c&15);
				}
				else
				{
					*dest++ = c;
				}
			}
			*dest = 0;
	  	ret = filterString;
	  }
	}
	return ret;
}

static bool isHex(char c)
{
	bool ret = false;
	if ( c >= '0' && c <= '9' )
		ret = true;
	else if ( c >= 'a' && c <= 'f' )
		ret = true;
	else if ( c >= 'A' && c <= 'F' )
		ret = true;
	return ret;
}

static char getHex(char c)
{
	char ret = 0;
	if ( c >= '0' && c <= '9' )
		ret = c-'0';
	else if ( c >= 'a' && c <= 'f' )
		ret = (c-'a')+10;
	else if ( c >= 'A' && c <= 'F' )
		ret = (c-'A')+10;
	return ret;
}

static const char *filterInputString(const char *str)
{
	const char *ret = str;
	if ( strchr(str,'%') )
	{
  	int len = (int)strlen(str);
  	if ( len < 1024 )
  	{
  		char *dest = filterString;
  		while ( *str )
  		{
  			char c = *str++;
  			if ( c == '%' )
  			{
  				char n = *str++;
  				if ( isHex(n) )
  				{
  					char n2 = *str++;
  					if ( isHex(n2) )
  					{
  						c = (getHex(n)<<4)|getHex(n2);
  						*dest++ = c;
  					}
  					else
  					{
  						*dest++ = '%';
  						*dest++ = n;
  						*dest++ = n2;
  					}
  				}
  				else
  				{
  					*dest++ = '%';
  					*dest++ = n;
  				}
  			}
  			else
  			{
  				*dest++ = c;
  			}
  		}
  		*dest = 0;

  		ret = filterString;
  	}
  }
	return ret;
}

static TiXmlNode	*getElement(const	char *name,	const	char *id,TiXmlNode *node_begin)
{
	assert(name);

	if (name ==	0)
	{
		return 0;
	}

	TiXmlNode	*ret = 0;

	TiXmlNode	*node	=	node_begin->FirstChild();

	while	(node	&& !ret)
	{
		if (node->Type() ==	TiXmlNode::ELEMENT)
		{
			if (strcasecmp(node->Value(), name) ==	0)
			{
				if (id)
				{
					TiXmlElement *element	=	node->ToElement();
					TiXmlAttribute *atr	=	element->FirstAttribute();
					if (atr)
					{
						// const	char *aname	=	atr->Name();
						const	char *avalue = atr->Value();
						if (strcasecmp(avalue,	id)	== 0)
						{
							ret	=	node;
							break;
						}
					}

				}
				else
				{
					ret	=	node;
					break;
				}
			}
		}

		//if (node->NoChildren())
		//{
			assert(node);
			while	(node->NextSibling() ==	NULL &&	node !=	node_begin )
			{
				node = node->Parent();
			}
			if (node ==	node_begin)
			{
				break;
			}
			assert(node);
			node = node->NextSibling();
		//}
		//else
		//{
		//	assert(node);
		//	node = node->FirstChild();
		//}

	}

	return ret;

}

static const	char *getElementText(const char	*name,bool &foundNode,TiXmlNode *begin_node)
{
	const	char *ret	=	0;

	TiXmlNode	*node	=	getElement(name,0,begin_node);
	if (node)
	{
    foundNode = true;
		TiXmlNode	*child = node->FirstChild();
		if (child	&& child->Type() ==	TiXmlNode::TEXT)
		{
			ret	=	child->Value();
		}
	}
  else
  {
    foundNode = false;
  }

	return ret;
}

class SchemaClass
{
public:

  SchemaClass(SCHEMA_CLASS ctype,TiXmlNode *node,int depth,SchemaClass *parent,int index,int lineno)
  {
  	mClassType = ctype;
  	mNode      = node;
  	mDepth     = depth;
  	mCurrent   = node;
    mClassName = EnumToString(mClassType);
    mParent    = parent;
    mIndex     = index;
    mPrevious  = 0;
    mNext      = 0;
    mLineNo    = lineno;
  }

  void setPrevious(SchemaClass *prev)
  {
    mPrevious = prev;
  }

  void setNext(SchemaClass *next)
  {
    mNext = next;
  }

	const char * locateAttribute(const char *txt)
	{
		const char *ret = 0;
		TiXmlElement *e = mNode->ToElement();
		if ( e )
		{
  		TiXmlAttribute *atr	=	e->FirstAttribute();
  		while	(atr)
  		{
  			const	char *aname	=	atr->Name();
  			const	char *avalue = atr->Value();
  			if ( strcmp(txt,aname) == 0 )
  			{
  				ret = avalue;
  				break;
  			}
  			atr	=	atr->Next();
  		}
		}
		return ret;
	}

	const char * locate(const char *txt)
	{
		bool ignore = false;
		const char *ret = locateAttribute(txt);
		if ( !ret )
		{
      bool foundNode;
			ret = getElementText(txt,foundNode,mNode);
			if ( !foundNode)
			{
				if ( !ignore )
				{
#if DEBUG_WARNING
  				reportWarning("Failed to locate item node(%-30s) '%s'",mNode->Value(), txt );
#endif
				}
			}
		}
		return ret;
	}

  int getLineNo(void) const { return mLineNo; };

  int          mLineNo;  // the line number in the original XML file where this was found.
	int          mDepth;
  TiXmlNode 	*mNode;
  TiXmlNode	  *mCurrent;            // current scan location.
  SCHEMA_CLASS mClassType;  // the class type.
  const char * mClassName;
  SchemaClass *mParent;
  int          mIndex;
  SchemaClass *mNext;          // next one of this type.
  SchemaClass *mPrevious;      // previous one of this type.
};

typedef NxArray< SchemaClass * > SchemaClassVector;

class SchemaStack
{
public:
	int	mCurrent;
	SchemaClass *mSearch;
};

class SchemaXML
{
public:
  SchemaXML(const char *fname,void *mem,int len,SchemaStream *ss)
  {
  	mStream = ss;
	  mDocument = new TiXmlDocument;
	  mSearch = 0;
	  mIsValid = false;
#if DEBUG_WARNING
		mFph = fopen("schemalog.txt", "wb");
#endif
    for (int i=0; i<SC_LAST; i++)
    {
      mSearchLocations[i] = 0;
    }

	  if (mDocument->LoadFile(fname,mem,len))
	  {
		  mNode	=	mDocument;
		  preParse(mNode);
	  }
	  else
	  {
	    reportError("Error	parsing	file '%s'	as XML", fname);
			reportError("XML	parse	error(%d)	on Row:	%d Column: %d",	mDocument->ErrorId(), mDocument->ErrorRow(), mDocument->ErrorCol());
			reportError("XML	error	description: \"%s\"",	mDocument->ErrorDesc());
  		delete mDocument;
	  	mDocument = 0;
	  	mNode = 0;
    }
    mStackPtr = 0;

  }

  ~SchemaXML(void)
  {
#if DEBUG_WARNING
    if ( mFph )
      fclose(mFph);
#endif
  	delete mDocument;
  	for (unsigned int i=0; i<mClasses.size(); i++)
  	{
  		SchemaClass *sc = mClasses[i];
  		delete sc;
  	}
  }

	bool isClass(const char *str,SCHEMA_CLASS &t,SchemaClass *last,TiXmlNode *node)
	{
		bool ret = false;

		if ( strcmp(str,"NXUSTREAM") == 0 )
		{
			reportWarning("This old version of NxuStream is no longer supported.\r\n");
		}
		else if ( strcmp(str,"NXUSTREAM2") == 0 )
		{
			t = SC_NXUSTREAM2;
			ret = true;
		}
		else
		{
			if ( !ret )
			{
				for (int i=0; i<SC_LAST; i++)
				{
					const char *c = EnumToString( (SCHEMA_CLASS) i );
					c+=3; // skip the enumeration prefix 'SC_'
					if ( strcmp(c,str) == 0 )
					{
						t = (SCHEMA_CLASS) i;
  					ret = true;
						break;
					}
				}
			}
		}


    if ( ret )
    {
      SCHEMA_CLASS sc = SC_LAST;
      if ( last ) sc = last->mClassType;
#if DEBUG_WARNING
      reportWarning("ISCLASS: %-30s Node:%-30s  Last: %-30s Type: %s", EnumToString( (SCHEMA_CLASS) t), node->Value(), EnumToString( sc ), str );
#endif
    }
    else
    {
      SCHEMA_CLASS sc = SC_LAST;
      if ( last ) sc = last->mClassType;
#if DEBUG_WARNING
      reportWarning("NOCLASS: %-30s Node:%-30s  Last: %-30s Type: %s", "", node->Value(), EnumToString( sc ), str );
#endif
    }

		return ret;
	}

  SchemaClass * findParent(int depth)
  {
    SchemaClass *ret = 0;
    int match = depth-1; //
    if ( match >= 0 )
    {
      int count = mClasses.size();
      for (int i=count-1; i>=0; i--)
      {
        SchemaClass *c = mClasses[i];
        if ( c->mDepth  <= match )
        {
          ret = c;
          break;
        }
      }
    }
    return ret;
  }

  void preParse(TiXmlNode *begin)
  {

  	TiXmlNode	*node	=	begin;
  	int	depth	=	0;

    SchemaClass *nodes[SC_LAST];

    for (int i=0; i<SC_LAST; i++) nodes[i] = 0;

  	while	(node)
  	{
  		SCHEMA_CLASS t;

  		if ( node->Type() == TiXmlNode::ELEMENT )
		  {

        SchemaClass *last = findParent(depth);
        const char *parent = "";
        if ( last )
          parent = last->mClassName;

        if (isClass(node->Value(), t, last, node) )
        {
    			if ( t == SC_NXUSTREAM2 )
    				mIsValid = true;

          int index = mClasses.size();

    			SchemaClass *sc = new SchemaClass(t,node,depth,last,index, node->Row() );

          if ( mSearchLocations[t] == 0 )
          {
            mSearchLocations[t] = sc;
          }

          SchemaClass *prev = nodes[t];

          if ( prev )
          {
            prev->setNext(sc);
          }

          sc->setPrevious(prev);

					nodes[t] = sc;
#if DEBUG_WARNING
          if ( mFph )
            fprintf(mFph,"%d) source(%s) is of type (%s) Depth(%d) Parent(%s) \r\n", mClasses.size(), node->Value(), sc->mClassName, depth, parent );
#endif
   				mClasses.push_back(sc);
        }
        else
        {
#if DEBUG_WARNING
          if ( mFph )
          {
            fprintf(mFph,"     ## %s does not map to a known major class type depth(%d) parent(%s).\r\n", node->Value(), depth, parent );
          }
#endif
        }

			}

  		if (node->NoChildren())
  		{
  			while	(node->NextSibling() ==	NULL &&	node !=	mDocument)
  			{
  				depth--;
  				node = node->Parent();
  			}
  			if (node ==	mDocument)
  			{
  				break;
  			}
  			assert(node);
  			node = node->NextSibling();
  		}
  		else
  		{
  			depth++;
  			node = node->FirstChild();
  		}
  	}
  }

	bool beginHeader(NxI32 headerId,const char *parent)
	{
		bool ret = false;

    SCHEMA_CLASS c = (SCHEMA_CLASS) headerId;

    bool nextOk = true;

    SchemaClass *sc = mSearchLocations[c];

    if ( sc )
    {

      // int lineno = sc->getLineNo();

		if (mSearch && mSearch != sc->mParent)
			return false;
		//{
		//	if (sc->mParent == mSearch)
		//	// Find the first opening tag outside the current scope
		//	SchemaClass *currentParent = mSearch;
		//	while (currentParent->mNode->NextSibling() == 0 && currentParent->mParent != 0)
		//		currentParent = currentParent->mParent;
		//	if (currentParent->mNode->NextSibling() != 0)
		//	{
		//		if (sc->mLineNo >= currentParent->mNode->NextSibling()->Row() || sc->mLineNo <= mSearch->mLineNo)
		//			return false;
		//	}
		//}

			ret = true;

   		// bool ok = true;

   		if ( parent )
   		{
   			const char *id = sc->locateAttribute("id");
				if ( id )
				{
					const char *v = getElement(parent);
					const char *idp = getElement(id);
   				if ( strcasecmp(idp,v) != 0 )
   				{
   					ret = false;
   				}
   			}
				else
				{
          ret = true;
//					reportWarning("unable to find 'id' for '%s' of '%s'\r\n", parent, sc->mNode->Value() );
				}
   		}

      if ( ret )
      {
        if ( mStackPtr < 64 )
        {
          mStack[mStackPtr] = mSearch;
          mStackPtr++;
        }

        mSearch = sc;

        if ( nextOk )
          mSearchLocations[c] = sc->mNext;
      }

    }

    return ret;
	}

  bool endHeader(void)
  {
  	bool ret = false;


    if ( mStackPtr )
    {
      mStackPtr--;
      mSearch = mStack[mStackPtr];
      ret = true;
    }

  	return ret;
  }

  NxI32 peekHeader(NxI32 or0,
                   NxI32 or1,
                   NxI32 or2,
                   NxI32 or3,
                   NxI32 or4,
                   NxI32 or5,
                   NxI32 or6,
                   NxI32 or7,
                   NxI32 or8,
                   NxI32 or9,
                   NxI32 or10)
  {
    NxI32 peekId = -1;

    int scan[11];

    scan[0] = or0;
    scan[1] = or1;
    scan[2] = or2;
    scan[3] = or3;
    scan[4] = or4;
    scan[5] = or5;
    scan[6] = or6;
    scan[7] = or7;
    scan[8] = or8;
    scan[9] = or9;
    scan[10] = or10;
    if ( peekId == -1 )
    {

      int nearest = 999999;

      for (int i=0; i<11; i++)
      {
        NxI32 header = scan[i];

        if ( header < 0)
          break;

        if ( header >= 0 && header < SC_LAST )
        {
  	//SCHEMA_CLASS ctype = (SCHEMA_CLASS) header;


          SchemaClass *sc = mSearchLocations[header];

    			if ( sc )
    			{
    				SchemaClass *parent = sc->mParent;
  					if ( parent == mSearch )
  					{
  						if ( sc->mIndex < nearest )
  						{
  							nearest = sc->mIndex;
  							peekId = header;
    					}
  					}
    			}
        }
      }
    }

		return peekId;
  }

  void load(NX_BOOL &v,const char *name,bool attribute)
  {
  	if ( mSearch )
  	{
  		const char *txt = mSearch->locate(name);
  		if ( txt )
  		{
				if ( strcasecmp(txt,"true") == 0 || strcasecmp(txt,"1") == 0 )
					v = true;
				else
					v = false;
  		}
		}
  }

  void load(NxU16 &v,const char *name,bool attribute)
  {
  	if ( mSearch )
  	{
  		const char *txt = mSearch->locate(name);
  		if ( txt )
  		{
  			v = (NxU16) strtol(txt, FL_UNSIGNED);
  		}
  	}
  }

  void load(NxU32 &v,const char *name,bool attribute)
  {
  	if ( mSearch )
  	{
  		const char *txt = mSearch->locate(name);
  		if ( txt )
  		{
  		  v = (NxU32) strtol(txt, FL_UNSIGNED);
  		}
  	}
  }

  void load(NxF32 &v,const char *name,bool attribute)
  {
  	if ( mSearch )
  	{
			const char *txt = 0;
 		  txt = mSearch->locate(name);
  		if ( txt )
  		{
  			Asc2Bin(txt,1,"f",&v);
  		}
  	}
  }

  void load(const char *&v,const char *name,bool attribute)
  {
  	if ( mSearch )
  	{
  		const char *txt = 0;

			bool isActorLookup = false;
			bool isConvexLookup = false;
			bool isMeshLookup = false;
      bool isClothLookup = false;

			if ( !txt )
			{
			  txt = mSearch->locate(name);
			}

  		if ( txt )
  		{
				if ( isActorLookup || isConvexLookup || isMeshLookup || isClothLookup )
				{
					int no = strtol(txt, FL_UNSIGNED);
					if ( isActorLookup )
					  v = actorLookup(no);
					else if ( isConvexLookup )
						v = convexLookup(no);
					else if ( isMeshLookup )
						v = meshLookup(no);
          else if ( isClothLookup )
            v = clothLookup(no);
				}
				else
				{
					txt = filterInputString(txt);
  			  v = getGlobalString(txt);
				}
  		}
  	}
  }

	const char *convexLookup(int index)
	{
		const char *ret = 0;
		if ( mStream->getCurrentCollection() )
		{
			int count = mStream->getCurrentCollection()->mConvexMeshes.size();
			if ( index >= 0 && index < count )
			{
				NxConvexMeshDesc *cm = mStream->getCurrentCollection()->mConvexMeshes[index];
				ret = cm->mId;
			}
		}
		return ret;
	}

	const char *meshLookup(int index)
	{
		const char *ret = 0;
		if ( mStream->getCurrentCollection() )
		{
			int count = mStream->getCurrentCollection()->mTriangleMeshes.size();
			if ( index >= 0 && index < count )
			{
				NxTriangleMeshDesc *cm = mStream->getCurrentCollection()->mTriangleMeshes[index];
				ret = cm->mId;
			}
		}
		return ret;
	}

	const char *clothLookup(int index)
	{
		const char *ret = 0;
		if ( mStream->getCurrentCollection() )
		{
			int count = mStream->getCurrentCollection()->mClothMeshes.size();
			if ( index >= 0 && index < count )
			{
				NxClothMeshDesc *cm = mStream->getCurrentCollection()->mClothMeshes[index];
				ret = cm->mId;
			}
		}
		return ret;
	}

	const char * actorLookup(int index)
	{
    const char *ret = 0;
    if ( mStream->getCurrentScene() )
    {
      int count = mStream->getCurrentScene()->mActors.size();
      if ( index >= 0 && index < count )
      {
        ret = mStream->getCurrentScene()->mActors[index]->mId;
      }
			else
			{
				if ( index != -1 )
					assert(0);
			}
    }
		return ret;
	}

  void load(NxArray< NxF32 > &v,const char *name,bool attribute)
  {
  	if ( mSearch )
  	{
  		const char *txt = mSearch->locate(name);
  		if ( txt )
  		{
  			int count=0;
  			void *mem = Asc2Bin(txt,count,"f");
  			if ( mem )
  			{
  				float *source = (float *)mem;
  				for (int i=0; i<count; i++)
  				{
  					v.push_back( source[i]);
  				}
  				delete (char*)mem;
  			}
  		}
  	}
  }

  void load(NxArray< NxU32 > &v,const char *name,bool attribute)
  {
  	if ( mSearch )
  	{
  		const char *txt = mSearch->locate(name);
  		if ( txt )
  		{
  			int count=0;
  			void *mem = Asc2Bin(txt,count,"d");
  			if ( mem )
  			{
  				NxU32 *source = (NxU32 *)mem;
  				for (int i=0; i<count; i++)
  				{
  					v.push_back( source[i]);
  				}
  				delete (char*)mem;
  			}
  		}
  	}
  }

  void load(NxArray< NxU16 > &v,const char *name,bool attribute)
  {
  	if ( mSearch )
  	{
  		const char *txt = mSearch->locate(name);
  		if ( txt )
  		{
  			int count=0;
  			void *mem = Asc2Bin(txt,count,"h");
  			if ( mem )
  			{
  				NxU16 *source = (NxU16 *)mem;
  				for (int i=0; i<count; i++)
  				{
  					v.push_back( source[i]);
  				}
  				delete (char*)mem;
  			}
  		}
  	}
  }

  void load(NxArray< NxU8 > &v,const char *name,bool attribute)
  {
  	if ( mSearch )
  	{
  		const char *txt = mSearch->locate(name);
  		if ( txt )
  		{
  			int count=0;
  			void *mem = Asc2Bin(txt,count,"x1");
  			if ( mem )
  			{
  				NxU8 *source = (NxU8 *)mem;
  				for (int i=0; i<count; i++)
  				{
  					v.push_back( source[i]);
  				}
  				delete (char*)mem;
  			}
  		}
  	}
  }

  void load(NxVec3 &v,const char *name,bool attribute)
  {
  	if ( mSearch )
  	{
  		const char *txt = mSearch->locate(name);
  		if ( txt )
  		{
  			Asc2Bin(txt,1,"fff",&v.x);
  		}
  	}
  }

  void load(NxQuat &v,const char *name,bool attribute)
  {
  	if ( mSearch )
  	{
  		const char *txt = mSearch->locate(name);
  		if ( txt )
  		{
  			NxF32 quat[4];
  			void *mem = Asc2Bin(txt,1,"ffff",quat);
  			if ( mem )
  			{
  				v.setXYZW(quat);
  			}
  		}
  	}
  }

  void load(NxMat34 &v,const char *name,bool attribute)
  {
  	if ( mSearch )
  	{
  		const char *txt = mSearch->locate(name);
  		if ( txt )
  		{
  			float matrix[12];
  			void *mem = Asc2Bin(txt,1,"fff fff fff fff",matrix);
  			if ( mem )
  			{

 				  v.M.setRowMajor(matrix);

  				v.t.x = matrix[9];
  				v.t.y = matrix[10];
  				v.t.z = matrix[11];
  			}
  		}
  	}
  }

  void load(NxMat33 &v,const char *name,bool attribute)
  {
  	if ( mSearch )
  	{
  		const char *txt = mSearch->locate(name);
  		if ( txt )
  		{
  			float matrix[9];
  			void *mem = Asc2Bin(txt,1,"fff fff fff",matrix);
  			if ( mem )
  			{
 				  v.setRowMajor(matrix);
  			}
  		}
  	}
  }

  void load(NxArray< NxVec3 > &v,const char *name,bool attribute)
  {
  	if ( mSearch )
  	{
  		const char *txt = mSearch->locate(name);
  		if ( txt )
  		{
  			int count=0;
  			void *mem = Asc2Bin(txt,count,"fff");
  			if ( mem )
  			{
  				float *source = (float *)mem;
  				for (int i=0; i<count; i++)
  				{
  					NxVec3 nv(source);
  					v.push_back(nv);
  					source+=3;
  				}
  				delete (char*)mem;
  			}
  		}
  	}
  }

  void load(NxArray< NxTri > &v,const char *name,bool attribute)
  {
  	if ( mSearch )
  	{
  		const char *txt = mSearch->locate(name);
  		if ( txt )
  		{
  			int count=0;
  			void *mem = Asc2Bin(txt,count,"ddd");
  			if ( mem )
  			{
  				NxU32 *source = (NxU32 *)mem;
  				for (int i=0; i<count; i++)
  				{
  					NxTri t(source[0],source[1],source[2]);
  					v.push_back(t);
  					source+=3;
  				}
  				delete (char*)mem;
  			}
  		}
  	}
  }

  void load(NxArray< NxTetra > &v,const char *name,bool attribute)
  {
  	if ( mSearch )
  	{
  		const char *txt = mSearch->locate(name);
  		if ( txt )
  		{
  			int count=0;
  			void *mem = Asc2Bin(txt,count,"dddd");
  			if ( mem )
  			{
  				NxU32 *source = (NxU32 *)mem;
  				for (int i=0; i<count; i++)
  				{
  					NxTetra t(source[0],source[1],source[2],source[3]);
  					v.push_back(t);
  					source+=4;
  				}
  				delete (char*)mem;
  			}
  		}
  	}
  }

  void load(NxBounds3 &v,const char *name,bool attribute)
  {
  	if ( mSearch )
  	{
  		const char *txt = mSearch->locate(name);
  		if ( txt )
  		{
  			float bounds[6];
  			void *mem = Asc2Bin(txt,2,"fff",bounds);
  			if ( mem )
  			{
  				v.min.set(bounds);
  				v.max.set(&bounds[3]);
  			}
  		}
  	}
  }

  void load(NxPlane &v,const char *name,bool attribute)
  {
  	if ( mSearch )
  	{
  		const char *txt = mSearch->locate(name);
  		if ( txt )
  		{
  			NxF32 plane[4];
  			void *mem = Asc2Bin(txt,1,"fff f",plane);
  			if ( mem )
  			{
				v.set(plane[0], plane[1], plane[2], plane[3]);
  			}
  		}
  	}
  }


	TiXmlDocument   	*mDocument;
	TiXmlNode         *mNode;
  SchemaClass       *mSearch;
  bool               mIsValid;
  SchemaClass       *mSearchLocations[SC_LAST];
	SchemaClassVector  mClasses;
#if DEBUG_WARNING
	FILE *mFph;
#endif
  SchemaClass       *mStack[64];
  int                mStackPtr;
  SchemaStream      *mStream; // parent stream.
};

static bool isUpper(const char c)
{
  bool ret = false;
  if ( c >= 'A' && c <= 'Z' ) ret = true;
  return ret;
}

const char *getElement(const char *name)
{
  const char *ret = name;

  if ( name && name[0] == 'm' && isUpper(name[1]) )
  {
  	static char	data[64	*32];
  	static int index = 0;

  	char *store	=	&data[index	*64];
  	index++;

  	if (index	== 32)
  	{
  		index	=	0;
  	}

    if ( isUpper(name[2]) ) // don't lower case the first letter if the next letter is also upper case!
    	store[0] = name[1];
    else
      store[0] = name[1]+32; // convert it to lower case.

    const char *source = &name[2];
    char *dest = &store[1];
    while ( *source )
    {
      *dest++ = *source++;
    }
    *dest = 0;
    ret = store;
  }

  return ret;
}

static const char	*fstring(NxF32 v,	bool binary	=	false)
{
	static char	data[64	*16];
	static int index = 0;

	char *ret	=	&data[index	*64];
	index++;
	if (index	== 16)
	{
		index	=	0;
	}

	if (v	== FLT_MAX)
	{
		strcpy(ret,	"FLT_MAX");
	}
	else if	(v ==	-FLT_MAX)
	{
		strcpy(ret,	"FLT_MIN");
	}
	else if	(v ==	1)
	{
		strcpy(ret,	"1");
	}
	else if	(v ==	0)
	{
		strcpy(ret,	"0");
	}
	else if	(v ==	 - 1)
	{
		strcpy(ret,	"-1");
	}
	else
	{
		if (binary)
		{
			unsigned int *iv = (unsigned int*) &v;
			sprintf(ret, "%.4f$%x",	v,	*iv);
		}
		else
		{
			sprintf(ret, "%.9f", v);
			const	char *dot	=	strstr(ret,	".");
			if (dot)
			{
				int	len	=	(int)strlen(ret);
				char *foo	=	&ret[len - 1];
				while	(*foo	== '0')
				{
					foo--;
				}
				if (*foo ==	'.')
				{
					*foo = 0;
				}
				else
				{
					foo[1] = 0;
				}
			}
		}
	}

	return ret;
}

class SchemaBlock
{
public:

  SchemaBlock(const char *header,SchemaStream *f,int depth)
  {
  	mName = header;
  	mFph = f;
  	mDepth = depth;
  	mFlushed = false;
  	indent(true,mDepth);
  	add(true,"<%s",header);
  }

  ~SchemaBlock(void)
  {
  	if ( mFph )
  	{
  		if ( mFlushed )
  		{
				assert( mHeader.size() == 0 );

      	if ( mBody.size() )
      	{
    		  mFph->write(&mBody[0], mBody.size() );
    		}

			  for (int i=0; i<mDepth; i++)
			  {
				  mFph->myprintf("  ");
			  }
     		mFph->myprintf("</%s>\r\n", mName );
  		}
  		else
  		{
      	mFph->write(&mHeader[0], mHeader.size() );

      	if ( mBody.size() )
      	{
    		  mFph->myprintf(">\r\n");
    		  mFph->write(&mBody[0], mBody.size());
				  for (int i=0; i<mDepth; i++)
				  {
					  mFph->myprintf("  ");
				  }
      		mFph->myprintf("</%s>\r\n", mName );
      	}
      	else
    	  {
    		  mFph->myprintf("/>\r\n");
      	}
      }
    }
  }

  void flush(void)
  {
  	if ( mHeader.size() )
  	{
    	mFph->write(&mHeader[0], mHeader.size());
    	mFph->myprintf(">\r\n");
    	mHeader.clear();
  	}
  	if ( mBody.size() )
  	{
   		mFph->write(&mBody[0], mBody.size() );
   		mBody.clear();
  	}
  	mFlushed = true; // if it was flushed already...
  }

  void store(NX_BOOL v,const char *name,bool attribute)
  {
  	if ( mFlushed ) assert( attribute == false ); // can't write new attributes after the header has been flushed!
  	const char *tf = "false";
  	if ( v ) tf = "true";

  	if ( attribute )
  		add(true," %s=\"%s\"", name, tf );
  	else
  	{
  		indent(false,mDepth+1);
  		add(false,"<%s>%s</%s>\r\n", name, tf, name );
  	}
  }

  void store(NxU32 v,const char *name,bool attribute)
  {
  	if ( mFlushed ) assert( attribute == false ); // can't write new attributes after the header has been flushed!
  	if ( attribute )
  		add(true," %s=\"%u\"", name, v );
  	else
  	{
  		indent(false,mDepth+1);
  		add(false,"<%s>%u</%s>\r\n", name, v, name );
  	}
  }

  void store(NxU16 v,const char *name,bool attribute)
  {
  	if ( mFlushed ) assert( attribute == false ); // can't write new attributes after the header has been flushed!
  	if ( attribute )
  		add(true," %s=\"%u\"", name, v );
  	else
  	{
  		indent(false,mDepth+1);
  		add(false,"<%s>%u</%s>\r\n", name, v, name );
  	}
  }

  void store(NxF32 v,const char *name,bool attribute)
  {
  	if ( mFlushed ) assert( attribute == false ); // can't write new attributes after the header has been flushed!
  	if ( attribute )
  		add(true," %s=\"%s\"", name, fstring(v) );
  	else
  	{
  		indent(false,mDepth+1);
  		add(false,"<%s>%s</%s>\r\n", name, fstring(v), name );
  	}
  }

  void store(const char *_v,const char *name,bool attribute)
  {
		const char *v = _v;
		if ( _v == 0 ) v = "";

		v = filterOutputString(v);

  	if ( mFlushed ) assert( attribute == false ); // can't write new attributes after the header has been flushed!
  	if ( attribute )
  		add(true," %s=\"%s\"", name, v );
  	else
  	{
  		indent(false,mDepth+1);
  		add(false,"<%s>%s</%s>\r\n", name, v, name );
  	}
  }

  void store(NxArray< NxF32 > &v,const char *name,bool attribute)
  {
  	assert(attribute == false);
  	indent(false,mDepth+1);
  	add(false,"<%s>", name );
  	bool lastReturn = false;
  	int returnCount = 0;
  	for (NxU32 i=0; i<v.size(); i++)
  	{
  		add(false,"%s ", fstring( v[i] ) );
  		if ( ((i+1)&15) == 0 )
  		{
  			add(false,"\r\n");
  			indent(false,mDepth+2);
  			lastReturn = true;
  			returnCount++;
  		}
  		else
  			lastReturn = false;
  	}
  	if ( !lastReturn && returnCount != 0)
  	{
  		add(false,"\r\n");
    	indent(false,mDepth+1);
    }
  	add(false,"</%s>\r\n", name );
  }

  void store(NxArray< NxU32 > &v,const char *name,bool attribute)
  {
  	assert(attribute == false);
  	indent(false,mDepth+1);
  	add(false,"<%s>", name );
  	bool lastReturn = false;
  	int returnCount = 0;
  	for (NxU32 i=0; i<v.size(); i++)
  	{
  		add(false,"%u ", v[i] );
  		if ( ((i+1)&15) == 0 )
  		{
  			add(false,"\r\n");
  			indent(false,mDepth+2);
  			lastReturn = true;
  			returnCount++;
  		}
  		else
  			lastReturn = false;
  	}
  	if ( !lastReturn && returnCount != 0)
  	{
  		add(false,"\r\n");
    	indent(false,mDepth+1);
    }
  	add(false,"</%s>\r\n", name );
  }

  void store(NxArray< NxU8 > &v,const char *name,bool attribute)
  {
  	assert( attribute == false );
  	indent(false,mDepth+1);
  	add(false,"<%s>", name );
  	bool lastReturn = false;
  	int returnCount = 0;
  	for (NxU32 i=0; i<v.size(); i++)
  	{
  		add(false,"%02X", v[i] );
  		if ( ((i+1)&63) == 0 )
  		{
  			add(false,"\r\n");
  			indent(false,mDepth+2);
  			lastReturn = true;
  			returnCount++;
  		}
  		else
  			lastReturn = false;
  	}
  	if ( !lastReturn && returnCount != 0)
  	{
  		add(false,"\r\n");
    	indent(false,mDepth+1);
    }
  	add(false,"</%s>\r\n", name );
  }

  void store(NxArray< NxU16 > &v,const char *name,bool attribute)
  {
  	assert( attribute == false );
  	indent(false,mDepth+1);
  	add(false,"<%s>", name );
  	bool lastReturn = false;
  	int returnCount = 0;
  	for (NxU32 i=0; i<v.size(); i++)
  	{
  		NxU32 uv = v[i];
  		add(false,"%u ", uv );
  		if ( ((i+1)&15) == 0 )
  		{
  			add(false,"\r\n");
  			indent(false,mDepth+2);
  			lastReturn = true;
  			returnCount++;
  		}
  		else
  			lastReturn = false;
  	}
  	if ( !lastReturn && returnCount != 0)
  	{
  		add(false,"\r\n");
    	indent(false,mDepth+1);
    }
  	add(false,"</%s>\r\n", name );
  }

  void store(NxVec3 &v,const char *name,bool attribute)
  {
  	if ( mFlushed ) assert( attribute == false ); // can't write new attributes after the header has been flushed!
  	if ( attribute )
  		add(true," %s=\"%s %s %s\"", name, fstring(v.x), fstring(v.y), fstring(v.z) );
  	else
  	{
  		indent(false,mDepth+1);
  		add(false,"<%s>%s %s %s</%s>\r\n", name, fstring(v.x), fstring(v.y), fstring(v.z), name );
  	}
  }

  void store(NxQuat &v,const char *name,bool attribute)
  {
  	if ( mFlushed ) assert( attribute == false ); // can't write new attributes after the header has been flushed!
  	float q[4];
  	v.getXYZW(q);

  	if ( attribute )
  		add(true," %s=\"%s %s %s %s\"", name, fstring(q[0]), fstring(q[1]), fstring(q[2]), fstring(q[3]) );
  	else
  	{
  		indent(false,mDepth+1);
  		add(false,"<%s>%s %s %s %s</%s>\r\n", name, fstring(q[0]), fstring(q[1]), fstring(q[2]), fstring(q[3]), name );
  	}
  }

  void store(NxMat34 &v,const char *name,bool attribute)
  {
  	assert( attribute == false );
  	float f[9];
  	v.M.getRowMajor(f);
 		indent(false,mDepth+1);
  	add(false,"<%s>%s %s %s  %s %s %s  %s %s %s  %s %s %s</%s>\r\n",
  	  name,
  	  fstring(f[0]),fstring(f[1]),fstring(f[2]),
  	  fstring(f[3]),fstring(f[4]),fstring(f[5]),
  	  fstring(f[6]),fstring(f[7]),fstring(f[8]),
  	  fstring(v.t.x),fstring(v.t.y),fstring(v.t.z),
  	  name );
  }

  void store(NxMat33 &v,const char *name,bool attribute)
  {
  	assert( attribute == false );
  	float f[9];
  	v.getRowMajor(f);
 		indent(false,mDepth+1);
  	add(false,"<%s>%s %s %s  %s %s %s  %s %s %s</%s>\r\n",
  	  name,
  	  fstring(f[0]),fstring(f[1]),fstring(f[2]),
  	  fstring(f[3]),fstring(f[4]),fstring(f[5]),
  	  fstring(f[6]),fstring(f[7]),fstring(f[8]),
  	  name );
  }

  void store(NxBounds3 &v,const char *name,bool attribute)
  {
  	assert( attribute == false );
 		indent(false,mDepth+1);
    add(false,"<%s>%s %s %s  %s %s %s</%s>\r\n",
      name,
      fstring(v.min.x),fstring(v.min.y),fstring(v.min.z),
      fstring(v.max.x),fstring(v.max.y),fstring(v.max.z),
      name);
  }

  void store(NxPlane &v,const char *name,bool attribute)
  {
    if ( mFlushed ) assert( attribute == false ); // can't write new attributes after the header has been flushed!
  	if ( attribute )
		add(true," %s=\"%s %s %s  %s\"", name, fstring(v.normal.x), fstring(v.normal.y), fstring(v.normal.z), fstring(v.d) );
  	else
  	{
  		indent(false,mDepth+1);
		add(false,"<%s>%s %s %s  %s</%s>\r\n",
          name,
          fstring(v.normal.x),fstring(v.normal.y),fstring(v.normal.z),fstring(v.d),
          name);
  	}
  }


  void store(NxArray< NxVec3 > &v,const char *name,bool attribute)
  {
  	assert( attribute == false );
  	indent(false,mDepth+1);
  	add(false,"<%s>", name );
  	bool lastReturn = false;
  	int returnCount = 0;
  	for (NxU32 i=0; i<v.size(); i++)
  	{
  		add(false,"%s %s %s  ", fstring( v[i].x ), fstring( v[i].y ), fstring( v[i].z ) );
  		if ( ((i+1)&3) == 0 )
  		{
  			add(false,"\r\n");
  			indent(false,mDepth+2);
  			lastReturn = true;
  			returnCount++;
  		}
  		else
  			lastReturn = false;
  	}
  	if ( !lastReturn && returnCount != 0)
  	{
  		add(false,"\r\n");
    	indent(false,mDepth+1);
    }
  	add(false,"</%s>\r\n", name );
  }

  void store(NxArray< NxTri > &v,const char *name,bool attribute)
  {
  	assert( attribute == false );
  	indent(false,mDepth+1);
  	add(false,"<%s>", name );
  	bool lastReturn = false;
  	int returnCount = 0;
  	for (NxU32 i=0; i<v.size(); i++)
  	{
  		add(false,"%u %u %u  ", v[i].a, v[i].b, v[i].c );
  		if ( ((i+1)&3) == 0 )
  		{
  			add(false,"\r\n");
  			indent(false,mDepth+2);
  			lastReturn = true;
  			returnCount++;
  		}
  		else
  			lastReturn = false;
  	}
  	if ( !lastReturn && returnCount != 0)
  	{
  		add(false,"\r\n");
    	indent(false,mDepth+1);
    }
  	add(false,"</%s>\r\n", name );
  }

  void store(NxArray< NxTetra > &v,const char *name,bool attribute)
  {
  	assert( attribute == false );
  	indent(false,mDepth+1);
  	add(false,"<%s>", name );
  	bool lastReturn = false;
  	int returnCount = 0;
  	for (NxU32 i=0; i<v.size(); i++)
  	{
  		add(false,"%u %u %u %u  ", v[i].a, v[i].b, v[i].c, v[i].d );
  		if ( ((i+1)&3) == 0 )
  		{
  			add(false,"\r\n");
  			indent(false,mDepth+2);
  			lastReturn = true;
  			returnCount++;
  		}
  		else
  			lastReturn = false;
  	}
  	if ( !lastReturn && returnCount != 0)
  	{
  		add(false,"\r\n");
    	indent(false,mDepth+1);
    }
  	add(false,"</%s>\r\n", name );
  }

	void add(bool header,const char *fmt,...)
	{
		char wbuff[2048];
		
		va_list ap;
		va_start(ap, fmt);
		vsprintf(wbuff, fmt, ap);
		va_end(ap);
		
		char *scan = wbuff;
		while ( *scan )
		{
			if ( header )
  			mHeader.push_back(*scan);
  		else
  			mBody.push_back(*scan);
			scan++;
		}
	}

	void indent(bool header,int depth)
	{
		for (int i=0; i<depth; i++)
		{
			add(header,"  ");
		}
	}

	SchemaStream   *mFph;
  const char     *mName;
  NxArray< char > mHeader;
  NxArray< char > mBody;
  int             mDepth;
	bool            mFlushed;
};

SchemaStream::SchemaStream(const char *fname,
                           bool isBinary,
                           const char *spec,    // "wb" "rb" or "wmem"
                           void *mem,
                           int len)
{
  mCurrentCollection = 0;
  mCurrentScene = 0;
  mCurrentActor = 0;
  mCurrentJoint = 0;
  mCurrentShape = 0;
  mCurrentCloth = 0;
  mCurrentFluid = 0;

	mBinary = isBinary;
 	mCurrentBlock = 0;
 	mStackPtr = 0;
  mFph = 0;
  mSchemaXML = 0;
  mReadMode = false;
  mIsValid = false;
  mHeaderStackPtr = 0;
  mFlipEndian = false;


  if ( strcasecmp(spec,"rb") == 0  )
  {
    mReadMode = true;
    if ( isBinary )
    {
    	mFph = nxu_fopen(fname,spec,mem,len);
      if ( mFph )
      {
        char stream[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        NxU32 sdk = 0;
        NxU32 uv = 0;
        NxU8  loadEndian=0;
    		read(stream,10);
    		read(loadEndian);
				NxU8 pe = (NxU8) gProcessorBigEndian;

    		if ( loadEndian != pe )
    			mFlipEndian = true;
    		else
    			mFlipEndian = false;

    		read(sdk);
    		read(uv);
        bool ok = true;
        if ( strcmp(stream,"NXUSTREAM") != 0 )
        {
          ok = false;
          reportError("File %s is not a valid binary NxuStream file, missing header", fname );
        }
        if ( ok && sdk != NX_SDK_VERSION_NUMBER )
        {
          ok = false;
          reportError("This binary file %s was saved out with a different version of the SDK. Saved with SDK%d but running SDK%d", fname, sdk, NX_SDK_VERSION_NUMBER );
        }
        if ( ok && uv != NXUSTREAM_VERSION )
        {
          ok = false;
          reportError("Even though the file %s was saved with the same SDK version, the version format has changed from %d to %d",fname,uv,NXUSTREAM_VERSION);
        }

        if ( ok )
        {
          mIsValid = true;
					if ( !beginHeader(SC_NXUSTREAM2) )
					{
						invalidate("Did not encoutner the expected header");
					}
        }
				else
        {
          invalidate("Invalid NxuStream file header");
        }
      }
      else
      {
        reportError("Failed to open binary file '%s' for read access.", fname );
      }

    }
    else
    {
    	mSchemaXML = new SchemaXML(fname,mem,len,this);
    	if ( !mSchemaXML->mDocument )
    	{
    		delete mSchemaXML;
    		mSchemaXML = 0;
    	}
    	else if ( mSchemaXML->mIsValid )
    	{
			  mIsValid = true;
    	}
    	else
    	{
    		reportError("Valid XML file but does not appear to contain any NXUSTREAM data.");
    		delete mSchemaXML;
    		mSchemaXML = 0;
    	}
    }
  }
  else
  {
  	mFph = nxu_fopen(fname,spec,mem,len);
    if ( mFph )
    {
    	if ( mBinary )
    	{
        mIsValid = true;
    		NxU32 sdk = NX_SDK_VERSION_NUMBER;
    		NxU32 uv  = NXUSTREAM_VERSION;
    		NxU8 endian = gSaveBigEndian;

    		if ( gSaveBigEndian != gProcessorBigEndian )
    			mFlipEndian = true;
    		else
    			mFlipEndian = false;

    		write("NXUSTREAM",10);
    		write(endian);
    		write(sdk);
    		write(uv);

    	}
    	else
    	{
        mIsValid = true;
        //printf("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
      }
      beginHeader(SC_NXUSTREAM2);
    }
    else
    {
      invalidate("Failed to open file '%s' for write access", fname );
    }
  }
}

void SchemaStream::endFlush(void)
{
  if ( mReadMode )
  {
  }
  else
  {
  	endHeader();

    if ( mBinary )
    {
      beginHeader(SC_LAST); // pad the EOF with a null header.
      endHeader();
    }

  	for (NxU32 i=0; i<mBlocks.size(); i++)
  	{
  		SchemaBlock *sb = mBlocks[i];
  		delete sb;
  	}
  	mBlocks.clear();
  	if ( mFph )
  	{
  		if ( mBinary )
  		{
  			closeLast();
  		}
  	}
  }
}

SchemaStream::~SchemaStream(void)
{
	endFlush();

	if ( mFph )
	{
  	nxu_fclose(mFph);
	}
	delete mSchemaXML;
}


void SchemaStream::closeLast(void)
{
	if ( mStackPtr )
	{
		mStackPtr--;
		size_t start = mStack[mStackPtr];
		size_t end   = (NxU32)ftell();
		size_t len   = (NxU32)(end-start);
		nxu_fseek(mFph,(long) start+sizeof(NxU32)+sizeof(NxI32),SEEK_SET);
		nxu_fwrite(&len,sizeof(NxU32),1,mFph);
		nxu_fseek(mFph,(long) end,SEEK_SET);
	}
}

void SchemaStream::pushLast(size_t loc)
{
	assert( mStackPtr < 32 );
	if ( mStackPtr < 32 )
	{
		mStack[mStackPtr] = loc;
		mStackPtr++;
	}
}

bool SchemaStream::beginHeader(NxI32 headerId,const char *parent)
{
  bool ok = true;

  if ( isValid() )
  {
    if ( mReadMode )
    {
      if ( mBinary )
      {
        size_t loc = (NxU32)ftell();

        read(mCurrentHeader.mId1);
        read(mCurrentHeader.mType);
        read(mCurrentHeader.mLen);
        read(mCurrentHeader.mId2);

        if ( isValid() && mCurrentHeader.isValid() )
        {
          if ( mHeaderStackPtr < 32 )
          {
            assert( mCurrentHeader.mType == headerId );
            if ( mCurrentHeader.mType == headerId )
            {
              mHeaderStack[mHeaderStackPtr] = mCurrentHeader;
              mHeaderStackPtr++;
            }
            else
            {
              ok = false; // not the header we expected
              nxu_fseek(mFph,(long) loc,SEEK_SET); // go back to the original location
              assert(mHeaderStackPtr);
              if ( mHeaderStackPtr )
                mCurrentHeader = mHeaderStack[mHeaderStackPtr];
            }
          }
          else
          {
            invalidate("Header stack overflow.");
          }
        }
        else
        {
          invalidate("Encountered invalid header ID, file is corrupted");
        }
      }
      else if ( mSchemaXML )
      {
     		ok = mSchemaXML->beginHeader(headerId,parent);
      }
    }
    else
    {
    	if ( mBinary )
    	{
    		NxU32 loc = (NxU32)ftell();
    		pushLast(loc);
        SchemaHeader h;
        h.mType =headerId;
        write(h.mId1);
        write(h.mType);
        write(h.mLen);
        write(h.mId2);
    	}
    	else
    	{
      	if ( mCurrentBlock )
      		mCurrentBlock->flush(); // flush the parent.. not attributes after this though!
        const char *str = EnumToString( (SCHEMA_CLASS) headerId ); // convert it into a string for XML layout
        str+=3;
        assert(str);
      	mCurrentBlock = new SchemaBlock(str,this,mBlocks.size()+1);
      	mBlocks.push_back(mCurrentBlock);
      }
    }
  }

  if ( !isValid() ) ok = false;

	if (!ok)
	{
#if DEBUG_WARNING
		reportWarning("Failed to begin header(%-30s) (%s)", EnumToString( (SCHEMA_CLASS)headerId), parent );
#endif
	}

	return ok;
}


void SchemaStream::endHeader(void)
{
  if ( isValid() )
  {
    if ( mReadMode )
    {
      if ( mBinary )
      {
        if ( mHeaderStackPtr )
        {
          mHeaderStackPtr--;
          mCurrentHeader = mHeaderStack[mHeaderStackPtr];
        }
      }
      else if ( mSchemaXML )
      {
     		mSchemaXML->endHeader();
      }
    }
    else
    {
    	if ( mBinary )
    	{
    		closeLast();
    	}
    	else
    	{
      	if ( mBlocks.size() )
      	{
          SchemaBlock* sb = mBlocks.back();
          delete sb;
          mBlocks.popBack();
      	}
      	if ( mBlocks.size() )
      		mCurrentBlock = mBlocks.back();
      	else
      		mCurrentBlock = 0;
    	}
    }
  }
}

void SchemaStream::writeMem(const void *mem,NxU32 size)
{
  if ( mFph && size )
  {
    size_t count = nxu_fwrite(mem,size,1,mFph);
    if ( count == 0 )
    {
      invalidate("Write failure saving NxuStream data");
    }
  }
}

void SchemaStream::readMem(void *mem,NxU32 size)
{
  assert( size );
  if ( mFph && size)
  {
    size_t count = nxu_fread(mem,size,1,mFph);
    if ( count == 0 )
    {
      invalidate("Read failure loading NxuStream file.");
    }
  }
}

void SchemaStream::store(NX_BOOL v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
  	if ( mCurrentBlock )
	  	mCurrentBlock->store(v,name,attribute);
    else if ( mBinary )
      write(v);
  }
}

void SchemaStream::store(NxU32 v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
  	if ( mCurrentBlock )
  		mCurrentBlock->store(v,name,attribute);
    else if ( mBinary )
      write(v);
  }
}

void SchemaStream::store(NxF32 v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
  	if ( mCurrentBlock )
  		mCurrentBlock->store(v,name,attribute);
    else if ( mBinary )
      write(v);
  }
}

void SchemaStream::store(const char *v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
  	if ( mCurrentBlock )
  		mCurrentBlock->store(v,name,attribute);
    else if ( mBinary )
    {
      NxU32 len = 0;
  		if ( v )
  		{
  			len = (NxU32)strlen(v);
        assert( len < 1024 );
        if ( len >= 1024 )
        {
          len = 0;
        }
  		}

      write(len);

  		if ( v && len )
  		{
        write(v,len+1);
  		}
    }
  }
}

void SchemaStream::store(NxArray< NxF32 > &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
  	if ( mCurrentBlock )
  		mCurrentBlock->store(v,name,attribute);
    else if ( mBinary )
    {
      NxU32 count = v.size();
      write(count);
      if ( count )
      {
        write(&v[0], count );
      }
    }
  }
}

void SchemaStream::store(NxArray< NxU32 > &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
  	if ( mCurrentBlock )
  		mCurrentBlock->store(v,name,attribute);
    else if ( mBinary )
    {
      NxU32 count = v.size();
      write(count);
      if ( count )
      {
        write(&v[0], count );
      }
    }
  }
}

void SchemaStream::store(NxArray< NxU8 > &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
  	if ( mCurrentBlock )
  		mCurrentBlock->store(v,name,attribute);
    else if ( mBinary )
    {
      NxU32 count = v.size();
      write(count);
      if ( count )
      {
        write(&v[0], count );
      }
    }
  }
}

void SchemaStream::store(NxArray< NxU16 > &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
  	if ( mCurrentBlock )
  		mCurrentBlock->store(v,name,attribute);
    else if ( mBinary )
    {
      NxU32 count = v.size();
      write(count);
      if ( count )
      {
        write(&v[0], count );
      }
    }
  }
}

void SchemaStream::store(NxVec3 &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
  	if ( mCurrentBlock )
  		mCurrentBlock->store(v,name,attribute);
    else if ( mBinary )
      write(v);
  }
}

void SchemaStream::store(NxQuat &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
  	if ( mCurrentBlock )
  		mCurrentBlock->store(v,name,attribute);
    else if ( mBinary )
    {
      NxF32 q[4];
      v.getXYZW(q);
      write(q,4);
    }
  }

}

void SchemaStream::store(NxMat34 &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
  	if ( mCurrentBlock )
  		mCurrentBlock->store(v,name,attribute);
    else if ( mBinary )
    {
      NxF32 matrix[16];
      v.getRowMajor44(matrix);
      write(matrix,16);
    }
  }
}

void SchemaStream::store(NxArray< NxVec3 > &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
  	if ( mCurrentBlock )
  		mCurrentBlock->store(v,name,attribute);
    else if ( mBinary )
    {
      NxU32 count = v.size();
      write(count);
      if ( count )
      {
        write(&v[0], count );
      }
    }
  }
}

void SchemaStream::store(NxArray< NxTri > &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
  	if ( mCurrentBlock )
  		mCurrentBlock->store(v,name,attribute);
    else if ( mBinary )
    {
      NxU32 count = v.size();
      write(count);
      if ( count )
      {
        write(&v[0], count );
      }
    }
  }
}

void SchemaStream::store(NxBounds3 &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
  	if ( mCurrentBlock )
  		mCurrentBlock->store(v,name,attribute);
    else if ( mBinary )
    {
		write(v.min.x);
		write(v.max.x);
		write(v.min.y);
		write(v.max.y);
		write(v.min.z);
		write(v.max.z);
    }
  }
}

void SchemaStream::store(NxPlane &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
  	if ( mCurrentBlock )
  		mCurrentBlock->store(v,name,attribute);
    else if ( mBinary )
	{
		write(v.normal);
		write(v.d);
	}
  }
}


void * SchemaStream::getMemBuffer(size_t &outputLength)
{
	void * ret = 0;
	outputLength = 0;
	if ( mFph )
	{
		endFlush(); // flush out the end of the data.
		ret = nxu_getMemBuffer(mFph,outputLength);
	}
	return ret;
}

void SchemaStream::myprintf(const char *fmt,...)
{
	char wbuff[2048];

	va_list ap;
	va_start(ap, fmt);
	vsprintf(wbuff, fmt, ap);
	va_end(ap);
		
  NxU32 len = (NxU32)strlen(wbuff);
  if ( len )
  {
    write(wbuff,len);
  }
}

void SchemaStream::store(NxU16 v,const char *name,bool attribute)
{
  name = getElement(name);
	if ( mCurrentBlock )
		mCurrentBlock->store(v,name,attribute);
  else
    write(v);
}


void SchemaStream::load(NX_BOOL &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
    if ( mBinary )
    {
      read(v);
    }
    else if ( mSchemaXML )
    {
      mSchemaXML->load(v,name,attribute);
    }
  }
}

void SchemaStream::load(NxU16 &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
    if ( mBinary )
    {
      read(v);
    }
    else if ( mSchemaXML )
    {
      mSchemaXML->load(v,name,attribute);
    }
  }
}

void SchemaStream::load(NxU32 &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
    if ( mBinary )
    {
      read(v);
    }
    else if ( mSchemaXML )
    {
      mSchemaXML->load(v,name,attribute);
    }
  }
}

void SchemaStream::load(NxF32 &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() && mBinary )
  {
    read(v);
  }
  else if ( mSchemaXML )
  {
    mSchemaXML->load(v,name,attribute);
  }
}

void SchemaStream::load(const char *&v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
    if ( mBinary )
    {
      v = 0;
      NxU32 len = 0;
      read(len);
      assert(len<1024);
      if ( len >= 1024 ) len = 0;
      if ( len )
      {
        char scratch[1024];
        memset(scratch,0,len+1);
        read(scratch,len+1);
        v = getGlobalString(scratch);
      }
    }
    else if ( mSchemaXML )
    {
      mSchemaXML->load(v,name,attribute);
    }
  }
}

void SchemaStream::load(NxArray< NxF32 > &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
    if ( mBinary )
    {
      NxU32 len = 0;
      read(len);
      if ( len && isValid() )
      {
      	v.resize(len);
      	read(&v[0],len);
        if ( !isValid() )
        {
          v.clear();
        }
      }
    }
    else if ( mSchemaXML )
    {
      mSchemaXML->load(v,name,attribute);
    }
  }
}

void SchemaStream::load(NxArray< NxU32 > &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
    if ( mBinary )
    {
      NxU32 len = 0;
      read(len);
      if ( len && isValid() )
      {
      	v.resize(len);
      	read(&v[0],len);
        if ( !isValid() )
        {
          v.clear();
        }
      }
    }
    else if ( mSchemaXML )
    {
      mSchemaXML->load(v,name,attribute);
    }
  }
}

void SchemaStream::load(NxArray< NxU16 > &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
    if ( mBinary )
    {
      NxU32 len = 0;
      read(len);
      if ( len && isValid() )
      {
      	v.resize(len);
      	read(&v[0], len);
        if ( !isValid() )
        {
          v.clear();
        }
      }
    }
    else if ( mSchemaXML )
    {
      mSchemaXML->load(v,name,attribute);
    }
  }
}

void SchemaStream::load(NxArray< NxU8 > &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
    if ( mBinary )
    {
      NxU32 len = 0;
      read(len);
      if ( len && isValid() )
      {
      	v.resize(len);
      	read(&v[0], len );
        if ( !isValid() )
        {
          v.clear();
        }
      }
    }
    else if ( mSchemaXML )
    {
      mSchemaXML->load(v,name,attribute);
    }

  }
}

void SchemaStream::load(NxVec3 &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
    if ( mBinary )
    {
      read(v);
    }
    else if ( mSchemaXML )
    {
      mSchemaXML->load(v,name,attribute);
    }
  }
}

void SchemaStream::load(NxQuat &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
    if ( mBinary )
    {
      NxF32 quat[4];
      read(quat,4);
      if ( isValid() )
      {
        v.setXYZW(quat);
      }
    }
    else if ( mSchemaXML )
    {
      mSchemaXML->load(v,name,attribute);
    }
  }
}

void SchemaStream::load(NxMat34 &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
    if ( mBinary )
    {
      NxF32 matrix[16];
      read(matrix,16);
      if ( isValid() )
      {
        v.setRowMajor44(matrix);
      }
    }
    else if ( mSchemaXML )
    {
      mSchemaXML->load(v,name,attribute);
    }
  }
}

void SchemaStream::load(NxArray< NxVec3 > &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
    if ( mBinary )
    {
      NxU32 len = 0;
      read(len);
      if ( len && isValid() )
      {
      	v.resize(len);
      	read(&v[0],len);
        if ( !isValid() )
        {
          v.clear();
        }
      }
    }
    else if ( mSchemaXML )
    {
      mSchemaXML->load(v,name,attribute);
    }

  }
}

void SchemaStream::load(NxArray< NxTri > &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
    if ( mBinary )
    {
      NxU32 len = 0;
      read(len);
      if ( len && isValid() )
      {
      	v.resize(len);
      	read(&v[0],len);
        if ( !isValid() )
        {
          v.clear();
        }
      }
    }
    else if ( mSchemaXML )
    {
      mSchemaXML->load(v,name,attribute);
    }
  }
}

void SchemaStream::load(NxBounds3 &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
    if ( mBinary )
    {
		read(v.min.x);
		read(v.max.x);
		read(v.min.y);
		read(v.max.y);
		read(v.min.z);
		read(v.max.z);
    }
    else if ( mSchemaXML )
    {
      mSchemaXML->load(v,name,attribute);
    }
  }
}

void SchemaStream::load(NxPlane &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
    if ( mBinary )
    {
		read(v.normal);
		read(v.d);
    }
    else if ( mSchemaXML )
    {
      mSchemaXML->load(v,name,attribute);
    }
  }
}


size_t SchemaStream::ftell(void)
{
  size_t ret = 0;

  if ( mFph )
    ret = nxu_ftell(mFph);

  return ret;
}

bool SchemaStream::peekHeader(NxI32 headerId,
									NxI32 *peek,
                  NxI32 or1,
                  NxI32 or2,
                  NxI32 or3,
                  NxI32 or4,
                  NxI32 or5,
                  NxI32 or6,
                  NxI32 or7,
                  NxI32 or8,
                  NxI32 or9,
                  NxI32 or10)

{
  bool ret = false;

  if ( isValid() )
  {
		if ( mBinary )
		{
			size_t loc = (NxU32)ftell();
			SchemaHeader h;
			peekRead(h.mId1);
			peekRead(h.mType);
			peekRead(h.mLen);
			peekRead(h.mId2);

			if ( isValid() && h.isValid() )
			{

				if ( h.mType == headerId ) ret = true;
 				else if ( h.mType == or1 ) ret = true;
				else if ( h.mType == or2 ) ret = true;
				else if ( h.mType == or3 ) ret = true;
				else if ( h.mType == or4 ) ret = true;
				else if ( h.mType == or5 ) ret = true;
				else if ( h.mType == or6 ) ret = true;
				else if ( h.mType == or7 ) ret = true;
				else if ( h.mType == or8 ) ret = true;
				else if ( h.mType == or9 ) ret = true;
				else if ( h.mType == or10 ) ret = true;
				if ( ret && peek ) *peek = h.mType;
			}
			if ( mFph )
			  nxu_fseek(mFph,(long) loc,SEEK_SET);
	  }
		else if ( mSchemaXML )
		{
			NxI32 pno = mSchemaXML->peekHeader(headerId,or1,or2,or3,or4,or5,or6,or7,or8,or9,or10);

			if ( pno >= 0 )
			{
				if ( peek ) *peek = pno;
				ret = true;
			}

		}
	}
	return ret;
}


bool SchemaHeader::isValid(void)
{
  bool ret = false;

  if ( mId1 == HEADER1 && mId2 == HEADER2 && mType >= 0 && mType < SC_LAST )
  {
    ret = true;
  }
	return ret;
}


void SchemaStream::invalidate(const char *fmt,...)
{
  mIsValid = false;
  if ( mFph )
  {
    nxu_fclose(mFph);
    mFph = 0;
  }
  char wbuff[2048];

	va_list ap;
	va_start(ap, fmt);
	vsprintf(wbuff, fmt, ap);
	va_end(ap);

  reportError("%s", wbuff );
}

void SchemaStream::setCurrentCollection(NxuPhysicsCollection *c)
{
  mCurrentCollection = c;
}

void SchemaStream::setCurrentScene(NxSceneDesc *s)
{
  mCurrentScene = s;
}

void SchemaStream::setCurrentJoint(NxJointDesc *j) { mCurrentJoint = j; };
void SchemaStream::setCurrentActor(NxActorDesc *a) { mCurrentActor = a; };
void SchemaStream::setCurrentShape(NxShapeDesc *s) { mCurrentShape = s; };
void SchemaStream::setCurrentFluid(NxFluidDesc *f) { mCurrentFluid = f; };
void SchemaStream::setCurrentCloth(NxClothDesc *c) { mCurrentCloth = c; };


NxSceneDesc        *locateSceneDesc(NxuPhysicsCollection *c,const char *name,NxU32 &index)
{
	NxSceneDesc *ret = 0;
	if ( name )
	{
		for (unsigned int i=0; i<c->mScenes.size(); i++)
		{
			NxSceneDesc *s = c->mScenes[i];
			if ( s->mId &&  strcmp(s->mId,name) == 0 )
			{
				index = i;
				ret = s;
				break;
			}
		}
	}
	return ret;
}

NxConvexMeshDesc   *locateConvexMeshDesc(NxuPhysicsCollection *c,const char *name)
{
	NxConvexMeshDesc *ret = 0;
	for (unsigned int i=0; i<c->mConvexMeshes.size(); i++)
	{
		NxConvexMeshDesc *s = c->mConvexMeshes[i];
		if ( s->mId &&  strcmp(s->mId,name) == 0 )
		{
			ret = s;
			break;
		}
	}
	return ret;
}

NxTriangleMeshDesc * locateTriangleMeshDesc(NxuPhysicsCollection *c,const char *name)
{
	NxTriangleMeshDesc *ret = 0;
	for (unsigned int i=0; i<c->mTriangleMeshes.size(); i++)
	{
		NxTriangleMeshDesc *s = c->mTriangleMeshes[i];
		if ( s->mId &&  strcmp(s->mId,name) == 0 )
		{
			ret = s;
			break;
		}
	}
	return ret;
}

NxHeightFieldDesc  *locateHeightFieldDesc(NxuPhysicsCollection *c,const char *name)
{
	NxHeightFieldDesc *ret = 0;

	for (unsigned int i=0; i<c->mHeightFields.size(); i++)
	{
		NxHeightFieldDesc *s = c->mHeightFields[i];
		if ( s->mId && strcmp(s->mId,name) == 0 )
		{
			ret = s;
			break;
		}
	}
	return ret;
}

NxActorDesc        *locateActorDesc(NxSceneDesc *s,const char *name)
{
	NxActorDesc *ret = 0;
	if ( s && name )
	{
		for (unsigned int i=0; i<s->mActors.size(); i++)
		{
			NxActorDesc *a = s->mActors[i];
			if ( a->mId && strcmp(a->mId, name ) == 0 )
			{
				ret = a;
				break;
			}
		}
	}
	return ret;
}

void SchemaStream::store(NxArray< NxTetra > &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
  	if ( mCurrentBlock )
  		mCurrentBlock->store(v,name,attribute);
    else if ( mBinary )
    {
      NxU32 count = v.size();
      write(count);
      if ( count )
      {
        write(&v[0], count );
      }
    }
  }
}

void SchemaStream::load(NxArray< NxTetra > &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
    if ( mBinary )
    {
      NxU32 len = 0;
      read(len);
      if ( len && isValid() )
      {
      	v.resize(len);
      	read(&v[0],len);
        if ( !isValid() )
        {
          v.clear();
        }
      }
    }
    else if ( mSchemaXML )
    {
      mSchemaXML->load(v,name,attribute);
    }
  }
}

//**********************
void SchemaStream::write(NxU8  data)
{
	writeMem(&data,sizeof(NxU8));
}

void SchemaStream::write(NxU16 data)
{
	if ( mFlipEndian )
	{
		const NxU8 *source = (const NxU8 *)&data;
		write(source[1]);
		write(source[0]);
	}
	else
	{
		writeMem(&data,sizeof(NxU16));
	}
}

void SchemaStream::write(NxU32 data)
{
	if ( mFlipEndian )
	{
		const NxU8 *source = (const NxU8 *)&data;
		write(source[3]);
		write(source[2]);
		write(source[1]);
		write(source[0]);
	}
	else
	{
		writeMem(&data,sizeof(NxU32));
	}
}

void SchemaStream::write(NxI32 data)
{
	if ( mFlipEndian )
	{
		const NxU8 *source = (const NxU8 *)&data;
		write(source[3]);
		write(source[2]);
		write(source[1]);
		write(source[0]);
	}
	else
	{
		writeMem(&data,sizeof(NxI32));
	}
}

void SchemaStream::write(NxF32 data)
{
	if ( mFlipEndian )
	{
		const NxU8 *source = (const NxU8 *)&data;
		write(source[3]);
		write(source[2]);
		write(source[1]);
		write(source[0]);
	}
	else
	{
		writeMem(&data,sizeof(NxF32));
	}
}


void SchemaStream::write(const NxVec3 &data)
{
	if ( mFlipEndian )
	{
		write(data.x);
		write(data.y);
		write(data.z);
	}
	else
	{
		writeMem(&data,sizeof(NxVec3));
	}
}

void SchemaStream::write(const NxF32 *data,NxU32 count)
{
	if ( mFlipEndian )
	{
		for (NxU32 i=0; i<count; i++)
		{
			write(data[i]);
		}
	}
	else
	{
		writeMem(data,sizeof(NxF32)*count);
	}

}

void SchemaStream::write(const NxU32 *data,NxU32 count)
{
	if ( mFlipEndian )
	{
		for (NxU32 i=0; i<count; i++)
		{
			write(data[i]);
		}
	}
	else
	{
		writeMem(data,sizeof(NxU32)*count);
	}
}

void SchemaStream::write(const NxU16 *data,NxU32 count)
{
	if ( mFlipEndian )
	{
		for (NxU32 i=0; i<count; i++)
		{
			write(data[i]);
		}
	}
	else
	{
		writeMem(data,sizeof(NxU16)*count);
	}
}

void SchemaStream::write(const NxVec3 *data,NxU32 count)
{
	if ( mFlipEndian )
	{
		for (NxU32 i=0; i<count; i++)
		{
			write(data[i]);
		}
	}
	else
	{
		writeMem(data,sizeof(NxVec3)*count);
	}
}

void SchemaStream::write(const char *mem,NxU32 size)
{
	writeMem(mem,size);
}

void SchemaStream::write(const NxU8 *mem,NxU32 count)
{
	writeMem(mem,count);
}

void SchemaStream::write(const NxTri *tris,NxU32 count)
{
	if ( mFlipEndian )
	{
		for (NxU32 i=0; i<count; i++)
		{
			const NxTri &t = tris[i];
			write(t.a);
			write(t.b);
			write(t.c);
		}
	}
	else
	{
		writeMem(tris,sizeof(NxTri)*count);
	}
}

void SchemaStream::write(const NxTetra *data,NxU32 count)
{
	if ( mFlipEndian )
	{
		for (NxU32 i=0; i<count; i++)
		{
			const NxTetra &t = data[i];
			write(t.a);
			write(t.b);
			write(t.c);
			write(t.d);
		}
	}
	else
	{
		writeMem(data,sizeof(NxTetra)*count);
	}
}


void SchemaStream::read(char *mem,NxU32 size)
{
	readMem(mem,size);
}

void SchemaStream::read(NxU8 &data)
{
	readMem(&data,1);
}

void SchemaStream::read(NxU16 &data)
{
	if ( mFlipEndian )
	{
		NxU8 *dest = (NxU8 *)&data;
		read(dest[1]);
		read(dest[0]);
	}
	else
	{
		readMem(&data,sizeof(NxU16));
	}
}

void SchemaStream::read(NxU32 &data)
{
	if ( mFlipEndian )
	{
		NxU8 *dest = (NxU8 *)&data;
		read(dest[3]);
		read(dest[2]);
		read(dest[1]);
		read(dest[0]);
	}
	else
	{
		readMem(&data,sizeof(NxU32));
	}
}

void SchemaStream::read(NxI32 &data)
{
	if ( mFlipEndian )
	{
		NxU8 *dest = (NxU8 *)&data;
		read(dest[3]);
		read(dest[2]);
		read(dest[1]);
		read(dest[0]);
	}
	else
	{
		readMem(&data,sizeof(NxI32));
	}
}

void SchemaStream::read(NxF32 &data)
{
	if ( mFlipEndian )
	{
		NxU8 *dest = (NxU8 *)&data;
		read(dest[3]);
		read(dest[2]);
		read(dest[1]);
		read(dest[0]);
	}
	else
	{
		readMem(&data,sizeof(NxF32));
	}
}

void SchemaStream::read(NxVec3 &v)
{
	if ( mFlipEndian )
	{
		read(v.x);
		read(v.y);
		read(v.z);
	}
	else
	{
		readMem(&v,sizeof(NxVec3));
	}
}

void SchemaStream::read(NxTri &data)
{
	if ( mFlipEndian )
	{
		read(data.a);
		read(data.b);
		read(data.c);
	}
	else
	{
		readMem(&data,sizeof(NxTri));
	}
}

void SchemaStream::read(NxTetra &data)
{
	if ( mFlipEndian )
	{
		read(data.a);
		read(data.b);
		read(data.c);
		read(data.d);
	}
	else
	{
		readMem(&data,sizeof(NxTetra));
	}
}

void SchemaStream::read(NxF32 *data,NxU32 count)
{
	if ( mFlipEndian )
	{
		for (NxU32 i=0; i<count; i++)
		{
			read(data[i]);
		}
	}
	else
	{
		readMem(data,sizeof(NxF32)*count);
	}
}

void SchemaStream::store(NxMat33 &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
  	if ( mCurrentBlock )
  		mCurrentBlock->store(v,name,attribute);
    else if ( mBinary )
    {
      NxF32 matrix[9];
      v.getRowMajor(matrix);
      write(matrix,9);
    }
  }
}



void SchemaStream::load(NxMat33 &v,const char *name,bool attribute)
{
  name = getElement(name);
  if ( isValid() )
  {
    if ( mBinary )
    {
      NxF32 matrix[9];
      read(matrix,9);
      if ( isValid() )
      {
        v.setRowMajor(matrix);
      }
    }
    else if ( mSchemaXML )
    {
      mSchemaXML->load(v,name,attribute);
    }
  }
}


void SchemaStream::read(NxU32 *data,NxU32 count)
{
	if ( mFlipEndian )
	{
		for (NxU32 i=0; i<count; i++)
		{
			read(data[i]);
		}
	}
	else
	{
		readMem(data,sizeof(NxU32)*count);
	}
}

void SchemaStream::read(NxU16 *data,NxU32 count)
{
	if ( mFlipEndian )
	{
		for (NxU32 i=0; i<count; i++)
		{
			read(data[i]);
		}
	}
	else
	{
		readMem(data,sizeof(NxU16)*count);
	}
}

void SchemaStream::read(NxVec3 *data,NxU32 count)
{
	if ( mFlipEndian )
	{
		for (NxU32 i=0; i<count; i++)
		{
			read(data[i]);
		}
	}
	else
	{
		readMem(data,sizeof(NxVec3)*count);
	}
}


void SchemaStream::read(NxU8 *mem,NxU32 count)
{
	readMem(mem,count);
}

void SchemaStream::read(NxTri *tris,NxU32 count)
{
	if ( mFlipEndian )
	{
		for (NxU32 i=0; i<count; i++)
		{
			read(tris[i]);
		}
	}
	else
	{
		readMem(tris,sizeof(NxTri)*count);
	}
}

void SchemaStream::read(NxTetra *data,NxU32 count)
{
	if ( mFlipEndian )
	{
		for (NxU32 i=0; i<count; i++)
		{
			read(data[i]);
		}
	}
	else
	{
		readMem(data,count*sizeof(NxTetra));
	}
}


void SchemaStream::peekReadMem(void *mem,NxU32 size)
{
  assert( size );
  if ( mFph && size)
  {
    size_t count = nxu_fread(mem,size,1,mFph);
    if ( count == 0 )
    {
      invalidate("Read failure loading NxuStream file.");
    }
  }

}

void SchemaStream::peekRead(NxU32 &v)
{
	if ( mFlipEndian )
	{
		NxU8 *dest = (NxU8 *)&v;
		peekReadMem(&dest[3],1);
		peekReadMem(&dest[2],1);
		peekReadMem(&dest[1],1);
		peekReadMem(&dest[0],1);
	}
	else
	{
		peekReadMem(&v,sizeof(NxU32));
	}
}

void SchemaStream::peekRead(NxI32 &v)
{
	if ( mFlipEndian )
	{
		NxU8 *dest = (NxU8 *)&v;
		peekReadMem(&dest[3],1);
		peekReadMem(&dest[2],1);
		peekReadMem(&dest[1],1);
		peekReadMem(&dest[0],1);
	}
	else
	{
		peekReadMem(&v,sizeof(NxU32));
	}
}

};
