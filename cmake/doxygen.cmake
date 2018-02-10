# ------------------------------------------------------------
# -------------- DOXYGEN documentation generator--------------
# ------------------------------------------------------------
# Doxygen tool for generating documentation from annotated C++ sources

option(SKIP_DOXYGEN "Skip building Doxygen documentation" OFF)
if (NOT SKIP_DOXYGEN)

    find_package(Doxygen)
    option(BUILD_DOXYGEN_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

    if (BUILD_DOXYGEN_DOCUMENTATION)
        if (NOT DOXYGEN_FOUND)
            message(FATAL_ERROR "Doxygen is needed to build the documentation.")
        endif ()
        message(STATUS "Doxygen FOUND")

        # Doxyfile.in contains the parameters and setting for a build
        set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
        set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        configure_file(${doxyfile_in} ${doxyfile} @ONLY)

        # generate documentation
        add_custom_target(doxygen
                COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_dir}
                COMMENT "Generating API documentation with Doxygen"
                VERBATIM)

        install(DIRECTORY DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/html)
        install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc)

    endif ()
else ()
    message(STATUS "Skipped using DOXYGEN")
endif ()
