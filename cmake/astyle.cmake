# ---------------------------------------------------------------
# -------------------- ASTYLE code beautifier--------------------
# ---------------------------------------------------------------
# use "make style" to format all C++ code
option(SKIP_ASTYLE "Skip using Astyle code formatter" OFF)
if (NOT SKIP_ASTYLE)

    find_package(Astyle)
    if (Astyle_FOUND)
        message(STATUS "Astyle FOUND")

        # Add a custom target
        if (WIN32)
            add_custom_target("style"
                    COMMAND
                    ${CMAKE_CURRENT_SOURCE_DIR}/../run_astyle.bat
                    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../"
                    COMMENT "Formating source code with Astyle."
                    VERBATIM
                    )
        else ()
            add_custom_target("style"
                    COMMAND
                    "${ASTYLE_EXECUTABLE}"
                    --options=${CMAKE_CURRENT_SOURCE_DIR}/../astyle.options
                    ${CMAKE_CURRENT_SOURCE_DIR}/../include/*.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp
                    COMMENT "Formating source code with Astyle."
                    VERBATIM
                    )
        endif ()
    endif ()
else ()
    message(STATUS "Skipped using ASTYLE")
endif ()


