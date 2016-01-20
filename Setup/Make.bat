@ rem #******************* Copyright 1997 by Steven Han.  All rights reserved
@ rem #** 
@ rem #**  Purpose : Makefile for the EasyBridge.
@ rem #**
@ rem #*****************************************************************************

@ echo Clearing staging area...
@ rem del setup.z
@ rem del Install\*.*
@ rem del Distribution\*.*
@ echo Compressing files...
@ C:\MSDEV\InstallShield\Program\icomp /h /i Source\*.* setup.z
@ C:\MSDEV\InstallShield\Program\packlist setup.lst
@ C:\MSDEV\InstallShield\Program\compile setup.rul	
@ echo Creating installation...
@ copy splash.bmp  Install\setup.bmp
@ copy setup.ini Install\disk1
@ copy C:\MSDEV\InstallShield\Program\setup.exe      Install
@ copy C:\MSDEV\InstallShield\Program\_setup.dll     Install
@ copy C:\MSDEV\InstallShield\Program\_isdel.exe     Install
@ copy C:\MSDEV\InstallShield\Program\_inst32i.ex_   Install
@ copy C:\MSDEV\InstallShield\Program\_isres.dll     Install
@ copy C:\MSDEV\InstallShield\Program\_setup.lib     Install
@ copy setup.ins Install
@ move setup.pkg Install
@ move setup.z Install
@ echo . > Install\disk1.id
