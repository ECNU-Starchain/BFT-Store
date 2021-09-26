
add_subdirectory(backends)

file(GLOB sources
	*.cpp
	*.h
)


# Generate log_tables.cpp
add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/log_tables.cpp
	COMMAND generate-log-tables log_tables.cpp
)
# Make dependencies work
add_custom_target(log_tables ALL DEPENDS log_tables.cpp)
# log_tables depends on generate-log-tables
add_dependencies(log_tables generate-log-tables)
set_target_properties(log_tables PROPERTIES FOLDER internal-targets)

# gfarithmetic
add_library(gfarithmetic ${sources} log_tables.cpp)

# gfarithmetic needs log_tables
add_dependencies(gfarithmetic log_tables)

add_dependencies(gfarithmetic 
	encode-basic
	encode-adv
	encode-ssse3
	encode-avx2
)

#target_compile_options(gfarithmetic PUBLIC "/EHsc")

target_link_libraries(gfarithmetic
	PUBLIC encode-basic
	PUBLIC encode-adv
	PUBLIC encode-ssse3
	PUBLIC encode-avx2
)
