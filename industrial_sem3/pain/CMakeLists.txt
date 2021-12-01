cmake_minimum_required(VERSION 3.1)

project(pain)

# Add SDL2 CMake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

set(CMAKE_SHARED_LIBRARY_PREFIX "")

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

aux_source_directory(src PAIN_SOURCES)
aux_source_directory(src/Utils PAIN_SOURCES)
aux_source_directory(src/BaseGUI PAIN_SOURCES)
aux_source_directory(src/EditorWidgets PAIN_SOURCES)
aux_source_directory(src/BaseGUILogic PAIN_SOURCES)
aux_source_directory(src/BaseGUILogic/BaseButton PAIN_SOURCES)
aux_source_directory(src/BaseGUILogic/Slider PAIN_SOURCES)
aux_source_directory(src/BaseGUILogic/Window PAIN_SOURCES)
aux_source_directory(src/EditorLogic PAIN_SOURCES)
aux_source_directory(src/EditorLogic/PaintWindow PAIN_SOURCES)
aux_source_directory(src/EditorLogic/Pallete PAIN_SOURCES)
aux_source_directory(src/EditorLogic/PalleteWindow PAIN_SOURCES)
aux_source_directory(src/EditorLogic/SizePicker PAIN_SOURCES)
aux_source_directory(src/EditorLogic/SizePickerWindow PAIN_SOURCES)
aux_source_directory(src/EditorLogic/ToolPicker PAIN_SOURCES)
aux_source_directory(src/EditorLogic/ToolPickerWindow PAIN_SOURCES)
aux_source_directory(src/Editor PAIN_SOURCES)
aux_source_directory(src/Math PAIN_SOURCES)
aux_source_directory(src/LGL PAIN_SOURCES)

aux_source_directory(src/Plugins/Brush PAIN_BRUSH_SOURCES)
add_library(loochek_brush SHARED ${PAIN_BRUSH_SOURCES})

aux_source_directory(src/Plugins/Eraser PAIN_ERASER_SOURCES)
add_library(loochek_eraser SHARED ${PAIN_ERASER_SOURCES})

aux_source_directory(src/Plugins/Fill PAIN_FILL_SOURCES)
add_library(loochek_fill SHARED ${PAIN_FILL_SOURCES})

aux_source_directory(src/Plugins/Negative PAIN_NEGATIVE_SOURCES)
add_library(loochek_negative SHARED ${PAIN_NEGATIVE_SOURCES})

find_package(SFML COMPONENTS graphics window)
if (SFML_FOUND)
    set(PAIN_SFML_SOURCES ${PAIN_SOURCES})
    aux_source_directory(src/LGL/SFML PAIN_SFML_SOURCES)
    add_executable(pain_sfml ${PAIN_SFML_SOURCES})
    target_link_libraries(pain_sfml sfml-graphics sfml-window dl)
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
