// FileBuilder.h: interface for the CFileBuilder class.
#include "direct.h"
#include "io.h"

/* 3.0 icon/cursor header  */
typedef struct {
    WORD iReserved;            /* always 0 */
    WORD iResourceType;
    WORD iResourceCount;       /* number of resources in file */
} ICOCURSORHDR;

/* 3.0 icon/cursor descriptor  */
typedef struct {
    BYTE iWidth;               /* width of image (icons only ) */
    BYTE iHeight;              /* height of image(icons only) */
    BYTE iColorCount;          /* number of colors in image */
    BYTE iUnused;              /*  */
    WORD iHotspotX;            /* hotspot x coordinate (CURSORS only) */
    WORD iHotspotY;            /* hotspot y coordinate (CURSORS only) */
    DWORD DIBSize;             /* size of DIB for this image */
    DWORD DIBOffset;           /* offset to DIB for this image */
} ICOCURSORDESC;

// Class Defs
class CFileBuilder  
{
	public:
		CFileBuilder();
		~CFileBuilder();
		void DeleteFileTable(void);

		bool NewFileTable(void);
		bool AddFile(LPSTR pFilename, LPSTR pPlacement);
		bool AddWildcardFiles(LPSTR pMediaRoot, LPSTR pMediaWidlcardFile);
		bool MakeEXE(LPSTR destEXEfilename, bool bEncryptionState, LPSTR pCompressDLL);

		bool ConstructEXE(LPSTR EXEfilename);
		bool ConstructPCK(LPSTR PCKfilename);
		bool AddFileToConstruct(LPSTR filename, LPSTR pPlacement);
		bool AddDataToConstruct(LPSTR filebuffer, DWORD filebuffersize);
		bool FinishPCK(void);

		bool ReplaceVersionInfoBlockInEXE(LPSTR pFilenameEXE, LPSTR pVersioBlock, DWORD dwOffsetToFirstEntry, DWORD dwVersionBlockSize);
		bool ChangeEXE(LPSTR pFilenameEXE, LPSTR gPathToPluginFolderForBuilder);
		bool MakeICOFromBMP(LPSTR pBMPFilename, LPSTR pDestICOFilename);

		bool SaveIconCursorFileFromInfo(LPSTR pszFullFileName, int iWidth, int iHeight, int iColors, int iHotspotX, int iHotSpotY, LPSTR pImg, DWORD dwImgSize);
		bool MakeCURFromBMP(LPSTR pBMPFilename, LPSTR pDestCURFilename);

		bool AddPCKToEXE(LPSTR pEXEFilename, DWORD KindOfExe);
		void GetPCKFileFromEXEFile(LPSTR destPCKfilename, LPSTR destEXEfilename);

		bool ReplaceDataBlockInEXE ( LPSTR pFilenameEXE, LPSTR pPattern, LPSTR pDataBlock, DWORD dwBlockSize );

	private:

		// EXE Construction
		HANDLE		m_hfile;
		DWORD		m_SizeOfEXECode;

		// File Table
		DWORD		m_dwFileTableSize;
		LPSTR*		m_pFileTable;
		LPSTR*		m_pFileTablePlacement;
		DWORD		m_dwFileTableIndex;

		// Encryption Vars
		bool		m_bEncryptionState;
};
