# ---------------------------------------------------------------
# ---------------------- CPPcheck code analyser------------------
# ---------------------------------------------------------------
# use "make cppcheck" to check all C++ code with CPPcheck

option(SKIP_CPPCHECK "Skip using CPPcheck code analyser" OFF)
if (NOT SKIP_CPPCHECK)
    find_package(cppcheck)

    if (cppcheck_FOUND)
        message(STATUS "Cppcheck FOUND")

        file(GLOB_RECURSE ALL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp)
        if (WIN32)
            add_custom_target(cppcheck
                    COMMAND
                    ${CMAKE_CURRENT_SOURCE_DIR}/../run_cppcheck.bat
                    WORKING_DIRECTORY
                    "${CMAKE_CURRENT_SOURCE_DIR}/../"
                    COMMENT "Linting code via cpplint.py"
                    VERBATIM
                    )
        else ()
            add_custom_target(
                    cppcheck
                    COMMAND cppcheck
                    --enable=warning,style,performance,portability,information,missingInclude
                    --std=c++11
                    --library=qt.cfg
                    --verbose
                    --quiet
                    -j2
                    -I ${CMAKE_CURRENT_SOURCE_DIR}/../include/
                    ${ALL_SRC_FILES}
            )
        endif ()
    endif ()
else ()
    message(STATUS "Skipped using CPPCHECK")
endif ()