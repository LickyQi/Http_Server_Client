//
// Created by 李琦 on 2020/3/25.
//

#include "send_res_client.h"
#include "client_http.hpp"
#include <boost/asio.hpp>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using boost::asio::ip::tcp;
using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;

std::mutex mt;

static string globle_face_ip_port;
static string globle_face_resource_path;

static string globle_plate_ip_port;
static string globle_plate_resource_path;

string stamp_to_standard(int stampTime)
{
    time_t tick = (time_t)stampTime;
    struct tm tm;
    char s[20];
    tm = *localtime(&tick);
    strftime(s, sizeof(s), "%Y%m%d_%H_%M_%S", &tm);
    return s;
}

int InitFaceClient(string ip_port, string resource_path){
    std::lock_guard<std::mutex> lk(mt);
    globle_face_ip_port = ip_port;
    globle_face_resource_path = resource_path;
    return 0;
}

int ReleaseFaceClient(){
    return 0;
}

int SendFaceRes(string cameraid, string c_x, string c_y, string c_h,
                string c_w, string c_age, string c_gender, string c_threshold,
                string totalimgbase64, string imgbase64, string stranger, string faceid,
                string timestap, uint32_t datetime){

    std::lock_guard<std::mutex> lk(mt);
    try {
        //构建JSON串
        rapidjson::StringBuffer data_value;
        rapidjson::Writer<rapidjson::StringBuffer> writer(data_value);

        writer.StartObject();

        writer.Key("cameraid");
        const char * temp = cameraid.c_str();
        writer.String(temp);

        writer.Key("c_x");
        temp = c_x.c_str();
        writer.String(temp);

        writer.Key("c_y");
        temp = c_y.c_str();
        writer.String(temp);

        writer.Key("c_h");
        temp = c_h.c_str();
        writer.String(temp);

        writer.Key("c_w");
        temp = c_w.c_str();
        writer.String(temp);

        writer.Key("c_age");
        temp = c_age.c_str();
        writer.String(temp);

        writer.Key("c_gender");
        temp = c_gender.c_str();
        writer.String(temp);

        writer.Key("c_threshold");
        temp = c_threshold.c_str();
        writer.String(temp);

        writer.Key("totalimgbase64");
        temp = totalimgbase64.c_str();
        writer.String(temp);

        writer.Key("imgbase64");
        temp = imgbase64.c_str();
        writer.String(temp);

        writer.Key("stranger");
        temp = stranger.c_str();
        writer.String(temp);

        writer.Key("faceid");
        temp = faceid.c_str();
        writer.String(temp);

        writer.Key("timestap");
        temp = timestap.c_str();
        writer.String(temp);

        writer.Key("datetime");
        temp = stamp_to_standard(datetime).c_str();
        writer.String(temp);

        writer.EndObject();
        //格式转换
        string str_json = data_value.GetString();

        HttpClient client(globle_face_ip_port);
        SimpleWeb::CaseInsensitiveMultimap header;
        header.emplace("Content-Type", "application/json");
        try {
            auto r2 = client.request("POST", globle_face_resource_path, str_json, header);
            cout << "Response content: " << r2->content.rdbuf() << endl << endl;
        } catch(const SimpleWeb::system_error &e) {
            cerr << "Client request error: " << e.what() << endl;
        }

        return 0;
    }
    catch(std::exception& e)
    {
        cout << "Face: " << e.what() << endl;
        return -1;
    }
}

int InitPlateClient(string ip_port, string resource_path){
    std::lock_guard<std::mutex> lk(mt);
    globle_plate_ip_port = ip_port;
    globle_plate_resource_path = resource_path;
    return 0;
}

int ReleasePlateClient(){
    return 0;
}

int SendPlateRes(string cameraid, string c_x, string c_y, string c_h,
                 string c_w, string c_threshold, string totalimgbase64,
                 string imgbase64, string plate_id){

    std::lock_guard<std::mutex> lk(mt);

    try {
        //构建JSON串
        rapidjson::StringBuffer data_value;
        rapidjson::Writer<rapidjson::StringBuffer> writer(data_value);

        writer.StartObject();

        writer.Key("cameraid");
        const char * temp = cameraid.c_str();
        writer.String(temp);

        writer.Key("c_x");
        temp = c_x.c_str();
        writer.String(temp);

        writer.Key("c_y");
        temp = c_y.c_str();
        writer.String(temp);

        writer.Key("c_h");
        temp = c_h.c_str();
        writer.String(temp);

        writer.Key("c_w");
        temp = c_w.c_str();
        writer.String(temp);

        writer.Key("c_threshold");
        temp = c_threshold.c_str();
        writer.String(temp);

        writer.Key("totalimgbase64");
        temp = totalimgbase64.c_str();
        writer.String(temp);

        writer.Key("imgbase64");
        temp = imgbase64.c_str();
        writer.String(temp);

        writer.Key("plate_id");
        temp = plate_id.c_str();
        writer.String(temp);

        writer.EndObject();
        //格式转换
        string str_json = data_value.GetString();

        HttpClient client(globle_plate_ip_port);
        SimpleWeb::CaseInsensitiveMultimap header;
        header.emplace("Content-Type", "application/json");
        try {
            auto r2 = client.request("POST", globle_plate_resource_path, str_json, header);
            cout << "Response content: " << r2->content.rdbuf() << endl << endl;
        } catch(const SimpleWeb::system_error &e) {
            cerr << "Client request error: " << e.what() << endl;
        }

        return 0;

    }
    catch(std::exception& e)
    {
        cout << "Plate: " << e.what() << endl;
        return -1;
    }
}

