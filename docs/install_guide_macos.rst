Install guide pre macOS
=======================

(vytvárané a testované pre verziu 10.13 High Sierra)
----------------------------------------------------

1. Inštalácia Homebrew príkazom:

::

	/usr/bin/ruby -e "$(curl -fsSL \https://raw.githubusercontent.com/Homebrew/install/master/install)"

2. Nainštalovať git:

::

	brew install git

3. Nainštalovať OpenCV - MUSÍ byť verzia 2.x, nie 3 a vyššia! (po inštalácii skontrolovať príkazom brew info opencv\@2, vyhovujúca verzia je napr.  2.4.13.4):

::

	brew install opencv@2

4. Vytvoriť symbolické linky pre opencv\@2:

::

	brew link opencv\@2  --force

5.	Nainštalovať OpenNI2:

* možné pomocou Homebrew nasledovne:

	::

		brew tap homebrew/science
		brew tap totakke/openni2
		brew install openni2

* dostupné `na stiahnutie v Google Drive <https://drive.google.com/open?id=0Bx3FU7Baa_8pOUlrTmJ4VHJzZFU>`_

6. Nainštalovať Boost:

::

	brew install boost

7.	Nainštalovať CMake:

::

	brew install cmake

8.	Nainštalovať Qt (preferovaná verzia  5.9.2):
::

	brew install qt

9.	Vytvoriť symbolické linky pre Qt:
::

	brew link qt --force

10.	skontrolovať verziu a symbolické linky pre Qt príkazom: qmake --version. Ak je výstup command not found, skontrolovať v Homebrew: brew info qt. Ak je Qt nainštalované, no nevieme nájsť qmake, Homebrew nevytvoril správne symbolické linky. Skúsiť brew unlink qt a znova brew link qt --force, prípadne reinstall

11.	Nainštalovať sphinx:

::

	pip install sphinx

alebo

::

	brew install sphinx

12.	Nainštalovať astyle:

::

	brew install astyle

13.	Nainštalovať pcl:

::

	brew install pcl

14.	Nainštalovať doxygen:

::

	brew install doxygen

15.	Nainštalovať cppcheck:

::

	brew install cppcheck

16.	Nainštalovať ovládače pre 3D myš dostupné na odkaze alebo v Google Drive projektu.

|

17.	Nainštalovať Qt Creator:

::

	brew cask install qt-creator

18.	Naklonovať projekt:

::

	git clone https://github.com/BergiSK/3dsoftviz --recursive

19.	Otvoriť Terminál, cd do priečinka ~/3dsoftviz a aktualizovať submoduly:

::

	git submodule update –-recursive

20.	Otvorit’ QtCreator a otvorit’ CmakeLists.txt z priečinka, kde bol naklonovaný projekt.

Nastavenie Qt Creatora:
-----------------------

* cmd + , pre otvorenie nastavení
* skontrolovať záložku Qt Versions, ak je dostupné Qt 5.x, nechať tak. Ak nie, treba nájsť binárku qmake (defaultne po inštalácii Qt cez Homebrew v /usr/local/bin/).
* skontrolovať záložku CMake. Ak nie je detekovaný žiadny, treba nájsť binárku cmake (defaultne po inštalácii CMake cez Homebrew v /usr/local/bin/).
* ísť do zákožky Kits.
* Zvoliť konfiguráciu a vpravo kliknúť na Clone
* Klinúť na novú konfiguráciu (dole sa otvorí editačné okno)
* Name zvoliť napr. unity
* C aj C++ Compiler zvoliť Clang
* Qt version zvoliť Qt 5.x
* CMake Tool zvoliť na CMake zo záložky CMake
* Vpravo hore klikúť na Make Default
* Potvrdiť tlačidlom OK

Build & Run konfigurácia:
-------------------------

* V ľavom menu Qt Creatora kliknúť na Projects
* Kliknúť na Build pri unity
* Edit build configuration zvoliť na Debug
* Build directory zvoliť na ~/3dsoftviz/build
* Pri položke Build steps kliknúť na Details a zaškrtnúť install_unity
* Kliknutím na Build project (kladivo) projekt zbuildovať
* Naľavo kliknúť na Run pri unity
* Pri Run configuration rozbaliť Add menu a zvoliť Custom Executable
* Zvoliť binárku 3dsoftviz/_install/bin/3DSoftviz
* Working directory nastaviť na 3dsoftviz/_install/bin

Známe problémy, riešenia a rady
--------------------------------

Q: Build error  "/usr/local/.//mkspecs/macx-clang"
**************************************************

::

	CMake Error at /usr/local/lib/cmake/Qt5Core/Qt5CoreConfig.cmake:15 (message):
	  The imported target "Qt5::Core" references the file

	     "/usr/local/.//mkspecs/macx-clang"

**A:** Chyba pravdepodobne je na strane Homebrew, pri inštalácii Qt5 nevytvorí dve symlinky (ani po zavolaní brew link). Musíte ich vytvoriť ručne (ak treba, upravte verziu v ceste, návod bol robený pri nainštalovanej 5.9.2):

::

	ln -s /usr/local/Cellar/qt/5.9.2/mkspecs /usr/local/mkspecs
	ln -s /usr/local/Cellar/qt/5.9.2/plugins /usr/local/plugins

Ak príkaz zlyhá, pretože symlinky už existujú, zmažte ich a skúste znova.
