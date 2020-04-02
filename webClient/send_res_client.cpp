//
// Created by 李琦 on 2020/3/25.
//

#include "send_res_client.h"
#include "json/json.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

boost::asio::io_service io_service_face;
tcp::resolver::iterator endpoint_iterator_face;

char* host_face;
char* port_face;

boost::asio::io_service io_service_plate;
tcp::resolver::iterator endpoint_iterator_plate;

char* host_plate;
char* port_plate;

std::mutex mt;

int InitFaceClient(char* url, char* port){

    std::lock_guard<std::mutex> lk(mt);

    try
    {
        //如果io_service存在复用的情况
        if(io_service_face.stopped())
            io_service_face.reset();

        // 从dns取得域名下的所有ip
        tcp::resolver resolver(io_service_face);
        tcp::resolver::query query(url, port);
        endpoint_iterator_face = resolver.resolve(query);
        host_face = url;
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
                string timestap, string datetime){

    std::lock_guard<std::mutex> lk(mt);

    try {
        // 尝试连接到其中的某个ip直到成功
        tcp::socket socket(io_service_face);
        boost::asio::connect(socket, endpoint_iterator_face);

        //构建JSON串
        Json::Value data_value;

        data_value["cameraid"] = cameraid;
        data_value["c_x"] = c_x;
        data_value["c_y"] = c_y;
        data_value["c_h"] = c_h;
        data_value["c_w"] = c_w;
        data_value["c_age"] = c_age;
        data_value["c_gender"] = c_gender;
        data_value["c_threshold"] = c_threshold;
        data_value["totalimgbase64"] = totalimgbase64;
        data_value["imgbase64"] = imgbase64;
        data_value["stranger"] = stranger;
        data_value["faceid"] = faceid;
        data_value["timestap"] = timestap;
        data_value["datetime"] = datetime;

        //格式转换
        string str_json = data_value.toStyledString();

        // Form the request.
        boost::asio::streambuf request;
        std::ostream request_stream(&request);
        request_stream << "POST "<< " HTTP/1.0\r\n";
        request_stream << "Host: " << host_face << ":" << port_face << "\r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "Content-Length: " << str_json.length() << "\r\n";
        request_stream << "Data: " << data_value << "\r\n";
        request_stream << "Content-Type: application/json\r\n";
        request_stream << "Connection: close\r\n\r\n";

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

    try
    {
        //如果io_service存在复用的情况
        if(io_service_plate.stopped())
            io_service_plate.reset();

        // 从dns取得域名下的所有ip
        tcp::resolver resolver(io_service_plate);
        tcp::resolver::query query(url, port);
        endpoint_iterator_plate = resolver.resolve(query);
        host_plate = url;
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

        //构建JSON串
        Json::Value data_value;

        data_value["cameraid"] = cameraid;
        data_value["c_x"] = c_x;
        data_value["c_y"] = c_y;
        data_value["c_h"] = c_h;
        data_value["c_w"] = c_w;
        data_value["c_threshold"] = c_threshold;
        data_value["totalimgbase64"] = totalimgbase64;
        data_value["imgbase64"] = imgbase64;
        data_value["plate_id"] = plate_id;

        //格式转换
        string str_json = data_value.toStyledString();

        // Form the request.
        boost::asio::streambuf request;
        std::ostream request_stream(&request);
        request_stream << "POST " << "" << " HTTP/1.0\r\n";
        request_stream << "Host: " << host_plate << ":" << port_plate << "\r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "Content-Length: " << str_json.length() << "\r\n";
        request_stream << "Data: " << data_value << "\r\n";
        request_stream << "Content-Type: application/json\r\n";
        request_stream << "Connection: close\r\n\r\n";

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

