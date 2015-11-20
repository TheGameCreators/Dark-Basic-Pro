// FileReader.h: interface for the CFileReader class.

// Includes
#include "windows.h"

// Internal Class Data
class CFileTable;

class CFileReader  
{
	public:
		CFileReader();
		~CFileReader();
		
		bool MakePCKFromEXE(LPSTR ActualEXEFilename, LPSTR pPCKFilename);
		bool CreateVirtualFileTable(char* ActualEXEFilename);
		bool CreateFileFromData(char* UseFilename, char* dataptr, DWORD datasize);
		void StripPathOnly(LPSTR pFilename);
		void StripFileOnly(LPSTR pFilename);
		void RecreatePathFolders(LPSTR pPath);
		bool RemoveVirtualFileTable(void);
		bool CreateInstallerFolder(LPSTR ActualEXEFilename);
		void FreeEXEDataFromMemory(void);
		bool FileExists(LPSTR pFile);
		bool PathExists(LPSTR pPath);
		void GoodDeleteFile(LPSTR filename);

		bool IsEXEInstaller(void) { if(m_dwKindOfExecutable==1) return true; else return false; }

	private:
		char*			m_pDataBlock;
		DWORD			m_DataBlockSize;
		CFileTable*		m_RefTable;

		DWORD			m_dwKindOfExecutable;
};

class CFileTable
{
	public:
		CFileTable();
		~CFileTable();

		void SetNext(CFileTable* pNext) { m_pNext = pNext; }
		void SetPrev(CFileTable* pPrev) { m_pPrev = pPrev; }
		CFileTable* GetNext(void) { return m_pNext; }
		CFileTable* GetPrev(void) { return m_pPrev; }
		void Add(CFileTable* pNewItem);

		void SetFilename(char* filename) { strcpy(m_filename, filename); }
		void SetDataPtr(char* ptr) { m_dataptr = ptr; }
		void SetDataSize(DWORD size) { m_datasize = size; }
		char* GetFilename(void) { return m_filename; }
		char* GetDataPtr(void) { return m_dataptr; }
		DWORD GetDataSize(void) { return m_datasize; }

	private:
		CFileTable*		m_pNext;
		CFileTable*		m_pPrev;

		char			m_filename[256];
		char*			m_dataptr;
		DWORD			m_datasize;
};

