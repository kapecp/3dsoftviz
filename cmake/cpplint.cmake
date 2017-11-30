# ---------------------------------------------------------------
# ----------------- CPPLINT code style checker-------------------
# ---------------------------------------------------------------
# use "make cpplint" to check all C++ code with cpplint.py

option(SKIP_CPPLINT "Skip using Cpplint code checker" OFF)
if (NOT SKIP_CPPLINT)

    find_package(cpplint)
    if (CPPLINT_FOUND)
        message(STATUS "CppLint FOUND")

        FILE(GLOB_RECURSE ALL_INCLUDE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../include/*.h)
        FILE(GLOB_RECURSE ALL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp)

        if (WIN32)
            add_custom_target("cpplint"
                    COMMAND
                    ${CMAKE_CURRENT_SOURCE_DIR}/../cpplint.bat # NOTE: this will probably work only on Linux/OSX
                    WORKING_DIRECTORY
                    "${CMAKE_CURRENT_BINARY_dir}"
                    COMMENT "Linting code via cpplint.py"
                    VERBATIM
                    )
        else ()
            add_custom_target("cpplint"
                    COMMAND
                    "${PYTHON_EXECUTABLE}"
                    "${CPPLINT_SCRIPT}"
                    ${ALL_INCLUDE_FILES}
                    ${ALL_SRC_FILES}
                    2>&1 | tee cpplint-report.txt # NOTE: this will probably work only on Linux/OSX
                    WORKING_DIRECTORY
                    "${CMAKE_CURRENT_BINARY_dir}"
                    COMMENT "Linting code via cpplint.py"
                    VERBATIM
                    )
        endif ()
    endif ()
else ()
    message(STATUS "Skipped using CPPLINT")
endif ()