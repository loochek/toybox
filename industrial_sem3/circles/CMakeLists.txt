cmake_minimum_required(VERSION 3.1)

project(circles)

# set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
# set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

aux_source_directory(. CIRCLE_SOURCES)
aux_source_directory(GUI CIRCLE_SOURCES)
aux_source_directory(Render CIRCLE_SOURCES)
aux_source_directory(Physics CIRCLE_SOURCES)
aux_source_directory(Math CIRCLE_SOURCES)

find_package(SFML COMPONENTS graphics window)
if (SFML_FOUND)
    aux_source_directory(SFML CIRCLE_SOURCES)
    add_executable(circles ${CIRCLE_SOURCES})
    target_link_libraries(circles sfml-graphics sfml-window)
endif()
