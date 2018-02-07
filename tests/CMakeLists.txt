# -------------------------------------------------------
# BDD Igloo tests
# -------------------------------------------------------

set( TEST_RUNNER all_tests.cpp )

#--------------- gitlib module tests --------------------
file( GLOB_RECURSE SRC_GITLIB_TESTS	"Repository/Git/GitLib/*.cpp" )

add_executable( test_gitlib_module ${TEST_RUNNER} ${SRC_GITLIB_TESTS} )
add_dependencies( test_gitlib_module igloo )

target_include_directories( test_gitlib_module 	PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ../include/Repository/Git )
target_compile_options( test_gitlib_module PUBLIC ${FLAGS_FOR_DEBUG} )
if ( Qt5Core_FOUND )
    #	qt5_use_modules( test_gitlib_module Core )
    target_link_libraries( test_gitlib_module Qt5::Core gitlib )
elseif( Qt4_FOUND )
    target_link_libraries( test_gitlib_module ${QT_LIBRARIES} gitlib )
endif()

# 'cotire' the executable
if( COMMAND cotire )
    set_target_properties( test_gitlib_module PROPERTIES COTIRE_UNITY_LINK_LIBRARIES_INIT "COPY_UNITY" )
    cotire( test_gitlib_module CONFIGURATIONS Debug Release )
endif()


#--------------------- run all tests --------------------
enable_testing()
add_test( testing_gitlib_module test_gitlib_module )
# -------------------------------------------------------