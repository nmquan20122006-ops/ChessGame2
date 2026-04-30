cmake_minimum_required(VERSION 3.16)
project(ChessGame2)
set(CMAKE_CXX_STANDARD 17)

set(SFML_DIR "D:/SFML/SFML-2.6.2/lib/cmake/SFML")
find_package(SFML 2.6 COMPONENTS graphics window audio system REQUIRED)

set(TGUI_INCLUDE "D:/TGUI-0.9.5-VisualStudio-64bit-for-SFML-2.6.2 (1)/TGUI-0.9/include")
set(TGUI_LIB
    optimized "D:/TGUI-0.9.5-VisualStudio-64bit-for-SFML-2.6.2 (1)/TGUI-0.9/lib/tgui.lib"
    debug     "D:/TGUI-0.9.5-VisualStudio-64bit-for-SFML-2.6.2 (1)/TGUI-0.9/lib/tgui-d.lib"
)

include_directories(
    Include
    Include/Core
    Include/UI
    Src
    Test/Include
    Constants
    Tweeny
    ${TGUI_INCLUDE}
)

file(GLOB_RECURSE CORE_SOURCES
    "Src/Audio/*.cpp"
    "Src/Bot/*.cpp"
    "Src/Input/*.cpp"
    "Src/UI/*.cpp"
)

file(GLOB GAME_CORE_SOURCES "Src/Game Core/*.cpp")
list(APPEND CORE_SOURCES ${GAME_CORE_SOURCES})

add_library(core ${CORE_SOURCES} "Test/Include/Undo_Test.h"  "Test/Src/Undo_Test.cpp" "Test/Src/BoardToFEN_Test.cpp" "Include/Core/ToFEN.h" "Src/Game Core/ToFEN.cpp")

target_link_libraries(core
    sfml-graphics
    sfml-window
    sfml-audio
    sfml-system
    ${TGUI_LIB}
)


add_executable(game
    Src/main/main.cpp
    Src/main/GameVideo.cpp
 "Test/Include/Undo_Test.h"  "Test/Src/Undo_Test.cpp" "Test/Src/BoardToFEN_Test.cpp" "Include/Core/ToFEN.h" "Src/Game Core/ToFEN.cpp")

target_link_libraries(game
    core
    sfml-graphics
    sfml-window
    sfml-audio
    sfml-system
    ${TGUI_LIB}
)

# ===== TEST =====
file(GLOB_RECURSE TEST_SOURCES
    "Test/Src/*.cpp"
)

add_executable(test_runner ${TEST_SOURCES} "Test/Include/Undo_Test.h"  "Test/Src/Undo_Test.cpp" "Test/Src/BoardToFEN_Test.cpp" "Include/Core/ToFEN.h" "Src/Game Core/ToFEN.cpp")

target_link_libraries(test_runner
    core
    sfml-graphics
    sfml-window
    sfml-audio
    sfml-system
    ${TGUI_LIB}
)

add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:sfml-graphics>
        $<TARGET_FILE:sfml-window>
        $<TARGET_FILE:sfml-audio>
        $<TARGET_FILE:sfml-system>
        $<TARGET_FILE_DIR:game>
)

add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<IF:$<CONFIG:Debug>,D:/TGUI-0.9.5-VisualStudio-64bit-for-SFML-2.6.2 (1)/TGUI-0.9/bin/tgui-d.dll,D:/TGUI-0.9.5-VisualStudio-64bit-for-SFML-2.6.2 (1)/TGUI-0.9/bin/tgui.dll>"
        $<TARGET_FILE_DIR:game>
)
add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:sfml-graphics>
        $<TARGET_FILE:sfml-window>
        $<TARGET_FILE:sfml-audio>
        $<TARGET_FILE:sfml-system>
        "D:/SFML/SFML-2.6.2/bin/openal32.dll"
        $<TARGET_FILE_DIR:game>
)
add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/../Assets
        $<TARGET_FILE_DIR:game>/Assets
)
add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Assets
        $<TARGET_FILE_DIR:game>/Assets
)
message(STATUS "Source dir: ${CMAKE_SOURCE_DIR}")