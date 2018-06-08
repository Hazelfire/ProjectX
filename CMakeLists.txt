#/****************************************************************************
# Copyright (c) 2013-2014 cocos2d-x.org
#
# http://www.cocos2d-x.org
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# ****************************************************************************/
cmake_policy(SET CMP0017 NEW)

cmake_minimum_required(VERSION 2.8)

set(APP_NAME ProjectX)
project (${APP_NAME})

set(COCOS2D_ROOT ${CMAKE_SOURCE_DIR}/cocos2d)
set(EXTERNAL_PATH ${CMAKE_SOURCE_DIR}/external)

set( CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${COCOS2D_ROOT}/cmake/Modules/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
include(CocosBuildHelpers)

# libcocos2d
set(BUILD_CPP_TESTS OFF CACHE BOOL "turn off build cpp-tests")
set(BUILD_LUA_LIBS OFF CACHE BOOL "turn off build lua related targets")
set(BUILD_JS_LIBS OFF CACHE BOOL "turn off build js related targets")

add_subdirectory(${COCOS2D_ROOT})

add_subdirectory(${EXTERNAL_PATH}/lua)
add_subdirectory(${EXTERNAL_PATH}/yaml-cpp)
add_subdirectory(${EXTERNAL_PATH}/websocketpp)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

# Some macro definitions
if(WINDOWS)
  message("Is windows")
  if(BUILD_SHARED_LIBS)
    ADD_DEFINITIONS (-D_USRDLL -D_EXPORT_DLL_ -D_USEGUIDLL -D_USREXDLL -D_USRSTUDIODLL)
  else()
    ADD_DEFINITIONS (-DCC_STATIC)
  endif()

  ADD_DEFINITIONS (-DCOCOS2DXWIN32_EXPORTS -D_WINDOWS -DWIN32 -D_WIN32)
  set(PLATFORM_FOLDER win32)
elseif(MACOSX OR APPLE)
  ADD_DEFINITIONS (-DCC_TARGET_OS_MAC)
  ADD_DEFINITIONS (-DUSE_FILE32API)

  set(PLATFORM_FOLDER mac)
elseif(LINUX)
  ADD_DEFINITIONS(-DLINUX)
  set(PLATFORM_FOLDER linux)
elseif(ANDROID)
  ADD_DEFINITIONS (-DUSE_FILE32API)
  set(PLATFORM_FOLDER android)
else()
  message( FATAL_ERROR "Unsupported platform, CMake will exit" )
endif()


# Compiler options
if(MSVC)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS
                  -wd4251 -wd4244 -wd4334 -wd4005 -wd4820 -wd4710
                  -wd4514 -wd4056 -wd4996 -wd4099)
else()
  set(CMAKE_C_FLAGS_DEBUG "-g -Wall -DCOCOS2D_DEBUG=1")
  set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-deprecated-declarations -Wno-reorder")
  if(CLANG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  endif()
endif(MSVC)



set(PLATFORM_SPECIFIC_SRC)
set(PLATFORM_SPECIFIC_HEADERS)
if(MACOSX OR APPLE)
set(PLATFORM_SPECIFIC_SRC
  proj.ios_mac/ios/main.m
  proj.ios_mac/ios/RootViewController.mm
  proj.ios_mac/ios/AppController.mm
)
set(PLATFORM_SPECIFIC_HEADERS
  proj.ios_mac/ios/RootViewController.h
  proj.ios_mac/ios/AppController.h
)
elseif(LINUX) #assume linux
set(PLATFORM_SPECIFIC_SRC
  proj.linux/main.cpp
)
elseif ( WIN32 )
set(PLATFORM_SPECIFIC_SRC
  proj.win32/main.cpp
)
set(PLATFORM_SPECIFIC_HEADERS
  proj.win32/main.h
  proj.win32/resource.h
)
endif()

include_directories(
  /usr/local/include/GLFW
  /usr/include/GLFW
  ${COCOS2D_ROOT}/cocos
  ${COCOS2D_ROOT}/cocos/platform
  ${COCOS2D_ROOT}/cocos/audio/include/
  Classes
  ${EXTERNAL_PATH}/lua/etc
  ${EXTERNAL_PATH}/lua/src
  ${EXTERNAL_PATH}/yaml-cpp/include
  ${EXTERNAL_PATH}/websocketpp

  ${EXTERNAL_PATH}/include
)
if ( WIN32 )
  include_directories(
  ${COCOS2D_ROOT}/external/glfw3/include/win32
  ${COCOS2D_ROOT}/external/win32-specific/gles/include/OGLES
)
endif( WIN32 )

find_package(Boost 1.60 REQUIRED COMPONENTS filesystem thread)
find_package(git2 REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${LIBGIT2_INCLUDE_DIR})
set(GAME_SRC
  Classes/Animal.cpp
  Classes/AnimatedSprite.cpp
  Classes/AnimateLabel.cpp
  Classes/AppDelegate.cpp
  Classes/Arena.cpp
  Classes/Creatures.cpp
  Classes/Debug.cpp
  Classes/FileOperation.cpp
  Classes/Particles.cpp
  Classes/Interactions.cpp
  Classes/Inventory.cpp
  Classes/Mortal.cpp
  Classes/MusicEngine.cpp
  Classes/PathFinder.cpp
  Classes/Player.cpp
  Classes/Save.cpp
  Classes/ScriptLoader.cpp
  Classes/SelectionWheel.cpp
  Classes/SpriteLoader.cpp
  Classes/SwipeSelector.cpp
  Classes/XBMPLabel.cpp
  Classes/XTileMap.cpp
  
  Classes/Lua/LuaActions.cpp
  Classes/Lua/LuaCreature.cpp
  Classes/Lua/LuaCreatureActions.cpp
  Classes/Lua/LuaGame.cpp
  Classes/Lua/LuaInterpreter.cpp
  Classes/Lua/LuaItemUses.cpp
  Classes/Lua/LuaPersonal.cpp
  Classes/Lua/LuaTileActions.cpp
  
  Classes/MapGen/MapGenerator.cpp
  Classes/MapGen/PerlinNoise.cpp
  Classes/MapGen/Schematics.cpp
  
  Classes/Mechs/Health.cpp

  Classes/Multiplayer/MultiplayerClient.cpp
  Classes/Multiplayer/MultiplayerLobby.cpp
  Classes/Multiplayer/MultiplayerServer.cpp
  Classes/Multiplayer/Puppet.cpp
  Classes/Multiplayer/PuppetMaster.cpp
  Classes/Multiplayer/XClient.cpp
  Classes/Multiplayer/XProtocol.cpp
  Classes/Multiplayer/XServer.cpp
  
  Classes/MyUI/Dialougue.cpp
  Classes/MyUI/LuaTerminal.cpp
  Classes/MyUI/ExitMenu.cpp
  
  Classes/Parser/Characters.cpp
  Classes/Parser/CreaturesParser.cpp
  Classes/Parser/ErrorParser.cpp
  Classes/Parser/FontParser.cpp
  Classes/Parser/Items.cpp
  Classes/Parser/Music.cpp
  Classes/Parser/Responces.cpp
  Classes/Parser/SchematicsParser.cpp
  Classes/Parser/SpriteParser.cpp
  Classes/Parser/StringOperations.cpp
  Classes/Parser/Terrain.cpp
  Classes/Parser/XMLCommon.cpp
  
  Classes/UIScenes/ErrorScreen.cpp
  Classes/UIScenes/HelloWorldScene.cpp
  Classes/UIScenes/LogScreen.cpp
  Classes/UIScenes/MapSelector.cpp
  Classes/UIScenes/OptionsMenu.cpp
  Classes/UIScenes/PlayerSelector.cpp
  Classes/UIScenes/SplashScreen.cpp
  Classes/UIScenes/DownloadBasePackageScene.cpp
  Classes/UIScenes/PackageManagerScene.cpp
  
  Classes/YAML/ConfigParser.cpp
  Classes/YAML/SaveParser.cpp
  Classes/YAML/ScriptIndex.cpp

  Classes/Packages/PackageManager.cpp
  Classes/Packages/Package.cpp
  Classes/Packages/GitRepository.cpp

  ${PLATFORM_SPECIFIC_SRC}
)

set(GAME_HEADERS
  Classes/Animal.h
  Classes/AnimatedSprite.h
  Classes/AnimateLabel.h
  Classes/AppDelegate.h
  Classes/Arena.h
  Classes/Creatures.h
  Classes/Debug.h
  Classes/FileOperation.h
  Classes/GameInformation.h
  Classes/Inventory.h
  Classes/Interactions.h
  Classes/Particles.h
  Classes/Mortal.h
  Classes/MusicEngine.h
  Classes/MyMath.h
  Classes/PathFinder.h
  Classes/Player.h
  Classes/ResourceMacros.h
  Classes/Save.h
  Classes/SaveInformation.h
  Classes/ScriptLoader.h
  Classes/SelectionWheel.h
  Classes/SizeMacros.h
  Classes/SpriteLoader.h
  Classes/SpriteTypes.h
  Classes/SwipeSelector.h
  Classes/Utils.h
  Classes/XBMPLabel.h
  Classes/XTileMap.h
  
  Classes/Lua/LuaActions.h
  Classes/Lua/LuaCreature.h
  Classes/Lua/LuaCreatureActions.h
  Classes/Lua/LuaGame.h
  Classes/Lua/LuaItemUses.h
  Classes/Lua/LuaPersonal.h
  Classes/Lua/LuaTileActions.h
  Classes/Lua/LuaInterpreter.h
  
  Classes/MapGen/MapGenerator.h
  Classes/MapGen/PerlinNoise.h
  Classes/MapGen/Schematics.h

  Classes/Mechs/Health.h
  
  Classes/Multiplayer/MultiplayerClient.h
  Classes/Multiplayer/MultiplayerLobby.h
  Classes/Multiplayer/MultiplayerServer.h
  Classes/Multiplayer/Puppet.h
  Classes/Multiplayer/PuppetMaster.h
  Classes/Multiplayer/XClient.h
  Classes/Multiplayer/XProtocol.hpp
  Classes/Multiplayer/XProtocolImpl.h
  Classes/Multiplayer/XServer.h
  
  Classes/MyUI/Dialougue.h
  Classes/MyUI/LuaTerminal.h
  Classes/MyUI/ExitMenu.h
  
  Classes/Parser/Characters.h
  Classes/Parser/CreaturesParser.h
  Classes/Parser/ErrorParser.h
  Classes/Parser/FontParser.h
  Classes/Parser/Items.h
  Classes/Parser/MovementDirection.hpp
  Classes/Parser/Music.h
  Classes/Parser/rapidxml.hpp
  Classes/Parser/Responces.h
  Classes/Parser/SchematicsParser.h
  Classes/Parser/SpriteParser.h
  Classes/Parser/StringOperations.h
  Classes/Parser/Terrain.h
  Classes/Parser/XMLCommon.h
  
  Classes/UIScenes/ErrorScreen.h
  Classes/UIScenes/HelloWorldScene.h
  Classes/UIScenes/LogScreen.h
  Classes/UIScenes/MapSelector.h
  Classes/UIScenes/OptionsMenu.h
  Classes/UIScenes/PlayerSelector.h
  Classes/UIScenes/SplashScreen.h
  Classes/UIScenes/DownloadBasePackageScene.h
  Classes/UIScenes/PackageManagerScene.h
  
  Classes/YAML/ConfigParser.h
  Classes/YAML/SaveParser.h
  Classes/YAML/ScriptIndex.h

  Classes/Packages/PackageManager.h
  Classes/Packages/Package.h
  Classes/Packages/GitRepository.h

  ${PLATFORM_SPECIFIC_HEADERS}
)

if(GAME_HEADERS)
  if ( WIN32 )
    add_executable(${APP_NAME} WIN32 ${GAME_SRC} ${GAME_HEADERS})
  else()
    add_executable(${APP_NAME} ${GAME_SRC} ${GAME_HEADERS})
  endif ( WIN32 )
else()
  if ( WIN32 )
    add_executable(${APP_NAME} WIN32 ${GAME_SRC})
  else()
    add_executable(${APP_NAME} ${GAME_SRC})
  endif ( WIN32 )
endif()

target_link_libraries(${APP_NAME} ${Boost_LIBRARIES})


target_link_libraries(${APP_NAME} ${LIBGIT2_LIBRARIES})

target_link_libraries(${APP_NAME} cocos2d)

target_link_libraries(${APP_NAME} liblua)
target_link_libraries(${APP_NAME} yaml-cpp)

set(APP_BIN_DIR "${CMAKE_BINARY_DIR}/bin")

set_target_properties(${APP_NAME} PROPERTIES
     RUNTIME_OUTPUT_DIRECTORY  "${APP_BIN_DIR}")

if ( WIN32 )
  #also copying dlls to binary directory for the executable to run
  pre_build(${APP_NAME}
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${APP_BIN_DIR}/Resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Resources ${APP_BIN_DIR}/Resources
    COMMAND ${CMAKE_COMMAND} -E copy ${COCOS2D_ROOT}/external/win32-specific/gles/prebuilt/glew32.dll ${APP_BIN_DIR}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${COCOS2D_ROOT}/external/win32-specific/zlib/prebuilt/zlib1.dll ${APP_BIN_DIR}/${CMAKE_BUILD_TYPE}
	)
else()
  pre_build(${APP_NAME}
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${APP_BIN_DIR}/Resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Resources ${APP_BIN_DIR}/Resources
    )

endif()
