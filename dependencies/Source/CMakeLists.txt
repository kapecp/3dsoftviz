

#~ SET (CMAKE_INCLUDE_PATH 
#~ 	${CMAKE_CURRENT_SOURCE_DIR}/lua-5.1.4/src
#~ 	${CMAKE_CURRENT_SOURCE_DIR}/diluculum-0.5/include
#~ )
#~ SET(LUA_LIBRARIES liblua)
#~ SET(LUA_LIBRARY liblua)

#~ INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/lua-5.1.4/etc)


SET(ZLIB_LIBRARY z)

#~ ADD_SUBDIRECTORY(lua-5.1.4)
#~ ADD_SUBDIRECTORY(diluculum-0.5)
#~ ADD_SUBDIRECTORY(lpeg-0.8.1)
#~ ADD_SUBDIRECTORY(leg-0.1.2)
#~ ADD_SUBDIRECTORY(luafilesystem-1.4.2)
ADD_SUBDIRECTORY(libnoisesrc-1.0.0)

#ADD_DEPENDENCIES(png13 z)

#~ ADD_DEPENDENCIES(diluculum liblua)
#~ ADD_DEPENDENCIES(lpeg liblua)
#~ ADD_DEPENDENCIES(lfs liblua)
