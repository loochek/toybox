cmake_minimum_required(VERSION 3.1)

project(circles)

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

aux_source_directory(src CIRCLE_SOURCES)
aux_source_directory(src/GameFramework CIRCLE_SOURCES)
aux_source_directory(src/GUI CIRCLE_SOURCES)
aux_source_directory(src/Render CIRCLE_SOURCES)
aux_source_directory(src/Physics CIRCLE_SOURCES)
aux_source_directory(src/Math CIRCLE_SOURCES)

find_package(SFML COMPONENTS graphics window)
if (SFML_FOUND)
    aux_source_directory(src/SFML CIRCLE_SOURCES)
    add_executable(circles ${CIRCLE_SOURCES})
    target_link_libraries(circles sfml-graphics sfml-window)
endif()
