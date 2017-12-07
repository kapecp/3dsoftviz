# ---------------------------------------------------------------
# ---------------- SPHINX documentation generator----------------
# ---------------------------------------------------------------

option(SKIP_SPHINX "Skip building Sphinx documentation" OFF)
if (NOT SKIP_SPHINX)

    find_package(Sphinx)
    if (Sphinx_FOUND)

        if (NOT DEFINED SPHINX_THEME)
            set(SPHINX_THEME sphinx_rtd_theme)
        endif ()

        if (NOT DEFINED SPHINX_THEME_DIR)
            set(SPHINX_THEME_DIR)
        endif ()
        message(STATUS "Sphinx FOUND")

        # configured documentation tools and intermediate build results
        set(BINARY_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}")

        # Sphinx cache with pickled ReST documents
        set(SPHINX_CACHE_DIR "${CMAKE_CURRENT_BINARY_DIR}/docs/_doctrees")

        # HTML output directory
        set(SPHINX_HTML_DIR "${CMAKE_CURRENT_BINARY_DIR}/docs/html")

        # prepare the parameters and setting for a build
        configure_file(
                "${CMAKE_CURRENT_SOURCE_DIR}/docs/conf.py.in"
                "${CMAKE_CURRENT_BINARY_DIR}/conf.py"
                @ONLY)

        # generate documentation
        add_custom_target(Sphinx_html
                ${SPHINX_EXECUTABLE}
                -q -b html
                -c "${CMAKE_CURRENT_BINARY_DIR}"
                -d "${SPHINX_CACHE_DIR}"
                "${CMAKE_CURRENT_SOURCE_DIR}/docs"
                "${SPHINX_HTML_DIR}"
                COMMENT "Building HTML documentation with Sphinx")

    endif ()
else ()
    message(STATUS "Skipped using SPHINX")
endif ()