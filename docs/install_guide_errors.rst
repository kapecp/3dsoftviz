.. _ref_install_guide_errors:

Časté problémy
==============

Inštalácia softvéru 3DSoftViz pozostáva z mnohých krokov. Pozorné postupovanie podľa inštrukcií je nevyhnutné, inak sa môžu vyskytnúť problémy. Nasledujúce problémy sme identifikovali. 

Ponuka generátorov je prázdna
-----------------------------

Ak sa generátor nedá vybrať, je dôležité zmazať súbor CMakeLists.txt.user.2.5pre1 z *%3DSoftViz%* a zopakovať predošlý krok.

CMake - chýbajúce moduly
------------------------

 #. cez git shell prejsť do *%3DSoftViz%*
 #. uistiť sa, že je nastavený na master vetvu 

    - na prepnutie: git checkout master

 #. zadať príkaz: git submodule update –init

CMake - cesta nebola nájdená
----------------------------

Ak nebola nájdené cesta k niektorému modulu, treba skontrolovať nastavenia premenných v RapidEE.

Chyba spojená s glut.h, glut.lib, glut.dll
------------------------------------------

Pokiaľ sa vyskytnú problémy typu:

 - nevie nájsť glut.h
 - hlási chyby priamo v glut.h
 - niečo iné s glut.h, glut.lib alebo glut.dll 

Tak najjednoduchšie riešenie je premenovať::

 - %OSG_DIR%/ThirdParty/VC10/x86/include/GL/glut.h  > ../glut.h.bak
 - %OSG_DIR%/ThirdParty/VC10/x86/lib/glut32.lib     > ../glut32.lib.bak
 - %OSG_DIR%/ThirdParty/VC10/x86/lib/glut32D.lib    > ../glut32D.lib.bak

Pri spustení crash programu s nullpoint exception
-------------------------------------------------

Pokiaľ sa projekt spustí iba cez argument: -DCMAKE_BUILD_TYPE=Release, 
ale cez -DCMAKE_BUILD_TYPE=Debug hlási nullpoint exception pri 
načítavaní obrázkov na pozadie a program crashne. Tak riešením 
je znovu stiahnuť/nakonfigurovať OSG podľa návodu.

Zm### pri buildovaní
---------------------

Ak došlo k erroru pri buildovaní: -Zm###, treba vykonať úpravu v
CMakeLists.txt > riadok 128 zmeniť /Zm216 na /Zm240 a spustiť CMake odznovu.

