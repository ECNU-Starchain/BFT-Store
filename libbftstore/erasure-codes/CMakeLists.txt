
file(GLOB sources
	*.h
	*.cpp
)

add_definitions(-DERASURE_BUILD)

add_library(erasure ${sources})

add_dependencies(erasure gfarithmetic)

target_include_directories(erasure PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../gf-arithmetic)

target_link_libraries(erasure PUBLIC gfarithmetic)
