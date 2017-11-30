# -----Dependencies - Searching external dependencies---------

# ------------------------------------------------------------
# --------------------------- Qt -----------------------------
# ------------------------------------------------------------
option(USE_QT4_FORCED "Force using Qt4 packages." OFF)

if (NOT USE_QT4_FORCED)
    # Find Qt5
    find_package(Qt5
            CONFIG COMPONENTS
            Widgets Gui OpenGL Sql
            WebEngine WebEngineWidgets
            Xml Network
            QUIET)
endif ()

# Find Qt4
if (NOT Qt5Core_FOUND)
    find_package(Qt4
            COMPONENTS
            QtCore QtGui QtOpenGL QtSql
            QtWebKit QtXml QtNetwork
            REQUIRED)
    set(QT_USE_QTOPENGL 1 )
    set(QT_USE_QTSQL 1 )
    set(QT_USE_QTWEBKIT 1 )
    set(QT_USE_QTXML 1 )
    set(QT_USE_QTNETWORK 1 )
    include(${QT_USE_FILE})
endif ()

if (USE_QT4_FORCED)
    message("Forced using Qt4 packages (deprecated)")
elseif (Qt5Core_FOUND)
    message(STATUS "Using Qt5 packages")
elseif (Qt4_FOUND)
    message(STATUS "Using Qt4 packages (deprecated)")
else ()
    message(STATUS "No suitable Qt packages found")
endif ()

# add special flag to GCC compiler when using Qt 4.6.x with GCC >4.6
if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND
        "${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}" VERSION_LESS "4.7" AND
        ${CMAKE_C_COMPILER_VERSION} VERSION_GREATER "4.6" )

    set( CMAKE_CXX_FLAGS "-fpermissive" )
endif()

# ------------------------------------------------------------
# --------------------- OpenSceneGraph -----------------------
# ------------------------------------------------------------
# Find OpenGL
find_package( OpenGL REQUIRED)

# Find OpenSceneGraph
find_package( OpenSceneGraph COMPONENTS
        osgGA
        osgUtil
        OpenThreads
        osg
        osgAnimation
        osgDB
        osgFX
        #osgIntrospection
        osgManipulator
        osgParticle
        osgShadow
        osgSim
        osgTerrain
        osgText
        osgVolume
        osgWidget
        )

message( STATUS "Using OpenSceneGraph version: " ${OPENSCENEGRAPH_VERSION} )

# OpenSceneGraph v3.5.5 removed osgQt (osgQt is now in it's own repository)
if( ${OPENSCENEGRAPH_VERSION} VERSION_GREATER 3.5.4 )
    message( "Building osgQt" )
    set( BUILD_OSGQT TRUE )
else()
    message( "Searching for osgQt" )
    find_package( osgQt )
    list( APPEND ALL_SYSTEM_HEADERS ${OSGQT_INCLUDE_DIR} )
endif( )

find_package( osgViewer )

list( APPEND ALL_SYSTEM_HEADERS
        ${OPENSCENEGRAPH_INCLUDE_DIRS}
        ${OSGVIEWER_INCLUDE_DIR}
        )

if( MSVC )
    list( APPEND ALL_COMPILE_DEFINITIONS -DOSG_LIBRARY_STATIC )
endif()

# ------------------------------------------------------------
# -------------------------- OpenCV --------------------------
# ------------------------------------------------------------
option(SKIP_OPENCV "Skip using OpenCV computer vision library." OFF)

# Find OpenCV
if (NOT SKIP_OPENCV)
    if (APPLE)
        find_package(OpenCV 2.4.13 PATHS "/usr/local/opt/opencv@2")
    else ()
        find_package(OpenCV)
    endif ()
endif ()

if( OpenCV_FOUND )
    if( WIN32 )
        list( APPEND ALL_COMPILE_DEFINITIONS -DNOMINMAX )
    endif()

    #list( APPEND ALL_SYSTEM_HEADERS	${OpenCV_INCLUDE_DIRS} )
    list( APPEND ALL_COMPILE_DEFINITIONS -DOPENCV_FOUND )

    message(STATUS "OpenCV Found: " ${OpenCV_VERSION})
    message(STATUS "OpenCV_LIB_DIR=${OpenCV_LIB_DIR}")
    message(STATUS "OpenCV_LIB_DIR=${OpenCV_LIBS}")
elseif (SKIP_OPENCV)
    message(STATUS "Skipped using OpenCV")
else ()
    message(STATUS "OpenCV NOT FOUND")
endif ()

# ------------------------------------------------------------
# -------------------------- OpenNI2--------------------------
# ------------------------------------------------------------
option(SKIP_OPENNI2 "Skip building OpenNI2 features." OFF)

# Find OPENNI2
if (NOT SKIP_OPENNI2)
    find_package(OPENNI2)
    if (OPENNI2_FOUND)

        list(APPEND ALL_SYSTEM_HEADERS ${OPENNI2_INCLUDE_DIRS})

        list(APPEND ALL_COMPILE_DEFINITIONS -DOPENNI2_FOUND)

        message(STATUS "OpenNI2 Found:" ${OPENNI2_VERSION})
        message(STATUS "OPENNI2_INCLUDE_DIRS=${OPENNI2_INCLUDE_DIRS}")
        message(STATUS "OPENNI2_LIBRARIES=${OPENNI2_LIBRARIES}")
    elseif (SKIP_OPENNI2)
        message(STATUS "Skipped building OpenNI2")
    else ()
        message(STATUS "OpenNI2 NOT FOUND")
    endif ()
endif ()

# ------------------------------------------------------------
# --------------------------- Nite2 --------------------------
# ------------------------------------------------------------
option(SKIP_NITE2 "Skip building Nite2 features." OFF)

# Find NITE2
if (NOT SKIP_NITE2)
    find_package(NITE2)
    if (NITE2_FOUND)

        list(APPEND ALL_SYSTEM_HEADERS ${NITE2_INCLUDE_DIRS} ${NITE2_OPENNI2_DRIVER_DIRS})

        list(APPEND ALL_COMPILE_DEFINITIONS -DNITE2_FOUND)
        message(STATUS "NITE2 Found:" ${NITE2_VERSION})
        message(STATUS "NITE2_INCLUDE_DIRS=${NITE2_INCLUDE_DIRS}")
        message(STATUS "NITE2_LIBRARIES=${NITE2_LIBRARIES}")
    elseif (SKIP_NITE2)
        message(STATUS "Skipped building NITE2")
    else ()
        message(STATUS "NITE2 NOT FOUND")
    endif ()
endif ()

# ------------------------------------------------------------
# -------------------------- FREENECT2 -----------------------
# ------------------------------------------------------------
option(SKIP_FREENECT2 "Skip building Freenect2 drivers for Kinect." OFF)

# Find FREENECT2
if (NOT SKIP_FREENECT2)
    find_package(FREENECT2)

    if (FREENECT2_FOUND)

        list(APPEND ALL_SYSTEM_HEADERS ${FREENECT2_INCLUDE_DIRS})
        list(APPEND ALL_COMPILE_DEFINITIONS -DFREENECT2_FOUND)

        message(STATUS "FREENECT2 Found:" ${FREENECT2_VERSION})
        message(STATUS "FREENECT2_INCLUDE_DIRS=${FREENECT2_INCLUDE_DIRS}")
        message(STATUS "FREENECT2_LIBRARIES=${FREENECT2_LIBRARIES}")
    elseif (SKIP_FREENECT2)
        message(STATUS "Skipped building FREENECT2")
    else ()
        message(STATUS "FREENECT2 NOT FOUND")
    endif ()
endif ()

# ------------------------------------------------------------
# ----------------------- KINECT SDK -------------------------
# ------------------------------------------------------------
if (WIN32 AND MSVC)

    option(SKIP_KINECT_SDK "Skip building Kinect SDK for Windows." OFF)

    if (NOT SKIP_KINECT_SDK)

        find_package(KINECTSDK)

        if (KINECTSDK_FOUND)

            list(APPEND ALL_SYSTEM_HEADERS ${KINECTSDK_INCLUDE_DIRS})
            list(APPEND ALL_COMPILE_DEFINITIONS -DKINECTSDK_FOUND)

            # find_package( SPEECHSDK )
            # if( SPEECHSDK_FOUND )
            # include_directories( SYSTEM ${SPEECHSDK_INCLUDE_DIRS} )
            # add_definitions( -DSPEECHSDK_FOUND )
            # message( STATUS "SPEECHSDK FOUND" )
            # else( SPEECHSDK_FOUND )
            # message ( STATUS "SPEECHSDK NOT FOUND" )
            # endif()

            message(STATUS "KINECTSDK Found:" ${KINECTSDK_VERSION})
            message(STATUS "KINECTSDK_INCLUDE_DIRS=${KINECTSDK_INCLUDE_DIRS}")
            message(STATUS "KINECTSDK_LIBRARIES=${KINECTSDK_LIBRARIES}")
        elseif (SKIP_KINECT_SDK)
            message(STATUS "Skipped building KINECTSDK")
        else ()
            message(STATUS "KINECTSDK NOT FOUND")
        endif ()
    endif ()
endif ()

# ------------------------------------------------------------
# ---------------- BOOST C++ boosting libraries---------------
# ------------------------------------------------------------
# Find Boost
set( Boost_USE_STATIC_LIBS OFF )
find_package( Boost 1.39 REQUIRED )
#add_definitions( -DBOOST_ALL_DYN_LINK )

# Include directories
list( APPEND ALL_SYSTEM_HEADERS	${Boost_INCLUDE_DIRS}  )

# ------------------------------------------------------------
# -----------------LEAP Leap Motion library-------------------
# ------------------------------------------------------------
#enable leap orion on windows
if( WIN32 )
    set( USE_LEAP_ORION ON )
endif()

option(SKIP_LEAP "Skip building Leap Motion library." OFF)
if (NOT SKIP_LEAP)

    find_package(LEAP)
    if (LEAP_FOUND)

        list(APPEND ALL_COMPILE_DEFINITIONS -DLEAP_FOUND)
        list(APPEND ALL_SYSTEM_HEADERS ${LEAP_INCLUDE_DIRS})

        message(STATUS "LEAP Found:" ${LEAP_VERSION})
        message(STATUS "LEAP_INCLUDE_DIRS=${LEAP_INCLUDE_DIRS}")
        message(STATUS "LEAP_LIB_DIR=${LEAP_LIBRARIES}")
    else ()
        message(STATUS "LEAP Not Found")
    endif ()
endif ()

# ------------------------------------------------------------
# ----------------- PCL Point Cloud Library-------------------
# ------------------------------------------------------------
option(SKIP_PCL "Skip building Point Cloud Library." OFF)
if (NOT SKIP_PCL)

    find_package(PCL 1.8)
    if (PCL_FOUND)

        list(APPEND ALL_COMPILE_DEFINITIONS -DPCL_FOUND)

        list(APPEND ALL_SYSTEM_HEADERS ${PCL_INCLUDE_DIRS})

        link_directories(${PCL_LIBRARY_DIRS})
        add_definitions(${PCL_DEFINITIONS})
        message(STATUS "PCL FOUND")
    elseif (SKIP_PCL)
        message(STATUS "Skipped building PCL")
    else ()
        message(STATUS "PCL NOT FOUND")
    endif ()
endif ()

# ------------------------------------------------------------
# ------------------------- FGLOVE ---------------------------
# ------------------------------------------------------------
option(SKIP_FGLOVE "Skip building FGLOVE module" ON)
if (NOT SKIP_FGLOVE)

    find_package(FGLOVE)
    if (FGLOVE_FOUND)
        add_definitions(-DFGLOVE_FOUND)
        include_directories(SYSTEM
                ${FGLOVE_INCLUDE_DIRS}
                )
        message(STATUS "FGLOVE_LIB_DIR=${FGLOVE_LIBRARIES}")
    elseif (SKIP_FGLOVE)
        message(STATUS "Skipped building FGLOVE")
    else ()
        message(STATUS "FGLOVE Not Found")
    endif ()
endif ()

# ------------------------------------------------------------
# ------------------- MOUSE3D 3Dconnexion --------------------
# ------------------------------------------------------------
option(SKIP_MOUSE3D "Skip building 3DMouse support" OFF)
if (NOT SKIP_MOUSE3D)

    if (WIN32 OR APPLE)
        find_package(MOUSE3D)
        if (WIN32)
            if (MOUSE3D_FOUND)
                include_directories(SYSTEM ${MOUSE3D_INCLUDE_DIRS})
                add_definitions(-DMOUSE3D_FOUND)
                message(STATUS "3DMouse FOUND")
            endif ()
        elseif (APPLE)
            if (MOUSE3D_FOUND)
                include_directories(SYSTEM ${MOUSE3D_INCLUDE_DIRS})
                add_definitions(-DMOUSE3D_FOUND)
                message(STATUS "3DMouse FOUND")
            endif ()
        endif ()
    elseif (UNIX)
        set(MOUSE3D_FOUND TRUE )
        include_directories(SYSTEM include/Mouse3d/LibMouse3d/Unix)
        find_package(X11 REQUIRED)
        include_directories(SYSTEM ${X11_INCLUDE_DIR})
        add_definitions(-DMOUSE3D_FOUND)
        message(STATUS "3DMouse FOUND")
    elseif (SKIP_MOUSE3D)
        message(STATUS "Skipped building 3DMouse")
    else ()
        message(STATUS "3DMouse NOT FOUND")
    endif ()
endif ()