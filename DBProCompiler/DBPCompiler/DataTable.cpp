// DataTable.cpp: implementation of the CDataTable class.
//
//////////////////////////////////////////////////////////////////////

#include "DataTable.h"

// Includes and external ptr for AssociateDLL scan
#include "DBPCompiler.h"
#include "direct.h"
extern CDBPCompiler* g_pDBPCompiler;
extern bool g_bExternaliseDLLS;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataTable::CDataTable()
{
	m_dwIndex=0;
	m_dwType=0;
	m_pNumeric=0;
	m_pString=NULL;
	m_pString2=NULL;
	m_pNext=NULL;
	m_bAddedToEXEData=false;
}

CDataTable::CDataTable(LPSTR pInitString)
{
	m_dwIndex=0;
	m_dwType=0;
	m_pNumeric=0;
	m_pString=new CStr(pInitString);
	m_pString2=new CStr("");
	m_pNext=NULL;
	m_bAddedToEXEData=false;
}

CDataTable::~CDataTable()
{
	SAFE_DELETE(m_pString);
	SAFE_DELETE(m_pString2);
}

void CDataTable::Free(void)
{
	CDataTable* pCurrent = this;
	while(pCurrent)
	{
		CDataTable* pNext = pCurrent->GetNext();
		delete pCurrent;
		pCurrent = pNext;
	}
}

void CDataTable::Add(CDataTable* pNew)
{
	CDataTable* pCurrent = this;
	while(pCurrent->m_pNext)
	{
		pCurrent=pCurrent->GetNext();
	}
	pCurrent->m_pNext=pNew;
}

bool CDataTable::AddNumeric(double dNum, DWORD dwIndex)
{
	// Create new data item
	CDataTable* pNewData = new CDataTable;
	pNewData->SetNumeric(dNum);

	// Set index
	pNewData->SetIndex(dwIndex);

	// Add to Data Table
	Add(pNewData);

	// Complete
	return true;
}

bool CDataTable::AddString(LPSTR pString, DWORD dwIndex)
{
	// Create new data item
	CDataTable* pNewData = new CDataTable;
	CStr* pStr = new CStr(pString);
	pNewData->SetString(pStr);
	pNewData->SetString2(NULL);

	// Set index
	pNewData->SetIndex(dwIndex);

	// Add to Data Table
	Add(pNewData);

	// Complete
	return true;
}

bool CDataTable::AddTwoStrings(LPSTR pString, LPSTR pString2, DWORD* dwIndex)
{
	// If string is NOT unique, fail
	DWORD dwResult = FindString(pString);
	if(dwResult>0)
	{
		*dwIndex=dwResult;
		return false;
	}

	// Create new data item
	CDataTable* pNewData = new CDataTable;
	CStr* pStr1 = new CStr(pString);
	CStr* pStr2 = new CStr(pString2);
	pNewData->SetString(pStr1);
	pNewData->SetString2(pStr2);

	// Set index
	pNewData->SetIndex(*dwIndex);

	// Add to Data Table
	Add(pNewData);

	// Complete
	return true;
}

bool CDataTable::AddUniqueString(LPSTR pString, DWORD* dwIndex)
{
	// If string is NOT unique, fail
	DWORD dwResult = FindString(pString);
	if(dwResult>0)
	{
		*dwIndex=dwResult;
		return false;
	}

	// Create new data item
	CDataTable* pNewData = new CDataTable;
	CStr* pStr = new CStr(pString);
	pNewData->SetString(pStr);
	pNewData->SetString2(NULL);

	// Set index
	pNewData->SetIndex(*dwIndex);

	// Add to Data Table
	Add(pNewData);

	// Complete
	return true;
}

DWORD CDataTable::FindString(LPSTR pFindString)
{
	// Find String
	CDataTable* pCurrent = this;
	while(pCurrent)
	{
		// Match list item with search string
		if(pCurrent->GetString())
			if(stricmp(pCurrent->GetString()->GetStr(), pFindString)==NULL)
				return pCurrent->GetIndex();

		pCurrent=pCurrent->GetNext();
	}

	// Failed to find
	return 0;
}

bool CDataTable::FindIndexStr(LPSTR pIndexAsString)
{
	// Convert String to Index
	DWORD dwFindIndex = atoi(pIndexAsString);

	// Find String
	CDataTable* pCurrent = this;
	while(pCurrent)
	{
		// Match list item with search string
		if(pCurrent->GetString())
			if(pCurrent->GetIndex()==dwFindIndex)
				return true;

		pCurrent=pCurrent->GetNext();
	}

	// Soft Failed to find
	return false;
}

bool CDataTable::NotExcluded ( LPSTR pFilename )
{
	// false if excluded from compile
	for ( DWORD i=1; i<g_pDBPCompiler->g_dwExcludeFilesCount; i++)
		if ( g_pDBPCompiler->g_pExcludeFiles [ i ] )
			if ( stricmp ( g_pDBPCompiler->g_pExcludeFiles [ i ], pFilename )==NULL )
				return false;

	// lee - 270308 - u67 - do not include DLL at all if flagged
	if ( g_bExternaliseDLLS==true )
		return false;

	// complete, not excluded
	return true;
}

int CDataTable::CompleteAnyLinkAssociates(void)
{
	// Scan user plugins - check if associations require any DBPro DLLs
	bool bAtLeastOneUserDLLNeeds3D = false;
	bool bAtLeastOneUserDLLNeedsSOUND = false;

	// reset index
	DWORD dwIndex=0;
	DWORD dwIndexBeforeAdds=0;

	// First pass basic DLLs, second pass is dependence additions
	for ( int iAddDependentsLoop=0; iAddDependentsLoop<2; iAddDependentsLoop++ )
	{
		for ( int iPass=0; iPass<2; iPass++ )
		{
			// Switch to PLUGINS-XXXX Folder
			char pOldDir [ _MAX_PATH ];
			getcwd ( pOldDir, _MAX_PATH );

			// Depends on pass value
			if ( iPass==0 ) _chdir(g_pDBPCompiler->GetInternalFile(PATH_PLUGINSUSERFOLDER));
			if ( iPass==1 ) _chdir(g_pDBPCompiler->GetInternalFile(PATH_PLUGINSLICENSEDFOLDER));

			// Go through DLLs from direct-command-list
			CDataTable* pCurrent = this->GetNext();
			while(pCurrent)
			{
				// Check if DLL is user-dll ( leefix - 011208 - u71 - gameFX needed to link to Basic3D! )
				LPSTR pDLLName = pCurrent->GetString()->GetStr();
				if ( strnicmp ( pDLLName, "dbpro", 5 )!=NULL || strnicmp ( pDLLName, "dbprogamefx", 11 )==NULL )
				{
					// must be user DLL (associated with main DLL)
					int iAssociationCode = 0;
					HMODULE hModule = LoadLibrary(pDLLName);
					if(hModule)
					{
						// get associate dll value if any
						if ( iAddDependentsLoop==0 )
						{
							typedef int ( *RETINTNOPARAM ) ( void );
							RETINTNOPARAM GetAssociatedDLLs = ( RETINTNOPARAM ) GetProcAddress ( hModule, "?GetAssociatedDLLs@@YAHXZ" );
							if (!GetAssociatedDLLs)
								GetAssociatedDLLs = (RETINTNOPARAM)GetProcAddress(hModule, "GetAssociatedDLLs");
							if ( GetAssociatedDLLs ) iAssociationCode=GetAssociatedDLLs();
						}
						else
						{
							// get num of additional dependencies
							int iNumDLLDependencies = 0;
							typedef int ( *RETINTNOPARAM ) ( void );
							RETINTNOPARAM GetNumDependencies = ( RETINTNOPARAM ) GetProcAddress ( hModule, "?GetNumDependencies@@YAHXZ" );
							if (!GetNumDependencies)
								GetNumDependencies = (RETINTNOPARAM)GetProcAddress(hModule, "GetNumDependencies");
							if ( GetNumDependencies ) iNumDLLDependencies=GetNumDependencies();
							if ( iNumDLLDependencies > 0 )
							{
								typedef const char * ( *RETLPSTRNOPARAM ) ( int n );
								RETLPSTRNOPARAM GetDependencyID = ( RETLPSTRNOPARAM ) GetProcAddress ( hModule, "?GetDependencyID@@YAPBDH@Z" );
								if (!GetDependencyID)
									GetDependencyID = (RETLPSTRNOPARAM)GetProcAddress(hModule, "GetDependencyID");
								// store dependencies in list
								for ( int iD=0; iD<iNumDLLDependencies; iD++ )
								{
									char pDependencyStr[256];
									//LPSTR pDependencyStr = new char[256];
									strcpy ( pDependencyStr, GetDependencyID(iD) );
									DWORD dwTry=dwIndex+1;
									if(AddUniqueString(pDependencyStr, &dwTry)) dwIndex=dwTry;
									//SAFE_DELETE(pDependencyStr);
								}
							}
						}
					}

					// free it if loaded
					if(hModule)
					{
						FreeLibrary(hModule);
						hModule=NULL;
					}

					// Association Codes (1=3d/2=sound/4-//)
					if ( iAssociationCode & 1 ) bAtLeastOneUserDLLNeeds3D=true;
					if ( iAssociationCode & 2 ) bAtLeastOneUserDLLNeedsSOUND=true;
				}

				// Next DLL
				if ( iAddDependentsLoop==0 && iPass==0 ) dwIndex++;
				pCurrent=pCurrent->GetNext();
			}

			// Restore dir before continue
			_chdir(pOldDir);
		}

		// DLL index before adding any associations
		if ( iAddDependentsLoop==0 ) dwIndexBeforeAdds=dwIndex;
	}

	// link Basic3D
	if ( bAtLeastOneUserDLLNeeds3D )
	{
		DWORD dwTry=dwIndex+1;
		if(AddUniqueString("DBProBasic3DDebug.dll", &dwTry)) dwIndex=dwTry;
	}

	// link Sound
	if ( bAtLeastOneUserDLLNeedsSOUND )
	{
		DWORD dwTry=dwIndex+1;
		if(AddUniqueString("DBProSoundDebug.dll", &dwTry)) dwIndex=dwTry;
	}

	// Scan all DLLS, and add any that are link-associated
	CDataTable* pCurrent = this->GetNext();
	while(pCurrent)
	{
		// If DLLTable Entry has string..
		if(pCurrent->GetString())
		{
			// DLL Name contained in stringname
			DWORD dwTry = 0;
			LPSTR pDLL = NULL;
			LPSTR pDLLName = pCurrent->GetString()->GetStr();
#define TRY_DLL(nm) dwTry=dwIndex+1;pDLL=nm;if(NotExcluded(pDLL))if(AddUniqueString(pDLL,&dwTry))dwIndex=dwTry
			// Add other DLLs Associated With These..
			if(stricmp(pDLLName, "DBProSetupDebug.dll")==NULL)
			{
				// Associate DLLs
				TRY_DLL("DBProBasic2DDebug.dll");
				TRY_DLL("DBProTextDebug.dll");
			}
			if(stricmp(pDLLName, "DBProTextDebug.dll")==NULL)
			{
				// Associate DLLs
				TRY_DLL("DBProSetupDebug.dll");
			}
			if(stricmp(pDLLName, "DBProInputDebug.dll")==NULL)
			{
				// Checklist Support
				TRY_DLL("DBProSystemDebug.dll");
			}
			if(stricmp(pDLLName, "DBProSpritesDebug.dll")==NULL)
			{
				// Image Support
				TRY_DLL("DBProImageDebug.dll");
			}
			if(stricmp(pDLLName, "DBProBasic3DDebug.dll")==NULL)
			{
				// Image Support
				TRY_DLL("DBProImageDebug.dll");
				// Transforms Support
				TRY_DLL("DBProTransformsDebug.dll");
			}
			if(stricmp(pDLLName, "DBProBasic2DDebug.dll")==NULL)
			{
				// Minimal DirectX
				TRY_DLL("DBProSetupDebug.dll");
			}
			if(stricmp(pDLLName, "DBProImageDebug.dll")==NULL
			|| stricmp(pDLLName, "DBProAnimationDebug.dll")==NULL
			|| stricmp(pDLLName, "DBProBitmapDebug.dll")==NULL)
			{
				// Sprite Support for pasting
				TRY_DLL("DBProSpritesDebug.dll");

				// Minimal DirectX
				TRY_DLL("DBProSetupDebug.dll");
				TRY_DLL("DBProBasic2DDebug.dll");
				TRY_DLL("DBProTextDebug.dll");
			}
			if(stricmp(pDLLName, "DBProMultiplayerDebug.dll")==NULL)
			{
				// Need access to memblock support
				TRY_DLL("DBProMemblocksDebug.dll");
			}
			if(stricmp(pDLLName, "DBProMemblocksDebug.dll")==NULL)
			{
				// Memblocks Access to Bitmap, Image, Sound and Mesh
				TRY_DLL("DBProBitmapDebug.dll");
				TRY_DLL("DBProImageDebug.dll");
				TRY_DLL("DBProSoundDebug.dll");
				TRY_DLL("DBProBasic3DDebug.dll");
			}
			if(stricmp(pDLLName, "DBProCameraDebug.dll")==NULL)
			{
				TRY_DLL("DBProSetupDebug.dll");
				TRY_DLL("DBProImageDebug.dll");
				TRY_DLL("DBProVectorsDebug.dll");
				TRY_DLL("DBProTransformsDebug.dll");
				TRY_DLL("DBProBasic3DDebug.dll");
			}
			if(stricmp(pDLLName, "DBProLightDebug.dll")==NULL)
			{
				TRY_DLL("DBProSetupDebug.dll");
				TRY_DLL("DBProCameraDebug.dll");
				TRY_DLL("DBProVectorsDebug.dll");
				TRY_DLL("DBProTransformsDebug.dll");
			}
			if(stricmp(pDLLName, "DBProMatrixDebug.dll")==NULL)
			{
				TRY_DLL("DBProSetupDebug.dll");
				TRY_DLL("DBProImageDebug.dll");
				TRY_DLL("DBProCameraDebug.dll");
				TRY_DLL("DBProVectorsDebug.dll");
				TRY_DLL("DBProTransformsDebug.dll");
			}
			if(stricmp(pDLLName, "DBProBasic3DDebug.dll")==NULL)
			{
				// Primary Support
				TRY_DLL("DBProSetupDebug.dll");
				TRY_DLL("DBProImageDebug.dll");
				TRY_DLL("DBProCameraDebug.dll");
				TRY_DLL("DBProLightDebug.dll");
				TRY_DLL("DBProTransformsDebug.dll");

				// Secondary Support
				TRY_DLL("DBProVectorsDebug.dll");
				TRY_DLL("ConvX.dll");
				TRY_DLL("Conv3DS.dll");
				TRY_DLL("ConvMDL.dll");
				TRY_DLL("ConvMD2.dll");
				TRY_DLL("ConvMD3.dll");
			}
			if(stricmp(pDLLName, "DBProWorld3DDebug.dll")==NULL )
			{
				// Primary Support
				TRY_DLL("DBProLODTerrainDebug.dll");
				TRY_DLL("DBProQ2BSPDebug.dll");
				TRY_DLL("DBProBasic3DDebug.dll");
				TRY_DLL("DBProVectorsDebug.dll");
				TRY_DLL("DBProTransformsDebug.dll");
				TRY_DLL("DBProOwnBSPDebug.dll");
			}
			if(stricmp(pDLLName, "DBProLODTerrainDebug.dll")==NULL )
			{
				// Primary Support
				TRY_DLL("DBProSetupDebug.dll");
				TRY_DLL("DBProImageDebug.dll");
				TRY_DLL("DBProCameraDebug.dll");
				TRY_DLL("DBProTransformsDebug.dll");
			}
			if(stricmp(pDLLName, "DBProCSGDebug.dll")==NULL )
			{
				// Primary Support
				TRY_DLL("DBProSetupDebug.dll");
			}
			if(stricmp(pDLLName, "DBProParticlesDebug.dll")==NULL )
			{
				// Primary Support
				TRY_DLL("DBProParticlesDebug.dll");
				TRY_DLL("DBProVectorsDebug.dll");
				TRY_DLL("DBProTransformsDebug.dll");
			}
			if(stricmp(pDLLName, "DBProSystemDebug.dll")==NULL )
			{
				// for access to display mem
				TRY_DLL("DBProSetupDebug.dll");
			}
			if(stricmp(pDLLName, "DBProVectorsDebug.dll")==NULL )
			{
				TRY_DLL("DBProSetupDebug.dll");
			}
			if(stricmp(pDLLName, "DBProTransformsDebug.dll")==NULL)
			{
				TRY_DLL("DBProSetupDebug.dll");
			}
#undef TRY_DLL
		}

		// Next entry in DLL Table
		pCurrent=pCurrent->GetNext();
	}

	// Complete
	return (dwIndex-dwIndexBeforeAdds); 
}

// WriteDBM

bool CDataTable::WriteDBMHeader(DWORD dwKindOfTable)
{
	// Blank Line
	CStr strDBMBlank(1);
	if(g_pDBMWriter->OutputDBM(&strDBMBlank)==false) return false;

	// header Line
	CStr strDBMLine(256);
	if(dwKindOfTable==1) strDBMLine.SetText("STRING:");
	if(dwKindOfTable==2) strDBMLine.SetText("DATA:");
	if(dwKindOfTable==3) strDBMLine.SetText("DLLS:");
	if(dwKindOfTable==4) strDBMLine.SetText("COMMANDS:");
	if(g_pDBMWriter->OutputDBM(&strDBMLine)==false) return false;

	return true;
}

bool CDataTable::WriteDBM(void)
{
	// Write out text
	CStr strDBMLine(256);
	strDBMLine.SetText(">>");
	if(GetType()==1)
	{
		strDBMLine.AddNumericText(GetIndex());
		strDBMLine.AddText("=");
		strDBMLine.AddDoubleText(GetNumeric());
	}
	if(GetType()==2)
	{
		strDBMLine.AddNumericText(GetIndex());
		strDBMLine.AddText("=");
		strDBMLine.AddText(GetString());
	}

	// Output details
	if(g_pDBMWriter->OutputDBM(&strDBMLine)==false) return false;

	// Write next one
	if(GetNext())
	{
		if((GetNext()->WriteDBM())==false) return false;
	}

	// Complete
	return true;
}

