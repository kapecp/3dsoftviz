Instalation guide
=========

[Arvis] is team project at FIIT STU.

###Requirements

* QT
* OSG
* Make
* Cmake
* OpenCV
* Libnoise
* [OpenNI2]
* Nite2
* [Aruco]
* Kinect Speech
 

###Installation
---------------
####Linux

> Console

```sh
sudo apt-get install make
sudo apt-get install cmake cmake-gui # cmake-qt-gui
sudo apt-get install qt4-dev-tools libqt4-dev libqt4-core libqt4-gui
sudo apt-get install qt-sdk qtcreator
sudo apt-get install openscenegraph libopenscenegraph-dev
sudo apt-get install libpng12-dev # libpng12-0

#opencv 
sudo apt-get install libopencv-dev
sudo apt-get install libopencv-core-dev
```
> or from package manager 

```sh
sudo apt-get install synaptic
```
> Optional - [OpenCV] shell scripts to install different version of OpenCV


> for Aruco and Libnoise

submodule "dependencies/libnoise"

    path = dependencies/libnoise
    url = git@github.com:LuaDist/libnoise.git
	
submodule "dependencies/aruco"

    path = dependencies/aruco
    url = git@github.com:LuaDist/aruco.git


> OpenNi2 and Nite2 - in progress

> Kinect Speech - not posibble

###Problems
[FIX] cmake "CMAKE_CXX_COMPILER-NOTFOUND" error

```sh
sudo apt-get install build-essential -y
```

Aruco - build GL and examples - remove CmakeLists in

    path = dependencies/aruco/utils
    
Libnoise - Windows problem unity build - comment 36 line

    path = dependencies/libnoise/src/basictypes.h
    
###Setup

    path = _build/

> normal build
    
```sh
cmake ..
make
make install
``` 
> unity build

```sh
cmake ..
make 3DSoftviz_unity
make install_unity
``` 
####Run
    
    path = _install/bin/3DSoftviz


License
----

[Arvis]:http://team05-13.ucebne.fiit.stuba.sk/
[Aruco]:https://github.com/rmsalinas/aruco
[OpenNI2]:http://structure.io/openni
[OpenCV]:https://github.com/jayrambhia/Install-OpenCV

