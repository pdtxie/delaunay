cmake_minimum_required(VERSION 3.16)
project(voronoi)

add_executable(voronoi main.c++)
add_executable(showme triangle/showme.c)

add_library(triangle
	triangle/triangle.c
)
target_include_directories(triangle PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/triangle)
set_target_properties(triangle PROPERTIES LINKER_LANGUAGE C)

target_link_libraries(voronoi PRIVATE triangle)

target_include_directories(showme PRIVATE /opt/X11/include)
target_link_directories(showme PRIVATE /opt/X11/lib)
target_link_libraries(showme PRIVATE X11)
