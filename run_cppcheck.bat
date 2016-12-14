@echo off
setlocal enabledelayedexpansion
set cppcheck_path= d:\timak\cppcheck\cppcheck.exe 
set mypath=%~dp0

::::--------------------------------------
set TargetDir= !mypath!src\Aruco\
echo Dir: %TargetDir% > cppcheck-report.txt
echo ---- >> cppcheck-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppcheck %TargetDir%
%cppcheck_path% --enable=warning,style,performance,portability,information,missingInclude --std=c++11 --library=qt.cfg --verbose --quiet -j2 -I !mypath!include/ !FilesAll! >> cppcheck-report.txt 2>&1
::::--------------------------------------
set TargetDir= !mypath!src\Core\
echo Dir: %TargetDir% >> cppcheck-report.txt
echo ---- >> cppcheck-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppcheck %TargetDir%
%cppcheck_path% --enable=warning,style,performance,portability,information,missingInclude --std=c++11 --library=qt.cfg --verbose --quiet -j2 -I !mypath!include/ !FilesAll! >> cppcheck-report.txt 2>&1
::::--------------------------------------
set TargetDir= !mypath!src\Data\
echo Dir: %TargetDir% >> cppcheck-report.txt
echo ---- >> cppcheck-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppcheck %TargetDir%
%cppcheck_path% --enable=warning,style,performance,portability,information,missingInclude --std=c++11 --library=qt.cfg --verbose --quiet -j2 -I !mypath!include/ !FilesAll! >> cppcheck-report.txt 2>&1
::::--------------------------------------
set TargetDir= !mypath!src\Importer\
echo Dir: %TargetDir% >> cppcheck-report.txt
echo ---- >> cppcheck-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppcheck %TargetDir%
%cppcheck_path% --enable=warning,style,performance,portability,information,missingInclude --std=c++11 --library=qt.cfg --verbose --quiet -j2 -I !mypath!include/ !FilesAll! >> cppcheck-report.txt 2>&1
::::--------------------------------------
set TargetDir= !mypath!src\Layout\
echo Dir: %TargetDir% >> cppcheck-report.txt
echo ---- >> cppcheck-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppcheck %TargetDir%
%cppcheck_path% --enable=warning,style,performance,portability,information,missingInclude --std=c++11 --library=qt.cfg --verbose --quiet -j2 -I !mypath!include/ !FilesAll! >> cppcheck-report.txt 2>&1
::::--------------------------------------
set TargetDir= !mypath!src\Math\
echo Dir: %TargetDir% >> cppcheck-report.txt
echo ---- >> cppcheck-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppcheck %TargetDir%
%cppcheck_path% --enable=warning,style,performance,portability,information,missingInclude --std=c++11 --library=qt.cfg --verbose --quiet -j2 -I !mypath!include/ !FilesAll! >> cppcheck-report.txt 2>&1
::::--------------------------------------
set TargetDir= !mypath!src\Network\
echo Dir: %TargetDir% >> cppcheck-report.txt
echo ---- >> cppcheck-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppcheck %TargetDir%
%cppcheck_path% --enable=warning,style,performance,portability,information,missingInclude --std=c++11 --library=qt.cfg --verbose --quiet -j2 -I !mypath!include/ !FilesAll! >> cppcheck-report.txt 2>&1
::::--------------------------------------
set TargetDir= !mypath!src\Fglove\
echo Dir: %TargetDir% >> cppcheck-report.txt
echo ---- >> cppcheck-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppcheck %TargetDir%
%cppcheck_path% --enable=warning,style,performance,portability,information,missingInclude --std=c++11 --library=qt.cfg --verbose --quiet -j2 -I !mypath!include/ !FilesAll! >> cppcheck-report.txt 2>&1
::::--------------------------------------
set TargetDir= !mypath!src\Kinect\
echo Dir: %TargetDir% >> cppcheck-report.txt
echo ---- >> cppcheck-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppcheck %TargetDir%
%cppcheck_path% --enable=warning,style,performance,portability,information,missingInclude --std=c++11 --library=qt.cfg --verbose --quiet -j2 -I !mypath!include/ !FilesAll! >> cppcheck-report.txt 2>&11
::::--------------------------------------
set TargetDir= !mypath!src\Manager\
echo Dir: %TargetDir% >> cppcheck-report.txt
echo ---- >> cppcheck-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppcheck %TargetDir%
%cppcheck_path% --enable=warning,style,performance,portability,information,missingInclude --std=c++11 --library=qt.cfg --verbose --quiet -j2 -I !mypath!include/ !FilesAll! >> cppcheck-report.txt 2>&1
::::--------------------------------------
set TargetDir= !mypath!src\Model\
echo Dir: %TargetDir% >> cppcheck-report.txt
echo ---- >> cppcheck-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppcheck %TargetDir%
%cppcheck_path% --enable=warning,style,performance,portability,information,missingInclude --std=c++11 --library=qt.cfg --verbose --quiet -j2 -I !mypath!include/ !FilesAll! >> cppcheck-report.txt 2>&1
::::--------------------------------------
set TargetDir= !mypath!src\OsgQtBrowser\
echo Dir: %TargetDir% >> cppcheck-report.txt
echo ---- >> cppcheck-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppcheck %TargetDir%
%cppcheck_path% --enable=warning,style,performance,portability,information,missingInclude --std=c++11 --library=qt.cfg --verbose --quiet -j2 -I !mypath!include/ !FilesAll! >> cppcheck-report.txt 2>&1
::::--------------------------------------
set TargetDir= !mypath!src\QOSG\
echo Dir: %TargetDir% >> cppcheck-report.txt
echo ---- >> cppcheck-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppcheck %TargetDir%
%cppcheck_path% --enable=warning,style,performance,portability,information,missingInclude --std=c++11 --library=qt.cfg --verbose --quiet -j2 -I !mypath!include/ !FilesAll! >> cppcheck-report.txt 2>&1
::::--------------------------------------
set TargetDir= !mypath!src\Util\
echo Dir: %TargetDir% >> cppcheck-report.txt
echo ---- >> cppcheck-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppcheck %TargetDir%
%cppcheck_path% --enable=warning,style,performance,portability,information,missingInclude --std=c++11 --library=qt.cfg --verbose --quiet -j2 -I !mypath!include/ !FilesAll! >> cppcheck-report.txt 2>&1
::::--------------------------------------
EXIT 0