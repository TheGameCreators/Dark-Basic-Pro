
//
// DBOBlock Functions Implementation
//

//////////////////////////////////////////////////////////////////////////////////
// DBOBLOCK HEADER ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#define _CRT_SECURE_NO_DEPRECATE
#include "DBOBlock.h"

//////////////////////////////////////////////////////////////////////////////////
// GLOBAL DATA ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

bool			g_bWriteAsText				= false;
LPSTR			g_pBlockEnd					= NULL;
DWORD			g_dwVersion					= 0;
LPSTR			g_pBlockStart				= NULL;

bool ScanFrame      ( sFrame* pFrame, LPSTR* ppBlock, DWORD* pdwSize );
bool ConstructFrame ( sFrame** ppFrame, LPSTR* ppBlock );

//////////////////////////////////////////////////////////////////////////////////
// INTERNAL WRITE FUNCTIONS //////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK_DLL bool WriteCR ( LPSTR* ppBlock, DWORD* pdwSize )
{
	if ( g_bWriteAsText )
	{
		if ( ppBlock )
		{
			*(BYTE*)*ppBlock = 13;
			*ppBlock += 1;
			*(BYTE*)*ppBlock = 10;
			*ppBlock += 1;
		}
		*pdwSize += 2;
	}
	return true;
}

DARKSDK_DLL bool WriteComma ( LPSTR* ppBlock, DWORD* pdwSize )
{
	char pCommaStr[2];
	strcpy(pCommaStr,",");
	DWORD dwLen = strlen(pCommaStr);
	if ( ppBlock )
	{
		memcpy ( *ppBlock, pCommaStr, dwLen );
		*ppBlock += dwLen;
	}
	*pdwSize += dwLen;
	return true;
}

DARKSDK_DLL bool WriteSpeechmark ( LPSTR* ppBlock, DWORD* pdwSize )
{
	char pMarkStr[2];
	pMarkStr[0]='"';
	pMarkStr[1]=0;
	DWORD dwLen = strlen(pMarkStr);
	if ( ppBlock )
	{
		memcpy ( *ppBlock, pMarkStr, dwLen );
		*ppBlock += dwLen;
	}
	*pdwSize += dwLen;
	return true;
}

DARKSDK_DLL bool WriteBYTE ( BYTE bNumber, LPSTR* ppBlock, DWORD* pdwSize )
{
	DWORD dwLen = 1;
	if ( g_bWriteAsText )
	{
		// make text value
		char pNumberStr[256];
		_itoa(bNumber, pNumberStr, 10);
		dwLen = strlen(pNumberStr);
		if ( ppBlock )
		{
			memcpy ( *ppBlock, pNumberStr, dwLen );
			*ppBlock += dwLen;
		}
		*pdwSize += dwLen;

		// make text comma
		WriteComma ( ppBlock, pdwSize );
	}
	else
	{
		if ( ppBlock )
		{
			*(BYTE*)*ppBlock = bNumber;
			*ppBlock += dwLen;
		}
		*pdwSize += dwLen;
	}
	return true;
}

DARKSDK_DLL bool WriteDWORD ( DWORD dwNumber, LPSTR* ppBlock, DWORD* pdwSize )
{
	DWORD dwLen = 4;
	if ( g_bWriteAsText )
	{
		// make text value
		char pNumberStr[256];
		_itoa(dwNumber, pNumberStr, 10);
		dwLen = strlen(pNumberStr);
		if ( ppBlock )
		{
			memcpy ( *ppBlock, pNumberStr, dwLen );
			*ppBlock += dwLen;
		}
		*pdwSize += dwLen;

		// make text comma
		WriteComma ( ppBlock, pdwSize );
	}
	else
	{
		if ( ppBlock )
		{
			*(DWORD*)*ppBlock = dwNumber;
			*ppBlock += dwLen;
		}
		*pdwSize += dwLen;
	}
	return true;
}

DARKSDK_DLL bool Write2BYTE ( DWORD dwNumber1, BYTE bNumber2, LPSTR* ppBlock, DWORD* pdwSize )
{
	WriteDWORD ( dwNumber1, ppBlock, pdwSize );
	WriteBYTE ( bNumber2, ppBlock, pdwSize );
	return true;
}

DARKSDK_DLL bool Write2DWORD ( DWORD dwNumber1, DWORD dwNumber2, LPSTR* ppBlock, DWORD* pdwSize )
{
	WriteDWORD ( dwNumber1, ppBlock, pdwSize );
	WriteDWORD ( dwNumber2, ppBlock, pdwSize );
	return true;
}

DARKSDK_DLL bool WriteCODE ( DWORD dwCode, DWORD dwCodeSize, LPSTR* ppBlock, DWORD* pdwSize )
{
	WriteDWORD ( dwCode, ppBlock, pdwSize );
	WriteDWORD ( dwCodeSize, ppBlock, pdwSize );
	return true;
}

DARKSDK_DLL bool WriteVECTOR ( D3DXVECTOR3 vecVector, LPSTR* ppBlock, DWORD* pdwSize )
{
	DWORD dwLen = sizeof(vecVector);
	if ( g_bWriteAsText )
	{
		for ( int x=0; x<3; x++)
		{
			// get matrix element
			char pNumberStr[256];
			sprintf ( pNumberStr, "%f", *(((float*)&vecVector)+x) );

			// make text value
			dwLen = strlen(pNumberStr);
			if ( ppBlock )
			{
				memcpy ( *ppBlock, pNumberStr, dwLen );
				*ppBlock += dwLen;
			}
			*pdwSize += dwLen;

			// make text comma
			WriteComma ( ppBlock, pdwSize );
		}
	}
	else
	{
		if ( ppBlock )
		{
			memcpy ( *ppBlock, &vecVector, dwLen );
			*ppBlock += dwLen;
		}
		*pdwSize += dwLen;
	}
	return true;
}

DARKSDK_DLL bool WriteMATRIX ( D3DXMATRIX mMatrix, LPSTR* ppBlock, DWORD* pdwSize )
{
	DWORD dwLen = sizeof(mMatrix);
	if ( g_bWriteAsText )
	{
		for ( int y=0; y<4; y++)
		{
			for ( int x=0; x<4; x++)
			{
				// get matrix element
				char pNumberStr[256];
				sprintf ( pNumberStr, "%f", mMatrix(x,y) );

				// make text value
				dwLen = strlen(pNumberStr);
				if ( ppBlock )
				{
					memcpy ( *ppBlock, pNumberStr, dwLen );
					*ppBlock += dwLen;
				}
				*pdwSize += dwLen;

				// make text comma
				WriteComma ( ppBlock, pdwSize );
			}
		}
	}
	else
	{
		if ( ppBlock )
		{
			memcpy ( *ppBlock, &mMatrix, dwLen );
			*ppBlock += dwLen;
		}
		*pdwSize += dwLen;
	}
	return true;
}

DARKSDK_DLL bool WriteMATERIAL ( D3DMATERIAL9 mMaterial, LPSTR* ppBlock, DWORD* pdwSize )
{
	DWORD dwLen = sizeof(mMaterial);
	if ( g_bWriteAsText )
	{
		DWORD dwBytes = sizeof(mMaterial);
		for ( DWORD i=0; i<dwBytes; i++)
		{
			// byte from material
			char pNumberStr[256];
			_itoa ( *((BYTE*)&mMaterial+i) , pNumberStr, 10);

			// make text value
			dwLen = strlen(pNumberStr);
			if ( ppBlock )
			{
				memcpy ( *ppBlock, pNumberStr, dwLen );
				*ppBlock += dwLen;
			}
			*pdwSize += dwLen;

			// make text comma
			WriteComma ( ppBlock, pdwSize );
		}
	}
	else
	{
		if ( ppBlock )
		{
			memcpy ( *ppBlock, &mMaterial, dwLen );
			*ppBlock += dwLen;
		}
		*pdwSize += dwLen;
	}
	return true;
}

DARKSDK_DLL bool WriteMEMORY ( BYTE* pData, DWORD dwBytes, LPSTR* ppBlock, DWORD* pdwSize )
{
	if ( g_bWriteAsText )
	{
		for ( DWORD i=0; i<dwBytes; i++)
		{
			// byte from material
			char pNumberStr[256];
			_itoa ( *(pData+i) , pNumberStr, 10);

			// make text value
			DWORD dwLen = strlen(pNumberStr);
			if ( ppBlock )
			{
				memcpy ( *ppBlock, pNumberStr, dwLen );
				*ppBlock += dwLen;
			}
			*pdwSize += dwLen;

			// make text comma
			WriteComma ( ppBlock, pdwSize );
		}
	}
	else
	{
		if ( ppBlock )
		{
			memcpy ( *ppBlock, pData, dwBytes );
			*ppBlock += dwBytes;
		}
		*pdwSize += dwBytes;
	}
	return true;
}

DARKSDK_DLL bool WriteString ( LPSTR pString, LPSTR* ppBlock, DWORD* pdwSize )
{
	DWORD dwLen = strlen(pString);
	if ( g_bWriteAsText )
	{
		// make text speechmark
		WriteSpeechmark ( ppBlock, pdwSize );

		// make text
		if ( ppBlock )
		{
			memcpy ( *ppBlock, pString, dwLen );
			*ppBlock += dwLen;
		}
		*pdwSize += dwLen;

		// make text speechmark
		WriteSpeechmark ( ppBlock, pdwSize );
		WriteComma ( ppBlock, pdwSize );
	}
	else
	{
		if ( ppBlock )
		{
			// string size
			*(DWORD*)*ppBlock = dwLen;
			*ppBlock += 4;

			// string data
			memcpy ( *ppBlock, pString, dwLen );
			*ppBlock += dwLen;
		}
		*pdwSize += 4 + dwLen;
	}
	return true;
}

DARKSDK_DLL bool WriteVertexData ( BYTE* pVertexData, DWORD dwFVFSize, DWORD dwVertexCount, LPSTR* ppBlock, DWORD* pdwSize )
{
	// No vertex data
	if ( pVertexData==NULL )
	{
		WriteDWORD ( 0, ppBlock, pdwSize );
		return true;
	}

	DWORD dwLen = dwFVFSize * dwVertexCount;
	if ( g_bWriteAsText )
	{
		DWORD dwBytes = dwVertexCount*dwFVFSize;
		for ( DWORD i=0; i<dwBytes; i++)
		{
			// byte from data
			char pNumberStr[256];
			_itoa ( *(pVertexData+i) , pNumberStr, 10);

			// make text value
			dwLen = strlen(pNumberStr);
			if ( ppBlock )
			{
				memcpy ( *ppBlock, pNumberStr, dwLen );
				*ppBlock += dwLen;
			}
			*pdwSize += dwLen;

			// make text comma
			WriteComma ( ppBlock, pdwSize );
		}
	}
	else
	{
		if ( ppBlock )
		{
			memcpy ( *ppBlock, pVertexData, dwLen );
			*ppBlock += dwLen;
		}
		*pdwSize += dwLen;
	}

	// okay
	return true;
}

DARKSDK_DLL bool WriteIndices ( WORD* pIndices, DWORD dwIndexCount, LPSTR* ppBlock, DWORD* pdwSize )
{
	// No indices
	if ( pIndices==NULL )
	{
		WriteDWORD ( 0, ppBlock, pdwSize );
		return true;
	}
	
	DWORD dwLen = dwIndexCount * sizeof(WORD);
	if ( g_bWriteAsText )
	{
		for ( DWORD i=0; i<dwIndexCount; i++)
		{
			// byte from data
			char pNumberStr[256];
			_itoa ( *((WORD*)pIndices+i) , pNumberStr, 10);

			// make text value
			dwLen = strlen(pNumberStr);
			if ( ppBlock )
			{
				memcpy ( *ppBlock, pNumberStr, dwLen );
				*ppBlock += dwLen;
			}
			*pdwSize += dwLen;

			// make text comma
			WriteComma ( ppBlock, pdwSize );
		}
	}
	else
	{
		if ( ppBlock )
		{
			memcpy ( *ppBlock, pIndices, dwLen );
			*ppBlock += dwLen;
		}
		*pdwSize += dwLen;
	}

	// okay
	return true;
}

DARKSDK_DLL bool WriteOffsetList ( int* pIndices, DWORD dwIndexCount, LPSTR* ppBlock, DWORD* pdwSize )
{
	// No indices
	if ( pIndices==NULL )
	{
		WriteDWORD ( 0, ppBlock, pdwSize );
		return true;
	}
	
	DWORD dwLen = dwIndexCount * sizeof(int);
	if ( g_bWriteAsText )
	{
		for ( DWORD i=0; i<dwIndexCount; i++)
		{
			// byte from data
			char pNumberStr[256];
			_itoa ( *((WORD*)pIndices+i) , pNumberStr, 10);

			// make text value
			dwLen = strlen(pNumberStr);
			if ( ppBlock )
			{
				memcpy ( *ppBlock, pNumberStr, dwLen );
				*ppBlock += dwLen;
			}
			*pdwSize += dwLen;

			// make text comma
			WriteComma ( ppBlock, pdwSize );
		}
	}
	else
	{
		if ( ppBlock )
		{
			memcpy ( *ppBlock, pIndices, dwLen );
			*ppBlock += dwLen;
		}
		*pdwSize += dwLen;
	}

	// okay
	return true;
}


//////////////////////////////////////////////////////////////////////////////////
// INTERNAL READ FUNCTIONS ///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK_DLL bool ReadBOOL ( bool* ppDest, LPSTR* ppBlock )
{
	// length of string
	if ( *(BYTE*)*ppBlock==0 )
		*(ppDest) = false;
	else
		*(ppDest) = true;

	*ppBlock += 1;

	// okay
	return true;
}

DARKSDK_DLL bool ReadDWORD ( DWORD* ppDest, LPSTR* ppBlock )
{
	// length of string
	*(ppDest) = *(DWORD*)*ppBlock;
	*ppBlock += 4;

	// okay
	return true;
}

DARKSDK_DLL bool ReadCODE ( DWORD* pdwCode, DWORD* pdwCodeSize, LPSTR* ppBlock )
{
	// code and codesize
	*(pdwCode) = *(DWORD*)*ppBlock;
	*ppBlock += 4;
	*(pdwCodeSize) = *(DWORD*)*ppBlock;
	*ppBlock += 4;

	// okay
	return true;
}

DARKSDK_DLL bool ReadVector ( D3DXVECTOR3* pVector, LPSTR* ppBlock )
{
	DWORD dwLength = sizeof(D3DXVECTOR3);
	memcpy ( pVector, *ppBlock, dwLength );
	*ppBlock += dwLength;
	return true;
}

DARKSDK_DLL bool ReadMatrix ( D3DXMATRIX* pMatrix, LPSTR* ppBlock )
{
	DWORD dwLength = sizeof(D3DXMATRIX);
	memcpy ( pMatrix, *ppBlock, dwLength );
	*ppBlock += dwLength;
	return true;
}

DARKSDK_DLL bool ReadMaterial ( D3DMATERIAL9* pMaterial, LPSTR* ppBlock )
{
	DWORD dwLength = sizeof(D3DMATERIAL9);
	memcpy ( pMaterial, *ppBlock, dwLength );
	*ppBlock += dwLength;
	return true;
}

DARKSDK_DLL bool ReadMemory ( BYTE** ppData, DWORD dwBytes, LPSTR* ppBlock )
{
	if ( (*ppData)==NULL ) (*ppData) = (BYTE*)new char [ dwBytes ];
	memcpy ( (*ppData), *ppBlock, dwBytes );
	*ppBlock += dwBytes;
	return true;
}

DARKSDK_DLL bool ReadIntoMemory ( BYTE* pData, DWORD dwBytes, LPSTR* ppBlock )
{
	memcpy ( pData, *ppBlock, dwBytes );
	*ppBlock += dwBytes;
	return true;
}

DARKSDK_DLL bool ReadString ( LPSTR pString, LPSTR* ppBlock )
{
	// length of string
	DWORD dwFullLength = *(DWORD*)*ppBlock;
	*ppBlock += 4;

	// limit string size
	DWORD dwLength = dwFullLength;
	if ( dwLength > 255 ) dwLength=255;
	if ( dwLength > 0 )
	{
		// create if not present
		if ( pString==NULL ) pString = new char[dwLength+1];

		// copy string
		memcpy ( pString, *ppBlock, dwLength );
		pString [ dwLength ] = 0;
		*ppBlock += dwFullLength;
	}

	// okay
	return true;
}

DARKSDK_DLL bool ReadVertexData ( BYTE** pbVertexData, DWORD dwFVFSize, DWORD dwVertexCount, LPSTR* ppBlock )
{
	DWORD dwLength = dwFVFSize * dwVertexCount;
	if ( dwLength > 0 )
	{
		(*pbVertexData) = (BYTE*)new char [ dwLength ];
		memcpy ( (*pbVertexData), *ppBlock, dwLength );
		*ppBlock += dwLength;
	}
	return true;
}

DARKSDK_DLL bool ReadIndexData ( WORD** pwIndexData, DWORD dwIndexCount, LPSTR* ppBlock )
{
	DWORD dwLength = sizeof(WORD) * dwIndexCount;
	if ( dwLength > 0 )
	{
		(*pwIndexData) = (WORD*)new char [ dwLength ];
		memcpy ( (*pwIndexData), *ppBlock, dwLength );
		*ppBlock += dwLength;
	}
	return true;
}

DARKSDK_DLL bool ReadOffsetListData ( int** pwIndexData, DWORD dwIndexCount, LPSTR* ppBlock )
{
	DWORD dwLength = sizeof(int) * dwIndexCount;
	if ( dwLength > 0 )
	{
		(*pwIndexData) = (int*)new char [ dwLength ];
		memcpy ( (*pwIndexData), *ppBlock, dwLength );
		*ppBlock += dwLength;
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////////////
// INTERNAL SCAN FUNCTIONS ///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK_DLL bool ScanBones ( sBone* pBoneArray, DWORD dwBoneCount, LPSTR* ppBlock, DWORD* pdwSize )
{
	// No bones
	if ( pBoneArray )
	{
		// for each texture in the array
		for ( DWORD bi=0; bi<dwBoneCount; bi++ )
		{
			// bone in question
			sBone* pBone = &(pBoneArray [ bi ]);

			// bone name
			LPSTR pCodeSizePtr = NULL;
			if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
			WriteCODE ( DBOBLOCK_BONES_NAME, 0, ppBlock, pdwSize );
			WriteString ( pBone->szName, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );
			if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;

			// bone num influences
			WriteCODE ( DBOBLOCK_BONES_NUMINFLUENCES, 4, ppBlock, pdwSize );
			WriteDWORD ( pBone->dwNumInfluences, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );

			// bone vertices
			DWORD dwMemorySize = sizeof(DWORD) * pBone->dwNumInfluences;
			if ( dwMemorySize > 0 )
			{
				WriteCODE	( DBOBLOCK_BONES_VERTICES, dwMemorySize, ppBlock, pdwSize );
				WriteMEMORY ( (BYTE*)pBone->pVertices, dwMemorySize, ppBlock, pdwSize );
			}
			WriteCR ( ppBlock, pdwSize );

			// bone weights
			dwMemorySize = sizeof(float) * pBone->dwNumInfluences;
			if ( dwMemorySize > 0 )
			{
				WriteCODE	( DBOBLOCK_BONES_WEIGHTS, dwMemorySize, ppBlock, pdwSize );
				WriteMEMORY ( (BYTE*)pBone->pWeights, dwMemorySize, ppBlock, pdwSize );
			}
			WriteCR ( ppBlock, pdwSize );

			// bone matrix translation
			pCodeSizePtr = NULL;
			if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
			WriteCODE ( DBOBLOCK_BONES_TRANSLATIONMATRIX, 0, ppBlock, pdwSize );
			WriteMATRIX ( pBone->matTranslation, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );
			if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;

			// end of this bone
			WriteCODE ( 0, 0, ppBlock, pdwSize );
		}
	}

	// zero code
	WriteCODE ( 0, 0, ppBlock, pdwSize );

	// okay
	return true;
}

DARKSDK_DLL bool ScanTextures ( sTexture* pTextureArray, DWORD dwTextureCount, LPSTR* ppBlock, DWORD* pdwSize )
{
	// No textures
	if ( pTextureArray )
	{
		// for each texture in the array
		for ( DWORD ti=0; ti<dwTextureCount; ti++ )
		{
			// texture in question
			sTexture* pTexture = &(pTextureArray [ ti ]);

			// texture name
			LPSTR pCodeSizePtr = NULL;
			if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
			WriteCODE ( DBOBLOCK_TEXTURE_NAME, 0, ppBlock, pdwSize );
			WriteString ( pTexture->pName, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );
			if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;

			// texture settings
			WriteCODE ( DBOBLOCK_TEXTURE_STAGE, 4, ppBlock, pdwSize );
			WriteDWORD ( pTexture->dwStage, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );

			WriteCODE ( DBOBLOCK_TEXTURE_BMODE, 4, ppBlock, pdwSize );
			WriteDWORD ( pTexture->dwBlendMode, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );

			WriteCODE ( DBOBLOCK_TEXTURE_BARG1, 4, ppBlock, pdwSize );
			WriteDWORD ( pTexture->dwBlendArg1, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );

			WriteCODE ( DBOBLOCK_TEXTURE_BARG2, 4, ppBlock, pdwSize );
			WriteDWORD ( pTexture->dwBlendArg2, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );

			WriteCODE ( DBOBLOCK_TEXTURE_ADDRU, 4, ppBlock, pdwSize );
			WriteDWORD ( pTexture->dwAddressU, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );

			WriteCODE ( DBOBLOCK_TEXTURE_ADDRV, 4, ppBlock, pdwSize );
			WriteDWORD ( pTexture->dwAddressV, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );

			WriteCODE ( DBOBLOCK_TEXTURE_MAG, 4, ppBlock, pdwSize );
			WriteDWORD ( pTexture->dwMagState, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );

			WriteCODE ( DBOBLOCK_TEXTURE_MIN, 4, ppBlock, pdwSize );
			WriteDWORD ( pTexture->dwMinState, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );

			WriteCODE ( DBOBLOCK_TEXTURE_MIP, 4, ppBlock, pdwSize );
			WriteDWORD ( pTexture->dwMipState, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );

			WriteCODE ( DBOBLOCK_TEXTURE_TCMODE, 4, ppBlock, pdwSize );
			WriteDWORD ( pTexture->dwTexCoordMode, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );

			WriteCODE ( DBOBLOCK_TEXTURE_PRIMST, 4, ppBlock, pdwSize );
			WriteDWORD ( (DWORD) pTexture->iStartIndex, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );

			WriteCODE ( DBOBLOCK_TEXTURE_PRIMCN, 4, ppBlock, pdwSize );
			WriteDWORD ( (DWORD) pTexture->iPrimitiveCount, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );

			// end of this texture
			WriteCODE ( 0, 0, ppBlock, pdwSize );
		}
	}

	// zero code
	WriteCODE ( 0, 0, ppBlock, pdwSize );

	// okay
	return true;
}

DARKSDK_DLL bool ScanMultiMaterial ( sMultiMaterial* pMultiMaterialArray, DWORD dwMultiMatCount, LPSTR* ppBlock, DWORD* pdwSize )
{
	// No multimaterial array
	if ( pMultiMaterialArray )
	{
		// for each in the array
		for ( DWORD ti=0; ti<dwMultiMatCount; ti++ )
		{
			// multimaterial in question
			sMultiMaterial* pMultiMat = &(pMultiMaterialArray [ ti ]);

			// write multimaterial name
			LPSTR pCodeSizePtr = NULL;
			if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
			WriteCODE ( DBOBLOCK_MULTIMAT_NAME, 0, ppBlock, pdwSize );
			WriteString ( pMultiMat->pName, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );
			if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;

			// write multimaterial material information
			WriteCODE	( DBOBLOCK_MULTIMAT_MATERIAL, sizeof(pMultiMat->mMaterial), ppBlock, pdwSize );
			WriteMATERIAL ( pMultiMat->mMaterial,			ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );

			// write multimaterial start
			WriteCODE ( DBOBLOCK_MULTIMAT_START, 4, ppBlock, pdwSize );
			WriteDWORD ( pMultiMat->dwIndexStart, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );

			// write multimaterial end
			WriteCODE ( DBOBLOCK_MULTIMAT_COUNT, 4, ppBlock, pdwSize );
			WriteDWORD ( pMultiMat->dwIndexCount, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );

			// write multimaterial poly
			WriteCODE ( DBOBLOCK_MULTIMAT_POLY, 4, ppBlock, pdwSize );
			WriteDWORD ( pMultiMat->dwPolyCount, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );

			// end of this
			WriteCODE ( 0, 0, ppBlock, pdwSize );
		}
	}

	// zero code
	WriteCODE ( 0, 0, ppBlock, pdwSize );

	// okay
	return true;
}

DARKSDK_DLL bool ScanMesh ( sMesh* pMesh, LPSTR* ppBlock, DWORD* pdwSize )
{
	// No mesh
	if ( pMesh )
	{
		// Write vertex information
		WriteCODE ( DBOBLOCK_MESH_FVF, 4, ppBlock, pdwSize );
		WriteDWORD ( pMesh->dwFVF, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );

		WriteCODE ( DBOBLOCK_MESH_FVFSIZE, 4, ppBlock, pdwSize );
		WriteDWORD ( pMesh->dwFVFSize, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );

		WriteCODE ( DBOBLOCK_MESH_VERTEXCOUNT, 4, ppBlock, pdwSize );
		WriteDWORD ( pMesh->dwVertexCount, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );

		WriteCODE ( DBOBLOCK_MESH_INDEXCOUNT, 4, ppBlock, pdwSize );
		WriteDWORD ( pMesh->dwIndexCount, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );

		// If non-standard vertex declaration, write it
		if ( pMesh->dwFVF==0 )
		{
			WriteCODE ( DBOBLOCK_MESH_VERTEXDEC, 4, ppBlock, pdwSize );
			WriteMEMORY ( (BYTE*)pMesh->pVertexDeclaration, sizeof(pMesh->pVertexDeclaration), ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );
		}

		// MIKE 130503 - changed the way in which sub frames were saved
		WriteCODE ( DBOBLOCK_MESH_SUBFRAMES, 4, ppBlock, pdwSize );
		WriteDWORD ( pMesh->dwSubMeshListCount, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );
		for ( DWORD dwSubMesh = 0; dwSubMesh < pMesh->dwSubMeshListCount; dwSubMesh++ )
		{
			ScanFrame ( &pMesh->pSubFrameList [ dwSubMesh ], ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );
		}
		
		// Write vertex data
		WriteCODE	( DBOBLOCK_MESH_VERTEXDATA,	pMesh->dwFVFSize*pMesh->dwVertexCount, ppBlock, pdwSize );
		WriteVertexData ( pMesh->pVertexData, pMesh->dwFVFSize, pMesh->dwVertexCount, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );

		// Write index data
		if ( pMesh->dwIndexCount )
		{
			WriteCODE	( DBOBLOCK_MESH_INDEXDATA, pMesh->dwIndexCount*sizeof(WORD), ppBlock, pdwSize );
			WriteIndices ( pMesh->pIndices, pMesh->dwIndexCount, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );
		}

		// Write primitive type
// leefix - 060306 - huge size specified, when only 4 bytes are written
//		WriteCODE	( DBOBLOCK_MESH_PRIMTYPE, pMesh->dwIndexCount*sizeof(WORD), ppBlock, pdwSize );
		WriteCODE	( DBOBLOCK_MESH_PRIMTYPE, 4, ppBlock, pdwSize );
		WriteDWORD ( (DWORD)pMesh->iPrimitiveType, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );
		
		// Write draw vertex count
// leefix - 060306 - huge size specified, when only 4 bytes are written
//		WriteCODE	( DBOBLOCK_MESH_DRAWVERTCOUNT, pMesh->dwIndexCount*sizeof(WORD), ppBlock, pdwSize );
		WriteCODE	( DBOBLOCK_MESH_DRAWVERTCOUNT, 4, ppBlock, pdwSize );
		WriteDWORD ( pMesh->iDrawVertexCount, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );

		// Write draw primitive count
// leefix - 060306 - huge size specified, when only 4 bytes are written
//		WriteCODE	( DBOBLOCK_MESH_DRAWPRIMCOUNT, pMesh->dwIndexCount*sizeof(WORD), ppBlock, pdwSize );
		WriteCODE	( DBOBLOCK_MESH_DRAWPRIMCOUNT, 4, ppBlock, pdwSize );
		WriteDWORD ( pMesh->iDrawPrimitives, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );

		// Write bone information
		WriteCODE	( DBOBLOCK_MESH_BONECOUNT,	4,	ppBlock, pdwSize );
		WriteDWORD	( pMesh->dwBoneCount,			ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );

		LPSTR pCodeSizePtr = NULL;
		if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
		WriteCODE	( DBOBLOCK_MESH_BONESDATA,	0,	ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );
		ScanBones	( pMesh->pBones, pMesh->dwBoneCount, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );
		if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;

		// Write material information
		WriteCODE	( DBOBLOCK_MESH_USEMATERIAL, 1,	ppBlock, pdwSize );
		WriteBYTE	( pMesh->bUsesMaterial, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );
		WriteCODE	( DBOBLOCK_MESH_MATERIAL, sizeof(pMesh->mMaterial), ppBlock, pdwSize );
		WriteMATERIAL ( pMesh->mMaterial,			ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );

		// Write texture information
		WriteCODE	( DBOBLOCK_MESH_TEXTURECOUNT, 4,ppBlock, pdwSize );
		WriteDWORD	( pMesh->dwTextureCount,		ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );

		pCodeSizePtr = NULL;
		if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
		WriteCODE	( DBOBLOCK_MESH_TEXTURES, 0,	ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );
		ScanTextures ( pMesh->pTextures, pMesh->dwTextureCount, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );
		if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;

		// Write multi material information
		WriteCODE	( DBOBLOCK_MESH_USEMULTIMAT, 1,		ppBlock, pdwSize );
		WriteBYTE	( pMesh->bUseMultiMaterial,			ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );
		WriteCODE	( DBOBLOCK_MESH_MULTIMATCOUNT, 4,	ppBlock, pdwSize );
		WriteDWORD	( pMesh->dwMultiMaterialCount,		ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );
		pCodeSizePtr = NULL;
		if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
		WriteCODE	( DBOBLOCK_MESH_MULTIMAT, 0,		ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );
		ScanMultiMaterial ( pMesh->pMultiMaterial, pMesh->dwMultiMaterialCount, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );
		if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;

		// Write render states
		WriteCODE	( DBOBLOCK_MESH_WIREFRAME,	1,	ppBlock, pdwSize );
		WriteBYTE	( pMesh->bWireframe,			ppBlock, pdwSize );
		WriteCR	( ppBlock, pdwSize );
		WriteCODE	( DBOBLOCK_MESH_LIGHT,		1,	ppBlock, pdwSize );
		WriteBYTE	( pMesh->bLight,				ppBlock, pdwSize );
		WriteCR	( ppBlock, pdwSize );
		WriteCODE	( DBOBLOCK_MESH_CULL,		1,	ppBlock, pdwSize );
		WriteBYTE	( pMesh->bCull,					ppBlock, pdwSize );
		WriteCR	( ppBlock, pdwSize );
		WriteCODE	( DBOBLOCK_MESH_FOG,		1,	ppBlock, pdwSize );
		WriteBYTE	( pMesh->bFog,					ppBlock, pdwSize );
		WriteCR	( ppBlock, pdwSize );
		WriteCODE	( DBOBLOCK_MESH_AMBIENT,	1,	ppBlock, pdwSize );
		WriteBYTE	( pMesh->bAmbient,				ppBlock, pdwSize );
		WriteCR	( ppBlock, pdwSize );
		WriteCODE	( DBOBLOCK_MESH_TRANSPARENCY,1,	ppBlock, pdwSize );
		WriteBYTE	( pMesh->bTransparency,			ppBlock, pdwSize );
		WriteCR	( ppBlock, pdwSize );
		WriteCODE	( DBOBLOCK_MESH_GHOST,		1,	ppBlock, pdwSize );
		WriteBYTE	( pMesh->bGhost,				ppBlock, pdwSize );
		WriteCR	( ppBlock, pdwSize );
		WriteCODE	( DBOBLOCK_MESH_GHOSTMODE,	4,	ppBlock, pdwSize );
		WriteDWORD	( pMesh->iGhostMode,			ppBlock, pdwSize );
		WriteCR	( ppBlock, pdwSize );
		WriteCODE	( DBOBLOCK_MESH_VISIBLE,	1,	ppBlock, pdwSize );
		WriteBYTE	( pMesh->bVisible,				ppBlock, pdwSize );
		WriteCR	( ppBlock, pdwSize );
		WriteCODE	( DBOBLOCK_MESH_LINKED,		1,	ppBlock, pdwSize );
		WriteBYTE	( pMesh->bLinked,				ppBlock, pdwSize );
		WriteCR	( ppBlock, pdwSize );

		// 250704 - Write FX effect file name (if any)
		if ( pMesh->bUseVertexShader )
		{
			LPSTR pCodeSizePtr = NULL;
			if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
			WriteCODE ( DBOBLOCK_MESH_FXEFFECTNAME, 0, ppBlock, pdwSize );
			WriteString ( pMesh->pEffectName, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );
			if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;
		}

		// 190804 - Write dwArbitaryValue to meshdata
		WriteCODE	( DBOBLOCK_MESH_ARBITARYVALUE, 4,	ppBlock, pdwSize );
		WriteDWORD	( pMesh->Collision.dwArbitaryValue,	ppBlock, pdwSize );
		WriteCR		( ppBlock, pdwSize );

		// 230904 - Write ZBIAS info
		WriteCODE	( DBOBLOCK_MESH_ZBIASFLAG,	1,	ppBlock, pdwSize );
		WriteBYTE	( pMesh->bZBiasActive,				ppBlock, pdwSize );
		WriteCR	( ppBlock, pdwSize );
		DWORD dwDWORD = *(DWORD*)&pMesh->fZBiasSlopeScale;
		WriteCODE	( DBOBLOCK_MESH_ZBIASSLOPE, 4,	ppBlock, pdwSize );
		WriteDWORD	( dwDWORD,	ppBlock, pdwSize );
		WriteCR		( ppBlock, pdwSize );
		dwDWORD = *(DWORD*)&pMesh->fZBiasDepth;
		WriteCODE	( DBOBLOCK_MESH_ZBIASDEPTH, 4,	ppBlock, pdwSize );
		WriteDWORD	( dwDWORD,	ppBlock, pdwSize );
		WriteCR		( ppBlock, pdwSize );
		WriteCODE	( DBOBLOCK_MESH_ZREAD,	1,	ppBlock, pdwSize );
		WriteBYTE	( pMesh->bZRead,				ppBlock, pdwSize );
		WriteCR	( ppBlock, pdwSize );
		WriteCODE	( DBOBLOCK_MESH_ZWRITE,	1,	ppBlock, pdwSize );
		WriteBYTE	( pMesh->bZWrite,				ppBlock, pdwSize );
		WriteCR	( ppBlock, pdwSize );

		// 061204 - Needed to retain alpha-test-depth-draw-system(universe)
		WriteCODE	( DBOBLOCK_MESH_ALPHATESTVALUE, 4,	ppBlock, pdwSize );
		WriteDWORD	( pMesh->dwAlphaTestValue,	ppBlock, pdwSize );
		WriteCR		( ppBlock, pdwSize );
	}

	// No more mesh codes
	WriteCODE ( 0, 0, ppBlock, pdwSize );

	// okay
	return true;
}

DARKSDK_DLL bool ScanFrame ( sFrame* pFrame, LPSTR* ppBlock, DWORD* pdwSize )
{
	// No frame
	if ( pFrame )
	{
		// Write name
		LPSTR pCodeSizePtr = NULL;
		if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
		WriteCODE ( DBOBLOCK_FRAME_NAME, 0, ppBlock, pdwSize );
		WriteString ( pFrame->szName, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );
		if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;
	
		// Write original matrix
		pCodeSizePtr = NULL;
		if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
		WriteCODE ( DBOBLOCK_FRAME_MATRIX, 0, ppBlock, pdwSize );
		WriteMATRIX ( pFrame->matOriginal, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );
		if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;

		// Write mesh
		pCodeSizePtr = NULL;
		if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
		WriteCODE ( DBOBLOCK_FRAME_MESH, 0, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );
		ScanMesh ( pFrame->pMesh, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );
		if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;

		// Write child
		pCodeSizePtr = NULL;
		if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
		WriteCODE ( DBOBLOCK_FRAME_CHILD, 0, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );
		ScanFrame ( pFrame->pChild, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );
		if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;

		// Write sibling
		pCodeSizePtr = NULL;
		if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
		WriteCODE ( DBOBLOCK_FRAME_SIBLING, 0, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );
		ScanFrame ( pFrame->pSibling, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );
		if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;

		// Write vector offset
		pCodeSizePtr = NULL;
		if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
		WriteCODE ( DBOBLOCK_FRAME_OFFSET, 0, ppBlock, pdwSize );
		WriteVECTOR ( pFrame->vecOffset, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );
		if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;

		// Write vector rotation
		pCodeSizePtr = NULL;
		if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
		WriteCODE ( DBOBLOCK_FRAME_ROTATION, 0, ppBlock, pdwSize );
		WriteVECTOR ( pFrame->vecRotation, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );
		if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;

		// Write vector scale
		pCodeSizePtr = NULL;
		if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
		WriteCODE ( DBOBLOCK_FRAME_SCALE, 0, ppBlock, pdwSize );
		WriteVECTOR ( pFrame->vecScale, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );
		if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;
	}

	// No more frame codes
	WriteCODE ( 0, 0, ppBlock, pdwSize );

	// okay
	return true;
}

DARKSDK_DLL bool WriteBoneMatrices ( sAnimation* pAnim, LPSTR* ppBlock, DWORD* pdwSize )
{
	LPSTR pCodeSizePtr = NULL;
	if ( ppBlock ) pCodeSizePtr = *ppBlock+4;

	for ( int iA = 0; iA < pAnim->iBoneFrameA; iA++ )
	{
		for ( int iB = 0; iB < pAnim->iBoneFrameB; iB++ )
		{
			WriteMATRIX ( pAnim->ppBoneFrames [ iA ] [ iB ], ppBlock, pdwSize );
			
		}
	}

	WriteCR ( ppBlock, pdwSize );
	if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;
	
	return true;
}

DARKSDK_DLL bool ReadBoneMatrices ( D3DXMATRIX** ppMatrix, DWORD dwA, DWORD dwB, LPSTR* ppBlock )
{
	DWORD dwLength = sizeof ( D3DXMATRIX );

	for ( int iFrame = 0; iFrame < (int)dwA; iFrame++ )
	{
		ppMatrix [ iFrame ] = new D3DXMATRIX [ dwB ];
	}

	for ( int iA = 0; iA < (int)dwA; iA++ )
	{
		for ( int iB = 0; iB < (int)dwB; iB++ )
		{
			memcpy ( ppMatrix [ iA ] [ iB ], *ppBlock, dwLength );
			*ppBlock += dwLength;		
		}
	}

	return true;
}

DARKSDK_DLL bool ScanAnimation ( sAnimation* pAnim, LPSTR* ppBlock, DWORD* pdwSize )
{
	// No animation
	if ( pAnim )
	{
		// Write name
		LPSTR pCodeSizePtr = NULL;
		if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
		WriteCODE ( DBOBLOCK_ANIM_NAME, 0, ppBlock, pdwSize );
		WriteString ( pAnim->szName, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );
		if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;

		// Write num pos keys
		WriteCODE	( DBOBLOCK_ANIM_NUMPOSKEYS, 4, ppBlock, pdwSize );
		WriteDWORD ( pAnim->dwNumPositionKeys, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );

		// Write pos data
		DWORD dwMemorySize = sizeof(sPositionKey) * pAnim->dwNumPositionKeys;
		if ( dwMemorySize > 0 )
		{
			WriteCODE	( DBOBLOCK_ANIM_POSDATA, dwMemorySize, ppBlock, pdwSize );
			WriteMEMORY ( (BYTE*)pAnim->pPositionKeys, dwMemorySize, ppBlock, pdwSize );
		}
		WriteCR ( ppBlock, pdwSize );

		// Write num rot keys
		WriteCODE	( DBOBLOCK_ANIM_NUMROTKEYS, 4, ppBlock, pdwSize );
		WriteDWORD ( pAnim->dwNumRotateKeys, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );

		// Write rot data
		dwMemorySize = sizeof(sRotateKey) * pAnim->dwNumRotateKeys;
		if ( dwMemorySize > 0 )
		{
			WriteCODE	( DBOBLOCK_ANIM_ROTDATA, dwMemorySize, ppBlock, pdwSize );
			WriteMEMORY ( (BYTE*)pAnim->pRotateKeys, dwMemorySize, ppBlock, pdwSize );
		}
		WriteCR ( ppBlock, pdwSize );

		// Write num scale keys
		WriteCODE	( DBOBLOCK_ANIM_NUMSCALEKEYS, 4, ppBlock, pdwSize );
		WriteDWORD ( pAnim->dwNumScaleKeys, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );

		// Write scale data
		dwMemorySize = sizeof(sScaleKey) * pAnim->dwNumScaleKeys;
		if ( dwMemorySize > 0 )
		{
			WriteCODE	( DBOBLOCK_ANIM_SCALEDATA, dwMemorySize, ppBlock, pdwSize );
			WriteMEMORY ( (BYTE*)pAnim->pScaleKeys, dwMemorySize, ppBlock, pdwSize );
		}
		WriteCR ( ppBlock, pdwSize );

		// Write num matrix keys
		WriteCODE	( DBOBLOCK_ANIM_NUMMATRIXKEYS, 4, ppBlock, pdwSize );
		WriteDWORD ( pAnim->dwNumMatrixKeys, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );

		// Write matrix data
		dwMemorySize = sizeof(sMatrixKey) * pAnim->dwNumMatrixKeys;
		if ( dwMemorySize > 0 )
		{
			WriteCODE	( DBOBLOCK_ANIM_MATRIXDATA, dwMemorySize, ppBlock, pdwSize );
			WriteMEMORY ( (BYTE*)pAnim->pMatrixKeys, dwMemorySize, ppBlock, pdwSize );
		}
		WriteCR ( ppBlock, pdwSize );

		if ( !pAnim->bBoneType )
		{
			pCodeSizePtr = NULL;
			if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
			WriteCODE ( DBOBLOCK_FRAME_BONETYPE, 0, ppBlock, pdwSize );
			WriteDWORD ( pAnim->bBoneType, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );
			if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;

			pCodeSizePtr = NULL;
			if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
			WriteCODE ( DBOBLOCK_FRAME_BONEOFFSETLISTSIZE, 4, ppBlock, pdwSize );
			WriteDWORD ( pAnim->iBoneOffsetListCount, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );
			if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;
			
			pCodeSizePtr = NULL;
			if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
			WriteCODE	( DBOBLOCK_FRAME_BONEOFFSETLIST, pAnim->iBoneOffsetListCount * sizeof ( int ), ppBlock, pdwSize );
			WriteOffsetList ( pAnim->piBoneOffsetList, pAnim->iBoneOffsetListCount, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );
			if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;

			pCodeSizePtr = NULL;
			if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
			WriteCODE ( DBOBLOCK_FRAME_BONEMATRIXSIZEA, 4, ppBlock, pdwSize );
			WriteDWORD ( pAnim->iBoneFrameA, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );
			if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;

			pCodeSizePtr = NULL;
			if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
			WriteCODE ( DBOBLOCK_FRAME_BONEMATRIXSIZEB, 4, ppBlock, pdwSize );
			WriteDWORD ( pAnim->iBoneFrameB, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );
			if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;

			pCodeSizePtr = NULL;
			if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
			WriteCODE	( DBOBLOCK_FRAME_BONEMATRIXLIST, 0, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );
			WriteBoneMatrices ( pAnim, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );
			if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;
		}
		
		// Write NEXT animdata (if present)
		if ( pAnim->pNext )
		{
			pCodeSizePtr = NULL;
			if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
			WriteCODE	( DBOBLOCK_ANIM_NEXT, 0, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );
			if ( pAnim->pNext ) ScanAnimation ( pAnim->pNext, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );
			if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;
		}
	}

	// No more anim codes
	WriteCODE ( 0, 0, ppBlock, pdwSize );

	// okay
	return true;
}

DARKSDK_DLL bool ScanAnimationSet ( sAnimationSet* pAnimSet, LPSTR* ppBlock, DWORD* pdwSize )
{
	// No animation set
	if ( pAnimSet )
	{
		// Write name
		LPSTR pCodeSizePtr = NULL;
		if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
		WriteCODE ( DBOBLOCK_ANIMSET_NAME, 0, ppBlock, pdwSize );
		WriteString ( pAnimSet->szName, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );
		if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;

		// Write length
		WriteCODE	( DBOBLOCK_ANIMSET_LENGTH, 4, ppBlock, pdwSize );
		WriteDWORD ( (DWORD)pAnimSet->ulLength, ppBlock, pdwSize );
		WriteCR ( ppBlock, pdwSize );

		// Write animation data
		if ( pAnimSet->pAnimation )
		{
			pCodeSizePtr = NULL;
			if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
			WriteCODE ( DBOBLOCK_ANIMSET_DATA, 0, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );
			ScanAnimation ( pAnimSet->pAnimation, ppBlock, pdwSize );
			WriteCR ( ppBlock, pdwSize );
			if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;
		}
	}

	// No more animationset codes
	WriteCODE ( 0, 0, ppBlock, pdwSize );

	// okay
	return true;
}

DARKSDK_DLL bool ScanObject ( sObject* pObject, LPSTR* ppBlock, DWORD* pdwSize )
{
	// write header [magic, version, res, res]
	WriteString	( "MAGICDBO", ppBlock, pdwSize );
	WriteDWORD	(  DBO_VERSION_NUMBER, ppBlock, pdwSize );
	WriteDWORD	(  0, ppBlock, pdwSize );
	WriteDWORD	(  0, ppBlock, pdwSize );
	WriteCR ( ppBlock, pdwSize );

	// scan frame data
	LPSTR pCodeSizePtr = NULL;
	if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
	WriteCODE ( DBOBLOCK_ROOT_FRAME, 0, ppBlock, pdwSize );
	WriteCR ( ppBlock, pdwSize );
	if ( !ScanFrame ( pObject->pFrame, ppBlock, pdwSize ) ) return false;
	WriteCR ( ppBlock, pdwSize );
	if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;

	// scan animation data
	pCodeSizePtr = NULL;
	if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
	WriteCODE ( DBOBLOCK_ROOT_ANIMATIONSET, 0, ppBlock, pdwSize );
	WriteCR ( ppBlock, pdwSize );
	if ( !ScanAnimationSet ( pObject->pAnimationSet, ppBlock, pdwSize ) ) return false;
	WriteCR ( ppBlock, pdwSize );
	if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;

	// 280305 - used when objects want to store custom data for example when they save and want to save out this data
	pCodeSizePtr = NULL;
	if ( ppBlock ) pCodeSizePtr = *ppBlock+4;
	WriteCODE ( DBOBLOCK_OBJECT_CUSTOMDATA, 0, ppBlock, pdwSize );
	WriteCR ( ppBlock, pdwSize );
	WriteDWORD ( pObject->dwCustomSize, ppBlock, pdwSize );
	WriteCR ( ppBlock, pdwSize );
	WriteMEMORY ( (BYTE*)pObject->pCustomData, pObject->dwCustomSize, ppBlock, pdwSize );
	WriteCR ( ppBlock, pdwSize );
	if ( pCodeSizePtr ) *(DWORD*)pCodeSizePtr=(*ppBlock-4)-pCodeSizePtr;

	// leefix - 070405 - No more tokens to parse (scan functions do this at end!)
	WriteCODE ( 0, 0, ppBlock, pdwSize );

	// okay
	return true;
}

//////////////////////////////////////////////////////////////////////////////////
// INTERNAL CONSTRUCT FUNCTIONS //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK_DLL bool ConstructBones ( sBone** ppBone, DWORD dwBoneCount, LPSTR* ppBlock )
{
	// get code
	DWORD dwCode = 0;
	DWORD dwCodeSize = 0;
	ReadCODE ( &dwCode, &dwCodeSize, ppBlock );
	if ( dwCode > 0 )
	{
		// create bone
		(*ppBone) = new sBone [ dwBoneCount ];
	}

	// V110 BETA6 - 100608 - why was this so, it scews up models with no bones!!
	// dwBoneCount=dwBoneCount-1;

	for ( DWORD bi=0; bi<dwBoneCount; bi++ )
	{
		// bone in question
		sBone* pBone = &((*ppBone) [ bi ]);

		while ( dwCode > 0 )
		{
			switch ( dwCode )
			{
				case DBOBLOCK_BONES_NAME :				ReadString ( (*pBone).szName,				ppBlock );	break;
				case DBOBLOCK_BONES_NUMINFLUENCES :		ReadDWORD ( &(*pBone).dwNumInfluences,		ppBlock ); break;
				case DBOBLOCK_BONES_VERTICES :			ReadMemory ( (BYTE**)&(*pBone).pVertices,	sizeof(DWORD) * (*pBone).dwNumInfluences,	ppBlock );	break;
				case DBOBLOCK_BONES_WEIGHTS :			ReadMemory ( (BYTE**)&(*pBone).pWeights,	sizeof(float) * (*pBone).dwNumInfluences,	ppBlock );	break;
				case DBOBLOCK_BONES_TRANSLATIONMATRIX :	ReadMatrix ( &(*pBone).matTranslation,		ppBlock ); break;

				default : 
					*ppBlock += dwCodeSize;
					break;
			}

			// get next code
			ReadCODE ( &dwCode, &dwCodeSize, ppBlock );
		}

		// get next code
		ReadCODE ( &dwCode, &dwCodeSize, ppBlock );
	}

	// okay
	return true;
}

DARKSDK_DLL bool ConstructTexture ( sTexture** ppTexture, DWORD dwTextureCount, LPSTR* ppBlock )
{
	// get code
	DWORD dwCode = 0;
	DWORD dwCodeSize = 0;
	ReadCODE ( &dwCode, &dwCodeSize, ppBlock );
	if ( dwCode > 0 )
	{
		// create texture
		(*ppTexture) = new sTexture [ dwTextureCount ];
	}
	for ( DWORD ti=0; ti<dwTextureCount; ti++ )
	{
		// texture in question
		sTexture* pTexture = &((*ppTexture) [ ti ]);

		while ( dwCode > 0 )
		{
			switch ( dwCode )
			{
				case DBOBLOCK_TEXTURE_NAME :	ReadString ( (pTexture)->pName,						ppBlock ); break;
				case DBOBLOCK_TEXTURE_STAGE :	ReadDWORD  ( &(pTexture)->dwStage,					ppBlock ); break;
				case DBOBLOCK_TEXTURE_BMODE :	ReadDWORD  ( &(pTexture)->dwBlendMode,				ppBlock ); break;
				case DBOBLOCK_TEXTURE_BARG1 :	ReadDWORD  ( &(pTexture)->dwBlendArg1,				ppBlock ); break;
				case DBOBLOCK_TEXTURE_BARG2 :	ReadDWORD  ( &(pTexture)->dwBlendArg2,				ppBlock ); break;
				case DBOBLOCK_TEXTURE_ADDRU :	ReadDWORD  ( &(pTexture)->dwAddressU,				ppBlock ); break;
				case DBOBLOCK_TEXTURE_ADDRV :	ReadDWORD  ( &(pTexture)->dwAddressV,				ppBlock ); break;
				case DBOBLOCK_TEXTURE_MAG :		ReadDWORD  ( &(pTexture)->dwMagState,				ppBlock ); break;
				case DBOBLOCK_TEXTURE_MIN :		ReadDWORD  ( &(pTexture)->dwMinState,				ppBlock ); break;
				case DBOBLOCK_TEXTURE_MIP :		ReadDWORD  ( &(pTexture)->dwMipState,				ppBlock ); break;
				case DBOBLOCK_TEXTURE_TCMODE :	ReadDWORD  ( &(pTexture)->dwTexCoordMode,			ppBlock ); break;
				case DBOBLOCK_TEXTURE_PRIMST :	ReadDWORD  ( (DWORD*)&(pTexture)->iStartIndex,		ppBlock ); break;
				case DBOBLOCK_TEXTURE_PRIMCN :	ReadDWORD  ( (DWORD*)&(pTexture)->iPrimitiveCount,	ppBlock ); break;

				default : 
					*ppBlock += dwCodeSize;
					break;
			}

			// get next code
			ReadCODE ( &dwCode, &dwCodeSize, ppBlock );
		}

		// get next code
		ReadCODE ( &dwCode, &dwCodeSize, ppBlock );
	}

	// okay
	return true;
}

DARKSDK_DLL bool ConstructMultiMaterial ( sMultiMaterial** ppMultiMaterial, DWORD dwMultiMatCount, LPSTR* ppBlock )
{
	// get code
	DWORD dwCode = 0;
	DWORD dwCodeSize = 0;
	ReadCODE ( &dwCode, &dwCodeSize, ppBlock );
	if ( dwCode > 0 )
	{
		// create multi material
		(*ppMultiMaterial) = new sMultiMaterial [ dwMultiMatCount ];

		// clear multi material data array
		memset ( (*ppMultiMaterial), 0, sizeof(sMultiMaterial) * dwMultiMatCount );
	}
	for ( DWORD ti=0; ti<dwMultiMatCount; ti++ )
	{
		// multi material in question
		sMultiMaterial* pMultiMat = &((*ppMultiMaterial) [ ti ]);

		while ( dwCode > 0 )
		{
			switch ( dwCode )
			{
				case DBOBLOCK_MULTIMAT_NAME :		ReadString   ( (pMultiMat)->pName,			ppBlock ); break;
				case DBOBLOCK_MULTIMAT_MATERIAL :	ReadMaterial ( &(pMultiMat)->mMaterial,		ppBlock ); break;
				case DBOBLOCK_MULTIMAT_START :		ReadDWORD    ( &(pMultiMat)->dwIndexStart,	ppBlock ); break;
				case DBOBLOCK_MULTIMAT_COUNT :		ReadDWORD    ( &(pMultiMat)->dwIndexCount,	ppBlock ); break;
				case DBOBLOCK_MULTIMAT_POLY :		ReadDWORD    ( &(pMultiMat)->dwPolyCount,	ppBlock ); break;

				default : 
					*ppBlock += dwCodeSize;
					break;
			}

			// get next code
			ReadCODE ( &dwCode, &dwCodeSize, ppBlock );
		}

		// get next code
		ReadCODE ( &dwCode, &dwCodeSize, ppBlock );
	}

	// okay
	return true;
}

DARKSDK_DLL bool ConstructMesh ( sMesh** ppMesh, LPSTR* ppBlock )
{
	// get code
	DWORD dwCode = 0;
	DWORD dwCodeSize = 0;
	ReadCODE ( &dwCode, &dwCodeSize, ppBlock );
	if ( dwCode > 0 )
	{
		// create mesh
		(*ppMesh) = new sMesh;
	}
	while ( dwCode > 0 )
	{
		switch ( dwCode )
		{
			// Read vertex information
			case DBOBLOCK_MESH_FVF :			ReadDWORD      ( &(*ppMesh)->dwFVF,				ppBlock );													break;
			case DBOBLOCK_MESH_FVFSIZE :		ReadDWORD      ( &(*ppMesh)->dwFVFSize,			ppBlock );													break;
			case DBOBLOCK_MESH_VERTEXCOUNT :	ReadDWORD      ( &(*ppMesh)->dwVertexCount,		ppBlock );													break;
			case DBOBLOCK_MESH_INDEXCOUNT :		ReadDWORD      ( &(*ppMesh)->dwIndexCount,		ppBlock );													break;
			case DBOBLOCK_MESH_VERTEXDEC :		ReadIntoMemory ( (BYTE*)(*ppMesh)->pVertexDeclaration, sizeof((*ppMesh)->pVertexDeclaration), ppBlock );	break;

			case DBOBLOCK_MESH_SUBFRAMES :
			{
				ReadDWORD ( (DWORD*)&(*ppMesh)->dwSubMeshListCount,	ppBlock );

				if ( ( *ppMesh )->dwSubMeshListCount > 0 )
				{
					( *ppMesh )->pSubFrameList = new sFrame [ ( *ppMesh )->dwSubMeshListCount ];

					for ( DWORD dwSubMesh = 0; dwSubMesh < ( *ppMesh )->dwSubMeshListCount; dwSubMesh++ )
					{
						sFrame* pFrames = new sFrame;

						ConstructFrame ( &pFrames, ppBlock );

						( *ppMesh )->pSubFrameList [ dwSubMesh ] = *pFrames;
					}
				}
			}
			break;
			
			case DBOBLOCK_MESH_VERTEXDATA :		
				
				ReadVertexData ( &(*ppMesh)->pVertexData, (*ppMesh)->dwFVFSize, (*ppMesh)->dwVertexCount, ppBlock );	
				break;
			
			case DBOBLOCK_MESH_INDEXDATA :
			{
				ReadIndexData ( &(*ppMesh)->pIndices, (*ppMesh)->dwIndexCount, ppBlock );

				if ( ( *ppMesh )->dwIndexCount == 0 )
					*ppBlock += dwCodeSize;
			}
			break;

			case DBOBLOCK_MESH_PRIMTYPE :		ReadDWORD              ( (DWORD*)&(*ppMesh)->iPrimitiveType, ppBlock );								break;
			case DBOBLOCK_MESH_DRAWVERTCOUNT :	ReadDWORD              ( (DWORD*)&(*ppMesh)->iDrawVertexCount, ppBlock );							break;
			case DBOBLOCK_MESH_DRAWPRIMCOUNT :	ReadDWORD              ( (DWORD*)&(*ppMesh)->iDrawPrimitives, ppBlock );							break;
			case DBOBLOCK_MESH_BONECOUNT :		ReadDWORD              ( &(*ppMesh)->dwBoneCount, ppBlock );										break;
			case DBOBLOCK_MESH_BONESDATA : 		ConstructBones         ( &(*ppMesh)->pBones, (*ppMesh)->dwBoneCount, ppBlock );						break;
			case DBOBLOCK_MESH_USEMATERIAL : 	ReadBOOL               ( &(*ppMesh)->bUsesMaterial, ppBlock );										break;
			case DBOBLOCK_MESH_MATERIAL : 		ReadMaterial           ( &(*ppMesh)->mMaterial,	ppBlock );											break;
			case DBOBLOCK_MESH_TEXTURECOUNT : 	ReadDWORD              ( &(*ppMesh)->dwTextureCount, ppBlock );										break;
			case DBOBLOCK_MESH_TEXTURES :		ConstructTexture       ( &(*ppMesh)->pTextures,	(*ppMesh)->dwTextureCount, ppBlock );				break;
			case DBOBLOCK_MESH_USEMULTIMAT : 	ReadBOOL               ( &(*ppMesh)->bUseMultiMaterial,	ppBlock );									break;
			case DBOBLOCK_MESH_MULTIMATCOUNT : 	ReadDWORD              ( &(*ppMesh)->dwMultiMaterialCount, ppBlock );								break;
			case DBOBLOCK_MESH_MULTIMAT	: 		ConstructMultiMaterial ( &(*ppMesh)->pMultiMaterial, (*ppMesh)->dwMultiMaterialCount, ppBlock );	break;
			case DBOBLOCK_MESH_WIREFRAME :		ReadBOOL               ( &(*ppMesh)->bWireframe, ppBlock );											break;
			case DBOBLOCK_MESH_LIGHT :			ReadBOOL               ( &(*ppMesh)->bLight, ppBlock );												break;
			case DBOBLOCK_MESH_CULL :			ReadBOOL               ( &(*ppMesh)->bCull,	ppBlock );												break;
			case DBOBLOCK_MESH_FOG :			ReadBOOL               ( &(*ppMesh)->bFog, ppBlock );												break;
			case DBOBLOCK_MESH_AMBIENT :		ReadBOOL               ( &(*ppMesh)->bAmbient, ppBlock );											break;
			case DBOBLOCK_MESH_TRANSPARENCY :	ReadBOOL               ( &(*ppMesh)->bTransparency, ppBlock );										break;
			case DBOBLOCK_MESH_GHOST :			ReadBOOL               ( &(*ppMesh)->bGhost, ppBlock );												break;
			case DBOBLOCK_MESH_GHOSTMODE :		ReadDWORD              ( (DWORD*)&(*ppMesh)->iGhostMode, ppBlock );									break;
			case DBOBLOCK_MESH_VISIBLE :		ReadBOOL               ( &(*ppMesh)->bVisible, ppBlock );											break;
			case DBOBLOCK_MESH_LINKED :			ReadBOOL               ( &(*ppMesh)->bLinked, ppBlock );											break;

			case DBOBLOCK_MESH_FXEFFECTNAME :	
			{
				// 250704 - create vertex effect object
				ReadString ( (*ppMesh)->pEffectName,	ppBlock );
				(*ppMesh)->bUseVertexShader = true;
				break;
			}

			case DBOBLOCK_MESH_ARBITARYVALUE :	
			{
				// 190804 - retain this value
				ReadDWORD              ( (DWORD*)&(*ppMesh)->Collision.dwArbitaryValue, ppBlock );
				break;
			}
				
			case DBOBLOCK_MESH_ZBIASFLAG :		ReadBOOL               ( &(*ppMesh)->bZBiasActive, ppBlock );										break;
			case DBOBLOCK_MESH_ZBIASSLOPE :		ReadDWORD              ( (DWORD*)&(*ppMesh)->fZBiasSlopeScale, ppBlock );							break;
			case DBOBLOCK_MESH_ZBIASDEPTH :		ReadDWORD              ( (DWORD*)&(*ppMesh)->fZBiasDepth, ppBlock );								break;
			case DBOBLOCK_MESH_ZREAD :			ReadBOOL               ( &(*ppMesh)->bZRead, ppBlock );												break;
			case DBOBLOCK_MESH_ZWRITE :			ReadBOOL               ( &(*ppMesh)->bZWrite, ppBlock );											break;

			case DBOBLOCK_MESH_ALPHATESTVALUE : ReadDWORD              ( (DWORD*)&(*ppMesh)->dwAlphaTestValue, ppBlock );									break;

			default : 
				*ppBlock += dwCodeSize;
			break;
		}

		// get next code
		ReadCODE ( &dwCode, &dwCodeSize, ppBlock );
	}

	// okay
	return true;
}



DARKSDK_DLL bool ConstructFrame ( sFrame** ppFrame, LPSTR* ppBlock )
{
	// get code
	DWORD dwCode = 0;
	DWORD dwCodeSize = 0;
	ReadCODE ( &dwCode, &dwCodeSize, ppBlock );
	if ( dwCode > 0 )
	{
		// create frame
		(*ppFrame) = new sFrame;
	}

	while ( dwCode > 0 )
	{
		switch ( dwCode )
		{
			case DBOBLOCK_FRAME_NAME :     ReadString     ( (*ppFrame)->szName, ppBlock );			break;
			case DBOBLOCK_FRAME_MATRIX :   ReadMatrix     ( &(*ppFrame)->matOriginal, ppBlock );	break;
			case DBOBLOCK_FRAME_MESH :     ConstructMesh  ( &(*ppFrame)->pMesh, ppBlock );			break;
			case DBOBLOCK_FRAME_CHILD :    ConstructFrame ( &(*ppFrame)->pChild, ppBlock );			break;
			case DBOBLOCK_FRAME_SIBLING :  ConstructFrame ( &(*ppFrame)->pSibling, ppBlock );		break;
			case DBOBLOCK_FRAME_OFFSET :   ReadVector     ( &(*ppFrame)->vecOffset, ppBlock );		break;
			case DBOBLOCK_FRAME_ROTATION : ReadVector     ( &(*ppFrame)->vecRotation, ppBlock );	break;
			case DBOBLOCK_FRAME_SCALE :    ReadVector     ( &(*ppFrame)->vecScale, ppBlock );		break;

			default :
				*ppBlock += dwCodeSize;
			break;
		}

		// get next code
		ReadCODE ( &dwCode, &dwCodeSize, ppBlock );
	}

	// okay
	return true;
}

DARKSDK_DLL bool ConstructAnimation ( sAnimation** ppAnim, LPSTR* ppBlock )
{
	// get code
	DWORD dwCode = 0;
	DWORD dwCodeSize = 0;
	ReadCODE ( &dwCode, &dwCodeSize, ppBlock );
	if ( dwCode > 0 )
	{
		// create animset
		(*ppAnim) = new sAnimation;
	}

	while ( dwCode > 0 )
	{
		switch ( dwCode )
		{
			case DBOBLOCK_ANIM_NAME : 				ReadString         ( (*ppAnim)->szName,	ppBlock );																		break;
			case DBOBLOCK_ANIM_NUMPOSKEYS :			ReadDWORD          ( &(*ppAnim)->dwNumPositionKeys,	ppBlock );															break;
			case DBOBLOCK_ANIM_POSDATA :			ReadMemory         ( (BYTE**)&(*ppAnim)->pPositionKeys,	sizeof(sPositionKey) * (*ppAnim)->dwNumPositionKeys, ppBlock );	break;
			case DBOBLOCK_ANIM_NUMROTKEYS :			ReadDWORD          ( &(*ppAnim)->dwNumRotateKeys, ppBlock );															break;
			case DBOBLOCK_ANIM_ROTDATA :			ReadMemory         ( (BYTE**)&(*ppAnim)->pRotateKeys, sizeof(sRotateKey) * (*ppAnim)->dwNumRotateKeys, ppBlock );		break;
			case DBOBLOCK_ANIM_NUMSCALEKEYS :		ReadDWORD          ( &(*ppAnim)->dwNumScaleKeys, ppBlock );																break;
			case DBOBLOCK_ANIM_SCALEDATA :			ReadMemory         ( (BYTE**)&(*ppAnim)->pScaleKeys, sizeof(sScaleKey) * (*ppAnim)->dwNumScaleKeys,	ppBlock );			break;
			case DBOBLOCK_ANIM_NUMMATRIXKEYS :		ReadDWORD          ( &(*ppAnim)->dwNumMatrixKeys, ppBlock );															break;
			case DBOBLOCK_ANIM_MATRIXDATA :			ReadMemory         ( (BYTE**)&(*ppAnim)->pMatrixKeys, sizeof(sMatrixKey) * (*ppAnim)->dwNumMatrixKeys, ppBlock );		break;
			case DBOBLOCK_FRAME_BONETYPE:			ReadDWORD          ( &(*ppAnim)->bBoneType,	ppBlock );																	break;
			case DBOBLOCK_FRAME_BONEOFFSETLISTSIZE: ReadDWORD          ( ( DWORD* ) &(*ppAnim)->iBoneOffsetListCount,	ppBlock );											break;
			case DBOBLOCK_FRAME_BONEOFFSETLIST:		ReadOffsetListData ( &(*ppAnim)->piBoneOffsetList, (*ppAnim)->iBoneOffsetListCount, ppBlock );							break;
			case DBOBLOCK_FRAME_BONEMATRIXSIZEA:	ReadDWORD		   ( ( DWORD* ) &(*ppAnim)->iBoneFrameA, ppBlock );														break;
			case DBOBLOCK_FRAME_BONEMATRIXSIZEB:	ReadDWORD		   ( ( DWORD* ) &(*ppAnim)->iBoneFrameB, ppBlock );														break;
			case DBOBLOCK_ANIM_NEXT :				ConstructAnimation ( &(*ppAnim)->pNext, ppBlock );																		break;

			case DBOBLOCK_FRAME_BONEMATRIXLIST:
				(*ppAnim)->ppBoneFrames = new D3DXMATRIX* [ (*ppAnim)->iBoneFrameA];
				ReadBoneMatrices ( (*ppAnim)->ppBoneFrames, (*ppAnim)->iBoneFrameA, (*ppAnim)->iBoneFrameA, ppBlock );
			break;

			default : 
				*ppBlock += dwCodeSize;
			break;
		}

		// get next code
		ReadCODE ( &dwCode, &dwCodeSize, ppBlock );
	}

	// okay
	return true;
}

DARKSDK_DLL bool ConstructAnimationSet ( sAnimationSet** ppAnimSet, LPSTR* ppBlock )
{
	// get code
	DWORD dwCode = 0;
	DWORD dwCodeSize = 0;
	
	ReadCODE ( &dwCode, &dwCodeSize, ppBlock );
	
	if ( dwCode > 0 )
	{
		// create animset
		(*ppAnimSet) = new sAnimationSet;
	}

	while ( dwCode > 0 )
	{
		switch ( dwCode )
		{
			case DBOBLOCK_ANIMSET_NAME :	ReadString         ( (*ppAnimSet)->szName,		ppBlock ); break;
			case DBOBLOCK_ANIMSET_LENGTH :  ReadDWORD		   ( &(*ppAnimSet)->ulLength,	ppBlock ); break;
			case DBOBLOCK_ANIMSET_DATA :	ConstructAnimation ( &(*ppAnimSet)->pAnimation,	ppBlock ); break;				

			default :
				*ppBlock += dwCodeSize;
			break;
		}

		// get next code
		ReadCODE ( &dwCode, &dwCodeSize, ppBlock );
	}

	// okay
	return true;
}

DARKSDK_DLL bool ConstructCustomData ( sObject** ppObject, LPSTR* ppBlock )
{
	// 280305 - used when objects want to store custom data for example when they save and want to save out this data
	DWORD dwCode = 0;
	DWORD dwCodeSize = 0;
	ReadCODE ( &dwCode, &dwCodeSize, ppBlock );
	SAFE_DELETE ( (*ppObject)->pCustomData );

	// LEEFIX - 150508 - the OLD method of custom data would not have 'dwCustomSize' and so cause ppBlock to advance wrongly
	// and thus the ReadCODE after the case will cause reading invalid memory (loading DBO from UU3D!)
	if ( g_dwVersion<=1 )
	{
		// leefix - 150508 - allowed for old version to fill custom data too
		(*ppObject)->dwCustomSize = dwCodeSize;
		(*ppObject)->pCustomData = new BYTE [ (*ppObject)->dwCustomSize ];
		ReadIntoMemory ( (BYTE*)(*ppObject)->pCustomData, (*ppObject)->dwCustomSize, ppBlock );
	}
	else
	{
		// new versions have extra customsize DWORD - get code
		while ( dwCode > 0 )
		{
			switch ( dwCode )
			{
				case DBOBLOCK_OBJECT_CUSTOMDATA:
				{
					ReadDWORD      ( &(*ppObject)->dwCustomSize, ppBlock );
					(*ppObject)->pCustomData = new BYTE [ (*ppObject)->dwCustomSize ];
					ReadIntoMemory ( (BYTE*)(*ppObject)->pCustomData, (*ppObject)->dwCustomSize, ppBlock );
				}
				break;

				// leefix - 070405 - this u58 addition caused saved DBO objects to run through garbage data
				default :
					*ppBlock += dwCodeSize;

				break;
			}

			// get next code
			// leefix - 070405 - also no final terminate code was written (in write part) meaning we are trapped here until random crash!
			ReadCODE ( &dwCode, &dwCodeSize, ppBlock );
		}
	}

	return true;
}

DARKSDK_DLL bool ConstructObject ( sObject** ppObject, LPSTR* ppBlock )
{
	// ensure is DBO block
	char pMagicString [ MAX_STRING ];

	ReadString ( pMagicString, ppBlock );

	if ( _stricmp ( pMagicString, "MAGICDBO" )==NULL )
	{
		// version information
		DWORD dwVersion=0;
		DWORD dwRes1=0, dwRes2=0;
		ReadDWORD	( &dwVersion,	ppBlock );
		ReadDWORD	( &dwRes1,		ppBlock );
		ReadDWORD	( &dwRes2,		ppBlock );
		g_dwVersion = dwVersion;

		// create object
		(*ppObject) = new sObject;

		// construct frame data
		DWORD dwCode = 0, dwCodeSize = 0;
		ReadCODE ( &dwCode, &dwCodeSize, ppBlock );
		if ( !ConstructFrame ( &(*ppObject)->pFrame, ppBlock ) )
		{
			// cannot construct frame
			return false;
		}

		// construct animation data
		ReadCODE ( &dwCode, &dwCodeSize, ppBlock );
		if ( !ConstructAnimationSet ( &(*ppObject)->pAnimationSet, ppBlock ) )
		{
			// cannot construct animation
			return false;
		}

		// 280305 - new custom data
		// lee - 280306 - u6rc2 - ONLY if there is more block data (older DBO files did not have any more data)
		if ( *ppBlock < g_pBlockEnd ) ConstructCustomData ( ppObject, ppBlock );
	}
	else
	{
		// not a DBO file
		return false;
	}

	// return result
	return true;
}

//////////////////////////////////////////////////////////////////////////////////
// BLOCK FUNCTIONS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK_DLL bool DBOConvertObjectToBlock ( sObject* pObject, DWORD* ppBlock, DWORD* pdwBlockSize )
{
	// determine size of block
	DWORD dwSize = 0;
	if ( !ScanObject ( pObject, NULL, &dwSize ) )
	{
		// could not parse object
		return false;
	}

	// create block memory
	LPSTR pBlockBase = new char[dwSize];

	// generate block data from object
	DWORD dwBytePosition = 0;
	LPSTR pBlock = pBlockBase;
	if ( !ScanObject ( pObject, &pBlock, &dwBytePosition ) )
	{
		// could not parse object
		SAFE_DELETE(pBlock);
		return false;
	}

	// store block size
	*ppBlock = (DWORD)pBlockBase;
	*pdwBlockSize = dwSize;

	// return okay
	return true;
}

DARKSDK_DLL bool DBOConvertBlockToObject ( DWORD pBlock, DWORD dwBlockSize, sObject** ppObject )
{
	// U74 - used to track DBO sizes (to find bugs)
	g_pBlockStart = (LPSTR)pBlock;

	// lee - 280306 - u6rc2 - defeat custom-data read crash (from old DBO files), by recording end of block data gloablly
	g_pBlockEnd = (LPSTR)pBlock + dwBlockSize;

	// construct object from block
	if ( !ConstructObject ( ppObject, (LPSTR*)&pBlock ) )
		return false;

	// return okay
	return true;
}

DARKSDK_DLL bool DBOLoadBlockFile ( LPSTR pFilename, DWORD* ppBlock, DWORD* pdwSize )
{
	// load file
	HANDLE hfile = CreateFile ( pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( hfile != INVALID_HANDLE_VALUE )
	{
		DWORD bytesread=0;
		*pdwSize = GetFileSize ( hfile, NULL );
		*ppBlock = (DWORD)new char[*pdwSize];
		ReadFile( hfile, (LPSTR)(*ppBlock), *pdwSize, &bytesread, NULL ); 
		CloseHandle ( hfile );
	}
	else
	{
		// could not load file
		return false;
	}

	// okay
	return true;
}

DARKSDK_DLL bool DBOSaveBlockFile ( LPSTR pFilename, DWORD pBlock, DWORD dwSize )
{
	// save new file
	DeleteFile ( pFilename );
	HANDLE hfile = CreateFile ( pFilename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( hfile != INVALID_HANDLE_VALUE )
	{
		DWORD byteswritten=0;
		WriteFile( hfile, (LPSTR)pBlock, dwSize, &byteswritten, NULL ); 
		CloseHandle ( hfile );
	}
	else
	{
		// could not create file
		return false;
	}

	// okay
	return true;
}


