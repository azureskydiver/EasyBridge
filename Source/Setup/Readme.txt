Notes on Building Easy Bridge
=============================

To use the install scripts in this directory, you'll need InstallShield version 6.

setup.rul, setup.ini, and setup.lst are the files that InstallShield needs.

Exebuild.ini is used by the InstallShield execubilder, a program that creates 
a self-installaing archive .exe.  

There are batch files to help ease the process:

compile.bat -- compiles the setup.rul script file.
Make -- puts together the install files.  You can also run nmake on the Makefile in this directory.
Build.bat -- Does the above, plus create the self-installing archive.
