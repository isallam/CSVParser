cmake_minimum_required (VERSION 2.8)
project (csv-test)

message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

IF(CMAKE_BUILD_TYPE MATCHES Debug)
  message("Debug build.")
ELSE()
  set(CMAKE_BUILD_TYPE Release)
  message("Release build.")
ENDIF()

#set(CMAKE_CXX_FLAGS "-std=c++11") 
#set(CMAKE_CXX_FLAGS " -O6 -g0  -m64 -Wall -flto -msse3 -Wextra -Wformat -pedantic -std=c++11 -ffast-math -march=native -fno-check-new -funroll-loops -Wno-deprecated -falign-loops=16 -Wformat-security -fstrict-aliasing -Wstrict-aliasing=2 -Wno-strict-overflow -Wno-unused-variable -Wno-variadic-macros -fomit-frame-pointer -Wno-unused-parameter -finline-limit=1000000 -Wno-unused-private-field -Wno-unused-local-typedefs")

set(CMAKE_CXX_FLAGS " -O6 -fno-inline -m64 -Wall -msse3 -Wextra -Wformat -pedantic -std=c++11 -ffast-math -march=native -D__extern_always_inline=\"\" -funroll-loops -Wno-deprecated -Wformat-security -fstrict-aliasing -Wstrict-aliasing=2 -Wno-strict-overflow -Wno-unused-variable -Wno-variadic-macros -Wno-unused-parameter -fno-inline -Wno-unused-local-typedefs")

ADD_DEFINITIONS(-DNDEBUG)
ADD_DEFINITIONS(-D__STDC_FORMAT_MACROS=1)
#ADD_DEFINITIONS(-D_GLIBCXX_USE_CXX11_ABI=0)

include_directories(. ./src)
 
SET(SOURCES main.cpp)

set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_CXX_FLAGS})

SET(SYSTEM_LIBS crypto dl pthread)

add_executable(csv_test ${SOURCES})

target_link_libraries(csv_test LINK_PUBLIC ${SYSTEM_LIBS} )
