cmake_minimum_required(VERSION 3.16)
set(CMAKE_OSX_SYSROOT "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX12.3.sdk")
project(DouDiZhu)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

set(MY_FLAGS "")
string(JOIN " " MY_FLAGS
  "-Wall"
  "-fdiagnostics-color=always"
  "-O2"
)
set(CMAKE_CXX_FLAGS
    # "-Wall -Wextra -Werror -pedantic -Wno-unused-result -Wconversion -Wno-unused-parameter -O2"
    ${MY_FLAGS}
)

set(GAME_FILES

  Game/Card.h
  Game/Card.cpp

  Game/Game.h
  Game/Game.cpp

  Game/Strategy.h
  Game/Strategy.cpp
)


add_executable(main Game/main.cpp ${GAME_FILES})