#******************* Copyright 1997 by Steven Han.  All rights reserved
#**
#**  Purpose : Makefile for the Easy Bridge.
#**
#*****************************************************************************


#*****************************************************************************
#
# Absolute minimum code required to build the InstallShield script
#
#*****************************************************************************
foo: bar

#
# Macros
#

ISBinPath = C:\MSDEV\InstallShield\Program
sh= $(IEDEV)\bin\ins												   # InstallShield exe's

#
#
#
bar:
	@ echo Clearing staging area...
#	@ del setup.z
#	@ del Install\*.*"
#	@ del Distribution\*.*"
	@ deltree /Y Install
	@ deltree /Y Distribution
	@ md Install
	@ md Distribution
	@ echo Compressing files...
	@ $(ISBinPath)\icomp /h /i Source\*.* setup.z
	@ $(ISBinPath)\packlist setup.lst
#	@ $(ISBinPath)\compile setup.rul	
	@ echo Creating installation...
	@ copy splash.bmp  Install\setup.bmp
	@ copy setup.ini Install\disk1
	@ copy $(ISBinPath)\setup.exe      Install
	@ copy $(ISBinPath)\_setup.dll     Install
	@ copy $(ISBinPath)\_isdel.exe     Install
	@ copy $(ISBinPath)\_inst32i.ex_   Install
	@ copy $(ISBinPath)\_isres.dll   Install
	@ copy $(ISBinPath)\_setup.lib     Install
	@ copy setup.ins Install
	@ move setup.pkg Install
	@ move setup.z Install
	@ echo . > Install\disk1.id
