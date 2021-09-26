cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ")


add_subdirectory(libbftstore)



add_compile_options(-Wno-unused-variable)
add_compile_options(-Wno-unused-parameter)
add_compile_options(-Wno-unused-function)
add_compile_options(-Wno-missing-field-initializers)
add_compile_options(-Wno-unknown-pragmas)
add_compile_options(-fno-omit-frame-pointer)
add_compile_options(-Wno-dev)
add_compile_options(-Wno-return-type)

add_executable(main example.cpp)
target_link_libraries(main PUBLIC bftstore)