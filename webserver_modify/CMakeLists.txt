cmake_minimum_required(VERSION 3.15)
project(webserver_modify)

set(CMAKE_CXX_STANDARD 11)

include_directories(include /usr/local/include)

link_directories(/usr/local/lib)

add_executable(webserver_modify test_server.cpp
        src/face_modify_server.cpp include/face_modify_server.h include/load_image_form_url.h)

target_link_libraries(webserver_modify opencv_core opencv_highgui opencv_imgproc opencv_imgcodecs curl)


