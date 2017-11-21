
include( ExternalProject )

# Generate cache
configure_file( ../config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/config.cmake )


# Build libnoise
ExternalProject_Add( dep_libnoise
       PREFIX libnoise
       SOURCE_DIR ${DEP_SOURCE}/libnoise
       BINARY_DIR libnoise
       CMAKE_ARGS
       -DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
       -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
       -C ${CMAKE_CURRENT_BINARY_DIR}/config.cmake
       )

add_library( libnoise SHARED IMPORTED GLOBAL)
add_dependencies( libnoise dep_libnoise )
set_target_properties( libnoise PROPERTIES
       IMPORTED_LOCATION ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}noise${CMAKE_SHARED_LIBRARY_SUFFIX} )

if( WIN32 )
       set_target_properties( libnoise PROPERTIES
              IMPORTED_LOCATION ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}noise${CMAKE_SHARED_LIBRARY_SUFFIX}
              IMPORTED_IMPLIB ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}noise${CMAKE_STATIC_LIBRARY_SUFFIX} )

       if( MSVC )
              # set "lib" prefix for MSVC, because libnoise.def file requires that the library is named "libnoise"
              set( MISS_MSVC_PREFIX lib )
              set_target_properties( libnoise PROPERTIES
                     IMPORTED_LOCATION ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}${MISS_MSVC_PREFIX}noise${CMAKE_SHARED_LIBRARY_SUFFIX}
                     IMPORTED_IMPLIB ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}${MISS_MSVC_PREFIX}noise${CMAKE_STATIC_LIBRARY_SUFFIX} )

       endif()
endif()


# Aruco
if( OpenCV_FOUND )
       # Build aruco
       ExternalProject_Add( dep_aruco
              PREFIX aruco
              SOURCE_DIR ${DEP_SOURCE}/aruco
              BINARY_DIR aruco
              CMAKE_ARGS
              -DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
              -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
              -C ${CMAKE_CURRENT_BINARY_DIR}/config.cmake
              )

       add_library( aruco SHARED IMPORTED GLOBAL)
       add_dependencies( aruco dep_aruco )
       set_target_properties( aruco PROPERTIES
              IMPORTED_LOCATION ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}aruco${CMAKE_SHARED_LIBRARY_SUFFIX} )

       if( WIN32 AND MSVC )
              set_target_properties( aruco PROPERTIES
                     IMPORTED_LOCATION ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}aruco${CMAKE_SHARED_LIBRARY_SUFFIX}
                     IMPORTED_IMPLIB ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}aruco${CMAKE_STATIC_LIBRARY_SUFFIX} )
       endif()
endif() # OpenCV_FOUND


# Build osgModeling
ExternalProject_Add( dep_osgModeling
       PREFIX osgModeling
       SOURCE_DIR ${DEP_SOURCE}/osgModeling
       BINARY_DIR osgModeling
       CMAKE_ARGS
       -DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
       -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
       -DBUILD_PLUGINS=ON
       -C ${CMAKE_CURRENT_BINARY_DIR}/config.cmake
       )

add_library( osgModeling SHARED IMPORTED GLOBAL)
add_dependencies( osgModeling dep_osgModeling )
set_target_properties( osgModeling PROPERTIES
       IMPORTED_LOCATION ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}osgModeling${CMAKE_SHARED_LIBRARY_SUFFIX} )

if( WIN32 AND MSVC )
       set_target_properties( osgModeling PROPERTIES
              IMPORTED_LOCATION ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}osgModeling${CMAKE_SHARED_LIBRARY_SUFFIX}
              IMPORTED_IMPLIB ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}osgModeling${CMAKE_STATIC_LIBRARY_SUFFIX} )
endif()

# Build osgQt
if( BUILD_OSGQT )
       ExternalProject_Add( dep_osgQt
              PREFIX osgQt
              SOURCE_DIR ${DEP_SOURCE}/osgQt
              BINARY_DIR osgQt
              CMAKE_ARGS
              -DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
              -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
              -C ${CMAKE_CURRENT_BINARY_DIR}/config.cmake
              )

       add_library( osgQt SHARED IMPORTED GLOBAL)
       add_dependencies( osgQt dep_osgQt )

       # osgQt's build system sets different library name based on Qt version
       if( Qt5Widgets_FOUND )
              set( OSGQT_NAME osgQt5 )
       else()
              set( OSGQT_NAME osgQt )
       endif()

       # osgQt's build system sets different library name posfixes based on build type
       # TODO: RelWithDebInfo, MinSizeRel
       # NOTE: we could use ExternalProject_Get_Property
       if( CMAKE_BUILD_TYPE MATCHES "Debug" )
              set( OSGQT_POSTFIX "d" )
       else()
              set( OSGQT_POSTFIX "" ) # release
       endif()

       set_target_properties( osgQt PROPERTIES
              IMPORTED_LOCATION ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}${OSGQT_NAME}${OSGQT_POSTFIX}${CMAKE_SHARED_LIBRARY_SUFFIX} )

       if( UNIX AND NOT APPLE )
              set_target_properties( osgQt PROPERTIES
                     IMPORTED_LOCATION ${DEP_BIN}/lib64/${CMAKE_SHARED_LIBRARY_PREFIX}${OSGQT_NAME}${OSGQT_POSTFIX}${CMAKE_SHARED_LIBRARY_SUFFIX} )
       endif()

       if( WIN32 AND MSVC )
              set_target_properties( osgQt PROPERTIES
                     IMPORTED_LOCATION ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}${OSGQT_NAME}${OSGQT_POSTFIX}${CMAKE_SHARED_LIBRARY_SUFFIX}
                     IMPORTED_IMPLIB ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}${OSGQT_NAME}${OSGQT_POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX} )
       endif()
endif()


# Build igloo
ExternalProject_Add( dep_igloo
       PREFIX igloo
       SOURCE_DIR ${DEP_SOURCE}/igloo
       BINARY_DIR igloo
       CMAKE_ARGS
       -DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
       -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
       -DBUILD_TESTS=ON
       -C ${CMAKE_CURRENT_BINARY_DIR}/config.cmake
       # little trick:
       # this solves building igloo twice (building normal target and building unity target)
       # prevent building the default target (make install)
       BUILD_COMMAND ""
       # force building the install_unity target
       INSTALL_COMMAND "${CMAKE_COMMAND}" --build . --target install_unity# -- "-j${N}"
       )

add_library( igloo INTERFACE IMPORTED GLOBAL)
add_dependencies( igloo dep_igloo )


# Building Lua or LuaJIT
if ( MSVC OR APPLE )
       # Build Lua
       ExternalProject_Add ( dep_lua
              PREFIX lua
              SOURCE_DIR ${DEP_SOURCE}/lua
              BINARY_DIR lua
              CMAKE_ARGS
              -DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
              -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
              -C ${CMAKE_CURRENT_BINARY_DIR}/config.cmake
              )

       # add lua library
       add_library( lua SHARED IMPORTED GLOBAL)
       add_dependencies( lua dep_lua )
       set_target_properties( lua PROPERTIES
              IMPORTED_LOCATION ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}lua${CMAKE_SHARED_LIBRARY_SUFFIX} )

       if( WIN32 AND MSVC )
              set_target_properties( lua PROPERTIES
                     IMPORTED_LOCATION ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}lua${CMAKE_SHARED_LIBRARY_SUFFIX}
                     IMPORTED_IMPLIB ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}lua${CMAKE_STATIC_LIBRARY_SUFFIX} )
       endif()

else ()
       # NOTE: Diluculum requires luaconf.h from original Lua, so original Lua headers must be available

       # Build LuaJIT
       ExternalProject_Add ( dep_luajit
              PREFIX luajit
              SOURCE_DIR ${DEP_SOURCE}/luajit
              BINARY_DIR luajit
              CMAKE_ARGS
              -DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
              -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
              -C ${CMAKE_CURRENT_BINARY_DIR}/config.cmake
              )

       # add lua library
       add_library( lua SHARED IMPORTED GLOBAL)
       add_dependencies( lua dep_luajit )
       set_target_properties( lua PROPERTIES
              IMPORTED_LOCATION ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}lua${CMAKE_SHARED_LIBRARY_SUFFIX} )

       if( WIN32 AND MSVC )
              set_target_properties( lua PROPERTIES
                     IMPORTED_LOCATION ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}lua${CMAKE_SHARED_LIBRARY_SUFFIX}
                     IMPORTED_IMPLIB ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}lua${CMAKE_STATIC_LIBRARY_SUFFIX} )
       endif()
endif()

if( NOT MSVC )
       # NOTE: on MSVC Diluculum code is directly inserted

       # Build Diluculum
       ExternalProject_Add ( dep_diluculum
              PREFIX diluculum
              SOURCE_DIR ${DEP_SOURCE}/diluculum
              BINARY_DIR diluculum
              CMAKE_ARGS
              -DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
              -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
              )

       # add diluculum library
       add_library( diluculum SHARED IMPORTED GLOBAL)
       add_dependencies( diluculum dep_diluculum )
       set_target_properties( diluculum PROPERTIES
              IMPORTED_LOCATION ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}diluculum${CMAKE_SHARED_LIBRARY_SUFFIX} )

       if( WIN32 AND MSVC )
              set_target_properties( diluculum PROPERTIES
                     IMPORTED_LOCATION ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}diluculum${CMAKE_SHARED_LIBRARY_SUFFIX}
                     IMPORTED_IMPLIB ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}diluculum${CMAKE_STATIC_LIBRARY_SUFFIX} )
       endif()
endif()

# Names of  dependencies
set ( DEP_NAMES
       "lpeg"
       "leg"
       "luametrics"
       "luafilesystem"
       "lualogging"
       "StackTracePlus"
       "luasocket"
       "mobdebug"
       "luacov"
       "luacheck"
       "lua_cliargs"
       "luasystem"
       "dkjson"
       "say"
       "luassert"
       "lua-term"
       "penlight"
       "mediator_lua"
       "busted"
       "luacomments"
       )

# Macro for adding external projects for each dependency
macro ( build_dependency _DEP_NAME )
       ExternalProject_Add ( dep_${_DEP_NAME}
              PREFIX ${_DEP_NAME}
              SOURCE_DIR ${DEP_SOURCE}/${_DEP_NAME}
              BINARY_DIR ${_DEP_NAME}
              CMAKE_ARGS
              -DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
              -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
              )
endmacro ()

# Adds external projects for each dependency
foreach (DEP ${DEP_NAMES})
       build_dependency( ${DEP} )
endforeach(DEP)

# Ensure correct order of build (dependencies between external dependencies)
if( MSVC OR APPLE )
       set( WHICH_LUA dep_lua )
else()
       set( WHICH_LUA dep_luajit )
endif()

foreach (DEP ${DEP_NAMES})
       add_dependencies( dep_${DEP} ${WHICH_LUA} )
endforeach(DEP)

if( NOT MSVC )
       add_dependencies( dep_diluculum ${WHICH_LUA} )
endif()


# path to include directory where external dependencies deploy headers
set( DEPENDENCIES_INCLUDE_DIR ${DEP_BIN}/include PARENT_SCOPE)

