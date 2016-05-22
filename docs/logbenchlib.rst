Logovacie a benchmarkingové knižnice
====================================

Logovacie knižnice
------------------

.. exceltable::
   :file: c:\Python27\3dsoftviz_doc\log_benchmark_libs.xls
   :header: 1
   :sheet: Loggers
   :selection: A1:H10
 
Pri použití Easylogging++:

   * #include "easylogging++.h” len tam kde treba použiť
   * INITIALIZE_EASYLOGGINGPP je len raz v main-e
 
Zdroje:
^^^^^^^

	* Loguru_
	* Logog_
	* Spdlog_
	* Plog_
	* Easyloggingpp_
	* G3log_
	* Glog_
	* log4cpp_
	* log4cplus_
	* log4cxx_
	* BOOSTlog_
	
Benchmarkingové kniznice
------------------------
   
.. exceltable::
   :file: c:\Python27\3dsoftviz_doc\log_benchmark_libs.xls
   :header: 1
   :sheet: Benchmarks
   :selection: A1:H5
   
Celero je najlepším výberom, len potrebuje C++11 compiler.

Pre iné platformy ako Windows je možné použiť Hayai.

   
Zdroje:
^^^^^^^

	* Hayai_
	* Celero_
	* Nonius_
	* Googlebenchmark_
   
.. _loguru: https://github.com/emilk/loguru
.. _logog: https://johnwbyrd.github.io/logog/index.html
.. _spdlog: https://github.com/gabime/spdlog
.. _plog: https://github.com/SergiusTheBest/plog
.. _easyloggingpp: https://github.com/easylogging/easyloggingpp/blob/master/README.md
.. _g3log: https://bitbucket.org/KjellKod/g2log/src https://github.com/KjellKod/g3log
.. _glog: https://github.com/google/glog
.. _log4cpp: http://log4cpp.sourceforge.net/ 
.. _log4cplus: https://sourceforge.net/projects/log4cplus 
.. _log4cxx: http://logging.apache.org/log4cxx/index.html
.. _boostlog: http://www.boost.org/doc/libs/1_60_0/libs/log/doc/html/index.html
.. _hayai: https://github.com/nickbruun/hayai
.. _celero: https://github.com/DigitalInBlue/Celero
.. _nonius: https://github.com/rmartinho/nonius
.. _googlebenchmark: https://github.com/google/benchmark