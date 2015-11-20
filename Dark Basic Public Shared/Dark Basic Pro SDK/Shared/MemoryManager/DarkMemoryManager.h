// Dark Memory Manager

/* 
Needs winmm.lib

mm_init() to start (optional)- The very first new or delete will fire off mm_init() anyway

To be provided by calling app, dll etc:
	char g_MM_DLLName [ 256 ];
	char g_MM_FunctionName [ 256 ];

Call mm_snapshot() to write out a snapshot file (place in end of main in a dll file to ensure at least one snapshot gets written)

Call mm_Report() in the main code (not dlls) to write the report from all snapshot files

files stored in:
	Logs: mmlogs
	Snapshots: mmsnapshots
	Snapshot of all: App root dolder
*/

// Since all code will use this header, commenting out the next line will remove all trace of MM
//#define __USE_MEMORY_MANAGER__

#ifdef  __USE_MEMORY_MANAGER__
void* operator new(size_t);
void operator delete(void* ptr);
void mm_init();
void mm_Memory();
void mm_Time();
bool mm_EmptyFolder ( char* szDirectory );
void mm_Add_Alloc ( void* p , size_t size);
void mm_Delete_Alloc ( void* p );
size_t mm_GetSize ( void* p);
__declspec ( dllexport ) void mm_SnapShot();
void mm_Report();
#endif