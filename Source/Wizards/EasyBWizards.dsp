# Microsoft Developer Studio Project File - Name="EasyBWizards" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=EasyBWizards - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "EasyBWizards.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "EasyBWizards.mak" CFG="EasyBWizards - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "EasyBWizards - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "EasyBWizards - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "EasyBWizards - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Bin\Release"
# PROP Intermediate_Dir "..\Bin\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".." /I "..\Help" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /def:".\EasyBWizards.def" /implib:"..\lib\Release\EasyBWizards.lib" /libpath:"..\Lib\Release"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "EasyBWizards - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Bin\Debug"
# PROP Intermediate_Dir "..\Bin\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".." /I "..\Help" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\EasyBWizards.def" /implib:"..\lib\Debug\EasyBWizardsD.lib" /pdbtype:sept /libpath:"..\Lib\Debug"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "EasyBWizards - Win32 Release"
# Name "EasyBWizards - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BidOptionsPropSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\BidOptsConfigPage.cpp
# End Source File
# Begin Source File

SOURCE=.\BidOptsGeneralPage.cpp
# End Source File
# Begin Source File

SOURCE=.\BidOptsMiscPage.cpp
# End Source File
# Begin Source File

SOURCE=.\BidOptsNoTrumpsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\BidOptsOpenPage.cpp
# End Source File
# Begin Source File

SOURCE=.\BidOptsParamsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\BidOptsTwoBidsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\DealOptionsPropSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\DealOptsMajorsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\DealOptsMinorsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\DealOptsMiscPage.cpp
# End Source File
# Begin Source File

SOURCE=.\DealOptsNTPage.cpp
# End Source File
# Begin Source File

SOURCE=.\DealOptsPointsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\DealOptsSlamPage.cpp
# End Source File
# Begin Source File

SOURCE=.\DispOptionsPropSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\DispOptsCardBacksPage.cpp
# End Source File
# Begin Source File

SOURCE=.\DispOptsCardsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\DispOptsDialogsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\DispOptsFontsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\DispOptsMiscPage.cpp
# End Source File
# Begin Source File

SOURCE=.\DispOptsSuitsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\EasyBWizards.cpp
# End Source File
# Begin Source File

SOURCE=.\EasyBWizards.def
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\EasyBWizards.rc
# End Source File
# Begin Source File

SOURCE=.\GameOptionsPropSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\GameOptsCountingPage.cpp
# End Source File
# Begin Source File

SOURCE=.\GameOptsFilesPage.cpp
# End Source File
# Begin Source File

SOURCE=.\GameOptsGIBPage.cpp
# End Source File
# Begin Source File

SOURCE=.\GameOptsInterfacePage.cpp
# End Source File
# Begin Source File

SOURCE=.\GameOptsMechanicsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\GameOptsScoringPage.cpp
# End Source File
# Begin Source File

SOURCE=..\Globals.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgConfigBiddingPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgConfigFinishPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgConfigGameMechanicsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgConfigHelpLevelPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgConfigIntroPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgConfigPausesPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgConfigSuitsDisplayPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgConfigViewSettingsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgConfigWizardData.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgramConfigWizard.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BidOptionsPropSheet.h
# End Source File
# Begin Source File

SOURCE=.\BidOptsConfigPage.h
# End Source File
# Begin Source File

SOURCE=.\BidOptsGeneralPage.h
# End Source File
# Begin Source File

SOURCE=.\BidOptsMiscPage.h
# End Source File
# Begin Source File

SOURCE=.\BidOptsNoTrumpsPage.h
# End Source File
# Begin Source File

SOURCE=.\BidOptsOpenPage.h
# End Source File
# Begin Source File

SOURCE=.\BidOptsParamsPage.h
# End Source File
# Begin Source File

SOURCE=.\BidOptsTwoBidsPage.h
# End Source File
# Begin Source File

SOURCE=.\DealOptionsPropSheet.h
# End Source File
# Begin Source File

SOURCE=.\DealOptsMajorsPage.h
# End Source File
# Begin Source File

SOURCE=.\DealOptsMinorsPage.h
# End Source File
# Begin Source File

SOURCE=.\DealOptsMiscPage.h
# End Source File
# Begin Source File

SOURCE=.\DealOptsNTPage.h
# End Source File
# Begin Source File

SOURCE=.\DealOptsPointsPage.h
# End Source File
# Begin Source File

SOURCE=.\DealOptsSlamPage.h
# End Source File
# Begin Source File

SOURCE=.\DispOptionsPropSheet.h
# End Source File
# Begin Source File

SOURCE=.\DispOptsCardBacksPage.h
# End Source File
# Begin Source File

SOURCE=.\DispOptsCardsPage.h
# End Source File
# Begin Source File

SOURCE=.\DispOptsDialogsPage.h
# End Source File
# Begin Source File

SOURCE=.\DispOptsFontsPage.h
# End Source File
# Begin Source File

SOURCE=.\DispOptsMiscPage.h
# End Source File
# Begin Source File

SOURCE=.\DispOptsSuitsPage.h
# End Source File
# Begin Source File

SOURCE=.\GameOptionsPropSheet.h
# End Source File
# Begin Source File

SOURCE=.\GameOptsCountingPage.h
# End Source File
# Begin Source File

SOURCE=.\GameOptsFilesPage.h
# End Source File
# Begin Source File

SOURCE=.\GameOptsGIBPage.h
# End Source File
# Begin Source File

SOURCE=.\GameOptsInterfacePage.h
# End Source File
# Begin Source File

SOURCE=.\GameOptsMechanicsPage.h
# End Source File
# Begin Source File

SOURCE=.\GameOptsScoringPage.h
# End Source File
# Begin Source File

SOURCE=.\ProgConfigBiddingPage.h
# End Source File
# Begin Source File

SOURCE=.\ProgConfigFinishPage.h
# End Source File
# Begin Source File

SOURCE=.\ProgConfigGameMechanicsPage.h
# End Source File
# Begin Source File

SOURCE=.\ProgConfigHelpLevelPage.h
# End Source File
# Begin Source File

SOURCE=.\ProgConfigIntroPage.h
# End Source File
# Begin Source File

SOURCE=.\ProgConfigPausesPage.h
# End Source File
# Begin Source File

SOURCE=.\ProgConfigSuitsDisplayPage.h
# End Source File
# Begin Source File

SOURCE=.\ProgConfigViewSettingsPage.h
# End Source File
# Begin Source File

SOURCE=.\ProgConfigWizardData.h
# End Source File
# Begin Source File

SOURCE=.\ProgramConfigWizard.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bmp00002.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bmp00003.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BridgeSceneBB1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BridgeSceneBB2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BridgeSceneBB3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BridgeSceneBB4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BridgeSceneBB5.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\BridgeSceneBB6.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BridgeSceneGG.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BridgeSceneGG1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BridgeSceneGG2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BridgeSceneGG3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BridgeSceneGG4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BridgeSceneGG5.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BridgeSceneGG6.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BridgeSceneGG7.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BridgeSceneGG8.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BridgeSceneGG9.bmp
# End Source File
# Begin Source File

SOURCE=.\res\EasyBWizards.rc2
# End Source File
# Begin Source File

SOURCE=.\Res\suit_ord.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Suit_Order_1.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Suit_Order_2.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Suit_Order_3.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Suit_Order_4.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Suit_Order_5.bmp
# End Source File
# Begin Source File

SOURCE=".\Res\Welcome Screen.bmp"
# End Source File
# End Group
# End Target
# End Project
