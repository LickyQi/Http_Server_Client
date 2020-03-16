cmake_minimum_required(VERSION 3.14)
project(httpclient)

set(CMAKE_CXX_STANDARD 11)

#添加头文件搜索路径
include_directories(/usr/local/Cellar/boost/1.72.0/include)

#添加库文件搜索路径
link_directories(/usr/local/Cellar/boost/1.72.0)

add_executable(httpclient main.cpp)