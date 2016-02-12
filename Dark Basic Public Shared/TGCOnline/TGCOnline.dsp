# Microsoft Developer Studio Project File - Name="TGCOnline" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=TGCOnline - Win32 DebugSERIALCODE
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TGCOnline.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TGCOnline.mak" CFG="TGCOnline - Win32 DebugSERIALCODE"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TGCOnline - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "TGCOnline - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "TGCOnline - Win32 ReleaseFREECERTIFICATES" (based on "Win32 (x86) Application")
!MESSAGE "TGCOnline - Win32 ReleaseTRIAL" (based on "Win32 (x86) Application")
!MESSAGE "TGCOnline - Win32 ReleaseVERIFIER" (based on "Win32 (x86) Application")
!MESSAGE "TGCOnline - Win32 ReleaseVERIFIER_DEBUG" (based on "Win32 (x86) Application")
!MESSAGE "TGCOnline - Win32 DebugTRIAL" (based on "Win32 (x86) Application")
!MESSAGE "TGCOnline - Win32 ReleaseTRIAL100DAY" (based on "Win32 (x86) Application")
!MESSAGE "TGCOnline - Win32 DebugSERIALCODE" (based on "Win32 (x86) Application")
!MESSAGE "TGCOnline - Win32 ReleaseSERIALCODE" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TGCOnline - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 wininet.lib md5.lib /nologo /subsystem:windows /machine:I386 /out:"..\Dark Basic Pro\Compiler\TGCOnline.exe"

!ELSEIF  "$(CFG)" == "TGCOnline - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wininet.lib md5.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\Dark Basic Pro\Compiler\TGCOnline.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "TGCOnline - Win32 ReleaseFREECERTIFICATES"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TGCOnline___Win32_ReleaseFREECERTIFICATES"
# PROP BASE Intermediate_Dir "TGCOnline___Win32_ReleaseFREECERTIFICATES"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "TGCOnline___Win32_ReleaseFREECERTIFICATES"
# PROP Intermediate_Dir "TGCOnline___Win32_ReleaseFREECERTIFICATES"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "FREECERTIFICATESVERSION" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wininet.lib md5.lib /nologo /subsystem:windows /machine:I386 /out:"..\Dark Basic Pro\Compiler\TGCOnline.exe"
# ADD LINK32 wininet.lib md5.lib /nologo /subsystem:windows /machine:I386 /out:"..\Dark Basic Pro\Compiler\TGCOnlineFree.exe"

!ELSEIF  "$(CFG)" == "TGCOnline - Win32 ReleaseTRIAL"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TGCOnline___Win32_ReleaseTRIAL"
# PROP BASE Intermediate_Dir "TGCOnline___Win32_ReleaseTRIAL"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "TGCOnline___Win32_ReleaseTRIAL"
# PROP Intermediate_Dir "TGCOnline___Win32_ReleaseTRIAL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "TGCTRIAL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wininet.lib md5.lib /nologo /subsystem:windows /machine:I386 /out:"..\Dark Basic Pro\Compiler\TGCOnline.exe"
# ADD LINK32 wininet.lib md5.lib /nologo /subsystem:windows /machine:I386 /out:"..\Dark Basic Pro\Compiler\TGCOnlineTrial.exe"

!ELSEIF  "$(CFG)" == "TGCOnline - Win32 ReleaseVERIFIER"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TGCOnline___Win32_ReleaseVERIFIER"
# PROP BASE Intermediate_Dir "TGCOnline___Win32_ReleaseVERIFIER"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "TGCOnline___Win32_ReleaseVERIFIER"
# PROP Intermediate_Dir "TGCOnline___Win32_ReleaseVERIFIER"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "TGCVERIFIER" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wininet.lib md5.lib /nologo /subsystem:windows /machine:I386 /out:"..\Dark Basic Pro\Compiler\TGCOnline.exe"
# ADD LINK32 wininet.lib md5.lib /nologo /subsystem:windows /machine:I386 /out:"..\Dark Basic Pro\Compiler\TGCVerifier.exe"

!ELSEIF  "$(CFG)" == "TGCOnline - Win32 ReleaseVERIFIER_DEBUG"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TGCOnline___Win32_ReleaseVERIFIER_DEBUG"
# PROP BASE Intermediate_Dir "TGCOnline___Win32_ReleaseVERIFIER_DEBUG"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "TGCOnline___Win32_ReleaseVERIFIER_DEBUG"
# PROP Intermediate_Dir "TGCOnline___Win32_ReleaseVERIFIER_DEBUG"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "TGCVERIFIER" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /ZI /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "TGCVERIFIER" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wininet.lib md5.lib /nologo /subsystem:windows /machine:I386 /out:"..\Dark Basic Pro\Compiler\TGCOnline.exe"
# ADD LINK32 wininet.lib md5.lib /nologo /subsystem:windows /incremental:yes /map /debug /machine:I386 /out:"..\Dark Basic Pro\Compiler\TGCVerifier.exe"

!ELSEIF  "$(CFG)" == "TGCOnline - Win32 DebugTRIAL"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TGCOnline___Win32_DebugTRIAL0"
# PROP BASE Intermediate_Dir "TGCOnline___Win32_DebugTRIAL0"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "TGCOnline___Win32_DebugTRIAL0"
# PROP Intermediate_Dir "TGCOnline___Win32_DebugTRIAL0"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "TGCTRIAL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wininet.lib md5.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\Dark Basic Pro\Compiler\TGCOnline.exe" /pdbtype:sept
# ADD LINK32 wininet.lib md5.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\Dark Basic Pro\Compiler\TGCOnlineTrial.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "TGCOnline - Win32 ReleaseTRIAL100DAY"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TGCOnline___Win32_ReleaseTRIAL100DAY"
# PROP BASE Intermediate_Dir "TGCOnline___Win32_ReleaseTRIAL100DAY"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "TGCOnline___Win32_ReleaseTRIAL100DAY"
# PROP Intermediate_Dir "TGCOnline___Win32_ReleaseTRIAL100DAY"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "TGCTRIAL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "TGCTRIAL" /D "ONEHUNDREDDAYTRIAL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wininet.lib md5.lib /nologo /subsystem:windows /machine:I386 /out:"..\Dark Basic Pro\Compiler\TGCOnlineTrial.exe"
# ADD LINK32 wininet.lib md5.lib /nologo /subsystem:windows /machine:I386 /out:"..\Dark Basic Pro\Compiler\TGCOnlineTrial.exe"

!ELSEIF  "$(CFG)" == "TGCOnline - Win32 DebugSERIALCODE"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TGCOnline___Win32_DebugSERIALCODE"
# PROP BASE Intermediate_Dir "TGCOnline___Win32_DebugSERIALCODE"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "TGCOnline___Win32_DebugSERIALCODE"
# PROP Intermediate_Dir "TGCOnline___Win32_DebugSERIALCODE"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "TGCSERIALCODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wininet.lib md5.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\Dark Basic Pro\Compiler\TGCOnline.exe" /pdbtype:sept
# ADD LINK32 wininet.lib md5.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\Dark Basic Pro\Compiler\TGCOnlineSerial.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "TGCOnline - Win32 ReleaseSERIALCODE"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TGCOnline___Win32_ReleaseSERIALCODE"
# PROP BASE Intermediate_Dir "TGCOnline___Win32_ReleaseSERIALCODE"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "TGCOnline___Win32_ReleaseSERIALCODE"
# PROP Intermediate_Dir "TGCOnline___Win32_ReleaseSERIALCODE"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "TGCSERIALCODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wininet.lib md5.lib /nologo /subsystem:windows /machine:I386 /out:"..\Dark Basic Pro\Compiler\TGCOnline.exe"
# ADD LINK32 wininet.lib md5.lib /nologo /subsystem:windows /machine:I386 /out:"..\Dark Basic Pro\Compiler\TGCOnlineSerial.exe"

!ENDIF 

# Begin Target

# Name "TGCOnline - Win32 Release"
# Name "TGCOnline - Win32 Debug"
# Name "TGCOnline - Win32 ReleaseFREECERTIFICATES"
# Name "TGCOnline - Win32 ReleaseTRIAL"
# Name "TGCOnline - Win32 ReleaseVERIFIER"
# Name "TGCOnline - Win32 ReleaseVERIFIER_DEBUG"
# Name "TGCOnline - Win32 DebugTRIAL"
# Name "TGCOnline - Win32 ReleaseTRIAL100DAY"
# Name "TGCOnline - Win32 DebugSERIALCODE"
# Name "TGCOnline - Win32 ReleaseSERIALCODE"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ExternalText.cpp
# End Source File
# Begin Source File

SOURCE=.\HTTPComm.cpp
# End Source File
# Begin Source File

SOURCE=.\HTTPComm.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TGCOnline.cpp
# End Source File
# Begin Source File

SOURCE=.\TGCOnlineDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ExternalText.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TGCOnline.h
# End Source File
# Begin Source File

SOURCE=.\TGCOnlineDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\picture.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TGCOnline.ico
# End Source File
# Begin Source File

SOURCE=.\TGCOnline.rc
# End Source File
# Begin Source File

SOURCE=.\res\TGCOnline.rc2
# End Source File
# End Group
# Begin Group "CertificateKey"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CertificateKey.cpp
# End Source File
# Begin Source File

SOURCE=.\CertificateKey.h
# End Source File
# Begin Source File

SOURCE=.\MD5.h
# End Source File
# End Group
# Begin Group "SerialCodeCheck"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SerialCodeCheck\md5.cpp
# End Source File
# Begin Source File

SOURCE=.\SerialCodeCheck\md5.h
# End Source File
# Begin Source File

SOURCE=.\SerialCodeCheck\SerialCodeCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\SerialCodeCheck\SerialCodeCheck.h
# End Source File
# End Group
# End Target
# End Project
