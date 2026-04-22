cmake_minimum_required(VERSION 3.20)
project(My_SFML_GUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

# Include FetchContent module for automatic dependency download
include(FetchContent)

# Disable SFML components we don't need
set(SFML_BUILD_AUDIO FALSE CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK FALSE CACHE BOOL "" FORCE)
set(SFML_BUILD_DOCS FALSE CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES FALSE CACHE BOOL "" FORCE)
set(SFML_INSTALL_PKGCONFIG_FILES FALSE CACHE BOOL "" FORCE)
set(SFML_INSTALL_CMAKEDIR FALSE CACHE BOOL "" FORCE)

# Fetch SFML (will automatically download freetype as a dependency)
FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SFML
)
FetchContent_MakeAvailable(sfml)

# Fetch nlohmann/json (single-include header-only library)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/json
)
FetchContent_MakeAvailable(nlohmann_json)

set(SOURCES
    src/main.cpp
    src/MyGUI.cpp
)

set(HEADERS
    include/engine/gui/MyGUI.hpp
    include/engine/event/Event.hpp
    include/engine/data/Tag.hpp
    include/engine/serialization/BinaryFileStream.hpp
    include/engine/core/SFMLBase.hpp
    include/engine/data/Skippable.hpp
    include/engine/data/VarianTmap.hpp
    include/engine/data/RollArray.hpp
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    nlohmann_json::nlohmann_json
)
