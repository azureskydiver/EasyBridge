# Microsoft Developer Studio Project File - Name="EASYB" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=EASYB - WIN32 DEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Easyb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Easyb.mak" CFG="EASYB - WIN32 DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "EASYB - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "EASYB - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "EASYB - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\WinDebug"
# PROP BASE Intermediate_Dir ".\WinDebug"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Bin\Debug"
# PROP Intermediate_Dir "Bin\Debug"
# PROP Ignore_Export_Lib 0
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "Utils" /I "Wizards" /I "Include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fr /Yu"stdafx.h" /FD /c
# ADD MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 version.lib winmm.lib EasyBUtilsD.lib EasyBWizardsD.lib /nologo /debug /machine:I386 /out:"Bin\Debug/EasyBridge.exe" /libpath:"Lib\Debug" /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "EASYB - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\WinRel"
# PROP BASE Intermediate_Dir ".\WinRel"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Bin\Release"
# PROP Intermediate_Dir "Bin\Release"
# PROP Ignore_Export_Lib 0
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /I "Utils" /I "Wizards" /I "Include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 version.lib winmm.lib EasyBUtils.lib EasyBWizards.lib /nologo /machine:I386 /out:"Bin\Release/EasyBridge.exe" /libpath:"Lib\Release" /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "EASYB - Win32 Debug"
# Name "EASYB - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\4thSuitForcingConvention.cpp
# End Source File
# Begin Source File

SOURCE=.\AnalysisDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Artificial2ClubConvention.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoHintDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\BidDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\BiddingFinishedDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\BiddingGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\BiddingOpen.cpp
# End Source File
# Begin Source File

SOURCE=.\BiddingRebid.cpp
# End Source File
# Begin Source File

SOURCE=.\BiddingRebid2.cpp
# End Source File
# Begin Source File

SOURCE=.\BiddingRebidExtended.cpp
# End Source File
# Begin Source File

SOURCE=.\BiddingResponse.cpp
# End Source File
# Begin Source File

SOURCE=.\BidDlgLarge.cpp
# End Source File
# Begin Source File

SOURCE=.\BidDlgSmall.cpp
# End Source File
# Begin Source File

SOURCE=.\BidEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\BlackwoodConvention.cpp
# End Source File
# Begin Source File

SOURCE=.\card.cpp
# End Source File
# Begin Source File

SOURCE=.\CardHoldings.cpp
# End Source File
# Begin Source File

SOURCE=.\CardLayout.cpp
# End Source File
# Begin Source File

SOURCE=.\CardList.cpp
# End Source File
# Begin Source File

SOURCE=.\CardLocation.cpp
# End Source File
# Begin Source File

SOURCE=.\Cash.cpp
# End Source File
# Begin Source File

SOURCE=.\CombinedHoldings.cpp
# End Source File
# Begin Source File

SOURCE=.\CombinedSuitHoldings.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigData.cpp
# End Source File
# Begin Source File

SOURCE=.\Convention.cpp
# End Source File
# Begin Source File

SOURCE=.\ConventionSet.cpp
# End Source File
# Begin Source File

SOURCE=.\CueBidConvention.cpp
# End Source File
# Begin Source File

SOURCE=.\deal.cpp
# End Source File
# Begin Source File

SOURCE=.\DealNumberDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Deck.cpp
# End Source File
# Begin Source File

SOURCE=.\DeclarerPlayEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\DefenderPlayEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\Discard.cpp
# End Source File
# Begin Source File

SOURCE=.\DisplayHoldings.cpp
# End Source File
# Begin Source File

SOURCE=.\drawhand.cpp
# End Source File
# Begin Source File

SOURCE=.\DrawParameters.cpp
# End Source File
# Begin Source File

SOURCE=.\Drop.cpp
# End Source File
# Begin Source File

SOURCE=.\DruryConvention.cpp
# End Source File
# Begin Source File

SOURCE=.\DummyPlayEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\EasyB.cpp
# End Source File
# Begin Source File

SOURCE=.\EasyB.rc
# End Source File
# Begin Source File

SOURCE=.\EasyBdoc.cpp
# End Source File
# Begin Source File

SOURCE=.\EasyBvw.cpp
# End Source File
# Begin Source File

SOURCE=.\EasyBVw2.cpp
# End Source File
# Begin Source File

SOURCE=.\editdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EventProcessor.cpp
# End Source File
# Begin Source File

SOURCE=.\ExitPlay.cpp
# End Source File
# Begin Source File

SOURCE=.\FileComments.cpp
# End Source File
# Begin Source File

SOURCE=.\FilePropertiesDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Finesse.cpp
# End Source File
# Begin Source File

SOURCE=.\Force.cpp
# End Source File
# Begin Source File

SOURCE=.\Gambling3NTConvention.cpp
# End Source File
# Begin Source File

SOURCE=.\GameRecord.cpp
# End Source File
# Begin Source File

SOURCE=.\GameReviewDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\GerberConvention.cpp
# End Source File
# Begin Source File

SOURCE=.\GIB.cpp
# End Source File
# Begin Source File

SOURCE=.\GIBDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\GIBMonitorThread.cpp
# End Source File
# Begin Source File

SOURCE=.\Globals.cpp
# End Source File
# Begin Source File

SOURCE=.\GuessedCard.CPP
# End Source File
# Begin Source File

SOURCE=.\GuessedCardHoldings.cpp
# End Source File
# Begin Source File

SOURCE=.\GuessedHandHoldings.cpp
# End Source File
# Begin Source File

SOURCE=.\GuessedSuitHoldings.cpp
# End Source File
# Begin Source File

SOURCE=.\HandHoldings.cpp
# End Source File
# Begin Source File

SOURCE=.\HistoryWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\HoldUp.cpp
# End Source File
# Begin Source File

SOURCE=.\Jacoby2NTConvention.cpp
# End Source File
# Begin Source File

SOURCE=.\JacobyTransferConvention.cpp
# End Source File
# Begin Source File

SOURCE=.\mainfrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MichaelsCueBidConvention.cpp
# End Source File
# Begin Source File

SOURCE=.\MyCustomDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\myfildlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NegativeDoublesConvention.cpp
# End Source File
# Begin Source File

SOURCE=.\NNetAutotrainStatusDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NNetConfigDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\NNetOutputDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\OvercallsConvention.cpp
# End Source File
# Begin Source File

SOURCE=.\PassedHandDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\PassedHandSimpleDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Play.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\player.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerStatusDialog.CPP
# End Source File
# Begin Source File

SOURCE=.\PlayList.cpp
# End Source File
# Begin Source File

SOURCE=.\print.cpp
# End Source File
# Begin Source File

SOURCE=.\rdfile.cpp
# End Source File
# Begin Source File

SOURCE=.\RdFilePBN.cpp
# End Source File
# Begin Source File

SOURCE=.\RoundFinishedDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Ruff.cpp
# End Source File
# Begin Source File

SOURCE=.\saveopts.cpp
# End Source File
# Begin Source File

SOURCE=.\ScoreDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenSizeWarningDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectHandDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ShutoutBidsConvention.cpp
# End Source File
# Begin Source File

SOURCE=.\SlamConvention.cpp
# End Source File
# Begin Source File

SOURCE=.\SplinterBidsConvention.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusAnalysesPage.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusCardLocationsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusFeedbackPage.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusGIBMonitorPage.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusHoldingsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusPlayPlanPage.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\StaymanConvention.cpp
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StrongTwoBidsConvention.cpp
# End Source File
# Begin Source File

SOURCE=.\subclass.cpp
# End Source File
# Begin Source File

SOURCE=.\SuitHoldings.cpp
# End Source File
# Begin Source File

SOURCE=.\TakeoutDoublesConvention.cpp
# End Source File
# Begin Source File

SOURCE=.\TestPlayDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\TrumpPull.cpp
# End Source File
# Begin Source File

SOURCE=.\Type1Finesse.cpp
# End Source File
# Begin Source File

SOURCE=.\Type2Finesse.cpp
# End Source File
# Begin Source File

SOURCE=.\Type3Finesse.cpp
# End Source File
# Begin Source File

SOURCE=.\TypeAFinesse.cpp
# End Source File
# Begin Source File

SOURCE=.\TypeBFinesse.cpp
# End Source File
# Begin Source File

SOURCE=.\UnusualNTConvention.cpp
# End Source File
# Begin Source File

SOURCE=.\WeakTwoBidsConvention.cpp
# End Source File
# Begin Source File

SOURCE=.\wrfile.cpp
# End Source File
# Begin Source File

SOURCE=.\WrFilePBN.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\4thSuitForcingConvention.h
# End Source File
# Begin Source File

SOURCE=.\AnalysisDialog.h
# End Source File
# Begin Source File

SOURCE=.\Artificial2ClubConvention.h
# End Source File
# Begin Source File

SOURCE=.\AutoHintDialog.h
# End Source File
# Begin Source File

SOURCE=.\BidDialog.h
# End Source File
# Begin Source File

SOURCE=.\BiddingFinishedDialog.h
# End Source File
# Begin Source File

SOURCE=.\BidDlgLarge.h
# End Source File
# Begin Source File

SOURCE=.\BidDlgSmall.h
# End Source File
# Begin Source File

SOURCE=.\BidEngine.H
# End Source File
# Begin Source File

SOURCE=.\Bidopts.h
# End Source File
# Begin Source File

SOURCE=.\Bidparams.H
# End Source File
# Begin Source File

SOURCE=.\BlackwoodConvention.h
# End Source File
# Begin Source File

SOURCE=.\CARD.H
# End Source File
# Begin Source File

SOURCE=.\CardHoldings.H
# End Source File
# Begin Source File

SOURCE=.\CardLayout.h
# End Source File
# Begin Source File

SOURCE=.\CardList.H
# End Source File
# Begin Source File

SOURCE=.\CardLocation.H
# End Source File
# Begin Source File

SOURCE=.\cardopts.h
# End Source File
# Begin Source File

SOURCE=.\Cash.h
# End Source File
# Begin Source File

SOURCE=.\CombinedHoldings.H
# End Source File
# Begin Source File

SOURCE=.\CombinedSuitHoldings.H
# End Source File
# Begin Source File

SOURCE=.\ConfigData.h
# End Source File
# Begin Source File

SOURCE=.\ConvCodes.h
# End Source File
# Begin Source File

SOURCE=.\Convention.h
# End Source File
# Begin Source File

SOURCE=.\ConventionSet.h
# End Source File
# Begin Source File

SOURCE=.\CueBidConvention.h
# End Source File
# Begin Source File

SOURCE=.\DealNumberDialog.h
# End Source File
# Begin Source File

SOURCE=.\DEALPARM.H
# End Source File
# Begin Source File

SOURCE=.\Deck.H
# End Source File
# Begin Source File

SOURCE=.\deckopts.h
# End Source File
# Begin Source File

SOURCE=.\DeclarerPlayEngine.H
# End Source File
# Begin Source File

SOURCE=.\DefenderPlayEngine.H
# End Source File
# Begin Source File

SOURCE=.\DEFINES.H
# End Source File
# Begin Source File

SOURCE=.\DialogInfo.h
# End Source File
# Begin Source File

SOURCE=.\Discard.h
# End Source File
# Begin Source File

SOURCE=.\DisplayHoldings.H
# End Source File
# Begin Source File

SOURCE=.\docopts.h
# End Source File
# Begin Source File

SOURCE=.\DrawParameters.h
# End Source File
# Begin Source File

SOURCE=.\Drop.h
# End Source File
# Begin Source File

SOURCE=.\DruryConvention.h
# End Source File
# Begin Source File

SOURCE=.\DummyPlayEngine.H
# End Source File
# Begin Source File

SOURCE=.\EasyB.h
# End Source File
# Begin Source File

SOURCE=.\EasyBdoc.h
# End Source File
# Begin Source File

SOURCE=.\EasyBvw.h
# End Source File
# Begin Source File

SOURCE=.\editdlg.h
# End Source File
# Begin Source File

SOURCE=.\EventProcessor.h
# End Source File
# Begin Source File

SOURCE=.\ExitPlay.h
# End Source File
# Begin Source File

SOURCE=.\FEEDBACK.H
# End Source File
# Begin Source File

SOURCE=.\FILECODE.H
# End Source File
# Begin Source File

SOURCE=.\FileCodePBN.H
# End Source File
# Begin Source File

SOURCE=.\FileComments.h
# End Source File
# Begin Source File

SOURCE=.\FilePropertiesDialog.h
# End Source File
# Begin Source File

SOURCE=.\Finesse.h
# End Source File
# Begin Source File

SOURCE=.\Force.h
# End Source File
# Begin Source File

SOURCE=.\Gambling3NTConvention.h
# End Source File
# Begin Source File

SOURCE=.\GameRecord.h
# End Source File
# Begin Source File

SOURCE=.\GameReviewDialog.h
# End Source File
# Begin Source File

SOURCE=.\GerberConvention.h
# End Source File
# Begin Source File

SOURCE=.\GIB.h
# End Source File
# Begin Source File

SOURCE=.\GIBDialog.h
# End Source File
# Begin Source File

SOURCE=.\GIBMonitorThread.h
# End Source File
# Begin Source File

SOURCE=.\Globals.h
# End Source File
# Begin Source File

SOURCE=.\GuessedCard.H
# End Source File
# Begin Source File

SOURCE=.\GuessedCardHoldings.H
# End Source File
# Begin Source File

SOURCE=.\GuessedHandHoldings.H
# End Source File
# Begin Source File

SOURCE=.\GuessedSuitHoldings.H
# End Source File
# Begin Source File

SOURCE=.\HandHoldings.H
# End Source File
# Begin Source File

SOURCE=.\Handopts.h
# End Source File
# Begin Source File

SOURCE=.\HistoryWnd.h
# End Source File
# Begin Source File

SOURCE=.\HoldUp.h
# End Source File
# Begin Source File

SOURCE=.\Jacoby2NTConvention.h
# End Source File
# Begin Source File

SOURCE=.\JacobyTransferConvention.h
# End Source File
# Begin Source File

SOURCE=.\MainFrameopts.h
# End Source File
# Begin Source File

SOURCE=.\mainfrm.h
# End Source File
# Begin Source File

SOURCE=.\MichaelsCueBidConvention.h
# End Source File
# Begin Source File

SOURCE=.\MyCustomDialog.h
# End Source File
# Begin Source File

SOURCE=.\MyException.h
# End Source File
# Begin Source File

SOURCE=.\MYFILDLG.H
# End Source File
# Begin Source File

SOURCE=.\NegativeDoublesConvention.h
# End Source File
# Begin Source File

SOURCE=.\NNetAutotrainStatusDlg.h
# End Source File
# Begin Source File

SOURCE=.\NNetConfigDialog.h
# End Source File
# Begin Source File

SOURCE=.\NNetOutputDialog.h
# End Source File
# Begin Source File

SOURCE=.\ObjectWithProperties.h
# End Source File
# Begin Source File

SOURCE=.\OptimizerThread.h
# End Source File
# Begin Source File

SOURCE=.\OvercallsConvention.h
# End Source File
# Begin Source File

SOURCE=.\PassedHandDialog.h
# End Source File
# Begin Source File

SOURCE=.\PassedHandSimpleDialog.h
# End Source File
# Begin Source File

SOURCE=.\Play.h
# End Source File
# Begin Source File

SOURCE=.\PlayEngine.H
# End Source File
# Begin Source File

SOURCE=.\PLAYER.H
# End Source File
# Begin Source File

SOURCE=.\playeropts.h
# End Source File
# Begin Source File

SOURCE=.\PlayerStatusDialog.H
# End Source File
# Begin Source File

SOURCE=.\PlayList.h
# End Source File
# Begin Source File

SOURCE=.\progopts.h
# End Source File
# Begin Source File

SOURCE=.\roundfinisheddialog.h
# End Source File
# Begin Source File

SOURCE=.\Ruff.h
# End Source File
# Begin Source File

SOURCE=.\saveopts.h
# End Source File
# Begin Source File

SOURCE=.\ScoreDialog.h
# End Source File
# Begin Source File

SOURCE=.\ScreenSizeWarningDlg.h
# End Source File
# Begin Source File

SOURCE=.\SelectHandDialog.h
# End Source File
# Begin Source File

SOURCE=.\ShutoutBidsConvention.h
# End Source File
# Begin Source File

SOURCE=.\SlamConvention.h
# End Source File
# Begin Source File

SOURCE=.\Sluff.h
# End Source File
# Begin Source File

SOURCE=.\SplinterBidsConvention.h
# End Source File
# Begin Source File

SOURCE=.\StatusAnalysesPage.h
# End Source File
# Begin Source File

SOURCE=.\StatusCardLocationsPage.h
# End Source File
# Begin Source File

SOURCE=.\StatusFeedbackPage.h
# End Source File
# Begin Source File

SOURCE=.\StatusGIBMonitorPage.h
# End Source File
# Begin Source File

SOURCE=.\StatusHoldingsPage.h
# End Source File
# Begin Source File

SOURCE=.\StatusPlayPlanPage.h
# End Source File
# Begin Source File

SOURCE=.\StatusSheet.h
# End Source File
# Begin Source File

SOURCE=.\StatusWnd.h
# End Source File
# Begin Source File

SOURCE=.\StaymanConvention.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\StrongTwoBidsConvention.h
# End Source File
# Begin Source File

SOURCE=.\SUBCLASS.H
# End Source File
# Begin Source File

SOURCE=.\SuitHoldings.H
# End Source File
# Begin Source File

SOURCE=.\TakeoutDoublesConvention.h
# End Source File
# Begin Source File

SOURCE=.\TestPlayDialog.h
# End Source File
# Begin Source File

SOURCE=.\TrumpPull.h
# End Source File
# Begin Source File

SOURCE=.\Type1Finesse.h
# End Source File
# Begin Source File

SOURCE=.\Type2Finesse.h
# End Source File
# Begin Source File

SOURCE=.\Type3Finesse.h
# End Source File
# Begin Source File

SOURCE=.\TypeAFinesse.h
# End Source File
# Begin Source File

SOURCE=.\TypeBFinesse.h
# End Source File
# Begin Source File

SOURCE=.\UnusualNTConvention.h
# End Source File
# Begin Source File

SOURCE=.\viewopts.h
# End Source File
# Begin Source File

SOURCE=.\WeakTwoBidsConvention.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Res\1.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\10.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\11.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\12.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\13.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\14.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\15.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\16.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\17.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\18.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\19.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\2.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\20.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\21.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\22.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\23.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\24.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\25.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\26.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\27.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\28.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\29.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\3.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\30.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\31.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\32.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\33.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\34.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\35.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\36.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\37.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\38.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\39.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\4.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\40.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\41.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\42.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\43.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\44.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\45.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\46.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\47.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\48.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\49.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\5.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\50.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\51.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\52.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\6.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\7.BMP
# End Source File
# Begin Source File

SOURCE=.\78.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\8.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\9.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\about_pr.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\about_pr2.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\alert.ico
# End Source File
# Begin Source File

SOURCE=.\Res\analysis_small.ico
# End Source File
# Begin Source File

SOURCE=.\Res\bcardback14.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\BITMAP1.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\bitmap3.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bitmap4.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bmp00036.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\BridgeScene.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\BridgeScene2.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\BridgeSceneGG.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\CARDBACK.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\cardback0.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cardback1.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cardback10.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cardback11.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cardback12.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cardback13.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cardback14.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cardback15.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cardback16.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cardback17.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cardback18.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cardback19.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cardback2.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cardback20.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cardback21.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cardback22.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cardback23.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cardback3.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cardback4.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cardback5.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cardback6.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cardback7.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cardback8.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cardback9.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cardbacks.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\CARDMASK.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\cardmask_s.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\click_mouse.cur
# End Source File
# Begin Source File

SOURCE=.\Res\ComputerBidPrompt.ico
# End Source File
# Begin Source File

SOURCE=.\Res\contract.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ctl_imag.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\CUR00001.CUR
# End Source File
# Begin Source File

SOURCE=.\Res\CUR00002.CUR
# End Source File
# Begin Source File

SOURCE=.\Res\CUR00003.CUR
# End Source File
# Begin Source File

SOURCE=.\Res\CUR00004.CUR
# End Source File
# Begin Source File

SOURCE=.\Res\CUR00005.CUR
# End Source File
# Begin Source File

SOURCE=.\Res\CURSOR1.CUR
# End Source File
# Begin Source File

SOURCE=.\Res\CURSOR_E.CUR
# End Source File
# Begin Source File

SOURCE=.\Res\cursor_e_invalid.cur
# End Source File
# Begin Source File

SOURCE=.\Res\cursor_exchange.cur
# End Source File
# Begin Source File

SOURCE=.\Res\cursor_grab.cur
# End Source File
# Begin Source File

SOURCE=.\Res\cursor_h.cur
# End Source File
# Begin Source File

SOURCE=.\Res\cursor_hand.cur
# End Source File
# Begin Source File

SOURCE=.\Res\cursor_hand_illegal.cur
# End Source File
# Begin Source File

SOURCE=.\Res\cursor_hand_open.cur
# End Source File
# Begin Source File

SOURCE=.\Res\CURSOR_N.CUR
# End Source File
# Begin Source File

SOURCE=.\Res\cursor_n_invalid.cur
# End Source File
# Begin Source File

SOURCE=.\Res\CURSOR_S.CUR
# End Source File
# Begin Source File

SOURCE=.\Res\cursor_s_invalid.cur
# End Source File
# Begin Source File

SOURCE=.\Res\CURSOR_W.CUR
# End Source File
# Begin Source File

SOURCE=.\Res\cursor_w_invalid.cur
# End Source File
# Begin Source File

SOURCE=.\Res\default_background.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\EASYB.ICO
# End Source File
# Begin Source File

SOURCE=.\Res\EASYB.RC2
# End Source File
# Begin Source File

SOURCE=.\Res\expand1.ico
# End Source File
# Begin Source File

SOURCE=.\Res\H_point.cur
# End Source File
# Begin Source File

SOURCE=.\Res\H_pointOpen.cur
# End Source File
# Begin Source File

SOURCE=.\Res\hidden_toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ICO00001.ICO
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Res\ico00002.ico
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Res\ico00003.ico
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Res\ico00004.ico
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Res\ico00005.ico
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Res\ico00006.ico
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Res\ico00007.ico
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Res\ico00008.ico
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Res\ico00009.ico
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Res\ico00010.ico
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Res\ico00011.ico
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Res\ico00012.ico
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Res\ico00013.ico
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Res\ico00014.ico
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Res\ico00015.ico
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Res\ico00016.ico
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Res\ico00017.ico
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Res\ico00018.ico
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Res\ico00019.ico
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Res\ico00020.ico
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Res\ico00021.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00022.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00023.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00024.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00025.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00026.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00027.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00028.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00029.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00030.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00031.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00032.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00033.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00034.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00035.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00036.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00037.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00038.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00039.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00040.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00041.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_1c.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_1d.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_1h.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_1nt.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_1s.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_2c.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_2d.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_2h.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_2nt.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_2s.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_3c.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_3d.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_3h.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_3nt.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_3s.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_4c.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_4d.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_4h.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_4nt.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_4s.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_5c.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_5d.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_5h.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_5nt.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_5s.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_6c.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_6d.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_6h.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_6nt.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_6s.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_7c.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_7d.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_7h.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_7nt.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_7s.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_collapse.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_expand.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico_play_history.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ICON1.ICO
# End Source File
# Begin Source File

SOURCE=.\Res\idbs_car.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\idr_clos.ico
# End Source File
# Begin Source File

SOURCE=.\Res\IDR_MAIN.ICO
# End Source File
# Begin Source File

SOURCE=.\Res\IDR_PLAY.ICO
# End Source File
# Begin Source File

SOURCE=.\Res\information.ico
# End Source File
# Begin Source File

SOURCE=.\Res\introduction.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\level_1.ico
# End Source File
# Begin Source File

SOURCE=.\Res\level_2.ico
# End Source File
# Begin Source File

SOURCE=.\Res\level_3.ico
# End Source File
# Begin Source File

SOURCE=.\Res\level_4.ico
# End Source File
# Begin Source File

SOURCE=.\Res\level_5.ico
# End Source File
# Begin Source File

SOURCE=.\Res\level_6.ico
# End Source File
# Begin Source File

SOURCE=.\Res\level_7.ico
# End Source File
# Begin Source File

SOURCE=.\Res\losers.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\losers_small.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\mainfram.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\mainframe_toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\play_east.cur
# End Source File
# Begin Source File

SOURCE=.\Res\play_north.cur
# End Source File
# Begin Source File

SOURCE=.\Res\play_south.cur
# End Source File
# Begin Source File

SOURCE=.\Res\play_too.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\play_toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\play_west.cur
# End Source File
# Begin Source File

SOURCE=.\Res\PlayerBidPrompt.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Portrait.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ProgramTitle.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\review_f.ico
# End Source File
# Begin Source File

SOURCE=.\Res\review_first.ico
# End Source File
# Begin Source File

SOURCE=.\Res\review_first_game.ico
# End Source File
# Begin Source File

SOURCE=.\Res\review_l.ico
# End Source File
# Begin Source File

SOURCE=.\Res\review_last.ico
# End Source File
# Begin Source File

SOURCE=.\Res\review_last_game.ico
# End Source File
# Begin Source File

SOURCE=.\Res\review_n.ico
# End Source File
# Begin Source File

SOURCE=.\Res\review_next.ico
# End Source File
# Begin Source File

SOURCE=.\Res\review_next_game.ico
# End Source File
# Begin Source File

SOURCE=.\Res\review_p.ico
# End Source File
# Begin Source File

SOURCE=.\Res\review_prev.ico
# End Source File
# Begin Source File

SOURCE=.\Res\review_prev_game.ico
# End Source File
# Begin Source File

SOURCE=.\Res\s_cardback0.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\s_cardback1.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\s_cardback2.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sc1.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sc10.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sc11.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sc12.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sc13.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sc2.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sc3.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sc4.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sc5.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sc6.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sc7.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sc8.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sc9.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sd1.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sd10.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sd11.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sd12.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sd13.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sd2.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sd3.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sd4.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sd5.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sd6.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sd7.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sd8.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sd9.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\secondary_toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sh1.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sh10.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sh11.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sh12.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sh13.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sh2.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sh3.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sh4.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sh5.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sh6.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sh7.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sh8.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\sh9.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\SPLASH.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\Splash16.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ss1.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ss10.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ss11.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ss12.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ss13.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ss2.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ss3.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ss4.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ss5.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ss6.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ss7.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ss8.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ss9.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\suit_club.ico
# End Source File
# Begin Source File

SOURCE=.\Res\suit_diamond.ico
# End Source File
# Begin Source File

SOURCE=.\Res\suit_heart.ico
# End Source File
# Begin Source File

SOURCE=.\Res\suit_nt.ico
# End Source File
# Begin Source File

SOURCE=.\Res\suit_spade.ico
# End Source File
# Begin Source File

SOURCE=.\Res\suits.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\swap_car.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\title.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\TOOLBAR.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\toolbar_.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\winners.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\winners_small.bmp
# End Source File
# End Group
# End Target
# End Project
