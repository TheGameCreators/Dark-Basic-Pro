 This folder contains the setup files nessacary 
to install the Creative EAX.dll v2.06 Software 
for the Windows(tm) OS on a users computer. 
Please read the following 4 sections for 
information pertaing to the use of this setup.


***********************************************
1. Utilizing the setup software.

 To utilize this setup application, you will
need to shell execute the setup.exe file from
your applications setup program.
***********************************************


***********************************************
2. Executing a silent setup.

 To achieve a silent setup, utilize the -s
command parameter when executing the setup.exe
program. This will disable all displaying of 
the setup process, and store the results of the 
setup in a file called setup.log.
***********************************************


***********************************************
3. Setup behavior.

 The setup application installs the Eax.dll into
the Windows\System folder. It is installed as a
self-registering component. It will only install
the dll if it does not exist, or if the currently
installed version is the same or below the version
to be installed.

 It is important to follow this behavior if a 
custom installation needs to be created.
***********************************************


***********************************************
4. Installed files.

 The following files will be installed on the
computer after executing this setup application.

File           Version    Date      Location
-------------  ---------  --------  ----------------
Eax.dll           2.06    02/22/01  \Windows\System

***********************************************
