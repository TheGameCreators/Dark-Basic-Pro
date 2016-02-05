#ifndef DBPROPHYSICS_BASIC3D_OBJECT_DEFS_H
#define DBPROPHYSICS_BASIC3D_OBJECT_DEFS_H

typedef void ( *DLL_SETDISABLETRANSFORM_FP )		( int, bool );


/*
typedef void ( *DLL_CREATEMESHFOROBJECT_FP  )		( int, DWORD, DWORD, DWORD );
typedef void  ( *DLL_LOCKVERTEXDATAFORLIMB_FP )		( int, int );
typedef void  ( *DLL_UNLOCKVERTEXDATA_FP )			( void );
typedef void  ( *DLL_SETINDEXDATA_FP )				( int, int );
typedef void  ( *DLL_SETVERTEXDATA_POSITION_FP )    ( int, float, float, float );
typedef void  ( *DLL_SETVERTEXDATA_DIFFUSE_FP )     ( int, DWORD dwDiffuse );
typedef void  ( *DLL_SETVERTEXDATA_UV_FP )          ( int, int, float, float );
typedef DWORD ( *DLL_GETVERTEXDATA_POSITION_FP )	( int );
typedef DWORD ( *DLL_GETVERTEXDATA_UV_FP )          ( int, int );
typedef int   ( *DLL_GETVERTEXDATA_VERTEXCOUNT_FP ) ( void );
*/

#define D3DFVF_RESERVED0        0x001
#define D3DFVF_POSITION_MASK    0x400E
#define D3DFVF_XYZ              0x002
#define D3DFVF_XYZRHW           0x004
#define D3DFVF_XYZB1            0x006
#define D3DFVF_XYZB2            0x008
#define D3DFVF_XYZB3            0x00a
#define D3DFVF_XYZB4            0x00c
#define D3DFVF_XYZB5            0x00e
#define D3DFVF_XYZW             0x4002
#define D3DFVF_NORMAL           0x010
#define D3DFVF_PSIZE            0x020
#define D3DFVF_DIFFUSE          0x040
#define D3DFVF_SPECULAR         0x080
#define D3DFVF_TEXCOUNT_MASK    0xf00
#define D3DFVF_TEXCOUNT_SHIFT   8
#define D3DFVF_TEX0             0x000
#define D3DFVF_TEX1             0x100
#define D3DFVF_TEX2             0x200
#define D3DFVF_TEX3             0x300
#define D3DFVF_TEX4             0x400
#define D3DFVF_TEX5             0x500
#define D3DFVF_TEX6             0x600
#define D3DFVF_TEX7             0x700
#define D3DFVF_TEX8             0x800


#endif