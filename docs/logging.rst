Logovanie s knižnicou Easylogging++
===================================
Úrovne logovania
----------------

	Easylogging++ poskytuje tieto úrovne logovania:
	
		* Global 	Generická úroveň, reprezentujúca všetky ostatné
		* Trace 	Zachytávame informácie, vhodné na back-trackovanie rôznych problémov.
		* Debug 	Zaznamenávame informácie, vhodné pri vývoji aplikácie.
		* Fatal 	Používame keď nastane chyba, ktorá pravdepodobne ukončí program.
		* Error 	Používame keď nastane závažnejšia chyba ale program bude naďalej pracovať.
		* Warning 	Používame keď nastane chyba ale program bude naďalej pracovať.
		* Info 		Používame na zachytenie priebehu aplikácie.
		* Verbose 	Nepoužívame.
		* Unknown 	Nepoužívame.
	
Čo a ako treba logovať
----------------------

	Čo je potrebné logovať:
		* Začiatok každej metódy
		* Všetky možné errory / warningy

	Ako treba logovať:
		* Začiatok každej metódy logujeme v tvare
		
		::
		
			function(){
				LOG( INFO ) << "MENO_BALIKU/MENO_TRIEDY/MENO_FUNKCIE(PARAMETRE)"
			}
	
		Pozor, parametre používať iba v prípade, že je to vhodné
		
		* Všetky možné errory / warningy logujeme v tvare
		
		::
		
				LOG( WARNING/ERROR/FATAL ) << "MENO_BALIKU/MENO_TRIEDY/MENO_FUNKCIE(PARAMETRE)"
				
		V tomto prípade sa snažíme zalogovať aj všetky potrebné parametre, ktoré spôsobili warning / error
