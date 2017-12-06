@echo off
setlocal enabledelayedexpansion

set mypath=%~dp0
:::: set asyle_path= c:\path to astyle

set "AStyleVerMin=3.0.1"
astyle --version 2>NUL || (echo. & echo ERROR: AStyle not found & goto End)
call :SubCheckVer || goto End


if %errorlevel% neq 0 (echo. & echo error: Something went wrong!)

:End
exit /b 1

:SubCheckVer
title Checking astyle version
for /f "tokens=4 delims= " %%A in ('astyle --ascii --version 2^>^&1 NUL') do (
  set "AStyleVer=%%A"
)

if %AStyleVer% lss %AStyleVerMin% (
  echo. & echo error: AStyle v%AStyleVer% is too old, please update AStyle to v%AStyleVerMin% or newer.
  exit /b 1
)
exit /b

:Start
:::: Kazde oddelenie predstavuje jeden folder
::::--------------------------------------
set TargetDir= !mypath!include\
set FilesAll=
for /r %TargetDir% %%f in (*.h) do (set FilesAll=!FilesAll! %%f)
echo astyle %TargetDir%
astyle -options=!mypath!astyle.options %FilesAll%
rem %astyle_path% -options=!mypath!astyle.options %FilesAll%
::::--------------------------------------
set TargetDir= !mypath!src\
set FilesAll=
for /r %TargetDir% %%f in (*.cpp) do (set FilesAll=!FilesAll! %%f)
echo astyle %TargetDir%
astyle -options=!mypath!astyle.options %FilesAll%
:::: %astyle_path% -options=!mypath!astyle.options %FilesAll%
::::--------------------------------------