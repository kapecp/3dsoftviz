.. _ref_install_guide_vc14_1:

Inštalačný návod pre Windows (VC14.11)
==========================

Tento návod bol úspešne otestovaný na operačnom systéme Windows 10 Pro.

Potrebný softvér
****************
 
Na inštaláciu potrebujeme klonovať projekt `3DSoftViz <https://github.com/BergiSK/3dsoftviz>`_ (Tím č.18 klonuje z `/pmarusin/3dsoftviz <https://github.com/pmarusin/3dsoftviz>`_) z Githubu a stiahnuť nasledovné:
**Nasledujúce programy neinštalovať do Program Files, PATH nesmie obsahovať medzeru!**

  - Microsoft Visual Studio 2017 *(môže byť naištalovaný štandardne do Program Files)*
  - CMake (`v3.9.5 <https://cmake.org/files/v3.9/cmake-3.9.5-win64-x64.msi>`_)
  - OpenSceneGraph (v3.4.1) *(jeden z nasledujúcich)*

    - `Full pack <https://drive.google.com/open?id=1K88IaB9V7t0Wi4QUMDm-an583xFOSok1>`_
    - `Release <http://objexx.com/OpenSceneGraph/OpenSceneGraph-3.4.1-VC2017-64-Release.7z>`_ + `Debug <http://objexx.com/OpenSceneGraph/OpenSceneGraph-3.4.1-VC2017-64-Debug.7z>`_ + Knižnice 3rd party dependencies `VC14.1  <https://download.osgvisual.org/3rdParty_VS2017_v141_x64_V11_small.7z>`_
  - Qt (v5.9.2) a Qt Creator (v4.4.1) - cez `Qt online installer <http://download.qt.io/official_releases/online_installers/qt-unified-windows-x86-online.exe>`_
  - OpenCV (v2.4.13.4) *(jeden z nasledujúcich)*

    - `Built <https://drive.google.com/open?id=1oAbB9FQMowXBwcruNO1Nui0CsIeI4gy3>`_ - buildnuté
    - `Source <https://github.com/opencv/opencv/archive/2.4.13.4.zip>`_ - iba zdrojové súbory, treba buildnúť
  - Boost (`v1.65.1  <https://sourceforge.net/projects/boost/files/boost-binaries/1.65.1/boost_1_65_1-msvc-14.1-64.exe/download>`_)
  - `RapidEE <http://www.rapidee.com/en/download>`_ - program na prácu s premennými prostredia
  - Kinect for Windows `SDK 1.8 <https://www.microsoft.com/en-us/download/details.aspx?id=40278>`_
  - OpenNI2 (`v2.2 <https://drive.google.com/open?id=1IVXWOvsGB5FYXW826qnnAA2tFFQLQ8Jk>`_)
  - NiTE (`v2.21 <https://drive.google.com/open?id=15MdeAVzOflT_4vSnzrYPAcCuq7Yb1Yfm>`_)
  - Debugging Tools for Windows 10 - WinDbg - `Win 10 <http://download.microsoft.com/download/E/1/F/E1F1E61E-F3C6-4420-A916-FB7C47FBC89E/standalonesdk/sdksetup.exe>`_  

(v pripade, že niektorý odkaz na stiahnutie ke offline, všetky komponenty sa dajú stiahnuť z nasledujúceho odkazu: `drive <https://drive.google.com/open?id=1GCaVvTrA7-Hhlv0_OTp7ewyDcJWdTZQD>`_)

Postup inštalácie
*****************

**Uvedené programy neinštalovať do Program Files, PATH nesmie obsahovať medzeru! Idealne všetky programy, knižnice, atď. dať do jedného adresára. Ideálne je mať všetko na spoločnom mieste kvôli prehľadnosti, napr. C:/Timak/**
 #. Nainštalovať Microsoft Visual Studio 2017 *(môže byť naištalovaný štandardne do Program Files)* spolu aj s rozšíreniami:
    
    #. VC++ 2017 v141 toolset
    #. Windows 10 SDK for Desktop C++
    #. Visual C++ tools for CMake
    #. Visual C++ ATL support
    #. MFC and ATL support
    #. C++/CLI support

 #. Nainštalovať CMake. (Cesta je v dokumente označená ako *%CMAKE_DIR%*). Pri inštalácii zvoliť "Add CMake to the system for all users". 
 #. Nainštalovať Qt a Qt Creator *(mal by sa nainštalovať automaticky s Qt)*:

    #. Nainštalovať Qt online installer (*%QT_INSTALLER_DIR%*)
    #. Spustiť MaintenanceTool.exe > Skip > Add or remove components
    #. Zvoliť z Qt 5.9.2 nasledujúce veci:

       - msvc2017 64-bit
       - všetko od Qt Charts po Qt Script
    #. Zvoliť z Tools Qt Creator 4.4.1 CDB Debugger Support
    #. Nainštalovať zvolené položky (%QT_INSTALLER_DIR%/5.9.2/msvc2017_64)

 #. Vytvoriť zložku OpenSceneGraph (*%OSG_DIR%*)
 #. V prípade stiahnutia Full pack:

    #. Rozbaliť zložky do *%OSG_DIR%*
    #. Vynechať nasledujúci krok.

 #. V prípade stiahnutia Release + Debug + Knižnice 3rd party dependencies VC14.1:
    
    #. Vytvoriť zložku Release v zložke *%OSG_DIR%*
    #. Rozbaliť do nej zložky zo stiahnutého Release
    #. Vytvoriť zložku Debug v zložke *%OSG_DIR%*
    #. Rozbaliť do nej zložky zo stiahnutého Debug
    #. Vytvoriť zložku 3rdParty v zložke *%OSG_DIR%*
    #. Rozbaliť do nej zložky zo stiahnutého 3rd party dependencies VC14.1

 #. V prípade stiahnutia zbuildovaných súborov OpenCV

    #. Rozbaliť zložky do *%OPENCV_DIR%*
    #. Vynechať nasledujúci krok.

 #. V prípade stiahnutia iba zdrojových súborov OpenCV

    #. Rozbaliť OpenCV do *%OPENCV_DIR%*/source
    #. Vytvoriť zložku install a build v *%OPENCV_DIR%*
    #. Spustiť CMake (cmake-gui.exe)
 
       #. source code > *%OPENCV_DIR%*/source
       #. binaries > *%OPENCV_DIR%*/build
       #. nastaviť 3rdparty:

          - stlačiť Add Entity
          - zadať Name: 3rdparty
          - zvoliť Type: PATH
          - zadať Value: *%OPENCV_DIR%*/source/3rdparty
          - stlačiť OK
       #. stlačiť Configure 
       #. nastagiť generator na Visual Studio 15 2017 Win64
       #. stlačiť Finish
       #. čakať na výpis
       #. nastaviť CMAKE_INSTALL_PREFIX na *%OPENCV_DIR%*/install
       #. stlačiť Generate

          - ak došlo k erroru: File > Delete Cache a skúsiť znovu
      
    #. Nájsť súbor OpenCV.sln v *%OPENCV_DIR%*/build
    #. Otvoriť súbor vo VS2017 **(ako správca!)**
    #. Nastaviť Solution Configuration na Debug, Solution Platform na x64
    #. Nájsť projekt ALL_BUILD > pravý klik > build
    #. Po skončení nájsť projekt INSTALL > pravý klik > build
    #. Nastaviť Solution Configuration na Release
    #. Nájsť projekt ALL_BUILD > pravý klik > build
    #. Po skončení nájsť projekt INSTALL > pravý klik > build
    #. Presunúť nainštalované súbory zo zložky install do *%OPENCV_DIR%*
    #. Zložky install, build a sources môzu byť vymazané z *%OPENCV_DIR%*

 #. Nainštalovať Boost (*%BOOST_DIR%*)
 #. Otvoriť súbor environment.txt a upraviť v ňom cesty k programom a knižniciam, ktoré sa nachádzajú na začiatku súboru.
			
	#. Spustiť powershell ako správca
	#. Skopírovať do powershellu obsah celého súboru	
	#. Vynechať pridávanie systemových premenných cez RapidEE (nasledujúci krok) a vykonať len kontrolu, či sa cesty spravne nastavili.
 
 #. Nainštalovať a otvoriť RapidEE, v ktorom sa vykonajú tieto zmeny **(ako správca!)**:

    #. do PATH pridať premenné:

       #. *%CMAKE_DIR%*/bin
       #. *%QT_INSTALLER_DIR%*/5.9.2/msvc2017_64/bin
       #. *%QT_INSTALLER_DIR%*/Tools/QtCreator/bin
       #. *%OSG_DIR%*/Debug/bin
       #. *%OSG_DIR%*/Release/bin
       #. *%OSG_DIR%*/3rdParty/x64/bin
       #. *%OPENCV_DIR%*/x64/vc15/bin

    #. Vytvoriť premennú CMAKE_INCLUDE_PATH a pridať:

       #. *%OSG_DIR%*/Debug/include
       #. *%OSG_DIR%*/Release/include
       #. *%OSG_DIR%*/3rdParty/x64/include
       #. *%OPENCV_DIR%*/include

    #. Vytvoriť premennú CMAKE_LIBRARY_PATH a pridať:

       #. *%OSG_DIR%*/Debug/lib
       #. *%OSG_DIR%*/Release/lib
       #. *%OSG_DIR%*/3rdParty/x64/lib
       #. *%OPENCV_DIR%*/x64/vc15/lib

    #. Vytvoriť premennú BOOST_INCLUDEDIR a pridať: *%BOOST_DIR%*/boost
    #. Vytvoriť premennú BOOST_LIBRARYDIR a pridať: *%BOOST_DIR%*/libs
    #. Vytvoriť premennú BOOST_ROOT a pridať: *%BOOST_DIR%*

    #. Vytvoriť premennú OPENCV_DIR a pridať: *%OPENCV_DIR%*

 #. Nainštalovať WinDbg.

=======

 #. Naklonovať projekt 3DSoftViz cez git shell (*%3DSoftViz%*)
 #. Cez command line prejsť do naklonovaného projektu a zavolať *git submodule update --init --recursive*

 #. Vytvoriť v priečinku *%3DSoftViz%* priečinky _build a _install
 #. Zo súboru *%3DSoftViz%*/resources/scripts/app/main.lua vymazať alebo zakomentovať prvý riadok (graph_importer = require "softviz/graph_importer")
 #. Spustiť QtCreator. Tools > Options... > Build and Run:
 
    #. záložka CMake – ak je nainštalovaný CMake, tak auto-detected, inak pridať manuálne *%CMAKE_DIR%*/bin/cmake.exe
    #. záložka Compilers – ak existuje VS2017, tak sú auto-detected
    #. záložka Qt Versions – ak je nainštalovaný Qt, tak auto-detected, inak zadať cestu *%QT_INSTALLER_DIR%*/5.9.2/msvc2017_64/bin/qmake.exe
    #. záložka Debuggers – ak je nainštalovaný WinDbg, tak sú auto-detected, inak pridať manuálne C:/Program Files (x86)/Windows Kits/10/Debuggers/x64/cdb.exe
    #. záložka Kits – vytvoriť nový a nastaviť hodnoty nasledovne:

       #. Name: Local PC
       #. Device type: Desktop
       #. Compiler: C: Microsoft Visual C++ Compiler 15.0 (amd64)
       #. Compiler: C++: Microsoft Visual C++ Compiler 15.0 (amd64)
       #. Debugger: Auto-detected CDB at C:/Program Files (x86)/Windows Kits/10/Debuggers/x64/cdb.exe
       #. Qt version: Qt 5.9.2 MSVC2017 64bit
       #. CMake Tool: System CMake at *%CMAKE_DIR%*/bin/cmake.exe

    #. záložka General – nastaviť Default build directory: *%3DSoftViz%*/_build
    #. Potvrdiť – OK

 #. File > Open File or Project... > vybrať CMakeLists.txt z *%3DSoftViz%*
 #. Skontrolovať výpis v časti 6 General Messages, na konci výpisu musí byť

     Generating done

     CMake Project was parsed successfully.

 #. Vybrať Projects > Build & Run > Local PC > Build, v časti Edit build configuration kliknúť na Add > Clone selected, nazvať „unity“
 #. Prejsť na vytvorený build config. „unity“, v časti Build Steps otvoriť Details a vypnúť pri build step Build: cmake.exe --build . --target možnosť *all* a označiť *install_unity*

 #. Stlačiť Build (kladivo vľavo dole - potrebné spraviť znova po každej následnej úprave systémových premenných)
 #. Po úspešnom zbuildovaní vybrať Projects > Build & Run > Local PC > Run, v časti Run pridať Add > Custom Executable a nastaviť:
 
    #. Executable: *%3DSoftViz%*/_install/bin/3DSoftviz.exe
    #. working directory: *%3DSoftViz%*/_install/bin/

 #. Spustiť program pomocou zeleného tlačidla Run (vľavo dole)
 
 **V prípade, že aplikácia ihneď po spustení crashne, napriek úspešnému buildu, jedná sa pravdepodobne o problém s grafickou kartou.
 Na notebookoch, ktoré majú externú grafickú kartu NVidia, je v tomto prípade treba cez NVidia Control Panel nastaviť jej použitie pre 3DSoftViz.exe**

Rozšírenie 3DSoftviz o Kinect
-----------------------------

 #. Nainštalovať Kinect for Windows
 #. Skontrolovať v RapidEE či sa vytvorila premenná *%KINECTSDK10_DIR%*, keď nie, vytvoriť a pridať: C:/Program Files/Microsoft SDKs/Kinect/v1.8
 #. Nainštalovať OpenNI2 (*%OPENNI2_DIR%*)
 #. Skontrolovať v RapidEE či sa vytvorili premenné (ak na konci majú *64*, vymazať 64):

    #. *%OPENNI2_INCLUDE%*, keď nie, vytvoriť a pridať: *%OPENNI2_DIR%*/Include/
    #. *%OPENNI2_LIB%*, keď nie, vytvoriť a pridať: *%OPENNI2_DIR%*/Lib/
    #. *%OPENNI2_REDIST%*, keď nie, vytvoriť a pridať: *%OPENNI2_DIR%*/Redist/
    #. *%OPENNI2_ROOT%*, keď nie, vytvoriť a pridať: *%OPENNI2_DIR%*

 #. Nainštalovať NiTE2 (*%NITE2_DIR%*)
 #. Skontrolovať v RapidEE či sa vytvorili premenné (ak na konci majú *64*, vymazať 64):

    #. *%NITE2_INCLUDE%*, keď nie, vytvoriť a pridať: *%NITE2_DIR%*/Include/
    #. *%NITE2_LIB%*, keď nie, vytvoriť a pridať: *%NITE2_DIR%*/Lib/
    #. *%NITE2_REDIST%*, keď nie, vytvoriť a pridať: *%NITE2_DIR%*/Redist/
    #. *%NITE2_ROOT%*, keď nie, vytvoriť a pridať: *%NITE2_DIR%*

 #. Pridať do premennej CMAKE_INCLUDE_PATH:

    #. *%OPENNI2_INCLUDE%*
    #. *%NITE2_INCLUDE%*

 #. Pridať do premennej CMAKE_LIBRARY_PATH:

    #. *%OPENNI2_ROOT%*/Driver
    #. *%OPENNI2_REDIST%*
    #. *%OPENNI2_REDIST%*/OpenNI2/Drivers
    #. *%OPENNI2_LIB%*
    #. *%NITE2_ROOT%*/Samples/Bin/OpenNI2/Drivers
    #. *%NITE2_LIB%*

 #. Pridať do premennej PATH:

    #. *%OPENNI2_REDIST%*/OpenNI2/Drivers
    #. *%OPENNI2_REDIST%*
    #. *%NITE2_REDIST%*
    #. *%NITE2_ROOT%*/Samples/Bin

 #. Spustiť CMake a skontrolovať vo výpise:

    #. OpenNI2 FOUND
    #. NITE2 FOUND
    #. KINECTSDK FOUND

