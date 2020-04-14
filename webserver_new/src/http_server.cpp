//
// Created by 李琦 on 2020/3/25.
//
#include <queue>
#include <future>
#include "http_server.h"
#include "base64.h"
#include "memory_pool.h"
#include "server_http.hpp"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
static HttpServer http_server;

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

    if(data_list.empty()) {
        //cout << "list empty" << endl;
        return NULL;
    }

    std::unique_lock<std::mutex> list_lock(mt_data);
    string data_str = data_list.front();
    data_list.pop();
    list_lock.unlock();

    //cout << data_str << endl;

//    if(data_str == "End!") {
//        jsonObj->version = -1;
//        return jsonObj;
//    }

    //读取JSON串
    rapidjson::Document read_value;
    read_value.Parse(data_str.c_str());

    if (!read_value.HasParseError()) { //reader将Json字符串解析到read_value

        jsonObj->version = read_value["version"].GetUint();
        jsonObj->ai_ip = read_value["ai_ip"].GetString();
        jsonObj->ai_mac = read_value["ai_mac"].GetString();
        jsonObj->camera_id = read_value["camera_id"].GetString();
        jsonObj->channel_id = read_value["channel_id"].GetString();
        jsonObj->unix_time = read_value["unix_time"].GetUint();
        jsonObj->total_img = read_value["total_img"].GetString();

//        char *image_encode = &read_value["total_img"].asString()[0];
//        /** Base64解码 */
//        unsigned int decode_col = 0;
//        //获取解码长度
//        decode_col = BASE64_DECODE_OUT_SIZE(read_value["total_img"].asString().size())-2;
//        //申请内存，动态创建数组
//        unsigned char * image_decode = new unsigned char[decode_col];
//        base64_decode(image_encode,read_value["total_img"].asString().size(),image_decode);
//
//        vector<unsigned char> str;
//        for(int i=0;i<decode_col;i++)
//            str.push_back(image_decode[i]);
//
//        jsonObj->total_img.resize(str.size());
//        memcpy(&jsonObj->total_img[0], str.data(), str.size());
//
//        delete []image_decode;

        /** 解码完成，释放内存 */

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
//            //获取解码长度
//            decode_col = BASE64_DECODE_OUT_SIZE(read_value["datas"][i]["crop_img"].asString().size())-2;
//            //申请内存，动态创建数组
//            image_decode = new unsigned char[decode_col];
//            base64_decode(image_encode,read_value["datas"][i]["crop_img"].asString().size(),image_decode);
//
//            for(int i=0;i<decode_col;i++)
//                str.push_back(image_decode[i]);
//
//            jsonObj->crop_imgs[i].img_data.resize(str.size());
//            memcpy(&jsonObj->crop_imgs[i].img_data[0], str.data(), str.size());
//
//            delete []image_decode;
            /** 解码完成，释放内存 */

        }
        return jsonObj;
    }
    else{
        cout << "data corrupted!" << endl;
        return NULL;
    }
}

int ReleaseDecodedJsonObj(DecodedJsonObj *obj){

    delete obj;
    return 0;
}

int StartServer(int port, string resource_path) {
    http_server.config.port = port;
    string path = "^";
    path.append(resource_path);
    path.append("$");

    http_server.resource[path]["POST"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
            try {
                    std::unique_lock<std::mutex> list_lock(mt_data);
                    data_list.push(request->content.string());
                    list_lock.unlock();
                    string msg = "OK";
                    *response << "HTTP/1.1 200 OK\r\n"
                            << "Content-Length: " << msg.length() << "\r\n\r\n"
                            << msg;
            }
            catch(const exception &e) {
                    *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
            }
    };

    // Start server and receive assigned port when server is listening for requests
    promise<unsigned short> server_port;
    thread server_thread([&server_port]() {
                    // Start server
                    http_server.start([&server_port](unsigned short port) {
                                    server_port.set_value(port);
                                    });
                    cout << "server thread end" << endl;
                    });
    cout << "Server listening on port " << server_port.get_future().get() << endl << endl;
    server_thread.detach();
    
    return 0;
}

int StopServer(){
    http_server.stop();
    return 0;
}
