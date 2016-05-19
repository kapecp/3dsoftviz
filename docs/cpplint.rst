Cpplint
=======

pouzity cpplint dostupny tu:

https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=1&cad=rja&uact=8&ved=0CB8QFjAA&url=http%3A%2F%2Fgoogle-styleguide.googlecode.com%2Fsvn%2Ftrunk%2Fcpplint%2Fcpplint.py&ei=Ii6pVO_bK8mAUZSVgSg&usg=AFQjCNGnSkFFrrX3TIA0XxI1TIaQVKTGxw&sig2=blEaRr0yiDS-Eh8vRmUDJQ&bvm=bv.82001339,d.d24

je to Python script

	- pokial by bol problem spustat, mozem to riesit ja

ako pouzivat (Linux)
--------------------

	pouzite testy su nastavene v CPPLINT.cfg
	
	dostupne testy sa vypisu: cpplint.py --filter=

	spustit v include adresari:
		cpplint.py Aruco/* Core/* Data/* Importer/* Layout/* Math/* Network/* Network/executors/*  \
		OpenCV/* QOpenCV/* Speech/* Viewer/* Clustering/* Clustering/Figures/* Fglove/* Kinect/*  \
		Kinect/RansacSurface/*  Manager/* Model/* OsgQtBrowser/* QOSG/* Util/*   2>&1 | tee report-include.txt

	spustit v src adresari:
		cpplint.py Aruco/* Core/* Data/* Importer/* Layout/* Math/* Network/* Network/executors/*  \
		OpenCV/* QOpenCV/* Speech/* Viewer/* Clustering/* Clustering/Figures/* Fglove/* Kinect/*  \
		Kinect/RansacSurface/*  Manager/* Model/* OsgQtBrowser/* QOSG/* Util/*   2>&1 | tee report-src.txt

poznamky k reportom
-------------------	

	!!! ak je nieco nejasne, treba sa opytat !!!

	ignorovat hlasky pre externy kod:
		noiseutils, 
		qtcolorpicker

		ciastocne pre cameramanipulator, QGraphicsViewAdapter

dolezite hlasky typu:
~~~~~~~~~~~~~~~~~~~~~

		Constructors callable with one argument should be marked explicit
		
			- nastudovat, asi staci pridat keyword explicit

		Is this a non-const reference? If so, make const or use a pointer
		
			- nastudovat (ci to nieje false-positive)

		Use int16/int64/etc, rather than the C type long
		
			- nastudovat

		Do not use namespace using-directives.
		
			- odstranit "using namemspace" (okrem externeho/cudieho kodu )

		Consider using rand_r(...) instead of rand(...) for improved thread safety.
		
			- kedze mame vlakna, asi by bolo vhodne. vid.:
				https://stackoverflow.com/questions/3973665/how-do-i-use-rand-r-and-how-do-i-use-it-in-a-thread-safe-way

		Are you taking an address of a cast?  This is dangerous: could be a temp var.  Take the address before doing the cast, rather than after
		 
			!!! indikuje vazny problem !!!


	flase-positive:
	
		Clustering/Figures/Cube.h:28:  Add #include <algorithm> for transform  [build/include_what_you_use] [4]
			Cube ma metodu tranform -> netreba include


ostatne hlasky:
~~~~~~~~~~~~~~~
	
		- treba skontrolovat - a mali by sa opravit (resp. uz som ich opravil ;-)

