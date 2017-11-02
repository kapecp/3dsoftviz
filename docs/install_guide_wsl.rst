Install guide pre Windows Subsystem for Linux (WSL)
===================================================


Inštalácia WSL
--------------

.. note::  Inštálácia WSL je možná len pre 64-bitovú verziu Windows 10 a build 1607+

1.  Spustiť **PowerShell** ako **administrátor** a spustiť nasledujúci príkaz:

::

	Enable-WindowsOptionalFeature -Online -FeatureName Microsoft-Windows-Subsystem-Linux

2. Spustiť **CMD** a spustiť: 
   
::
	
	bash

3. Potvrdiť inštaláciu a nastaviť meno a heslo

4. WSL nepodporuje defaultne GUI aplikácie, no ich obraz sa dá exportovať na XServer:

	* nainštalovať Xlaunch (alebo iný)
	* v nastaveniach nastaviť port výstupného displeja
	* v terminály zadať export DISPLAY=:*port* (ten ktorý sme nastavili v Xlaunch)
   
   

Inštalácia projektu
-------------------

1. Nainštalovať prostredníctvom Synaptic nasledujúce programy

	* **Git** - git
	* **OpenSceneGraph** - libopenscenegraph-dev
	* **Boost** - libboost-all-dev
	* **Lua** - liblua5.1-0-dev
	* **OpenCV** - libopencv-dev
	* **FreeGlut3**
    	* freeglut3
    	* freeglut3-dev

	* **CMake** `3.5.0 <https://cmake.org/files/v3.5/cmake-3.5.0.tar.gz>`_
    		* inštalovať zo source files, nie Synaptic

2. Nainštalovať Qt5 cez ppa (cez Synaptic je dostupná len verzia 4):

::
	 
	sudo add-apt-repository ppa:beineri/opt-qt591-xenial
	sudo apt-get update
	sudo apt-get install qt59-meta-full


3. Pre správne fungovanie je potrebné nastaviť flag pre spustiteľnosť knižníc Qt príkazom:

	* sudo execstack -c /opt/qt59/lib/*
	* v súčasnej verzii execstack nie je dostupný, treba ho najskôr nainštalovať

4. Klonovať cez Git projekt 3dsoftvizQt

	* git clone ** **url repozitára** **
	* git submodule update --init --recursive

5. Prepnúť sa do aktuálnej vetvy: 

	* git checkout *vetva*
	* git submodule update --init --recursive

6. Zbuildovanie projektu

	* Otvoriť QtCreator príkazom: 

  	:: 

  		sudo /./opt/qt59/bin/qtcreator


	* V QtCreator File >> Open file or project >> Otvoriť CmakeList.txt v zložke 3dsoftviz
	* Vytvoriť projekt
		* Ak úspešne (dá sa kliknúť na FINISH) >> Kliknúť FINISH
		* Ak neúspešne – Nájsť chybu vo výpise
      
7. Nastaviť build v QtCreator-i

	Projects >> Build and Run >> Build
	Build Settings >> Add >> Clone Selected >> pomenovať “unity” - automaticky prepne na unity build mode
	Build Steps >> Details >> zaškrtnúť install_unity
	
	Nastavenie počtu jadier na buildovanie projektu...
	Details >> Additional arguments “-jN”, kde N reprezentuje počet VIRTUÁLNYCH jadier

8. V Tools >> Options >> Build & Run >> QtVersions je potrebné cez *Add* pridať /opt/qt59/bin/qmake
9. V Tools >> Options >> Build & Run >> Kits je potrebné nastaviť verziu Qt5.9..
10. Spustiť buildovanie
11. Po buildovaní v Projects >> Run >> Add >> Custom Executable nastaviť 

::

	Executable: {project_dir}/_install/bin/3DSoftviz
	Working directory: {project_dir}/_install/bin

12. Spustiť projekt
