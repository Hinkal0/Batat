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
    "${CMAKE_CURRENT_SOURCE_DIR}/config/config.ini"
    "${RUNTIME_DIR}/config.ini"
  VERBATIM
)

add_custom_target(textures
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/textures"
    "${RUNTIME_DIR}/textures"
  VERBATIM
)

add_custom_target(fonts
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts"
    "${RUNTIME_DIR}/fonts"
  VERBATIM
)

add_executable(batat
  src/entry.cpp

  src/config/Config.h
  src/config/Parser.cpp
  src/config/Parser.h

  src/core/Event.h
  src/core/GameManager.cpp
  src/core/GameManager.h
  src/core/InputSystem.cpp
  src/core/InputSystem.h
  src/core/Resources.cpp
  src/core/Resources.h
  src/core/Scene.cpp
  src/core/Scene.h
  src/core/Window.cpp
  src/core/Window.h

  src/entities/Character.h
  src/entities/Object.h
  src/entities/Player.cpp
  src/entities/Player.h
  src/entities/Projectile.cpp
  src/entities/Projectile.h
  src/entities/Slash.cpp
  src/entities/Slash.h

  src/entities/enemies/Enemy.cpp
  src/entities/enemies/Enemy.h
  src/entities/enemies/OnionGuard.cpp
  src/entities/enemies/OnionGuard.h
  src/entities/enemies/PeaShooter.cpp
  src/entities/enemies/PeaShooter.h
  src/entities/enemies/RadishRunner.cpp
  src/entities/enemies/RadishRunner.h
  src/entities/enemies/Yabloko.cpp
  src/entities/enemies/Yabloko.h

  src/ui/Bar.cpp
  src/ui/Bar.h
  src/ui/Hud.cpp
  src/ui/Hud.h
  src/ui/Interface.cpp
  src/ui/Interface.h
  src/ui/LabeledValue.cpp
  src/ui/LabeledValue.h
  src/ui/Menu.cpp
  src/ui/Menu.h
  src/ui/Text.cpp
  src/ui/Text.h
  src/ui/UIElement.h

  src/world/Map.cpp
  src/world/Map.h
)

target_include_directories(batat PRIVATE src)

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
    SFML::Audio
)

add_dependencies(batat config textures fonts)
