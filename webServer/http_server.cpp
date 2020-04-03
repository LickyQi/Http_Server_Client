//
// Created by 李琦 on 2020/3/25.
//
#include "http_server.h"
#include "base64.h"
#include "memory_pool.h"
#include <boost/asio.hpp>
#include "json/json.h"
#include <queue>

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
    Json::Reader reader;
    Json::Value read_value;

    if (reader.parse(data,read_value)) { //reader将Json字符串解析到read_value

        jsonObj->version = read_value["version"].asUInt();
        jsonObj->ai_ip = read_value["ai_ip"].asString();
        jsonObj->ai_mac = read_value["ai_mac"].asString();
        jsonObj->camera_id = read_value["camera_id"].asString();
        jsonObj->channel_id = read_value["channel_id"].asString();
        jsonObj->unix_time = read_value["unix_time"].asUInt();

        char *image_encode = &read_value["total_img"].asString()[0];
        /** Base64解码 */
        unsigned int decode_col = 0;
        //获取解码长度
        decode_col = BASE64_DECODE_OUT_SIZE(read_value["total_img"].asString().size())-2;
        //申请内存，动态创建数组
        unsigned char * image_decode = new unsigned char[decode_col];
        base64_decode(image_encode,read_value["total_img"].asString().size(),image_decode);

        vector<unsigned char> str;
        for(int i=0;i<decode_col;i++)
            str.push_back(image_decode[i]);

        jsonObj->total_img.resize(str.size());
        memcpy(&jsonObj->total_img[0], str.data(), str.size());

        delete []image_decode;

        /** 解码完成，释放内存 */

        jsonObj->crop_img_num = read_value["datas"].size();

        jsonObj->crop_imgs = new CropImg[read_value["datas"].size()];

        for (int i = 0; i < read_value["datas"].size(); i++) {
            jsonObj->crop_imgs[i].type = read_value["datas"][i]["type"].asInt();
            jsonObj->crop_imgs[i].c_x =  read_value["datas"][i]["c_x"].asInt();
            jsonObj->crop_imgs[i].c_y=  read_value["datas"][i]["c_y"].asInt();
            jsonObj->crop_imgs[i].c_h =  read_value["datas"][i]["c_h"].asInt();
            jsonObj->crop_imgs[i].c_w =  read_value["datas"][i]["c_w"].asInt();
            jsonObj->crop_imgs[i].c_threshold =  read_value["datas"][i]["c_threshold"].asDouble();
            jsonObj->crop_imgs[i].img_data =  read_value["datas"][i]["crop_img"].asString();

            /** Base64解码 */
            //获取解码长度
            decode_col = BASE64_DECODE_OUT_SIZE(read_value["datas"][i]["crop_img"].asString().size())-2;
            //申请内存，动态创建数组
            image_decode = new unsigned char[decode_col];
            base64_decode(image_encode,read_value["datas"][i]["crop_img"].asString().size(),image_decode);

            for(int i=0;i<decode_col;i++)
                str.push_back(image_decode[i]);

            jsonObj->crop_imgs[i].img_data.resize(str.size());
            memcpy(&jsonObj->crop_imgs[i].img_data[0], str.data(), str.size());

            delete []image_decode;
            /** 解码完成，释放内存 */

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
                std::unique_lock<std::mutex> list_lock(mt_data);
                errorcode = -1;
                list_lock.unlock();
                return -1;
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
