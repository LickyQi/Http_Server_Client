cmake_minimum_required(VERSION 3.15)
project(wenServer_with_AVX)

set(CMAKE_CXX_STANDARD 11)

#添加头文件搜索路径
include_directories(/usr/local/include/ /Users/liqi/Desktop/base64-master/include/)
include_directories(/Users/liqi/Desktop/rapidjson-master/include)

#添加库文件搜索路径
link_directories(/usr/local/lib/ /Users/liqi/Desktop/base64-master/lib/)

add_executable(wenServer_with_AVX main.cpp util.cpp util.h http_server.cpp http_server.h memory_pool.h)

#添加Opencv库
target_link_libraries(wenServer_with_AVX opencv_world server)