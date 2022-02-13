cmake_minimum_required(VERSION 3.1)

project(sorting)

# Add SDL2 CMake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/plugins)
set(CMAKE_CXX_STANDARD 17)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

aux_source_directory(src SORTING_SOURCES)
aux_source_directory(src/Utils SORTING_SOURCES)
aux_source_directory(src/BaseGUI SORTING_SOURCES)
aux_source_directory(src/BaseGUILogic SORTING_SOURCES)
aux_source_directory(src/BaseGUILogic/BaseButton SORTING_SOURCES)
aux_source_directory(src/BaseGUILogic/Slider SORTING_SOURCES)
aux_source_directory(src/BaseGUILogic/Window SORTING_SOURCES)
aux_source_directory(src/SortingWidgets SORTING_SOURCES)
aux_source_directory(src/SortingLogic SORTING_SOURCES)
aux_source_directory(src/SortingLogic/AnalysisWindow SORTING_SOURCES)
aux_source_directory(src/Math SORTING_SOURCES)
aux_source_directory(src/LGL SORTING_SOURCES)

find_package(SFML COMPONENTS graphics window)
if (SFML_FOUND)
    set(SORTING_SFML_SOURCES ${SORTING_SOURCES})
    aux_source_directory(src/LGL/SFML SORTING_SFML_SOURCES)
    add_executable(sorting_sfml ${SORTING_SFML_SOURCES})
    target_link_libraries(sorting_sfml sfml-graphics sfml-window dl)
endif()

# find_package(SDL2 REQUIRED)
# find_package(SDL2_gfx REQUIRED)
# find_package(SDL2_ttf REQUIRED)
# if (SDL2_FOUND)
#     set(CIRCLE_SDL_SOURCES ${CIRCLE_SOURCES})
#     aux_source_directory(src/LGL/SDL CIRCLE_SOURCES)
#     add_executable(circles_sdl ${CIRCLE_SDL_SOURCES})
#     target_compile_definitions(circles_sdl PUBLIC CIRCLES_USE_SDL)
#     target_link_libraries(circles_sdl ${SDL2_LIBRARIES} ${SDL2_GFX_LIBRARIES} ${SDL2_TTF_LIBRARIES})
# endif()
