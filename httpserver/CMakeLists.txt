cmake_minimum_required(VERSION 3.14)
project(httpserver)

set(CMAKE_CXX_STANDARD 11)

#添加头文件搜索路径
include_directories(/usr/local/Cellar/boost/1.72.0/include)

#添加库文件搜索路径
link_directories(/usr/local/Cellar/boost/1.72.0)

add_executable(httpserver main.cpp connection.cpp connection.hpp connection_manager.cpp
        connection_manager.hpp header.hpp mime_types.cpp mime_types.hpp reply.cpp reply.hpp request.hpp request_handler.cpp
        request_handler.hpp request_parser.cpp request_parser.hpp server.cpp server.hpp)