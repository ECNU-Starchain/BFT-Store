cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ")

include(cmake/ProjectBoost.cmake)
include(cmake/ProjectRocksDB.cmake)
include(cmake/ProjectSecp256k1.cmake)
include(cmake/ProjectSnappy.cmake)
include(cmake/ProjectTBB.cmake)

file(GLOB SRC_LIST  "src/*.cpp")
file(GLOB HEADERS  "include/*")


add_subdirectory(gf-arithmetic)
add_subdirectory(erasure-codes)
add_subdirectory(lohi-generator)
add_subdirectory(log-table-generator)



add_compile_options(-Wno-unused-variable)
add_compile_options(-Wno-unused-parameter)
add_compile_options(-Wno-unused-function)
add_compile_options(-Wno-missing-field-initializers)
add_compile_options(-Wno-unknown-pragmas)
add_compile_options(-fno-omit-frame-pointer)
add_compile_options(-Wno-dev)
add_compile_options(-Wno-return-type)

add_library(bftstore ${SRC_LIST} ${HEADERS})
target_include_directories(bftstore SYSTEM BEFORE PUBLIC include ${BOOST_INCLUDE_DIR})
target_link_libraries(bftstore PUBLIC erasure TBB RocksDB Secp256k1 Boost::Filesystem pthread)