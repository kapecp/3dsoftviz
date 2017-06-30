@echo off
setlocal enabledelayedexpansion

set mypath=%~dp0
set cpplint_path= c:\Python27\Scripts\cpplint.exe

:::: Kazde oddelenie predstavuje jedej folder
::::--------------------------------------
set TargetDir= !mypath!src\Aruco\
echo Dir: %TargetDir%  > cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------------
set TargetDir= !mypath!include\Aruco\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!src\Core\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!include\Core\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!src\Data\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!include\Data\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!src\Importer\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!include\Importer\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!src\Layout\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!include\Layout\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!src\Math\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!include\Math\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!src\Network\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!include\Network\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!src\Fglove\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!include\Fglove\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!src\Kinect\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!include\Kinect\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!src\Manager\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!include\Manager\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!src\Model\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!include\Model\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!src\OsgQtBrowser\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!include\OsgQtBrowser\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!src\QOSG\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!include\QOSG\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!src\Util\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!include\Util\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!src\Leap\
echo Dir: %TargetDir% >> cppcheck-report.txt
echo ---- >> cppcheck-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppcheck %TargetDir%
%cppcheck_path% --enable=warning,style,performance,portability,information,missingInclude --std=c++11 --library=qt.cfg --verbose --quiet -j2 -I !mypath!include/ !FilesAll! >> cppcheck-report.txt 2>&1
::::--------------------------------------
EXIT 0