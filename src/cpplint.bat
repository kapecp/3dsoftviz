@echo off
setlocal enabledelayedexpansion

set cpplint_path= c:\Python27\Scripts\cpplint.exe
:: Kazde oddelenie predstavuje jedej folder
::--------------------------------------
set TargetDir= Aruco\
echo Dir: %TargetDir%  > report.txt
echo ---- >> report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo %TargetDir%
%cpplint_path%  %FilesAll% >> report.txt 2>&1
::::--------------------------------
set TargetDir= Core\
echo Dir: %TargetDir%  >> report.txt
echo ---- >> report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo %TargetDir%
%cpplint_path%  %FilesAll% >> report.txt 2>&1
::::--------------------------------
set TargetDir= Data\
echo Dir: %TargetDir%  >> report.txt
echo ---- >> report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo %TargetDir%
%cpplint_path%  %FilesAll% >> report.txt 2>&1
::::--------------------------------
set TargetDir= Importer\
echo Dir: %TargetDir%  >> report.txt
echo ---- >> report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo %TargetDir%
%cpplint_path%  %FilesAll% >> report.txt 2>&1
::::--------------------------------
set TargetDir= Layout\
echo Dir: %TargetDir%  >> report.txt
echo ---- >> report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo %TargetDir%
%cpplint_path%  %FilesAll% >> report.txt 2>&1
::::--------------------------------
set TargetDir= Math\
echo Dir: %TargetDir%  >> report.txt
echo ---- >> report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo %TargetDir%
%cpplint_path%  %FilesAll% >> report.txt 2>&1
::::--------------------------------
set TargetDir= Network\

echo Dir: %TargetDir%  >> report.txt
echo ---- >> report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo %TargetDir%
%cpplint_path%  %FilesAll% >> report.txt 2>&1
::::--------------------------------
set TargetDir= Fglove\
echo Dir: %TargetDir%  >> report.txt
echo ---- >> report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo %TargetDir%
%cpplint_path%  %FilesAll% >> report.txt 2>&1
::::--------------------------------
set TargetDir= Kinect\
echo Dir: %TargetDir%  >> report.txt
echo ---- >> report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo %TargetDir%
%cpplint_path%  %FilesAll% >> report.txt 2>&1
::::--------------------------------
::::set TargetDir= Kinect\RansacSurface\
::set TargetDir= Aruco\
::echo Dir: %TargetDir%  >> report.txt
::echo ---- >> report.txt
::set FilesAll= 
::for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
::%cpplint_path%  %FilesAll% >> report.txt 2>&1
::::--------------------------------
set TargetDir= Manager\
echo Dir: %TargetDir%  >> report.txt
echo ---- >> report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo %TargetDir%
%cpplint_path%  %FilesAll% >> report.txt 2>&1
::::--------------------------------
set TargetDir= Model\
echo Dir: %TargetDir%  >> report.txt
echo ---- >> report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo %TargetDir%
%cpplint_path%  %FilesAll% >> report.txt 2>&1
::::--------------------------------
set TargetDir= OsgQtBrowser\
echo Dir: %TargetDir%  >> report.txt
echo ---- >> report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo %TargetDir%
%cpplint_path%  %FilesAll% >> report.txt 2>&1
::::--------------------------------
set TargetDir= QOSG\
echo Dir: %TargetDir%  >> report.txt
echo ---- >> report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo %TargetDir%
%cpplint_path%  %FilesAll% >> report.txt 2>&1
::::--------------------------------
set TargetDir= Util\
echo Dir: %TargetDir%  >> report.txt
echo ---- >> report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo %TargetDir%
%cpplint_path%  %FilesAll% >> report.txt 2>&1
::::--------------------------------
set TargetDir= !mypath!include\Leap\
echo Dir: %TargetDir%  >> cpplint-report.txt
echo ---- >> cpplint-report.txt
set FilesAll= 
for /r %TargetDir% %%f in (*.h *.cpp) do (set FilesAll=!FilesAll! %%f)
echo cppplint %TargetDir%
%cpplint_path%  %FilesAll% >> cpplint-report.txt 2>&1
::::--------------------------------