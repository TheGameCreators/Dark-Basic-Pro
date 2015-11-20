/*
            File : DBPro/File.hpp
       Generated : 2011/09/02 20:14:36
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_File_hpp
#define INCLUDE_GUARD_DBPro_File_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


namespace DBPro
{
    LPSTR __cdecl Appname();
    int __cdecl CanMakeFile(LPCSTR Filename);
    void __cdecl CloseFile(int FileNumber);
    void __cdecl CopyFile(LPCSTR SourceFilename,LPCSTR DestinationFilename);
    void __cdecl DeleteDirectory(LPCSTR DirectoryName);
    void __cdecl DeleteFile(LPCSTR Filename);
    void __cdecl Dir();
    void __cdecl Drivelist();
    int __cdecl ExecutableRunning(DWORD ExecutableId);
    DWORD __cdecl ExecuteExecutable(LPCSTR Filename,LPCSTR Commandline,LPCSTR Directory);
    DWORD __cdecl ExecuteExecutable(LPCSTR Filename,LPCSTR Commandline,LPCSTR Directory,int Priority);
    void __cdecl ExecuteFile(LPCSTR Filename,LPCSTR Commandline,LPCSTR Directory);
    void __cdecl ExecuteFile(LPCSTR Filename,LPCSTR Commandline,LPCSTR Directory,int WaitFlag);
    int __cdecl FileEnd(int FileNumber);
    int __cdecl FileExist(LPCSTR Filename);
    int __cdecl FileOpen(int FileNumber);
    int __cdecl FileSize(LPCSTR Filename);
    void __cdecl FindFirst();
    void __cdecl FindNext();
    LPSTR __cdecl GetDir();
    LPSTR __cdecl GetFileCreation();
    LPSTR __cdecl GetFileDate();
    LPSTR __cdecl GetFileName();
    int __cdecl GetFileType();
    void __cdecl MakeDirectory(LPCSTR DirectoryName);
    void __cdecl MakeFileFromMemblock(int FileNumber,int MemblockNumber);
    void __cdecl MakeFile(LPCSTR Filename);
    void __cdecl MakeMemblockFromFile(int MemblockNumber,int FileNumber);
    void __cdecl MoveFile(LPCSTR SourceFilename,LPCSTR DestinationFilename);
    LPSTR __cdecl Mydocdir();
    void __cdecl OpenToRead(int FileNumber,LPCSTR Filename);
    void __cdecl OpenToWrite(int FileNumber,LPCSTR Filename);
    int __cdecl PathExist(LPCSTR PathString);
    void __cdecl PerformChecklistForDrives();
    void __cdecl PerformChecklistForFiles();
    int __cdecl ReadByteFromFile(LPCSTR Filename,int Position);
    void __cdecl ReadDirblock(int FileNumber,LPCSTR FolderToRead);
    int __cdecl ReadFile(int FileNumber,int Variable);
    void __cdecl ReadFileblock(int FileNumber,LPCSTR FilenameToRead);
    LPSTR __cdecl ReadFilemapString(LPCSTR a);
    DWORD __cdecl ReadFilemapValue(LPCSTR FilemapName);
    float __cdecl ReadFloat(int FileNumber,float Variable);
    void __cdecl ReadMemblock(int FileNumber,int MemblockNumber);
    LPSTR __cdecl ReadString(int FileNumber,LPCSTR VariableString);
    void __cdecl RenameFile(LPCSTR SourceFilename,LPCSTR NewFilename);
    void __cdecl SetDir(LPCSTR Path$);
    void __cdecl SkipBytes(int FileNumber,int BytesToSkip);
    void __cdecl StopExecutable(DWORD ExecutableId);
    LPSTR __cdecl Windir();
    void __cdecl WriteByteToFile(LPCSTR Filename,int Position,int Bytevalue);
    void __cdecl WriteByte(int FileNumber,int Variable);
    void __cdecl WriteDirblock(int FileNumber,LPCSTR FolderToCreate);
    void __cdecl WriteFile(int FileNumber,int Variable);
    void __cdecl WriteFileblock(int FileNumber,LPCSTR FilenameToCreate);
    void __cdecl WriteFilemapString(LPCSTR FilemapName,LPCSTR String);
    void __cdecl WriteFilemapValue(LPCSTR FilemapName,DWORD Value);
    void __cdecl WriteFloat(int FileNumber,float Variable);
    void __cdecl WriteLong(int FileNumber,int Variable);
    void __cdecl WriteMemblock(int FileNumber,int MemblockNumber);
    void __cdecl WriteString(int FileNumber,LPCSTR String);
    void __cdecl WriteWord(int FileNumber,int Variable);
    int __cdecl ReadByte(int FileNumber);
    int __cdecl ReadLong(int FileNumber);
    int __cdecl ReadWord(int FileNumber);
}

#endif
