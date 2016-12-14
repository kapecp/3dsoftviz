Návod pre Linux
===============


1. Nainštalovať Synaptic Package Manager

  .. image:: /images/packagemanager.jpg
     :align: center

2. Nainštalovať prostredníctvom Synaptic nasledujúce programy

  * **Git** - git
  * **OpenSceneGraph** - libopenscenegraph-dev
  * **Qt4** - libqt4-dev
  * **QtCreator** - qtcreator
  * **Boost** - libboost-all-dev
  * **Lua** - liblua5.1-0-dev
  * **OpenCV** - libopencv-dev
  * **FreeGlut3**

    * freeglut3
    * freeglut3-dev

  * **CMake** `3.5.0 <https://cmake.org/files/v3.5/cmake-3.5.0.tar.gz>`_

    * inštalovať zo source files, nie Synaptic

.. note:: Názvy knižníc sú ukázané pre Ubuntu

3. Klonovať cez Git projekt 3dsoftviz (AlphaReach klonuje z repozitára Cimox)

	* git clone ** **url repozitára** **
	* git submodule update --init --recursive

4. Zbuildovanie projektu

  * Otvoriť QtCreator
  * File >> Open file or project >> Otvoriť CmakeList.txt v zložke 3dsoftviz
  * Vytvoriť projekt

    * Ak úspešne (dá sa kliknúť na FINISH) >> Kliknúť FINISH
    * Ak neúspešne – Nájsť chybu vo výpise

5. Nastaviť build v QtCreator-i

	Projects >> Build and Run >> Build
	Build Settings >> Add >> Clone Selected >> pomenovať “unity” - automaticky prepne na unity build mode
	Build Steps >> Details >> zaškrtnúť install_unity
	
	
	.. image:: /images/build.jpg
	   :align: center
	
	
	Nastavenie počtu jadier na buildovanie projektu...
	Details >> Additional arguments “-jN”, kde N reprezentuje počet VIRTUÁLNYCH jadier


6. Pridanie Kinectu do projectu

	* Nainštalovať `NiTE <https://drive.google.com/drive/folders/0Bx3FU7Baa_8pdEJESGRJcU1vN1U>`_ – pridať systémové premenné
	* Nainštalovať `OpenNI <https://drive.google.com/drive/folders/0Bx3FU7Baa_8pajVqcHhnOWRJa1k>`_ alebo cez sudo apt-get install – pridať systémové premenné
	* Naištalovať Freenect2 - postupovať podľa <https://github.com/OpenKinect/libfreenect2/blob/master/README.md#linux`_ -je potrebné aby fungoval aj OpenNI test
	.. note:: Ak je potrebný prístup k knižniciam kvôli vývoju tak je potrebné nastaviť premenné FREENECT2_INCLUDE a FREENECT2_LIBRARY

	.. note:: Pre použitie je nutný Kinect for XBOX
	
.. attention:: Na OSX, ak je found OpenNI2 a NiTE2, aplikacia crashne pri spustani s chybovou hlaskou:

               - *not found libNiTE2.dylib*
               - *not found libOpenNI2.dylib*

               Treba tieto kniznice skopirovat z ich domovskych priecinkov (z lib alebo redist).
			   
7. Inštalácia drivera pre 3dmyš

  * Nainštalovať Motif3 pomocou Synaptic Package Manager
  * Pre uistenie, že 3dmyš funguje, skúsiť xapp aplikáciu z drivera
     * Pre spustenie drivera (ktory musi bezat pre pouzivanie mysi) spustite prikaz
       ::

         sudo /etc/3DxWare/daemon/3dxsrv -d usb
     * Pre podrobnejsie instrukcie ohladne instalacie a pouzivania drivera otvorit navod prilozeny v suboroch oficialneho drivera pre Linux "**InstallationInstructions_Linux.txt**"
