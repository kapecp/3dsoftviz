.. _ref_install_guide_vc10:

Inštalačný návod (VC10)
=======================
v1.5 (17. 11. 2015)

.. toctree::
   :maxdepth: 1

   install_guide_changelog
   install_guide_errors

Návod pre Windows
-----------------

Tento návod bol úspešne otestovaný na operačných systémoch Windows 7, 8, 8.1, 10.

Potrebný softvér
****************

Na inštaláciu potrebujeme klonovať projekt `3DSoftViz <https://github.com/dominikhorniak/RealityNotFound>`_ (Tím č.4 klonuje z `/cimox/3dsoftviz <https://github.com/cimox/3dsoftviz>`_) z Githubu a stiahnuť nasledovné:

  - CMake `(v3.5.0) <https://cmake.org/download/>`_
  - OpenSceneGraph *(jeden z nasledujúcich)*

    - OpenSceneGraph `(source) <https://github.com/openscenegraph/OpenSceneGraph/tree/OpenSceneGraph-3.4>`_ - iba zdrojové súbory -> treba buildnúť (cca 40-50min)
    - OpenSceneGraph `(v3.4.0) <https://mega.nz/#!cIpy2TZQ!4k29x33yVvQDJRXT8B1J_3F-L1t30B3A2Z-uoQF9OeU>`_ - buildnuté (17.10.2015)
    - OpenSceneGraph `(v3.2.1) <https://drive.google.com/folderview?id=0B_DYSHDPz1SnZXoyNzhEZWJ6M00&usp=sharing&tid=0Bx3FU7Baa_8pZHI3NmxrRGdwVEE#list>`_

  - Kinect for Windows `SDK 1.8 <https://www.microsoft.com/en-us/download/details.aspx?id=40278>`_
  - Microsoft VisualStudio 2010 `SP1 <https://www.microsoft.com/en-us/download/details.aspx?id=23691>`_ **(okrem Express edition!)**
  - Qt `(v4.8.5) <http://ftp.fau.de/qtproject/archive/qt/4.8/4.8.5/qt-win-opensource-4.8.5-vs2010.exe>`_
  - QtCreator `(v3.6.1) <http://ftp.fau.de/qtproject/official_releases/qtcreator/3.6/3.6.1/qt-creator-opensource-windows-x86-3.6.1.exe>`_
  - OpenCV `(v2.4.10) <https://sourceforge.net/projects/opencvlibrary/files/opencv-win/2.4.10/opencv-2.4.10.exe/download>`_
  - Boost `(v1.57.0) <https://sourceforge.net/projects/boost/files/boost-binaries/1.57.0/boost_1_57_0-msvc-10.0-32.exe/download>`_
  - `RapidEE <http://www.rapidee.com/en/download>`_ - program na prácu s premennými prostredia
  - Inštalácia knižnice 3rd party dependencies `VC10 <https://drive.google.com/folderview?id=0B_DYSHDPz1SnR1h6ZWc3dTA5ZjA&usp=sharing&tid=0Bx3FU7Baa_8pZHI3NmxrRGdwVEE>`_
  - `OpenNI2 <https://drive.google.com/open?id=0B_DYSHDPz1SndC1yWkZBUUlzZTQ>`_
  - `NiTE2 <https://drive.google.com/open?id=0B_DYSHDPz1SnWThlV3laenpGb0E>`_
  - Debugging Tools for Windows

    - WinDbg - `Win 8.1 <http://download.microsoft.com/download/B/0/C/B0C80BA3-8AD6-4958-810B-6882485230B5/standalonesdk/sdksetup.exe>`_
    - WinDbg - `Win 10 <http://download.microsoft.com/download/E/1/F/E1F1E61E-F3C6-4420-A916-FB7C47FBC89E/standalonesdk/sdksetup.exe>`_

  - `FreeGlut <https://drive.google.com/open?id=0B3covCFoz_c0allNNjhFcGhuV1k>`_

Postup inštalácie
*****************

 #. Nainštalovať CMake. (Cesta je v dokumente označená ako *%CMAKE_DIR%*)
 #. Nainštalovať Qt (*%QT_DIR%*)
 #. Nainštalovať QtCreator do zložky Qt
 #. Vytvoriť zložku OpenSceneGraph (*%OSG_DIR%*) a *%OSG_DIR%*/ThirdParty
 #. Rozbaliť 3rd Party Knižnice (VC10) do *%OSG_DIR%*/ThirdParty/
 #. Rozbaliť FreeGlut do *%OSG_DIR%*/ThirdParty/VC10/x86
 #. V prípade stiahnutia zbuildovaných súborov OSG (mega.nz)

    #. Rozbaliť zložky build a install do *%OSG_DIR%*
    #. Vynechať nasledujúci krok.

 #. V prípade stiahnutia iba zdrojových súborov OSG (oficiálna stránka)

    #. Rozbaliť OSG_3.4 do *%OSG_DIR%*
    #. Vytvoriť zložku build a install v *%OSG_DIR%*
    #. Premenovať súbory::

       %OSG_DIR%/ThirdParty/VC10/x86/include/GL/glut.h  > ../glut.h.bak
       %OSG_DIR%/ThirdParty/VC10/x86/lib/glut32.lib     > ../glut32.lib.bak
       %OSG_DIR%/ThirdParty/VC10/x86/lib/glut32D.lib    > ../glut32D.lib.bak

    #. Spustiť CMake (cmake-gui.exe)

       #. source code > *%OSG_DIR%*/OSG_3.4
       #. binaries > *%OSG_DIR%*/build
       #. stlačiť Configure (VS2010 kompilátor)
       #. stlačiť Generate

          - ak došlo k erroru: File > Delete Cache a skúsiť znovu

       .. figure:: images/install_guide/CMAKE_BUILD_OSG.png
          :align: center

          CMake pre OSG

    #. Nájsť súbor OpenSceneGraph.sln v *%OSG_DIR%*/build
    #. Otvoriť súbor vo VS2010
    #. Nastaviť Solution Configuration na Debug
    #. Nájsť projekt ALL_BUILD > pravý klik > build
    #. Po skončení nájsť projekt INSTALL > pravý klik > build
    #. Nastaviť Solution Configuration na Release
    #. Nájsť projekt ALL_BUILD > pravý klik > build
    #. Po skončení nájsť projekt INSTALL > pravý klik > build
    #. Presunúť nainštalované súbory (default c:/Program Files (x86)/OpenSceneGraph) do *%OSG_DIR%*/install

 #. Rozbaliť FreeGlut do *%OSG_DIR%*/ThirdParty/VC10/x86/ (prepísať súbory)
 #. Nainštalovať OpenCV (*%OPENCV_DIR%*)
 #. Rozbaliť Boost (*%BOOST_DIR%*)

    Ideálne je mať všetko na spoločnom mieste kvôli prehľadnosti, napr.

       .. figure::  images/install_guide/SW_FOLDERS.png
          :align: center

          Nainštalovaný SW

 #. Nainštalovať a otvoriť RapidEE, v ktorom sa vykonajú tieto zmeny:

    #. do PATH pridať premenné:

       #. *%CMAKE_DIR%*/bin
       #. *%QT_DIR%*/bin
       #. *%QT_DIR%*/Qtcreator/bin
       #. *%OSG_DIR%*/build/bin
       #. *%OSG_DIR%*/ThirdParty/VC10/x86/bin
       #. *%OPENCV_DIR%*/build/x86/vc10/bin


       .. figure:: images/install_guide/PATH_VAR.png
          :align: center

          PATH premenná

    #. Vytvoriť premennú CMAKE_INCLUDE_PATH a pridať:

       #. *%OSG_DIR%*/install/include
       #. *%OSG_DIR%*/ThirdParty/VC10/x86/include
       #. *%OPENCV_DIR%*/build/include


       .. figure:: images/install_guide/CMAKE_INCLUDE_PATH_VAR.png
          :align: center

          CMAKE_INCLUDE_PATH premenná

    #. Vytvoriť premennú CMAKE_LIBRARY_PATH a pridať:

       #. *%OSG_DIR%*/build/lib
       #. *%OSG_DIR%*/install/lib
       #. *%OSG_DIR%*/ThirdParty/VC10/x86/lib
       #. *%OPENCV_DIR%*/build/x86/vc10/lib


       .. figure:: images/install_guide/CMAKE_LIBRARY_PATH_VAR.png
          :align: center

          CMAKE_LIBRARY_PATH premenná

    #. Vytvoriť premennú BOOST_INCLUDEDIR a pridať: *%BOOST_DIR%*/boost
    #. Vytvoriť premennú BOOST_LIBRARYDIR a pridať: *%BOOST_DIR%*/libs
    #. Vytvoriť premennú BOOST_ROOT a pridať: *%BOOST_DIR%*


       .. figure::  images/install_guide/BOOST_VAR.png
          :align: center

          BOOST premmené

    #. Vytvoriť premennú OPENCV_DIR a pridať: *%OPENCV_DIR%*/build


       .. figure::  images/install_guide/OPENCV_DIR_VAR.png
          :align: center

          OPENCV_DIR premenná

 #. Naklónovať projekt 3DSoftViz cez git shell (*%3DSoftViz%*)
 #. Vytvoriť v priečinku *%3DSoftViz%* priečinky _build a _install
 #. Spustiť QtCreator. Tools > Options... > Build and Run:

    #. záložka CMake – zadať cestu *%CMAKE_DIR%*/bin/cmake.exe
    #. záložka Compilers – ak existuje VS2010 tak sú autodetected
    #. záložka Qt Versions – zadať cestu *%QT_DIR%*/bin/qmake.exe
    #. záložka Kits – vytvoriť nový a vybrať hodnoty nasledovne:


       .. figure:: images/install_guide/QTC_OPTIONS_KITS.png
          :align: center

          QtC Kits nastavenia

    #. záložka General – nastaviť default build directory: *%3DSoftViz%*/_build
    #. Potvrdiť – OK

 #. File > Open File or Project... > vybrať CMakeLists.txt z *%3DSoftViz%*
 #. Zadať do poľa Arguments jeden z nasledujúcich prepínačov:

    - -DCMAKE_BUILD_TYPE=Debug
    - -DCMAKE_BUILD_TYPE=Release

 #. Vybrať z nastavený generátor – NMake Generator (názov kitu)

       (Chyba: Generátor nebol nájdený :ref:`<riešenie> <ref_install_guide_errors>`)

 #. Stlačiť Run CMake


       .. figure:: images/install_guide/QTC_CMAKE_WIZARD.png
          :align: center

          QtC CMake wizard

       (Chyba: Chýbajúce moduly :ref:`<riešenie> <ref_install_guide_errors>`)

       (Chyba: Cesta nebola nájdená :ref:`<riešenie> <ref_install_guide_errors>`)

 #. Ukončiť – Finish
 #. Vybrať Projects > Build & Run > Build, v časti Edit build configuration kliknúť na Add > Clone selected, nazvať napr. „unity“
 #. Prejsť na vytvorený build config. „unity“, v časti Build Steps otvoriť Details a zaškrtnúť pri build step *jom.exe* možnosť *install_unity*


       .. figure:: images/install_guide/QTC_PROJECT_BUILD.png
          :align: center

          QtC build project

 #. Skontrolovať nastavenie build config – unity


       .. figure::  images/install_guide/QTC_PROJECT_BUILD_CONFIG.png
          :align: center

          QtC build config

 #. Stačiť Build (kladivko vľavo dole)
 #. Po úspešnom zbuildovaní vybrať Projects > Build & Run > Run, v časti Run pridať Add > Custom Executable a nastaviť:

    #. executable: *%3DSoftViz%*/_install/bin/3DSoftviz.exe
    #. working directory: *%3DSoftViz%*/_install/bin/


       .. figure:: images/install_guide/QTC_PROJECT_RUN.png
          :align: center

          QtC run project

 #. Skontrolovať nastavenie run config – zadaná cesta


       .. figure::  images/install_guide/QTC_PROJECT_RUN_CONFIG.png
          :align: center

          QtC run config

 #. Spustiť program pomocou zeleného tlačidla Run

Rozšírenie 3DSoftviz o Kinect
-----------------------------

 #. Nainštalovať Kinect for Windows
 #. Skontrolovať v RapidEE či sa vytvorila premenná *%KINECTSDK10_DIR%*


       .. figure::  images/install_guide/KINECTSDK_VAR.png
          :align: center

          KINECTSDK10_DIR premenná

 #. Nainštalovať OpenNI2 (OpenNI-Windows-x86-2.2.msi)

    - **x86! – inak sa môžu vyskytnúť problémy s linkovaním**

 #. Skontrolovať v RapidEE či sa vytvorili premenné:

    #. *%OPENNI2_INCLUDE%*
    #. *%OPENNI2_LIB%*
    #. *%OPENNI2_REDIST%*
    #. *%OPENNI2_ROOT%*


       .. figure::  images/install_guide/OPENNI2_VAR.png
          :align:   center

          NITE2 premenné

 #. Nainštalovať NiTE2 (NiTE-Windows-x86-2.2.msi)

    - **x86! – inak sa môžu vyskytnúť problémy s linkovaním**

 #. Skontrolovať v RapidEE či sa vytvorili premenné:

    #. *%NITE2_INCLUDE%*
    #. *%NITE2_LIB%*
    #. *%NITE2_REDIST%*
    #. *%NITE2_ROOT%*


       .. figure::  images/install_guide/NITE2_VAR.png
          :align:   center

          OPENNI2 premenné

 #. Pridať do premennej CMAKE_INCLUDE_PATH:

    #. *%OPENNI2_INCLUDE%*
    #. *%NITE2_INCLUDE%*


       .. figure::  images/install_guide/CMAKE_INCLUDE_PATH_FULL_VAR.png
          :align:   center

          OPENNI2 premenné

 #. Pridať do premennej CMAKE_LIBRARY_PATH:

    #. *%OPENNI2_ROOT%*/Driver
    #. *%OPENNI2_REDIST%*
    #. *%OPENNI2_REDIST%*/OpenNI2/Drivers
    #. *%OPENNI2_LIB%*
    #. *%NITE2_ROOT%*/Samples/Bin/OpenNI2/Drivers
    #. *%NITE2_LIB%*


       .. figure::  images/install_guide/CMAKE_LIBRARY_PATH_FULL_VAR.png
          :align:   center

          OPENNI2 premenné

 #. Pridať do premennej PATH:

    #. *%OPENNI2_REDIST%*/OpenNI2/Drivers
    #. *%OPENNI2_REDIST%*
    #. *%NITE2_REDIST%*
    #. *%NITE2_ROOT%*/Samples/Bin


       .. figure::  images/install_guide/PATH_FULL_VAR.png
          :align:   center

          OPENNI2 premenné

 #. Spustiť CMake a skontrolovať vo výpise:

    #. OpenNI2 FOUND
    #. NITE2 FOUND
    #. KINECTSDK FOUND

Nastavenie debuggera v QtCreator
--------------------------------

 #. Nainštalovať WinDbg
 #. Skontrolovať v QtCreator Tools > Options > Build & Run > záložka Debuggers či sú autodetected


       .. figure::  images/install_guide/QTC_OPTIONS_DEBUGGERS.png
          :align:   center

          QtC debugger nastavenia

 #. Pridať do QtCreator Tools > Options > Build & Run > záložka Kits pre vytvorený profil položku Debugger (x86)
 #. Spustiť CMake (-DCMAKE_BUILD_TYPE=Debug)
 #. Zvoliť možnosť Debug (vľavo dole medzi Run a Build)
