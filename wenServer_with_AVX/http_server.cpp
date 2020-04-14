//
// Created by 李琦 on 2020/3/25.
//
#include "http_server.h"
#include "util.h"
#include "memory_pool.h"
#include <boost/asio.hpp>
#include <queue>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"

using namespace std;
using boost::asio::ip::tcp;

// define memory pool with block size as struct size
MemoryPool<sizeof(DecodedJsonObj), 10> MemPool;

//lock
int errorcode = 0;
queue <string> data_list;
std::mutex mt_data;

void* DecodedJsonObj::operator new(size_t)
{
    //std::cout << "new object space" << std::endl;
    return MemPool.allocate();
}

void DecodedJsonObj::operator delete(void* p)
{
    //std::cout << "free object space" << std::endl;
    MemPool.deallocate(p);
}

DecodedJsonObj* ReadDecodedJsonObj(){

    DecodedJsonObj *jsonObj = new DecodedJsonObj;

    if(data_list.empty())
        return NULL;

    string data_str = data_list.front();
    std::unique_lock<std::mutex> list_lock(mt_data);
    data_list.pop();
    list_lock.unlock();

    if(data_str == "End!") {
        jsonObj->version = -1;
        return jsonObj;
    }

    int len = data_str.length();
    char* data =new char[len+1];
    strcpy(data,data_str.c_str());

    //读取JSON串
    rapidjson::Document read_value;
    read_value.Parse(data);

    if (!read_value.HasParseError()) { //reader将Json字符串解析到read_value

        jsonObj->version = read_value["version"].GetUint();
        jsonObj->ai_ip = read_value["ai_ip"].GetString();
        jsonObj->ai_mac = read_value["ai_mac"].GetString();
        jsonObj->camera_id = read_value["camera_id"].GetString();
        jsonObj->channel_id = read_value["channel_id"].GetString();
        jsonObj->unix_time = read_value["unix_time"].GetUint();

        jsonObj->total_img = read_value["total_img"].GetString();

//        vector<char> str;
//        /** Base64解码 */
//        str = Base64String2CharVec(read_value["total_img"].GetString());
//        jsonObj->total_img.resize(str.size());
//        memcpy(&jsonObj->total_img[0], str.data(), str.size());

        jsonObj->crop_img_num = read_value["datas"].Size();
        jsonObj->crop_imgs = new CropImg[read_value["datas"].Size()];

        for (rapidjson::SizeType i = 0; i < read_value["datas"].Size(); i++) {
            jsonObj->crop_imgs[i].type = read_value["datas"][i]["type"].GetUint();
            jsonObj->crop_imgs[i].c_x =  read_value["datas"][i]["c_x"].GetUint();
            jsonObj->crop_imgs[i].c_y=  read_value["datas"][i]["c_y"].GetUint();
            jsonObj->crop_imgs[i].c_h =  read_value["datas"][i]["c_h"].GetUint();
            jsonObj->crop_imgs[i].c_w =  read_value["datas"][i]["c_w"].GetUint();
            jsonObj->crop_imgs[i].c_threshold =  read_value["datas"][i]["c_threshold"].GetDouble();

            jsonObj->crop_imgs[i].img_data =  read_value["datas"][i]["crop_img"].GetString();

            /** Base64解码 */
//            str = Base64String2CharVec(read_value["datas"][i]["crop_img"].GetString());
//            jsonObj->crop_imgs[i].img_data.resize(str.size());
//            memcpy(&jsonObj->crop_imgs[i].img_data[0], str.data(), str.size());

        }
        delete[] data;
        return jsonObj;
    }
    else{
        delete[] data;
        return NULL;
    }
}

int ReleaseDecodedJsonObj(DecodedJsonObj *obj){

    delete obj;
    return 0;
}

int DataListening(char *port,char* url){

    try {
        boost::asio::io_context io_context;
        tcp::acceptor a(io_context, tcp::endpoint(boost::asio::ip::address_v4::from_string(url),
                                                  std::atoi(port)));
        for (;;) {
            // listening
            tcp::socket socket = a.accept();
            boost::asio::streambuf listening;
            boost::asio::read_until(socket, listening, "\r\n\r\n");

            boost::system::error_code error;
            if (error == boost::asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error

            // Reply
            boost::asio::streambuf reply;
            std::ostream reply_stream(&reply);
            string data_str = "Ok";
            reply_stream << "POST " << "" << " HTTP/1.0\r\n";
            reply_stream << "Host: " << url << ":" << port << "\r\n";
            reply_stream << "Accept: */*\r\n";
            reply_stream << "Content-Length: " << data_str.length() << "\r\n";
            reply_stream << "Content-Type: application/json\r\n";
            reply_stream << "Connection: close\r\n\r\n";
            // Send the reply.
            boost::asio::write(socket, reply);

            // Check that response is OK.
            std::istream listening_stream(&listening);

//            string str;
//            while (std::getline(listening_stream, str)) {
//                std::cout << str << std::endl;
//            }

            std::string http_version, page, http_method;
            listening_stream >> http_method >> page >> http_method;

            if (!listening_stream || page != "/anasys/facerecognizeservice") {
                cout << "Invalid listening" << endl;
                continue;
            }
            // 包头
            std::string header;
            while (std::getline(listening_stream, header) && header != "{\r"){
            }

            // 读取所有剩下的数据作为body
            while (boost::asio::read(socket, listening,
                                     boost::asio::transfer_at_least(1), error)){
            }

            //JSON有数据
            string js = "";
            if (listening.size())
            {
                std::istream listening_stream(&listening);
                std::istreambuf_iterator<char> eos;
                js.append(string(std::istreambuf_iterator<char>(listening_stream), eos));
            }

            std::unique_lock<std::mutex> list_lock(mt_data);
            data_list.push(js);
            list_lock.unlock();

        }
        std::unique_lock<std::mutex> list_lock(mt_data);
        errorcode = 0;
        list_lock.unlock();
        return 0;
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        std::unique_lock<std::mutex> list_lock(mt_data);
        errorcode = -1;
        list_lock.unlock();
        return -1;
    }
}

int StartServer(char* port, char* host) {

    string s(host);
    int pos = s.find_first_of('/');
    string ip = s.substr(0,pos);
    string sub_host = "";
    if(pos != -1)
        sub_host = s.substr(pos);

    char* ip_char = new char[ip.length()+1];
    strcpy(ip_char,ip.c_str());

    thread t1(DataListening,port,ip_char);
    t1.detach();

    std::chrono::milliseconds timespan(100);
    std::this_thread::sleep_for(timespan);

    if(errorcode == -1)
        return -1;

    return 0;
}

int StopServer(char *port,char* url){

    std::chrono::milliseconds timespan(10000); // or whatever
    std::this_thread::sleep_for(timespan);

    string s(url);
    int pos = s.find_first_of('/');
    string ip = s.substr(0,pos);
    string sub_host = "";
    if(pos != -1)
        sub_host = s.substr(pos);

    //End flag
    std::unique_lock<std::mutex> list_lock(mt_data);
    string flag = "End!";
    data_list.push(flag);
    list_lock.unlock();

    try
    {
        boost::asio::io_context io_context;
        tcp::socket socket(io_context);
        //(2)通过connect函数连接服务器，打开socket连接。
        tcp::endpoint end_point(boost::asio::ip::address::from_string(ip), std::atoi(port));
        socket.connect(end_point);

        socket.close();
        io_context.reset();
        return 0;
    }
    catch (std::exception& e)
    {
        std::cerr << "Stop Exception: " << e.what() << "\n";
        return -1;
    }
}
