# Microsoft Developer Studio Project File - Name="Main" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=MAIN - WIN32 DEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Main.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Main.mak" CFG="MAIN - WIN32 DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Main - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Main - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Main - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MAIN_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MAIN_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 winmm.lib dxguid.lib /nologo /dll /machine:I386 /out:"D:\_CODING\Dark Basic Pro Shared\Dark Basic Pro\Compiler\plugins-user\Enhancements.dll"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Main - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MAIN_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MAIN_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib dxguid.lib /nologo /dll /debug /machine:I386 /out:"D:\_CODING\Dark Basic Pro Shared\Dark Basic Pro\Compiler\plugins-user\Enhancements.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Main - Win32 Release"
# Name "Main - Win32 Debug"
# Begin Group "Core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Core.cpp
# End Source File
# Begin Source File

SOURCE=.\Core.h
# End Source File
# Begin Source File

SOURCE=.\globstruct.cpp
# End Source File
# Begin Source File

SOURCE=.\globstruct.h
# End Source File
# End Group
# Begin Group "Table"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\commands.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "EAX"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EAX.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "OS"

# PROP Default_Filter ""
# Begin Group "CPU"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CpuUsage.cpp
# End Source File
# Begin Source File

SOURCE=.\CpuUsage.h
# End Source File
# Begin Source File

SOURCE=.\OSCpu.cpp
# End Source File
# Begin Source File

SOURCE=.\OSCpuInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\OSCpuInfo.h
# End Source File
# Begin Source File

SOURCE=.\PerfCounters.h
# End Source File
# End Group
# Begin Group "Hard Drive"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\OSHardDrive.cpp
# End Source File
# End Group
# Begin Group "Name"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\OSComputerNames.cpp
# End Source File
# End Group
# Begin Group "Display"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\OSDisplay.cpp
# End Source File
# End Group
# Begin Group "Dialog"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\OSDialog.cpp
# End Source File
# End Group
# Begin Group "Internet"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\OSInternet.cpp
# End Source File
# End Group
# Begin Group "Speech"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\OSSpeech.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Memory"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\OSMemory.cpp
# End Source File
# End Group
# End Group
# Begin Group "File Blocks"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FileBlocks.cpp
# End Source File
# Begin Source File

SOURCE=.\FileBlocksBlowfish.cpp
# End Source File
# Begin Source File

SOURCE=.\FileBlocksBlowfish.h
# End Source File
# Begin Source File

SOURCE=.\FileBlocksData.h
# End Source File
# End Group
# Begin Group "File Mapping"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FileMapping.cpp
# End Source File
# Begin Source File

SOURCE=.\IPC.cpp
# End Source File
# Begin Source File

SOURCE=.\IPC.h
# End Source File
# End Group
# Begin Group "Data"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Data.cpp
# End Source File
# Begin Source File

SOURCE=.\Data.h
# End Source File
# End Group
# End Target
# End Project
