@rem Builds the distributable
@
@rem -----------------------------------------------
@echo Copying files...
@
@copy c:\msdev\misc\easyb\Release\EasyBridge.exe c:\msdev\misc\easyb\setup\source\ProgramFiles
@copy c:\msdev\misc\easyb\Release\CJ60Lib.dll c:\msdev\misc\easyb\setup\source\ProgramFiles
@copy c:\msdev\misc\easyb\Release\EasyBUtils.dll c:\msdev\misc\easyb\setup\source\ProgramFiles
@copy c:\msdev\misc\easyb\Release\EasyBWizards.dll c:\msdev\misc\easyb\setup\source\ProgramFiles
@copy c:\msdev\misc\easyb\Help\EasyBridge.hlp c:\msdev\misc\easyb\setup\source\ProgramFiles
@ren c:\msdev\misc\easyb\Help\EasyBridge.hlp EasyBridge.hlp
@copy c:\msdev\misc\easyb\Help\EasyBridge.cnt c:\msdev\misc\easyb\setup\source\ProgramFiles
@copy c:\msdev\misc\easyb\readme.doc c:\msdev\misc\easyb\setup\source\ProgramFiles
@copy c:\msdev\misc\easyb\history.doc c:\msdev\misc\easyb\setup\source\ProgramFiles
@
@rem -----------------------------------------------
@echo Assembling the distribution files...
@c:\msdev\VC98\bin\nmake
@
@rem -----------------------------------------------
@rem call the InstallShield EXE builder
@
@echo Building the installation executable...
@start /wait c:\msdev\installshield\Utility\Exebldr\Exebuild c:\msdev\misc\easyb\setup\exebuild.ini
@ren c:\msdev\misc\easyb\setup\distribution\setupex.exe EZBSetup.exe
@
@rem echo Zipping...
@
@rem cd distribution
@rem c:\bin\pkzip -a EZB050 EZBSetup.exe 
@rem cd ..
@
@rem copy to Home Page directory
@echo Copying setup file to web area...
@rem copy distribution\EZBSetup.exe "c:\Misc\Home Page"
@
@echo Done.
