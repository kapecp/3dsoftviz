Metodika na písanie BDD testov 
==============================

Písanie testov
--------------

1. Vytvorme normálny .cpp súbor a v hlavičke uveďme:

::
	
	#include <igloo/igloo_alt.h>
	using namespace igloo;
		
2. Najskôr popíšeme, čo chceme testovať pomocou Describe-u:

::
	
	Describe(a_foo_bar) {
	...
	}

3. V tele Describe-u uvedieme funkcie, ktorými budeme testovať spravanie. 
   Funkcie pomenujme tak, aby boli samoopisné:
::	
	
	It( foo_should_be_bar ) {
	...
	}

4. V tele It-u už píšeme klasické asserty, eg:

::

   Assert::That( 2+4, Is().EqualTo(6) );
   Assert::That( "FIIT", Is().EqualTo("LIFE") );

Spustenie testov
----------------

1. Testy (vsetky *.cpp) pre vas modul umiestnujte do priecinka:
	
::

	~root/tests/Foo

Testy fungujú tak, že si buildnete vlastný .exe súbor, ktorý odtestuje danú 
funkcionalitu. Test si buildnete následovne:
	- otvorte hlavný CMakeLists.txt
	- ctrl+f -> "BDD Igloo tests"

2. Teraz si potrebujete nakopirovat nejaky kod, minimalne by ste mali mat:
	
::

	file( GLOB_RECURSE SRC_FOO_TESTS	"tests/Foo/*.cpp" )
	add_executable( test_foo_module ${TEST_RUNNER} ${SRC_FOO_TESTS} )
	add_dependencies( test_foo_module igloo )
	target_compile_options( test_foo_module PUBLIC ${FLAGS_FOR_DEBUG} )


Dole pod testami (alebo ctrl+f -> "run all tests") pridajte ešte jeden
riadok kódu:

::

	add_test( testing_foo_module test_foo_module )

3. Teraz musite zmeny ulozit a nechat zbehnut CMake
	- pravym na project -> Run CMake
	- v QTCreatori zmente build/run target na Tests/test_foo_module

4. Pokial uz mate v priecinku tests/foo testy mali by sa buildnut/zbehnut