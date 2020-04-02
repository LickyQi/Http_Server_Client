#include <iostream>
#include "send_res_client.h"
#include <thread>

int main(int argc, char* argv[])
{
    char *host = "172.16.3.101";
    //char *host = "127.0.0.1";
    char *port = "1080";

    string cameraid = "abcdef110212";
    string c_x = "1160";
    string c_y = "440";
    string c_h = "25";
    string c_w = "18";
    string c_age = "18";
    string c_gender = "men";
    string c_threshold = "0.8";
    string totalimgbase64 = "xxxxxxxxx";
    string imgbase64 = "xxxxxxx";
    string stranger = "0";
    string faceid = "45";
    string timestap = "1149";
    string datetime = "20200117_09_54_22";
    string plate_id = "京A:88771";

    int client_face = InitFaceClient(host,port);
    int client_plate = InitPlateClient(host,port);

    std::thread t1(SendFaceRes, cameraid, c_x, c_y,  c_h,
                   c_w, c_age,c_gender,c_threshold,
                   totalimgbase64, imgbase64,  stranger,  faceid,
                   timestap, datetime);

    std::thread t2(SendPlateRes,cameraid,c_x, c_y, c_h,
                              c_w, c_threshold,totalimgbase64,
                             imgbase64, plate_id);
    t1.join();
    t2.join();

    ReleaseFaceClient();
    ReleasePlateClient();
    return 0;
}