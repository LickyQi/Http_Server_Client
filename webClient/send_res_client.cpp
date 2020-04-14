//
// Created by 李琦 on 2020/3/25.
//

#include "send_res_client.h"
#include <boost/asio.hpp>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using boost::asio::ip::tcp;

boost::asio::io_service io_service_face;
tcp::resolver::iterator endpoint_iterator_face;

string host_face;
string port_face;
string sub_host_face;

boost::asio::io_service io_service_plate;
tcp::resolver::iterator endpoint_iterator_plate;

string host_plate;
string port_plate;
string sub_host_plate;

std::mutex mt;

string stamp_to_standard(int stampTime)
{
    time_t tick = (time_t)stampTime;
    struct tm tm;
    char s[20];
    tm = *localtime(&tick);
    strftime(s, sizeof(s), "%Y%m%d_%H_%M_%S", &tm);
    return s;
}

int InitFaceClient(char* url, char* port){

    std::lock_guard<std::mutex> lk(mt);

    string s(url);
    int pos = s.find_first_of('/');
    string ip = s.substr(0,pos);
    if(pos == -1)
        sub_host_face = "";
    else
        sub_host_face = s.substr(pos);

    try
    {
        //如果io_service存在复用的情况
        if(io_service_face.stopped())
            io_service_face.reset();

        // 从dns取得域名下的所有ip
        tcp::resolver resolver(io_service_face);
        tcp::resolver::query query(ip, port);
        endpoint_iterator_face = resolver.resolve(query);
        host_face = ip;
        port_face = port;
    }
    catch(std::exception& e)
    {
        cout << "Face: " << e.what() << endl;
        return -1;
    }
    return 0;
}

int ReleaseFaceClient(){

    std::lock_guard<std::mutex> lk(mt);

    try {
        io_service_face.reset();
        io_service_face.stop();
    }
    catch(std::exception& e)
    {
        cout << "Face: " << e.what() << endl;
        return -1;
    }
    return 0;
}

int SendFaceRes(string cameraid, string c_x, string c_y, string c_h,
                string c_w, string c_age, string c_gender, string c_threshold,
                string totalimgbase64, string imgbase64, string stranger, string faceid,
                string timestap, int datetime){

    std::lock_guard<std::mutex> lk(mt);

    try {
        // 尝试连接到其中的某个ip直到成功
        tcp::socket socket(io_service_face);
        boost::asio::connect(socket, endpoint_iterator_face);

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

        // Form the request.
        boost::asio::streambuf request;
        std::ostream request_stream(&request);
        request_stream << "POST "<< sub_host_face <<" HTTP/1.0\r\n";
        request_stream << "Host: " << host_face << ":" << port_face << "\r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "Content-Length: " << str_json.length() << "\r\n";
        request_stream << "Content-Type: application/json\r\n";
        request_stream << "Connection: close\r\n";
        request_stream << str_json << "\r\n\r\n";

        // Send the request.
        boost::asio::write(socket, request);

        boost::system::error_code error;
        if (error == boost::asio::error::eof)
            return -1; // Connection closed cleanly by peer.
        else if (error)
            throw boost::system::system_error(error); // Some other error


        // Read the response status line.
        boost::asio::streambuf response;
        boost::asio::read_until(socket, response, "\r\n");

        std::istream response_stream(&response);

        string str;
        while (std::getline(response_stream, str)) {
            std::cout << str << std::endl;
        }

        /*

        // Check that response is OK.
        std::string http_version;
        response_stream >> http_version;
        unsigned int status_code;
        response_stream >> status_code;
        std::string status_message;
        std::getline(response_stream, status_message);
        if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
            cout << "Invalid response" << endl;
            return -1;
        }
        // 如果服务器返回非200都认为有错,不支持301/302等跳转
        if (status_code != 200) {
            cout << "Response returned with status code != 200 " << endl;
            return -1;
        }

        // 包头
        std::string header;
        std::vector<string> headers;
        while (std::getline(response_stream, header) && header != "\r")
            headers.push_back(header);

        // 读取所有剩下的数据作为包体
        while (boost::asio::read(socket, response,
                                 boost::asio::transfer_at_least(1), error)) {
        }
        //响应有数据
        if (response.size()) {
            std::istream response_stream(&response);
            std::istreambuf_iterator<char> eos;
            cout << string(std::istreambuf_iterator<char>(response_stream), eos) << endl;
        }
*/
        return 0;
    }
    catch(std::exception& e)
    {
        cout << "Face: " << e.what() << endl;
        return -1;
    }
}

int InitPlateClient(char* url, char* port){

    std::lock_guard<std::mutex> lk(mt);

    string s(url);
    int pos = s.find_first_of('/');
    string ip = s.substr(0,pos);
    if(pos == -1)
        sub_host_face = "";
    else
        sub_host_face = s.substr(pos);

    try
    {
        //如果io_service存在复用的情况
        if(io_service_plate.stopped())
            io_service_plate.reset();

        // 从dns取得域名下的所有ip
        tcp::resolver resolver(io_service_plate);
        tcp::resolver::query query(ip, port);
        endpoint_iterator_plate = resolver.resolve(query);
        host_plate = ip;
        port_plate = port;
    }
    catch(std::exception& e)
    {
        cout << "Plate: " << e.what() << endl;
        return -1;
    }
    return 0;
}

int ReleasePlateClient(){

    std::lock_guard<std::mutex> lk(mt);

    try {
        io_service_plate.reset();
        io_service_plate.stop();
    }
    catch(std::exception& e)
    {
        cout << "Plate: " << e.what() << endl;
        return -1;
    }
    return 0;
}

int SendPlateRes(string cameraid, string c_x, string c_y, string c_h,
                 string c_w, string c_threshold, string totalimgbase64,
                 string imgbase64, string plate_id){
    std::lock_guard<std::mutex> lk(mt);

    try {
        // 尝试连接到其中的某个ip直到成功
        tcp::socket socket(io_service_plate);
        boost::asio::connect(socket, endpoint_iterator_plate);

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

        // Form the request.
        boost::asio::streambuf request;
        std::ostream request_stream(&request);
        request_stream << "POST " << sub_host_plate << " HTTP/1.0\r\n";
        request_stream << "Host: " << host_plate << ":" << port_plate << "\r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "Content-Length: " << str_json.length() << "\r\n";
        request_stream << "Content-Type: application/json\r\n";
        request_stream << "Connection: close\r\n";
        request_stream << str_json << "\r\n\r\n";

        // Send the request.
        boost::asio::write(socket, request);

        boost::system::error_code error;
        if (error == boost::asio::error::eof)
            return -1; // Connection closed cleanly by peer.
        else if (error)
            throw boost::system::system_error(error); // Some other error

        /*
        // Read the response status line.
        boost::asio::streambuf response;
        boost::asio::read_until(socket, response, "\r\n");

        // Check that response is OK.
        std::istream response_stream(&response);
        std::string http_version;
        response_stream >> http_version;
        unsigned int status_code;
        response_stream >> status_code;
        std::string status_message;
        std::getline(response_stream, status_message);
        if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
            cout << "Invalid response" << endl;
            return -1;
        }
        // 如果服务器返回非200都认为有错,不支持301/302等跳转
        if (status_code != 200) {
            cout << "Response returned with status code != 200 " << endl;
            return -1;
        }

        // 传说中的包头可以读下来了
        std::string header;
        std::vector<string> headers;
        while (std::getline(response_stream, header) && header != "\r")
            headers.push_back(header);

        // 读取所有剩下的数据作为包体
        while (boost::asio::read(socket, response,
                                 boost::asio::transfer_at_least(1), error)) {
        }
        //响应有数据
        if (response.size()) {
            std::istream response_stream(&response);
            std::istreambuf_iterator<char> eos;
            cout << string(std::istreambuf_iterator<char>(response_stream), eos) << endl;
        }
*/
        return 0;
    }
    catch(std::exception& e)
    {
        cout << "Plate: " << e.what() << endl;
        return -1;
    }

    return 0;
}

