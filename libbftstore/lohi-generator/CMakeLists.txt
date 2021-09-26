
file(GLOB sources
	*.cpp
	*.h
)

set(log_tables_location "${CMAKE_CURRENT_BINARY_DIR}/../gf-arithmetic/log_tables.cpp")

set_source_files_properties(${log_tables_location} PROPERTIES GENERATED TRUE)

add_executable(generate-lohi-table ${sources} ${log_tables_location})

add_dependencies(generate-lohi-table log_tables)
