if(MSVC)
    set(OC_CONFIG "x64-${CMAKE_BUILD_TYPE}")
elseif(APPLE)
    set(OC_CONFIG "osx")
else()
    set(OC_CONFIG "linux")
endif()


if(NOT DEFINED FSS_BUILD_TYPE)
    if(DEFINED CMAKE_BUILD_TYPE)
        set(FSS_BUILD_TYPE ${CMAKE_BUILD_TYPE})
    else()
        set(FSS_BUILD_TYPE "Release")
    endif()
endif()

if(MSVC)
    set(FSS_CONFIG_NAME "${FSS_BUILD_TYPE}")
    if("${FSS_CONFIG_NAME}" STREQUAL "RelWithDebInfo" )
        set(FSS_CONFIG_NAME "Release")
    endif()
    set(OC_CONFIG "x64-${FSS_CONFIG_NAME}")
elseif(APPLE)
    set(OC_CONFIG "osx")
else()
    set(OC_CONFIG "linux")
endif()

if(NOT FSS_BUILD_DIR)
    set(FSS_BUILD_DIR "${CMAKE_CURRENT_LIST_DIR}/../out/build/${OC_CONFIG}")
else()
    message(STATUS "FSS_BUILD_DIR preset to ${FSS_BUILD_DIR}")
endif()


set(FSS_INSOURCE_FIND_DEPS (EXISTS ${CMAKE_CURRENT_LIST_DIR}/FSSFindBuildDir.cmake))
if(NOT DEFINED OC_THIRDPARTY_HINT)
    if(FSS_INSOURCE_FIND_DEPS)
        # we currenty are in the FSS source tree, FSS/cmake
        set(OC_THIRDPARTY_HINT "${CMAKE_CURRENT_LIST_DIR}/../out/install/${OC_CONFIG}")

        if(NOT DEFINED OC_THIRDPARTY_INSTALL_PREFIX)
            set(OC_THIRDPARTY_INSTALL_PREFIX ${OC_THIRDPARTY_HINT})
        endif()
    else()
        # we currenty are in install tree, <install-prefix>/lib/cmake/FSS
        set(OC_THIRDPARTY_HINT "${CMAKE_CURRENT_LIST_DIR}/../../..")
    endif()
endif()

if(NOT OC_THIRDPARTY_CLONE_DIR)
    set(OC_THIRDPARTY_CLONE_DIR "${CMAKE_CURRENT_LIST_DIR}/../out/")
endif()


if(NOT CRYPTOTOOLS_BUILD_DIR)
    set(CRYPTOTOOLS_BUILD_DIR "${FSS_BUILD_DIR}/cryptoTools/")
endif()

if(NOT EXISTS "${FSS_BUILD_DIR}")
    execute_process(COMMAND mkdir -p "${FSS_BUILD_DIR}")
    # message(FATAL_ERROR "failed to find the FSS build directory. Looked at FSS_BUILD_DIR: ${FSS_BUILD_DIR}")
endif()