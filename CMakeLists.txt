cmake_minimum_required(VERSION 3.21)

project(fss)

string(ASCII 27 Esc)
set(E "${Esc}[m")
set(R "${Esc}[31m")
set(G "${Esc}[32m")

include(cmake/fssPreamble.cmake)

find_package(cryptoTools QUIET)

set(ENABLE_BOOST ON CACHE BOOL "Enable Boost for CryptoTools" FORCE)
# set(FETCH_BOOST ON CACHE BOOL "Enable Boost for CryptoTools" FORCE)
set(ENABLE_RELIC ON CACHE BOOL "Enable Relic for CryptoTools" FORCE)

set(FETCH_AUTO ON CACHE BOOL "Enable Relic for CryptoTools" FORCE)
# set(FETCH_RELIC ON CACHE BOOL "Enable Relic for CryptoTools" FORCE)
# set(ENABLE_COPROTO ON CACHE BOOL "Enable Relic for CryptoTools" FORCE)
# set(FETCH_COPROTO ON CACHE BOOL "Enable Relic for CryptoTools" FORCE)
# set(FETCH_LIBDIVIDE ON CACHE BOOL "Enable Relic for CryptoTools" FORCE)

set(CMAKE_CXX_FLAGS "-maes -msse2 -msse3 -msse4.1 -mpclmul -mavx2 -march=native ${CMAKE_CXX_FLAGS}")


if (cryptoTools_FOUND)
  message(STATUS "${G}Found CryptoTools${E}")
elseif(NOT cryptoTools_FOUND)
  message(STATUS "${R}CryptoTools Not Found, Compiling...${E}")
  if (NOT EXISTS "${PROJECT_SOURCE_DIR}/thirdparty/cryptoTools/CMakeLists.txt")
    find_package(Git REQUIRED)
    execute_process(COMMAND git submodule update --init --recursive thirdparty/cryptoTools
                    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
  endif()
  add_subdirectory(thirdparty/cryptoTools)
endif()

# add_executable(net net.cc)
add_subdirectory(src)


target_link_libraries(test cryptoTools)