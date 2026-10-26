cmake_minimum_required(VERSION 3.22)

project(Batat)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(RUNTIME_DIR "${CMAKE_BINARY_DIR}/bin")

include(FetchContent)

FetchContent_Declare(
  SFML
  GIT_REPOSITORY https://github.com/SFML/SFML
  GIT_TAG 0fa201c969e48ecc253581c5841ce73f44d42f49
)

FetchContent_MakeAvailable(SFML)

add_custom_target(config
  COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/config.ini
    ${RUNTIME_DIR}/config.ini
)

add_custom_target(textures
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/textures
    ${RUNTIME_DIR}/textures
)

# add_custom_target(fonts
#   COMMAND ${CMAKE_COMMAND} -E copy_directory
#     ${CMAKE_SOURCE_DIR}/fonts
#     ${RUNTIME_DIR}/fonts
# )

add_executable(batat
  entry.cpp
  GameManager.cpp
  Window.cpp
  Parser.cpp
  Scene.cpp
  Object.h
  Player.cpp
  Map.cpp
  Character.h
  Projectile.cpp
  Slash.cpp
  Textures.cpp
)

set_target_properties(batat 
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${RUNTIME_DIR}
)

if (WIN32)
  target_link_options(batat PRIVATE -mwindows -static)
endif()

target_link_libraries(batat
  PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

add_dependencies(batat config textures)