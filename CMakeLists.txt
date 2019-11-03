cmake_minimum_required(VERSION 3.14)
project(camera)

set(CMAKE_CXX_STANDARD 14)

# 设置输出目录为 build/Debug/bin build/Debug/lib
set(OUTPUT_DIRECTORY_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY_ROOT}/bin" CACHE PATH "Runtime directory" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY_ROOT}/lib" CACHE PATH "Library directory" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY_ROOT}/lib" CACHE PATH "Archive directory" FORCE)

# 更新 CMAKE Module 查找路径， 根据自己环境设置
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} /usr/local/share/OpenCV)

find_package(OpenCV REQUIRED)

#message(STATUS ${OpenCV_LIBS})
# 添加src子目录
add_subdirectory(src)

add_executable(camera main.cpp)

target_link_libraries(camera ${OpenCV_LIBS})