
find_package(PythonInterp)

find_file(CPPLINT_SCRIPT

	NAMES cpplint.py cpplint cpplint.exe
	PATHS

	NAMES cpplint.py cpplint.exe
	PATHS 
		c:/Python27/Scripts
    
		/usr/local/bin
		c:/Python27/Scripts
)

mark_as_advanced(CPPLINT_SCRIPT)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args( CPPLINT DEFAULT_MSG PYTHONINTERP_FOUND CPPLINT_SCRIPT )
