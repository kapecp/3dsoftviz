AStyle
=======
		
Ako pouzivat v QTCreatore (Windows)
-----------------------------------
1)	Stiahni  `AStyle <https://sourceforge.net/projects/astyle/?source=typ_redirect>`_ do priecinku s programmi tykajucim sa projektu $ASTYLE_PATH.
1)	Pridaj cestu k astyle-u do systemovej premennej PATH: $ASTYLE_PATH/bin (napr. d:/timak/AStyle/bin)
3)	Spusti QTCreator
4)	Projects -> Build & Run -> Build
5)	V Build Settings -> Edit Build configuration klikni Add -> clone selected a zadaj "style"
6)	V Build steps rozklikni Details a v Targets oznac "style". Ak su oznacene aj ine targety, tak ich je potrebne odznacit(Vysledok: Make: jom.exe style).
7)	Klikni na kladivko v lavom dolnom rohu (Build).
