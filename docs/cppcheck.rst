CppCheck
========

http://cppcheck.sourceforge.net

pouzite cppcheck v 1.70

Ako pouzivat
------------

1)	v Edit->Preferences nastavit:
		General: check "Force checking all #ifdef configurations"
		Paths: pridat cestu do "RealityNotFound/include"
		Advanced: check "Show inconclusive errors"		!!! toto hlasi dost false-positive, ALE obcas najde dolezite veci !!!
2)	Check->Directory	a vybrat "RealityNotFound/src"

poznamky k reportom:	!!! ak je nieco nejasne, treba sa opytat !!!

	!!! ignorovat hlasky pre externy kod: !!!
		noiseutils, 
		qtcolorpicker

dolezite hlasky typu:
~~~~~~~~~~~~~~~~~~~~~

	Technically the member function XYZ can be const.
	
		- najma funkcie typu "getter" mozu byt const, napr.:
		
		::
		
			int getX () const {
				return 1;
			}

	Technically the member variable XYZ can be const.
	
		- treba skontrolovat!!!

	The class 'RestrictionsManager' does not have a constructor.
	
		- kazda trieda by mala mat konstruktor (kompiler nam sice vytvori defaul, ale... vid. nizsie)

	 'class Type' does not have a copy constructor which is recommended since the class contains a pointer to allocated memory.
		
		- zavisi, ci sa "copy constructor" v kode pouziva - nutna kontrola

	| The scope of the variable 'gesto_hore' can be reduced.
	| Variable 'gesto_hore' is assigned a value that is never used.
	
		- to je jasne 

	C-style pointer casting
	
		- to je riesene aj cez cpplint v samostatnej hotfix branch
		- v gcc da sa zapnut -Wold-style-cast	-> momentalne hlasi velmi vela warningov
		- je to kvoli citatelnosti, ALE ma to svoje opodstatnenie

	Member variable 'Cube::at' is not initialized in the constructor.
	
		- vsetky class variables by mali byt inicializovane v konstruktore cez "initialization list"
	
		!!! najma pointre !!! na NULL, resp. cez new (a delete v destructore)

	Possible null pointer dereference: conn - otherwise it is redundant to check it against null.
	Possible leak in public function. The pointer 'nodeTypeComboBox' is not deallocated before it is allocated.
	
		!!! treba skotrolovat - indikuje zavanu chybu !!! 

	Uninitialized variable: newGraph
	
		!!! treba skotrolovat - indikuje zavanu chybu !!!

ostatne hlasky:
~~~~~~~~~~~~~~~
	- treba skontrolovat - a mali by sa opravit