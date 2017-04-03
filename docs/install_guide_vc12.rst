.. _ref_install_guide_vc12:

Inštalačný návod (VC12)
=======================
v1.6 (13. 03. 2016)

.. toctree::
   :maxdepth: 1
   
   install_guide_changelog
   install_guide_errors
   
Návod pre Windows
-----------------

Tento návod bol úspešne otestovaný na operačných systémoch Windows 7, 8, 8.1, 10.

Potrebný softvér
****************

**Uvedené programy neinštalovať do Program Files, PATH nesmie obsahovať medzeru!** 
Na inštaláciu potrebujeme klonovať projekt `3DSoftViz <https://github.com/dominikhorniak/RealityNotFound>`_ (Tím č.4 klonuje z `/cimox/3dsoftviz <https://github.com/cimox/3dsoftviz>`_) z Githubu a stiahnuť nasledovné:

  - CMake `(v3.6.2) <https://cmake.org/files/v3.6/cmake-3.6.2-win64-x64.msi>`_
  - OpenSceneGraph *(jeden z nasledujúcich)*

    - OpenSceneGraph `(source) <https://github.com/openscenegraph/OpenSceneGraph>`_ - iba zdrojové súbory -> treba buildnúť (cca 40-60min)
    - stable release: OpenSceneGraph `(v3.4.0) <https://drive.google.com/open?id=0B54yBSwttuiDblhwNWZqTW9ORVk>`_ - buildnuté (13.03.2016)
    - developer release: OpenSceneGraph `(v3.5.1) <https://drive.google.com/open?id=0B3covCFoz_c0bDA4OWlSdVBjTUU>`_ - buildnuté (13.03.2016) 

  - Kinect for Windows `SDK 1.8 <https://www.microsoft.com/en-us/download/details.aspx?id=40278>`_
  - Microsoft VisualStudio 2013 **(okrem Express edition!)**
  - Qt `(v4.8.6) <https://drive.google.com/open?id=0B54yBSwttuiDNmtPbmxFQmcyTEk>`_
  - QtCreator `(v3.6.1) <http://ftp.fau.de/qtproject/official_releases/qtcreator/3.6/3.6.1/qt-creator-opensource-windows-x86-3.6.1.exe>`_
  - OpenCV `(v2.4.10) <https://drive.google.com/open?id=0B54yBSwttuiDajlhNDN0QnlILXc>`_
  - Boost `(v1.57.0) <https://sourceforge.net/projects/boost/files/boost-binaries/1.57.0/boost_1_57_0-msvc-12.0-32.exe/download>`_
  - `RapidEE <http://www.rapidee.com/en/download>`_ - program na prácu s premennými prostredia
  - Inštalácia knižnice 3rd party dependencies `VC12 <https://drive.google.com/open?id=0B3covCFoz_c0eE01TDUxTmhNTkk>`_
  - `OpenNI2 <https://drive.google.com/open?id=0B_DYSHDPz1SndC1yWkZBUUlzZTQ>`_
  - `NiTE2 <https://drive.google.com/open?id=0B_DYSHDPz1SnWThlV3laenpGb0E>`_
  - Debugging Tools for Windows 

    - WinDbg - `Win 8.1 <http://download.microsoft.com/download/B/0/C/B0C80BA3-8AD6-4958-810B-6882485230B5/standalonesdk/sdksetup.exe>`_
    - WinDbg - `Win 10 <http://download.microsoft.com/download/E/1/F/E1F1E61E-F3C6-4420-A916-FB7C47FBC89E/standalonesdk/sdksetup.exe>`_  

Postup inštalácie
*****************
**Uvedené programy neinštalovať do Program Files, PATH nesmie obsahovať medzeru! Idealne vsetky programy, kniznice, atd. dat do jedneho adresara.**
 #. Nainštalovať CMake. (Cesta je v dokumente označená ako *%CMAKE_DIR%*). Pri inštalácií zvoliť "Add CMake to the system for all users". 
 #. Nainštalovať Qt (*%QT_DIR%*)
 #. Nainštalovať QtCreator.
 #. Vytvoriť zložku OpenSceneGraph (*%OSG_DIR%*)
 #. V prípade stiahnutia zbuildovaných súborov OSG (mega.nz)

    #. Rozbaliť zložky build a install do *%OSG_DIR%*
    #. Vynechať nasledujúci krok.

 #. V prípade stiahnutia iba zdrojových súborov OSG (oficiálna stránka)

    #. Rozbaliť OSG do *%OSG_DIR%*/source
    #. Vytvoriť zložku build a install v *%OSG_DIR%*
    #. Rozbaliť 3rd Party Knižnice (VC12) do *%OSG_DIR%*/ThirdParty/
    #. Spustiť CMake (cmake-gui.exe)
 
       #. source code > *%OSG_DIR%*/source
       #. binaries > *%OSG_DIR%*/build
       #. stlačiť Configure (VS2013 kompilátor)
       #. nastaviť 3rdParty na *%OSG_DIR%*/ThirdParty/VC12/X86
       #. stlačiť Generate

          - ak došlo k erroru: File > Delete Cache a skúsiť znovu

       .. _image_cmake_osg:
       .. figure::  \images\install_guide\update\CMAKE_BUILD_OSG.png
          :align:   center

          CMake pre OSG
      
    #. Nájsť súbor OpenSceneGraph.sln v *%OSG_DIR%*/build
    #. Otvoriť súbor vo VS2013 **(ako správca!)**
    #. Nastaviť Solution Configuration na Debug
    #. Nájsť projekt ALL_BUILD > pravý klik > build
    #. Po skončení nájsť projekt INSTALL > pravý klik > build
    #. Nastaviť Solution Configuration na Release
    #. Nájsť projekt ALL_BUILD > pravý klik > build
    #. Po skončení nájsť projekt INSTALL > pravý klik > build
    #. Presunúť nainštalované súbory (default c:\Program Files (x86)\OpenSceneGraph) do *%OSG_DIR%*/install

 #. Nainštalovať OpenCV (*%OPENCV_DIR%*)
 #. Rozbaliť Boost (*%BOOST_DIR%*)
  
    Ideálne je mať všetko na spoločnom mieste kvôli prehľadnosti, napr. 

       .. _image_sw_folders:
       .. figure::  \images\install_guide\update\SW_FOLDERS.png
          :align:   center

          Nainštalovaný SW

 #. Otvorit subor environment.txt a upravit v nom cesty k programom a knizniciam, ktore sa nachadzaju na zaciatku suboru.
	
		.. _image_path_var:
		.. figure::  \images\install_guide\update\PATH_POWERSHELL.png
			:align:	center
		
			Pozadovane premenne, ktore treba nastavit
			
	#. Spustit powershell ako spravca .
	#. Skopirovat do powershellu obsah celeho suboru.	
	#. Vynechat pridavanie systemovych premennych cez RapidEE (nasledujuci krok) a vykonat len kontrolu, ci sa cesty spravne nastavili.
 
 #. Nainštalovať a otvoriť RapidEE, v ktorom sa vykonajú tieto zmeny **(ako správca!)**:

    #. do PATH pridať premenné:

       #. *%CMAKE_DIR%*/bin
       #. *%QT_DIR%*/bin
       #. *%QT_DIR%*/Qtcreator/bin
       #. *%OSG_DIR%*/build/bin
       #. *%OSG_DIR%*/ThirdParty/VC12/x86/bin
       #. *%OPENCV_DIR%*/build/x86/vc12/bin

       .. _image_path_var:
       .. figure::  \images\install_guide\update\PATH_VAR.png
          :align:   center

          PATH premenná

    #. Vytvoriť premennú CMAKE_INCLUDE_PATH a pridať:

       #. *%OSG_DIR%*/install/include
       #. *%OSG_DIR%*/ThirdParty/VC12/x86/include
       #. *%OPENCV_DIR%*/build/include

       .. _image_cmake_include_path_var:
       .. figure::  \images\install_guide\update\CMAKE_INCLUDE_PATH_VAR.png
          :align:   center

          CMAKE_INCLUDE_PATH premenná

    #. Vytvoriť premennú CMAKE_LIBRARY_PATH a pridať:

       #. *%OSG_DIR%*/build/lib
       #. *%OSG_DIR%*/install/lib
       #. *%OSG_DIR%*/ThirdParty/VC12/x86/lib
       #. *%OPENCV_DIR%*/build/x86/vc12/lib

       .. _image_cmake_library_path_var:
       .. figure::  \images\install_guide\update\CMAKE_LIBRARY_PATH_VAR.png
          :align:   center

          CMAKE_LIBRARY_PATH premenná

    #. Vytvoriť premennú BOOST_INCLUDEDIR a pridať: *%BOOST_DIR%*/boost
    #. Vytvoriť premennú BOOST_LIBRARYDIR a pridať: *%BOOST_DIR%*/libs
    #. Vytvoriť premennú BOOST_ROOT a pridať: *%BOOST_DIR%*

       .. _image_boost_var:
       .. figure::  \images\install_guide\update\BOOST_VAR.png
          :align:   center

          BOOST premmené

    #. Vytvoriť premennú OPENCV_DIR a pridať: *%OPENCV_DIR%*/build

       .. _image_opencv_dir_var:
       .. figure::  \images\install_guide\update\OPENCV_DIR_VAR.png
          :align:   center

          OPENCV_DIR premenná

 #. Nainštalovať Debugging Tools for Windows.
 #. Naklónovať projekt 3DSoftViz cez git shell (*%3DSoftViz%*)
=======

 #. Naklonovať projekt 3DSoftViz cez git shell (*%3DSoftViz%*)
 #. Cez command line prejsť do naklonovaného projektu a zavolať *git submodule --init --recursive*

 #. Vytvoriť v priečinku *%3DSoftViz%* priečinky _build a _install
 #. Spustiť QtCreator. Tools > Options... > Build and Run:
 
    #. záložka CMake – zadať cestu *%CMAKE_DIR%*/bin/cmake.exe
    #. záložka Compilers – ak existuje VS2013 tak sú autodetected
    #. záložka Qt Versions – zadať cestu *%QT_DIR%*/bin/qmake.exe
    #. záložka Kits – vytvoriť nový a vybrať hodnoty nasledovne:

       .. _image_qtc_kits_config:
       .. figure::  \images\install_guide\update\QTC_OPTIONS_KITS.png
          :align:   center

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

       .. _image_qtc_cmake_wizard:
       .. figure::  \images\install_guide\update\QTC_CMAKE_WIZARD.png
          :align:   center

          QtC CMake wizard

       (Chyba: Chýbajúce moduly :ref:`<riešenie> <ref_install_guide_errors>`)

       (Chyba: Cesta nebola nájdená :ref:`<riešenie> <ref_install_guide_errors>`)

 #. Ukončiť – Finish
 #. Vybrať Projects > Build & Run > Build, v časti Edit build configuration kliknúť na Add > Clone selected, nazvať napr. „unity“
 #. Prejsť na vytvorený build config. „unity“, v časti Build Steps otvoriť Details a zaškrtnúť pri build step *jom.exe* možnosť *install_unity*

       .. _image_qtc_build_project:
       .. figure::  \images\install_guide\update\QTC_PROJECT_BUILD.png
          :align:   center

          QtC build project

 #. Skontrolovať nastavenie build config – unity

       .. _image_qtc_build_config:
       .. figure::  \images\install_guide\update\QTC_PROJECT_BUILD_CONFIG.png
          :align:   center

          QtC build config

 #. Stačiť Build (kladivko vľavo dole - potreba spraviť znova po každej následnej úprave systémových premenných)
 #. Po úspešnom zbuildovaní vybrať Projects > Build & Run > Run, v časti Run pridať Add > Custom Executable a nastaviť:
 
    #. executable: *%3DSoftViz%*/_install/bin/3DSoftviz.exe
    #. working directory: *%3DSoftViz%*/_install/bin/

       .. _image_qtc_run_project:
       .. figure::  \images\install_guide\update\QTC_PROJECT_RUN.png
          :align:   center

          QtC run project

 #. Skontrolovať nastavenie run config – zadaná cesta

       .. _image_qtc_run_config:
       .. figure::  \images\install_guide\update\QTC_PROJECT_RUN_CONFIG.png
          :align:   center

          QtC run config

 #. Spustiť program pomocou zeleného tlačidla Run
 
 **V prípade, že aplikácia ihneď po spustení crashne, napriek úspešnému buildu, jedná sa pravdepodobne o problém s grafickou kartou.
 Na notebookoch, ktoré majú externú grafickú kartu NVidia, je v tomto prípade treba cez NVidia Control Panel nastaviť jej použitie pre 3DSoftViz.exe**

Rozšírenie 3DSoftviz o Kinect
-----------------------------

 #. Nainštalovať Kinect for Windows
 #. Skontrolovať v RapidEE či sa vytvorila premenná *%KINECTSDK10_DIR%*

       .. _image_kinect_sdk_var:
       .. figure::  \images\install_guide\update\KINECTSDK_VAR.png
          :align:   center

          KINECTSDK10_DIR premenná

 #. Nainštalovať OpenNI2 (OpenNI-Windows-x86-2.2.msi)
 
    - **x86! – inak sa môžu vyskytnúť problémy s linkovaním**

 #. Skontrolovať v RapidEE či sa vytvorili premenné:
 
    #. *%OPENNI2_INCLUDE%*
    #. *%OPENNI2_LIB%*
    #. *%OPENNI2_REDIST%*
    #. *%OPENNI2_ROOT%*

       .. _image_nite2_var:
       .. figure::  \images\install_guide\update\OPENNI2_VAR.png
          :align:   center

          NITE2 premenné

 #. Nainštalovať NiTE2 (NiTE-Windows-x86-2.2.msi)
 
    - **x86! – inak sa môžu vyskytnúť problémy s linkovaním**

 #. Skontrolovať v RapidEE či sa vytvorili premenné:
 
    #. *%NITE2_INCLUDE%*
    #. *%NITE2_LIB%*
    #. *%NITE2_REDIST%*
    #. *%NITE2_ROOT%*

       .. _image_openni2_var:
       .. figure::  \images\install_guide\update\NITE2_VAR.png
          :align:   center

          OPENNI2 premenné

 #. Pridať do premennej CMAKE_INCLUDE_PATH:
 
    #. *%OPENNI2_INCLUDE%*
    #. *%NITE2_INCLUDE%*

       .. _image_cmake_include_path_full_var:
       .. figure::  \images\install_guide\update\CMAKE_INCLUDE_PATH_FULL_VAR.png
          :align:   center

          OPENNI2 premenné

 #. Pridať do premennej CMAKE_LIBRARY_PATH:
 
    #. *%OPENNI2_ROOT%*/Driver
    #. *%OPENNI2_REDIST%*
    #. *%OPENNI2_REDIST%*/OpenNI2/Drivers
    #. *%OPENNI2_LIB%*
    #. *%NITE2_ROOT%*/Samples/Bin/OpenNI2/Drivers
    #. *%NITE2_LIB%*

       .. _image_cmake_library_path_full_var:
       .. figure::  \images\install_guide\update\CMAKE_LIBRARY_PATH_FULL_VAR.png
          :align:   center

          OPENNI2 premenné

 #. Pridať do premennej PATH:
 
    #. *%OPENNI2_REDIST%*/OpenNI2/Drivers
    #. *%OPENNI2_REDIST%*
    #. *%NITE2_REDIST%*
    #. *%NITE2_ROOT%*/Samples/Bin

       .. _image_path_full_var:
       .. figure::  \images\install_guide\update\PATH_FULL_VAR.png
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

       .. _image_qtc_debugger_config:
       .. figure::  \images\install_guide\update\QTC_OPTIONS_DEBUGGERS.png
          :align:   center

          QtC debugger nastavenia

 #. Pridať do QtCreator Tools > Options > Build & Run > záložka Kits pre vytvorený profil položku Debugger (x86)
 #. Spustiť CMake (-DCMAKE_BUILD_TYPE=Debug)
 #. Zvoliť možnosť Debug (vľavo dole medzi Run a Build)
