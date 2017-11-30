.. contents::

Integrácia Kinect-u
===================

Zdroj: Tímový projekt č. 18 2013/2014

K jedným z najznámejších zariadení, ktoré umožňujú ovládanie v
obohatenej realite patrí Kinect, preto v našom riešení zohráva dôležitú
úlohu. Na implementáciu sme sa na rozdiel od klasického prístupu SDK
Kinect knižníc od Windowsu rozhodli použiť open source riešenie OpenNI2
a na ňom založenú knižnicu NiTE2, ktorá je určená na prácu s
rozpoznávaním objektov ako sú ruky, kostra človeka alebo vytvorenie
hĺbkovej mapy priestoru.

Indentifikované Use-case-y pre Kinect:

-  použitie rúk na ovládanie grafu

-  hýbanie myškou

-  implementované gestá

Kinect v programe je určený na viacero funkcií ako to je znázornené vo
forme diagramu prípadu použitia na Obrázku nižšie. Medzi najvýznamnejšie
patrí použitie rúk na ovládanie grafu a hýbanie myškou, taktiež významnú
úlohu hrajú implementované gestá.

|image0|

Figure 1: Prípady použitia Kinnectu v projekte

Používateľ je pomocou jednej ruky schopný ovládať myš, kde gestá ruky sú
namapované na akcie myši. Používateľ vie pomocou oboch rúk ovládať graf,
pohybovať s ním a taktiež má k dispozícií gestá na určité akcie s
grafom. 3DSoftviz rozpoznáva hlasové povely pomocou zariadenia Kinect
(Modul Kinect Speech).

3DSoftviz je schopný rozpoznať tvár používateľa pomocou modulu
FaceRecognition. Na základe rozpoznanej tváre sa vykonáva transformácia
vykresleného grafu.

Pred samotným rozpoznaním je potrebné, aby sa príslušný klasifikátor
tvárí naučil. Následne ju správne klasifikuje na základe haar-like
príznakov.

Medzi ďalšie vykonané zmeny patrí:

-  pridanie možnosti rotovať a posúvať graf

-  otáčania kamery pomocou pohybov, pozície hlavy alebo značkou

Reprezentácia vnorených grafov, obmedzovanie layoutu, doplnenie funkcií pre spájanie a rozklikávanie uzlov
==========================================================================================================

Zdroj: Tímový projekt č. 18 2010/2011

Vnorený graf predstavuje graf, ktorý sa nachádza v rámci uzla grafu,
ktorý je

vnorenému grafu nadradený.

Návrh vychádza z existujúcej štruktúry, teda dátovej reprezentácie uzol,
hrana,

graf. Do týchto existujúcich prvkov, respektíve tried, sú doplnené
atribúty a

metódy potrebné pre vytváranie kompozitnej dátovej štruktúry na úrovni
uzlov

grafu, ktoré majú buď rodičovské uzly (nadradené), alebo obsahujú
vnorené uzly.

Pridaná možnosť určiť obmedzenie, kde v priestore sa môže nachádzať daný
uzol.

Zmeny zasahujú do modulu Layout:

-  nové triedy na definovanie a výpočet obmedzení

-  zásahy do behu layoutovacieho algoritmu (trieda FRAlgorithm)

-  zmena pozície navrhnutej obmedzovačom na pozíciu spĺňajúcu obmedzenie

-  ignorovanie uzlov s nastaveným parametrom ignored (napríklad kvôli
   pomocným

   uzlom, ktoré definujú tvar obmedzenia a nemôžu vplývať na iné uzly a
   tiež nemôžu

   byť presúvané layoutovacím algoritmom).

Do používateľského rozhrania sú pridané tlačidlá na pridanie obmedzení:

na povrch gule, do gule a do roviny. Interakcia prebieha nasledovne:

1. Používateľ vyberie uzly, ktorých pozícia má byť obmedzená.

2. Zvolí pridanie obmedzenia (tlačidlom podľa typu obmedzenia).

3. Pridajú sa pomocné uzly (do stredu výberu), ktoré definujú tvar
   obmedzenia (pre guľu 2 uzly – stred a povrch; pre rovinu 3 uzly).

4. Presúvaním pomocných uzlov používateľ definuje polohu a iné parametre
   tvaru obmedzenia.

Bola pridaná funkcionalita pre spájanie a rozdeľovanie uzlov, ktoré sa
neskôr začlenili pod záložku clustering.

|image1|

Pribudli možnosti používateľského rozhrania pre prácu s databázou –
ukladanie a načítavanie grafov (Obr. nižšie).

|image2|

Bol doplnený modul Data o pridávanie pomocných uzlov, ktoré ovplyvňujú
rozloženie multihrán (Obr. nižšie).

|image3|

Zobrazenie grafu s jednou multihranou

Filtrovanie zobrazeného hypergrafu pomocou dopytovacieho jazyka, doplnenie magickej šošovky
===========================================================================================

Zdroj: Bc. Zuzana Číková: Využitie dopytovacích jazykov a magických
šošoviek pri vizualizácii grafov (diplomová práca)

Rozšírenie projektu 3DSoftviz (vtedy 3DVisual) o 2 väčšie ucelené časti:

1. rozšírenie o možnosť filtrovať zobrazeného hypergrafu pomocou
   dopytovacieho jazyka

2. doplnenie magickej šošovky do upraveného systému. Systém, do ktorého
   bude výsledné riešenie integrované umožňuje zobrazenie grafov v 3D
   priestore a kompletná implementácia umožňuje nasledujúce
   funkcionality:

-  používateľ zadá dopyt na odfiltrovanie zobrazeného hypergrafu.

-  používateľ môže špecifikovať parametre modifikujúce zobrazenie v
   dopytovacom reťazci; (veľkosť, transparencia, farba a zobrazenie
   popisov)

-  výsledný dopytovací jazyk umožnuje modifikovať ľubovoľné prvky
   hypergrafu, ktoré sa v dopyte môžu vyskytnúť: uzly, incidencie aj
   hyperhrany.

-  pomocou dopytu možné filtrovanie lľubovoľného počtu hyperhrán

-  dopyt zadaný v existujúcom dopytovacom jazyku transformovaný na
   hypergrafovú štruktúru

-  možné lokálne filtrovať dáta pomocou magickej šošovky umiestnenej v
   strede obrazovky, ktorá zobrazuje vizualizáciu vyfiltrovaného a
   modifikovaného hypergrafu.

    |image4|

Vizualizácia hypergrafu bez použitia šošovky (vľavo) a s použitím
šošovky fil- trujúcej vzťahy typu inheritance (vpravo).

Pridané boli dva ovládacie prvky:

-  Dopytovací panel - slúži na zadávanie dopytov, ktoré sú v súlade s
   pravidlami dopytovacieho jazyka definovaného v zdrojovej práci.

-  Ovládač šošovky - slúži na vypnutie/zapnutie šošovky. Pri jej vypnutí
   ostane vo vizualizačnom okne zobrazený iba pôvodný hypergraf,
   prípadne prázdna scéna, pokiaľ dopyt ešte nebol zadaný.

|image5|

Zobrazenie tried implementujúcich tri konkrétne rozhrania.

Vizuálne uzly
=============

Zdroj: Bc. Jakub Ukrop: Vizuálne sprístupňovanie informácií uložených v
grafoch (diplomová práca)

Vizuálny uzol je billboard v tvare obdĺžnika umiestnený v 3D priestore.
Má 3 hlavné grafické atribúty: farbu, veľkosť a obsah. Farba slúži na
odlíšenie uzlov podľa niektorého z parametrov priradeného uzlu pri na-
čítavaní zo súboru.

Otvorené uzly majú ofarbený rám, uzavreté uzly sú ofarbené celé. Ukážka
ofar- benia je na obrázku nižšie.

|image6|

Všetky druhy vizuálnych uzlov: text, 3D objekt, webová stránka a
obrázok.

Veľkosť uzla alebo skupiny uzlov je nastaviteľná manuálne používateľom.
Slúži najmä na zvýraznenie podrobností obsahu uzla.

Obsah uzla je zobrazený len vtedy, ak je uzol otvorený (tak definujeme
otvorenosť uzla). Obsah vypĺňa celý jeho rám (výnimkou sú 3D objekty) a
je vždy paralelný s rovinou premietania (billboarding).

Podporované sú 4 typy obsahu:

-  obrázok – v jednom s formátov png, jpg, gif, tiff

-  text – jednoduchý neformátovaný text

-  webová stránka – zobrazená vo webovom prehladači

-  3D objekt – vo formáte osg

Typ obsahu je možne meniť za behu aplikácie. Hrany, podobne ako uzavreté
uzly, sú zobrazené ako 2D textúry natočené ku kamere.

|image7|

Graf predsokratovských filozofov a ich vzťahov.

Kolaboratívne virtuálne prostredie 
===================================

Zdroj: Bc. Ivan Pleško:

Kolaboratívne virtuálne prostredie pre vizualizáciu dát

Implementácia plne funkčného kolaboratívneho virtuálneho prostredia pre
vizualizáciu dát (diplomová práca)

Po spustení aplikácie vidia všetci používatelia prázdnu scénu s
horizontom. Po dohode sa rozhodnú, ktorý z nich bude v kolaborácii
figurovať ako server. Ostatní používatelia sa k serveru pripoja zadaním
správnej IP adresy. Graf, ktorý vidno na obrázku 5.1, bol na serverovej
inštancii načítaný zo súboru a k ostatným klientom sa preniesol pomocou
siete. Na tomto obrázku vidíme ukážku obrazovky dvoch spustených
inštancií (jedna serverová, druhá klientská). Obaja používatelia sa
pozerajú na ten istý graf, no každý z takej perspektívy, ktorá mu
vyhovuje.

|image8|

*Dvaja používatelia si prezerajú ten istý graf, každý z takej
perspektívy, ktorá mu vyhovuje *

|image9|

Používateľ Server pozoruje používateľ Nick, ktorý našiel
zaujímavú oblasť grafu a zvýraznil ju obalením do gule.

.. |image0| image:: images/feature-review/kinnect-usecases.png
   :width: 5.26389in
   :height: 4.50000in
.. |image1| image:: images/feature-review/mode-merging-and-splitting.png
   :width: 1.87500in
   :height: 0.75000in
.. |image2| image:: images/feature-review/db-ui.png
   :width: 5.11111in
   :height: 3.16667in
.. |image3| image:: images/feature-review/multiedge-example.png
   :width: 3.91667in
   :height: 2.12500in
.. |image4| image:: images/feature-review/magic-lens-example-1.png
   :width: 6.29583in
   :height: 2.19097in
.. |image5| image:: images/feature-review/magic-lens-example-2.png.png
   :width: 6.29583in
   :height: 4.42569in
.. |image6| image:: images/feature-review/visual-node-example-1.png
   :width: 6.29583in
   :height: 2.52847in
.. |image7| image:: images/feature-review/visual-nose-example-2.png
   :width: 6.29583in
   :height: 3.38333in
.. |image8| image:: images/feature-review/colab-example-1.png
   :width: 6.29583in
   :height: 3.48125in
.. |image9| image:: images/feature-review/colab-examle-2.png
   :width: 6.29583in
   :height: 4.05278in
