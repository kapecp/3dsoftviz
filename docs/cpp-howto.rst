Ako programovat v C++
=====================

Ako pisat headre a cpp subory
-----------------------------

	includuje sa v tomto poradi (plati pre .h aj .cpp):
	
		* headre z projektu
		* headre z Qt a OSG
		* systemove headre

	v .cpp  je prvy header prislusne .h-cko toho .cpp

		.. important:: **pravidlo:** includujem najprv tie, ktore mozu includnut co najviac

pravidla pre pisanie .h
~~~~~~~~~~~~~~~~~~~~~~~

		- obsahuju iba defincie metod, NIE implementaciu metod

		- includujeme LEN co je treba pre header, NIC naviac

		- | ak je v triede/metode pouzity pointer typ, napr.
		  | Node* n;
		  | tak staci pouzit doprednu deklaraciu, t.j.
		  |	class Node;
		  | a netreba includovat Node.h

	.. attention::  **toto nefunguje**, ked:
	
					- sa dedi trieda
					- to neni pointer, t.j. je to Node n;
					- ked je pouzity osg::ref_ptr pointer, resp. templaty


pravidla pre pisanie .cpp
~~~~~~~~~~~~~~~~~~~~~~~~~

		| ak je typ premennej pouzity LEN v .cpp (typicky lokalna premenna v metode),
		| tak prislusny header davame len do .cpp


ZLE praktiky, resp. co nerobit
------------------------------

	NIKDY v headroch a cpp suboroch nepouzivat "using namespace"
		
		- nepouzivat "using" keyword

		ak uz je pouzite, tak to treba upravit na :
		
		::
		
			namespace XYZ {
				... implementacia ...
			}

	NEPOUZIVAT "0" (nulu) ako NULL pointer, ale
	
		- pouzit nullptr (ak kompilator podporuje C++11), pripadne pouzit NULL


DOBRE praktiky, resp. co robit
------------------------------

	inicializovat VSETKY atributy "pri" konstruktore cez "initialization list"
	
		- najme pointre
		
			- skontrolvat ak je "new" v konstruktore, tak MUSI byt "delete" v destruktore
				(neplati pre: osg::ref_ptr)
				
		- inicializovat  v takom poradi v akom su zapisane v triede:
		
			- usporiadat atributy od "najvacsich" (napr. pointer, trieda, double) po "najmensie" (int, char, bool)
				aj ked sa bude mixovat public/private

	"std::cout"
	
		- pokial je v kode, ktory pouzva Qt, tak prerobit na qDebug
		- resp. najst a pouzivat nejaku externu kniznicu na logovanie

	Ak sa pouzije cudzi existujuci kod tak, ze sa priamo jeho zdrojaky pridaju do existujuceho projektu
	
		- tak treba aby zozstal povodny a upravy sa riesili napr. v zdedenej triede


	WARNING-y - opravuju sa vsetky warningy v nasom kode (minimalizacia moznych problemov)

	pravidelne sa robi staticka analyzka kodu pomocou nastrojov cppcheck a cpplint
	
		- vid. metodiky ccpchceck, cpplint

	pravidelne sa zdrojovy kod formatuje pomocou nastroja astyle

	pravidelne sa merguju vsetky prace