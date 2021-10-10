cmake_minimum_required(VERSION 3.1)

project(circles)

# Add SDL2 CMake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

#set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
#set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

aux_source_directory(src CIRCLE_SOURCES)
aux_source_directory(src/GameFramework CIRCLE_SOURCES)
aux_source_directory(src/GUI CIRCLE_SOURCES)
aux_source_directory(src/Render CIRCLE_SOURCES)
aux_source_directory(src/Physics CIRCLE_SOURCES)
aux_source_directory(src/Math CIRCLE_SOURCES)

find_package(SFML COMPONENTS graphics window)
if (SFML_FOUND)
    set(CIRCLE_SFML_SOURCES ${CIRCLE_SOURCES})
    aux_source_directory(src/SFML CIRCLE_SFML_SOURCES)
    add_executable(circles_sfml ${CIRCLE_SFML_SOURCES})
    target_link_libraries(circles_sfml sfml-graphics sfml-window)
endif()

find_package(SDL2 REQUIRED)
find_package(SDL2_gfx REQUIRED)
find_package(SDL2_ttf REQUIRED)
if (SDL2_FOUND)
    set(CIRCLE_SDL_SOURCES ${CIRCLE_SOURCES})
    aux_source_directory(src/SDL CIRCLE_SDL_SOURCES)
    add_executable(circles_sdl ${CIRCLE_SDL_SOURCES})
    target_compile_definitions(circles_sdl PUBLIC CIRCLES_USE_SDL)
    target_link_libraries(circles_sdl ${SDL2_LIBRARIES} ${SDL2_GFX_LIBRARIES} ${SDL2_TTF_LIBRARIES})
endif()