cmake_minimum_required(VERSION 3.15)
project(webClient)

set(CMAKE_CXX_STANDARD 11)

#添加头文件搜索路径
include_directories(/usr/local/include)

#添加库文件搜索路径
link_directories(/usr/local/lib)

add_executable(webClient main.cpp send_res_client.h send_res_client.cpp)

#添加JSON库
target_link_libraries(webClient json_linux-gcc-4.2.1_libmt)