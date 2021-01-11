# 配置说明：包括Linux和Windows下的说明

# Linux下的说明
# 安装支持SSL的curl，一般是依次安装openssl和curl，否则无法解析https
# 基本都是./configure和make及make install的做法
# 1. openssl安装见https://github.com/openssl/openssl/blob/master/NOTES-Unix.md
# 2. curl安装见https://curl.se/docs/install.html
#    安装curl时指定openssl库和安装路径的示例:
#    env PKG_CONFIG_PATH=/path/to/OpenSSL/lib/pkgconfig ./configure --with-ssl --prefix=/path/to/curl/
# 3. 在build文件夹下使用
#    cmake .. # 默认为Debug版
# 或者cmake .. -DCMAKE_BUILD_TYPE=Release # Release版
# 可选cmake .. -DCURL_PATH=/path/to/curl # 指定Curl的安装位置
# 然后cmake --build . # 构建
# 打包cpack

# Windows下的说明
# Windows下支持SSL的curl安装很麻烦，借助vcpkg管理安装则简单得多 # 其实上面的Linux也可以完全适用该方法
# 1. 先安装vcpkg本身，路径不要包含中文和空格，详见https://github.com/Microsoft/vcpkg/#quick-start-windows
# 2. 然后安装curl静态库(-static)而非动态库，否则最终生成的是动态链接版的多个dll和exe集合:
#    见https://github.com/curl/curl/blob/master/docs/INSTALL.md#building-using-vcpkg
#    安装命令vcpkg install curl[tool]:x86-windows-static # 也可以使用x64-windows-static只要保持一致即可
# 3. 在文件夹build下使用命令构建Debug版
#    cmake .. -DCMAKE_TOOLCHAIN_FILE=path\to\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x86-windows-static
#    cmake --build . # 默认为debug版
# 或者Release版
#    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=path\to\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x86-windows-static
#    cmake --build . --config Release # Release版
# 4. Windows下打包需要安装NSIS, 详见https://nsis.sourceforge.io/
# 打包cpack -C Release

cmake_minimum_required(VERSION 3.10)
project(yuki VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "set build type to debug")
string(COMPARE EQUAL "${CMAKE_BUILD_TYPE}" "Debug" DEBUG)
message("CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
aux_source_directory("./" src_files) # 加入本项目源码
add_executable(yuki ${src_files})

INCLUDE (CheckIncludeFiles)
if(UNIX AND NOT APPLE)
    CHECK_INCLUDE_FILES ("unistd.h;sys/types.h;sys/stat.h" LINUX_OK_H) # Linux关于文件夹处理的头文件
    if(NOT LINUX_OK_H)
        message(FATAL_ERROR "Not Found headers: unistd.h;sys/types.h;sys/stat.h")
    endif()
elseif(WIN32)
    CHECK_INCLUDE_FILES("io.h;direct.h;errno.h;windows.h" WIN_OK_H) # Win关于文件夹处理的头文件
    if(NOT WIN_OK_H)
        message(FATAL_ERROR "Not Found headers: io.h;direct.h;errno.h;windows.h")
    endif()
endif()

set(CURL_PATH "" CACHE PATH "Path to curl where exist include and lib")
if(CURL_PATH)
    find_library(CURL_LIB NAMES curl libcurl HINTS "${CURL_PATH}" PATH_SUFFIXES "lib" REQUIRED)
    message("CURL_LIB=${CURL_LIB}")
    target_link_libraries(yuki ${CURL_LIB}) # 链接curl
    CHECK_INCLUDE_FILES("${CURL_PATH}/include/curl/curl.h" CURL_HEADERS)
    if(NOT CURL_HEADERS)
        message(FATAL_ERROR "Not found ${CURL_PATH}/include/curl/curl.h")
    endif()
    target_include_directories(yuki PRIVATE ${CURL_PATH}/include)
else()
    find_package(CURL CONFIG REQUIRED)
    target_link_libraries(yuki PRIVATE CURL::libcurl)
endif()

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/yuki.h.in yuki.h) # 配置版本号和系统等
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/util.h.in util.h) # 配置版本号和系统等
target_include_directories(yuki PUBLIC ${PROJECT_BINARY_DIR})# 给出yuki.h.in生成的yuki.h的目录
set_target_properties(yuki PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

install(TARGETS yuki DESTINATION bin)
include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set(CPACK_PACKAGE_VERSION_MAJOR ${YUKI_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${YUKI_VERSION_MINOR})
include(CPack)