cmake_minimum_required(VERSION 3.16)
project(ChessGame2)

set(CMAKE_CXX_STANDARD 17)

# ===================== SFML =====================
set(SFML_DIR "D:/SFML/SFML-2.6.2/lib/cmake/SFML")
find_package(SFML 2.6 COMPONENTS graphics window audio system REQUIRED)

# ===================== TGUI =====================
set(TGUI_INCLUDE "D:/TGUI-0.9.5-VisualStudio-64bit-for-SFML-2.6.2/TGUI-0.9/include")

set(TGUI_LIB
    optimized "D:/TGUI-0.9.5-VisualStudio-64bit-for-SFML-2.6.2/TGUI-0.9/lib/tgui.lib"
    debug     "D:/TGUI-0.9.5-VisualStudio-64bit-for-SFML-2.6.2/TGUI-0.9/lib/tgui-d.lib"
)

# ===================== INCLUDE =====================
include_directories(
    Include
    Include/Core
    Include/UI
    Src
    Constants
    Tweeny
    ${TGUI_INCLUDE}
)

# ===================== CORE SOURCES =====================
file(GLOB_RECURSE CORE_SOURCES
    "Src/Audio/*.cpp"
    "Src/Bot/*.cpp"
    "Src/Input/*.cpp"
    "Src/UI/*.cpp"
    "Src/Game Core/*.cpp"
)

# ===================== CORE LIB =====================
add_library(core STATIC ${CORE_SOURCES})

target_link_libraries(core
    sfml-graphics
    sfml-window
    sfml-audio
    sfml-system
    ${TGUI_LIB}
)

# ===================== GAME APP =====================
add_executable(game
    Src/main/main.cpp
    Src/main/GameVideo.cpp
)

target_link_libraries(game
    core
)

# ===================== TEST =====================
file(GLOB_RECURSE TEST_SOURCES
    "Test/Src/*.cpp"
)

add_executable(test_runner ${TEST_SOURCES})

target_link_libraries(test_runner
    core
    sfml-graphics
    sfml-window
    sfml-audio
    sfml-system
    ${TGUI_LIB}
)

# ===================== RUNTIME FILES =====================
add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Assets
        $<TARGET_FILE_DIR:game>/Assets
)

# ===================== WORKING DIRECTORY =====================
set_target_properties(game PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)