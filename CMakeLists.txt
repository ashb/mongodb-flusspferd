cmake_minimum_required(VERSION 2.6.4)

execute_process( COMMAND flusspferd --cmake OUTPUT_VARIABLE hippo_cmake)

include("${hippo_cmake}")

# Clear the cache for these variables
SET(lib lib-NOTFOUND)
SET(inc_dir inc_dir-NOTFOUND)

find_path( inc_dir mongo/client/dbclient.h "${MONGODB_ROOT}/include" )

find_library( lib mongoclient "${MONGODB_ROOT}/lib" "${MONGODB_ROOT}/lib64" )

if(NOT lib)
  message(FATAL_ERROR "Unable to find mongoclient link library.\nPlease pass -DMONGODB_ROOT=<dir>")
elseif(NOT inc_dir)
  message(FATAL_ERROR "Unable to find mongo header files.\nPlease pass -DMONGODB_ROOT=<dir>")
endif()

add_definitions("-Wall")
include_directories(${inc_dir})

set(Boost_FIND_REQUIRED TRUE)
find_package(Boost 1.40.0 COMPONENTS program_options)

if(Boost_LIBRARY_DIRS)
    link_directories(${Boost_LIBRARY_DIRS})
    include_directories(${Boost_INCLUDE_DIR})
else()
    message(FATAL_ERROR "Boost libraries not found. Please set -DBOOST_ROOT or -DBOOST_LIBRARY_DIR manually.")
endif()


FILE(GLOB_RECURSE sources 
     "${CMAKE_SOURCE_DIR}/src/*.cpp" 
     "${CMAKE_SOURCE_DIR}/src/*.hpp" )

flusspferd_plugin( "mongodb" SOURCES ${sources} LIBRARIES ${lib} ${Boost_PROGRAM_OPTIONS_LIBRARY} )
