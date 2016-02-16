#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <assert.h>

#include "SoftMeshObj.h"
#include "TetraGraphics.h"
#include <NxArray.h>
#include <NxVec3.h>

//********************************************
//**** Inline source code to the InPlaceParser
//********************************************
#define MAXARGS 512

namespace SOFTBODY
{

class TextParserInterface
{
public:
	virtual int ParseLine(int lineno,int argc,const char **argv) =0;  // return TRUE to continue parsing, return FALSE to abort parsing process
};

enum TetSeparatorType
{
	ST_DATA,        // is data
	ST_HARD,        // is a hard separator
	ST_SOFT,        // is a soft separator
	ST_EOS          // is a comment symbol, and everything past this character should be ignored
};

class TextParser
{
public:
	TextParser(void)
	{
		Init();
	}

	TextParser(char *data,int len)
	{
		Init();
		SetSourceData(data,len);
	}

	TextParser(const char *fname)
	{
		Init();
		SetFile(fname);
	}

	~TextParser(void)
  {
	 if ( mMyAlloc )
   	{
   		free(mData);
   	}
  }

	void Init(void)
	{
		mQuoteChar = 34;
		mData = 0;
		mLen  = 0;
		mMyAlloc = false;
		for (int i=0; i<256; i++)
		{
			mHard[i] = ST_DATA;
			mHardString[i*2] = i;
			mHardString[i*2+1] = 0;
		}
		mHard[0]  = ST_EOS;
		mHard[32] = ST_SOFT;
		mHard[9]  = ST_SOFT;
		mHard[13] = ST_SOFT;
		mHard[10] = ST_SOFT;
	}

	bool SetFile(const char *fname) // use this file as source data to parse.
  {
    bool ret = false;

  	if ( mMyAlloc )
  	{
  		free(mData);
  	}
  	mData = 0;
  	mLen  = 0;
  	mMyAlloc = false;

    FILE *fph = fopen(fname,"rb");
  	if ( fph )
  	{
      fseek(fph,0L,SEEK_END);
      mLen = ftell(fph);
      fseek(fph,0L,SEEK_SET);

  		if ( mLen )
  		{
  			mData = (char *) malloc(sizeof(char)*(mLen+1));
  			int ok = fread(mData, mLen, 1, fph );
  			if ( !ok )
  			{
  				free(mData);
  				mData = 0;
  			}
  			else
  			{
  				mData[mLen] = 0; // zero byte terminate end of file marker.
  				mMyAlloc = true;
          ret = true;
  			}
  		}
      fclose(fph);
  	}
    return ret;
  }

	void SetSourceData(char *data,int len)
	{
		mData = data;
		mLen  = len;
		mMyAlloc = false;
	};

	int  Parse(TextParserInterface *callback) // returns true if entire file was parsed, false if it aborted for some reason
  {
  	int ret = 0;
  	assert( callback );
  	if ( mData )
  	{
  		int lineno = 0;

  		char *foo   = mData;
  		char *begin = foo;

  		while ( *foo )
  		{
  			if ( *foo == 10 || *foo == 13 )
  			{
  				++lineno;
  				*foo = 0;

  				if ( *begin ) // if there is any data to parse at all...
  				{
  					int v = ProcessLine(lineno,begin,callback);
  					if ( v )
  						ret = v;
  				}

  				++foo;
  				if ( *foo == 10 )
  					++foo; // skip line feed, if it is in the carraige-return line-feed format...
  				begin = foo;
  			}
  			else
  			{
  				++foo;
  			}
  		}

  		lineno++; // lasst line.

  		int v = ProcessLine(lineno,begin,callback);
  		if ( v )
  			ret = v;
  	}
  	return ret;
  }

	int ProcessLine(int lineno,char *line,TextParserInterface *callback)
  {
  	int ret = 0;

  	const char *argv[MAXARGS];
  	int argc = 0;

  	char *foo = line;

  	while ( !EOS(*foo) && argc < MAXARGS )
  	{
  		foo = SkipSpaces(foo); // skip any leading spaces

  		if ( EOS(*foo) )
  			break;

  		if ( *foo == mQuoteChar ) // if it is an open quote
  		{
  			++foo;
  			if ( argc < MAXARGS )
  			{
  				argv[argc++] = foo;
  			}
  			while ( !EOS(*foo) && *foo != mQuoteChar )
  				++foo;
  			if ( !EOS(*foo) )
  			{
  				*foo = 0; // replace close quote with zero byte EOS
  				++foo;
  			}
  		}
  		else
  		{
  			foo = AddHard(argc,argv,foo); // add any hard separators, skip any spaces

  			if ( IsNonSeparator(*foo) )  // add non-hard argument.
  			{
  				bool quote  = false;
  				if ( *foo == mQuoteChar )
  				{
  					++foo;
  					quote = true;
  				}

  				if ( argc < MAXARGS )
  				{
  					argv[argc++] = foo;
  				}

  				if ( quote )
  				{
  					while (*foo && *foo != mQuoteChar )
  						++foo;
  					if ( *foo )
  						*foo = 32;
  				}

  				// continue..until we hit an eos ..
  				while ( !EOS(*foo) ) // until we hit EOS
  				{
  					if ( IsWhiteSpace(*foo) ) // if we hit a space, stomp a zero byte, and exit
  					{
  						*foo = 0;
  						++foo;
  						break;
  					}
  					else if ( IsHard(*foo) ) // if we hit a hard separator, stomp a zero byte and store the hard separator argument
  					{
  						const char *hard = &mHardString[*foo*2];
  						*foo = 0;
  						if ( argc < MAXARGS )
  						{
  							argv[argc++] = hard;
  						}
  						++foo;
  						break;
  					}
  					++foo;
  				} // end of while loop...
  			}
  		}
  	}

  	if ( argc )
  	{
  		ret = callback->ParseLine(lineno, argc, argv );
  	}

  	return ret;
  }

	const char ** GetArglist(char *line,int &count) // convert source string into an arg list, this is a destructive parse.
  {
  	const char **ret = 0;

  	static const char *argv[MAXARGS];
  	int argc = 0;

  	char *foo = line;

  	while ( !EOS(*foo) && argc < MAXARGS )
  	{
  		foo = SkipSpaces(foo); // skip any leading spaces

  		if ( EOS(*foo) )
  			break;

  		if ( *foo == mQuoteChar ) // if it is an open quote
  		{
  			++foo;
  			if ( argc < MAXARGS )
  			{
  				argv[argc++] = foo;
  			}
  			while ( !EOS(*foo) && *foo != mQuoteChar )
  				++foo;
  			if ( !EOS(*foo) )
  			{
  				*foo = 0; // replace close quote with zero byte EOS
  				++foo;
  			}
  		}
  		else
  		{
  			foo = AddHard(argc,argv,foo); // add any hard separators, skip any spaces

  			if ( IsNonSeparator(*foo) )  // add non-hard argument.
  			{
  				bool quote  = false;
  				if ( *foo == mQuoteChar )
  				{
  					++foo;
  					quote = true;
  				}

  				if ( argc < MAXARGS )
  				{
  					argv[argc++] = foo;
  				}

  				if ( quote )
  				{
  					while (*foo && *foo != mQuoteChar )
  						++foo;
  					if ( *foo )
  						*foo = 32;
  				}

  				// continue..until we hit an eos ..
  				while ( !EOS(*foo) ) // until we hit EOS
  				{
  					if ( IsWhiteSpace(*foo) ) // if we hit a space, stomp a zero byte, and exit
  					{
  						*foo = 0;
  						++foo;
  						break;
  					}
  					else if ( IsHard(*foo) ) // if we hit a hard separator, stomp a zero byte and store the hard separator argument
  					{
  						const char *hard = &mHardString[*foo*2];
  						*foo = 0;
  						if ( argc < MAXARGS )
  						{
  							argv[argc++] = hard;
  						}
  						++foo;
  						break;
  					}
  					++foo;
  				} // end of while loop...
  			}
  		}
  	}

  	count = argc;
  	if ( argc )
  	{
  		ret = argv;
  	}

  	return ret;
  }

	void SetHardSeparator(char c) // add a hard separator
	{
		mHard[c] = ST_HARD;
	}

	void SetHard(char c) // add a hard separator
	{
		mHard[c] = ST_HARD;
	}


	void SetCommentSymbol(char c) // comment character, treated as 'end of string'
	{
		mHard[c] = ST_EOS;
	}

	void ClearHardSeparator(char c)
	{
		mHard[c] = ST_DATA;
	}


	void DefaultSymbols(void) // set up default symbols for hard seperator and comment symbol of the '#' character.
  {
  	SetHardSeparator(',');
  	SetHardSeparator('(');
  	SetHardSeparator(')');
  	SetHardSeparator('=');
  	SetHardSeparator('[');
  	SetHardSeparator(']');
  	SetHardSeparator('{');
  	SetHardSeparator('}');
  	SetCommentSymbol('#');
  }

	bool EOS(char c)
	{
		if ( mHard[c] == ST_EOS )
		{
			return true;
		}
		return false;
	}

	void SetQuoteChar(char c)
	{
		mQuoteChar = c;
	}

	bool HasData( void ) const
	{
		return ( mData != 0 );
	}

private:

	inline char * AddHard(int &argc,const char **argv,char *foo)
  {
  	while ( IsHard(*foo) )
  	{
  		const char *hard = &mHardString[*foo*2];
  		if ( argc < MAXARGS )
  		{
  			argv[argc++] = hard;
  		}
  		++foo;
  	}
	  return foo;
  }

	inline bool   IsHard(char c)
  {
  	return mHard[c] == ST_HARD;
  }

	inline char * SkipSpaces(char *foo)
  {
  	while ( !EOS(*foo) && IsWhiteSpace(*foo) )
  		++foo;
  	return foo;
  }

	inline bool   IsWhiteSpace(char c)
  {
  	return mHard[c] == ST_SOFT;
  }

	inline bool   IsNonSeparator(char c) // non seperator,neither hard nor soft
  {
  	return ( !IsHard(c) && !IsWhiteSpace(c) && c != 0 );
  }

	bool   mMyAlloc; // whether or not *I* allocated the buffer and am responsible for deleting it.
	char  *mData;  // ascii data to parse.
	int    mLen;   // length of data
	TetSeparatorType  mHard[256];
	char   mHardString[256*2];
	char           mQuoteChar;
};

class TetraHelper : public TextParserInterface
{
public:

  TetraHelper(SoftMeshInterface *mface)
  {
  	mMeshInterface = mface;
    mProcessMaterials = false;
    mCurrentMaterial = 0;
  }

  ~TetraHelper(void)
  {
  	release();
  }

	void release(void)
	{
		mPositions.clear();
		mIndices.clear();
  	for (NxU32 i=0; i<mMaterials.size(); i++)
  	{
  		TetraMaterial *tm = mMaterials[i];
  		delete tm;
  	}
		mMaterials.clear();
	}

  bool loadObj(const char *fname)
  {
    bool ret = false;


    TextParser ipp;
    ipp.DefaultSymbols();

		if ( gFileInterface )
			fname = gFileInterface->getSoftFileName(fname,true);

    if ( ipp.SetFile(fname) )
    {
      mProcessMaterials = 0;
			mCurrentMaterial = 0;
      mProcessMaterials = false;
      ipp.Parse(this);
    }

		release();

    return ret;
  }

  bool loadTet(const char *fname)
  {
    bool ret = false;

		if ( gFileInterface )
			fname = gFileInterface->getSoftFileName(fname,true);


    TextParser ipp;
    ipp.DefaultSymbols();
    if ( ipp.SetFile(fname) )
    {
      ipp.Parse(this);
      ret = true;
    }

		release();

    return ret;
  }

	int ParseLine(int lineno,int argc,const char **argv)  // return TRUE to continue parsing, return FALSE to abort parsing process
  {
    if ( mProcessMaterials )
    {
			const char *foo = argv[0];
      if ( stricmp(foo,"newmtl") == 0 )
      {
        mCurrentMaterial = new TetraMaterial(argv[1]);
        mMaterials.push_back(mCurrentMaterial);
      }
      else if ( mCurrentMaterial && stricmp(foo,"map_kd") == 0 && argc >= 2 )
      {
        strncpy(mCurrentMaterial->mTexture,argv[1],512);
      }
    }
    else if ( stricmp(argv[0],"v") == 0 && argc == 4 )
    {
      NxVec3 v;
      v.x = (float) atof( argv[1] );
      v.y = (float) atof( argv[2] );
      v.z = (float) atof( argv[3] );
      mPositions.push_back(v);
	  mMeshInterface->softMeshPosition(&v);
    }
    else if ( stricmp(argv[0],"vn") == 0 && argc == 4 )
    {
      NxVec3 v;
      v.x = (float) atof( argv[1] );
      v.y = (float) atof( argv[2] );
      v.z = (float) atof( argv[3] );
      mNormals.push_back(v);
    }
    else if ( stricmp(argv[0],"t") == 0 && argc == 5 )
    {
      NxU32 i1 = atoi( argv[1] );
      NxU32 i2 = atoi( argv[2] );
      NxU32 i3 = atoi( argv[3] );
      NxU32 i4 = atoi( argv[4] );

      assert( i1 >= 0 && i1 < mPositions.size() );
      assert( i2 >= 0 && i2 < mPositions.size() );
      assert( i3 >= 0 && i3 < mPositions.size() );
      assert( i4 >= 0 && i4 < mPositions.size() );

	  mMeshInterface->softMeshTetrahedron( &mPositions[i1].x, &mPositions[i2].x, &mPositions[i3].x, &mPositions[i4].x );
	  mMeshInterface->softMeshIndices(&i1, &i2, &i3, &i4);
	}
    else if ( strcmp(argv[0],"mtllib") == 0 && argc >= 2 )
    {
      // ok, load this material library...
      const char *fname = argv[1];
      mProcessMaterials = true;
	  	if ( gFileInterface )
				fname = gFileInterface->getSoftFileName(fname,true); // get the full path name
      TextParser ipp(fname);
      ipp.Parse(this);
      mProcessMaterials = false;
    }
    else if ( strcmp(argv[0],"usemtl") == 0 && argc>= 2 )
    {
			TetraMaterial *mat = locateMaterial(argv[1]);
			if ( mat )
			{
				mMeshInterface->softMeshMaterial(*mat);
      }
    }
    else if ( stricmp(argv[0],"vt") == 0 && argc == 3 )
    {
      NxVec3 v;
      v.x = (float) atof( argv[1] );
      v.y = (float) atof( argv[2] );
      v.z = 0;
      mTexels.push_back(v);
    }
    else if ( stricmp(argv[0],"vt") == 0 && argc == 4 )
    {
      NxVec3 v;
      v.x = (float) atof( argv[1] );
      v.y = (float) atof( argv[2] );
      v.z = (float) atof( argv[3] );
      mTexels.push_back(v);
    }
    else if ( stricmp(argv[0],"f") == 0 && argc >= 4 )
    {
      TetraGraphicsVertex v[32];
      int vcount = argc-1;
      for (int i=1; i<argc; i++)
      {
        getVertex(v[i-1],argv[i]);
      }
      mMeshInterface->softMeshTriangle(	v[0], v[1], v[2] );
      if ( vcount >=3 ) // do the fan
      {
        for (int i=2; i<(vcount-1); i++)
        {
          mMeshInterface->softMeshTriangle(v[0],v[i],v[i+1]);
        }
      }
    }
		return 0;
  }

  void getVertex(TetraGraphicsVertex &v,const char *face) const
  {
    v.mPos[0] = 0;
    v.mPos[1] = 0;
    v.mPos[2] = 0;

    v.mNormal[0] = 0;
    v.mNormal[1] = 0;
    v.mNormal[2] = 0;

    v.mTexel[0] = 0;
    v.mTexel[1] = 0;

    int index = atoi( face )-1;

    if ( index < 0 )
      index = mPositions.size()+index+1;

    if ( index >= 0 && index < (int)mPositions.size() )
    {
			v.mPos[0] = mPositions[index].x;
			v.mPos[1] = mPositions[index].y;
			v.mPos[2] = mPositions[index].z;
    }


    const char *texel = strchr(face,'/');

    if ( texel )
    {
      int tindex = atoi( texel+1) - 1;
      if ( tindex < 0 )
        tindex = mTexels.size()+tindex+1;

      if ( tindex >=0 && tindex < (int)mTexels.size() )
      {
        v.mTexel[0] = mTexels[tindex].x;
        v.mTexel[1] = mTexels[tindex].y;
      }

      const char *normal = strchr(texel+1,'/');

      if ( normal )
      {
        int index = atoi( face )-1;
        if ( index < 0 )
          index = mNormals.size()+index+1;

        if ( index >= 0 && index < (int)mNormals.size() )
        {
    			v.mNormal[0] = mNormals[index].x;
    			v.mNormal[1] = mNormals[index].y;
    			v.mNormal[2] = mNormals[index].z;
        }

      }
    }
  }

	TetraMaterial * locateMaterial(const char *str) const
	{
		TetraMaterial *ret = 0;
		for (NxU32 i=0; i<mMaterials.size(); i++)
		{
			TetraMaterial *tm = mMaterials[i];
			if ( strcmp(str,tm->mName) == 0 )
			{
				ret = tm;
				break;
			}
		}
		return ret;
	}

private:

  NxArray< NxVec3 >          mPositions;
  NxArray< NxVec3 >          mNormals;
  NxArray< NxVec3 >          mTexels;
public:
  NxArray< NxU32 >           mIndices;
private:
  NxArray< TetraMaterial *>  mMaterials;
  TetraMaterial             *mCurrentMaterial;

  bool                       mProcessMaterials;
  SoftMeshInterface          *mMeshInterface;

};


bool loadSoftMeshObj(const char *oname,const char *tname,SoftMeshInterface *smi)
{
	bool ret = false;

	TetraHelper th(smi);

  if ( oname )
  	ret = th.loadObj(oname);
  if ( tname )
  	th.loadTet(tname);
 
  return ret;
}

//NxArray<NxU32> loadSoftMeshObjRet(const char *oname,const char *tname,SoftMeshInterface *smi)
//{
//	bool ret = false;
//
//	TetraHelper th(smi);
//
//  if ( oname )
//  	ret = th.loadObj(oname);
//  if ( tname )
//  	th.loadTet(tname);
//  
//  return th.mIndices;
//}

}; // END OF SOFTBODY NAMESPACE
