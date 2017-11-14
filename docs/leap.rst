LeapSenzor
==========

.. uml::

	!include uml/LeapSenzor.wsd

LeapThread dedi of QThread
- include Leap, LeapController
- vytvara samostatny thread pre leap senzor

LeapListener dedi od Listener
- include Leap, LeapActions, DirectionDetector, FingerPositionDetector
- prekonava funkcie Listenera ktory sa staraju o zmenu stavu leap senzora

LeapController dedi od Controller
- include Leap, LeapListener
- spusta a zastavuje pocuvanie leap senzora

LeapAction
- include DirectionDetector, MouseControl, CameraManipulator, Leap, Math, Core
- Obsahuje funkcie ktore sa vykonaju na zaklade detekovaneho gesta

FingerPositionDetector
- include Leap
- obsahuje funkcie na pracu s poziciou prstou

DirectionDetector
- include Leap, LeapMath
- obsahuje funkcie na pracu s natocenim roznych casti ruky
