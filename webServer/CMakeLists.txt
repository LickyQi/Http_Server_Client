cmake_minimum_required(VERSION 3.15)
project(webServer)

set(CMAKE_CXX_STANDARD 11)

#添加头文件搜索路径
include_directories(/usr/local/include)

#添加库文件搜索路径
link_directories(/usr/local/lib)

add_executable(webServer base64.c base64.h http_server.cpp http_server.h memory_pool.h main.cpp)

#添加JSON库
target_link_libraries(webServer json_linux-gcc-4.2.1_libmt)