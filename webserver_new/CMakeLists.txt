cmake_minimum_required(VERSION 3.15)
project(webserver_new)

set(CMAKE_CXX_STANDARD 11)

include_directories(include /usr/local/include)

link_directories(/usr/local/lib)



add_executable(webserver_new test_client.cpp src/base64.cpp src/http_server.cpp src/send_res_client.cpp)

