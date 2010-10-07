mkdir _build
cd _build
mkdir Windows
cd Windows

mkdir Debug
mkdir Release

mkdir img
mkdir img\arrows
mkdir img\gui
mkdir config

cd Release
mkdir img
mkdir img\arrows
mkdir img\gui
mkdir config

cd ..\Debug
mkdir img
mkdir img\arrows
mkdir img\gui
mkdir config

cd ..\..\..
copy dependencies\Windows\bin\Debug\*.dll _BUILD\Windows\Debug
copy dependencies\Windows\bin\Release\*.dll _BUILD\Windows\Release


copy resources\img _BUILD\Windows\img
copy resources\img\arrows _BUILD\Windows\img\arrows
copy resources\img\gui _BUILD\Windows\img\gui
copy resources\config _BUILD\Windows\config


copy resources\img _BUILD\Windows\Debug\img
copy resources\img\arrows _BUILD\Windows\Debug\img\arrows
copy resources\img\gui _BUILD\Windows\Debug\img\gui
copy resources\config _BUILD\Windows\Debug\config

copy resources\img _BUILD\Windows\Release\img
copy resources\img\arrows _BUILD\Windows\Release\img\arrows
copy resources\img\gui _BUILD\Windows\Release\img\gui
copy resources\config _BUILD\Windows\Release\config

cd _BUILD\Windows
cmake ..\.. -G "Visual Studio 9 2008"

cd ..\.. 