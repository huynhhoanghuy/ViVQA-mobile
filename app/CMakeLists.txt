cmake_minimum_required(VERSION 3.4.1)
set(TARGET pytorch_nativeapp_VQA)
project(${TARGET} CXX)
set(CMAKE_CXX_STANDARD 14)


# THIS HAS TO COME BEFORE THE PROJECT LINE

set(CMAKE_VERBOSE_MAKEFILE TRUE CACHE BOOL "" FORCE)



if("${ANDROID_ABI}" STREQUAL "")
    EXECUTE_PROCESS(COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE)
    set(ANDROID_ABI ${ARCHITECTURE})
endif()
set(ANDROID_ABI "arm64-v8a")
message(STATUS "Compile for architecture: ${ANDROID_ABI}")
message("Compile for architecture: ${ANDROID_ABI}")

set(build_DIR ${CMAKE_SOURCE_DIR}/build)

set(pytorch_testapp_cpp_DIR ${CMAKE_CURRENT_LIST_DIR}/src/main/cpp)
message("CMAKE_CURRENT_LIST_DIR: ${CMAKE_CURRENT_LIST_DIR}")
message("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB pytorch_testapp_SOURCES
        ${pytorch_testapp_cpp_DIR}/native-lib.cpp
        )

message("pytorch_testapp_SOURCES: ${pytorch_testapp_SOURCES}")


# OpenCV
set(distribution_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../distribution)
set(OpenCV_DIR ${distribution_DIR}/libs/arm64-v8a/sdk/native/jni)
message("distribution_DIR: ${distribution_DIR}")
message("OpenCV_DIR: ${OpenCV_DIR}")
find_package(OpenCV 4.5.4 REQUIRED)




include_directories(${distribution_DIR}/libs/arm64-v8a/opencv2)

# Find PyTorch libraries and include directories
set(PYTORCH_ROOT_DIR ${distribution_DIR}/libs/arm64-v8a/org.pytorch)


file(GLOB PYTORCH_INCLUDE_DIRS "${build_DIR}/pytorch_android*.aar/headers")
file(GLOB PYTORCH_LINK_DIRS "${build_DIR}/pytorch_android*.aar/jni/${ANDROID_ABI}")
message("######### PYTORCH_INCLUDE_DIRS: ${PYTORCH_INCLUDE_DIRS}")
message("######### PYTORCH_LINK_DIRS: ${PYTORCH_LINK_DIRS}")

find_library(PYTORCH_LIBRARY pytorch_jni_lite
        PATHS ${PYTORCH_LINK_DIRS}
        NO_CMAKE_FIND_ROOT_PATH)
find_library(FBJNI_LIBRARY fbjni
        PATHS ${PYTORCH_LINK_DIRS}
        NO_CMAKE_FIND_ROOT_PATH)

message("######### PYTORCH_LIBRARY: ${PYTORCH_LIBRARY}")
message("######### FBJNI_LIBRARY: ${FBJNI_LIBRARY}")


# PyTorch
add_library(${TARGET} SHARED
        ${pytorch_testapp_SOURCES}
        )


target_include_directories(${TARGET} PRIVATE
        ${CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES}
        ${OpenCV_INCLUDE_DIRS}
#        ${PYTORCH_INCLUDE_DIR}
        ${PYTORCH_INCLUDE_DIRS}
        )

target_link_libraries(${TARGET}
        ${OpenCV_LIBS}
        -pthread
        ${PYTORCH_LIBRARY}
        ${FBJNI_LIBRARY}
        log)

target_compile_options(${TARGET} PRIVATE -fexceptions)




