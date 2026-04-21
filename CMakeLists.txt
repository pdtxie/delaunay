cmake_minimum_required(VERSION 3.16)
project(voronoi)

add_executable(voronoi main.c++)

add_library(triangle
	triangle/triangle.c
)
target_include_directories(triangle PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/triangle)
set_target_properties(triangle PROPERTIES LINKER_LANGUAGE C)

target_link_libraries(voronoi PRIVATE triangle)
