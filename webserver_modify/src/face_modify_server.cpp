//
// Created by 李琦 on 2020/4/13.
//
#include <queue>
#include <future>
#include <string>
#include "face_modify_server.h"
#include "server_http.hpp"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "memory_pool.h"
#include "load_image_form_url.h"

using namespace std;
using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
static HttpServer http_server;

// define memory pool with block size as struct size
MemoryPool<sizeof(FaceModifyInfo), 10> MemPool;

//lock
queue <FaceModifyInfo> data_list;
std::mutex mt_data;

void* FaceModifyInfo::operator new(size_t)
{
    //std::cout << "new object space" << std::endl;
    return MemPool.allocate();
}

void FaceModifyInfo::operator delete(void* p)
{
    //std::cout << "free object space" << std::endl;
    MemPool.deallocate(p);
}

int ReadFaceModifyInfo(FaceModifyInfo *info){

    if(data_list.empty()) {
        cout << "list is empty" << endl;
        return -1;
    }

    std::unique_lock<std::mutex> list_lock(mt_data);
    *info = data_list.front();
    data_list.pop();
    list_lock.unlock();

    return 0;
}

int StartFaceModifyServer(int port, string resource_path) {
    http_server.config.port = port;
    string path = "^";
    path.append(resource_path);
    path.append("$");

    // POST
    http_server.resource[path]["POST"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
        try {
            //读取JSON串
            rapidjson::Document read_value;
            read_value.Parse(request->content.string().c_str());

            FaceModifyInfo *infoObj = new FaceModifyInfo;

            if (!read_value.HasParseError()) { //reader将Json字符串解析到read_value

                infoObj->method = 0;
                infoObj->person_id = read_value["person_id"].GetUint();
                infoObj->unix_time = read_value["unix_time"].GetUint();
                std::promise<int> promise;
                infoObj->prom = &promise;

                for (rapidjson::SizeType i = 0; i < read_value["img_urls"].Size(); i++) {
                    cv::Mat image = curlImg(read_value["img_urls"][i].GetString());
                    if (image.empty()) {
                        cout << "Load image fail !" << endl;
                        return -1; // load fail
                    }
                    infoObj->face_imgs.push_back(image);
                }

                std::unique_lock<std::mutex> list_lock(mt_data);
                data_list.push(*infoObj);
                list_lock.unlock();

                // wait for promise
                std::chrono::milliseconds timespan(500); // or whatever
                std::this_thread::sleep_for(timespan);

                int wait_pro = infoObj->prom->get_future().get();

                if(wait_pro==1) {
                    string msg = "success";
                    *response << "HTTP/1.1 200 OK\r\n" << "Content-Length: " << msg.length() << "\r\n\r\n" << msg;
                } else{
                    string msg = "fail";
                    *response << "HTTP/1.1 200 OK\r\n" << "Content-Length: " << msg.length() << "\r\n\r\n" << msg;
                }
            }
            else{
                cout << "data corrupted!" << endl;
                string msg = "data corrupted!";
                *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << msg.length() << "\r\n\r\n" << msg;
                return -1;
            }
        }
        catch(const exception &e) {
            *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
            return -1;
        }
        return 0;
    };

    //PUT
    http_server.resource[path]["PUT"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
        try {
            //读取JSON串
            rapidjson::Document read_value;
            read_value.Parse(request->content.string().c_str());

            FaceModifyInfo *infoObj = new FaceModifyInfo;

            if (!read_value.HasParseError()) { //reader将Json字符串解析到read_value

                infoObj->method = 1;
                infoObj->person_id = read_value["person_id"].GetUint();
                infoObj->unix_time = read_value["unix_time"].GetUint();
                infoObj->image_num = read_value["image_num"].GetUint();
                std::promise<int> promise;
                infoObj->prom = &promise;

                for (rapidjson::SizeType i = 0; i < read_value["img_urls"].Size(); i++) {
                    cv::Mat image = curlImg(read_value["img_urls"][i].GetString());
                    if (image.empty()) {
                        cout << "Load image fail !" << endl;
                        return -1; // load fail
                    }
                    infoObj->face_imgs.push_back(image);
                }

                std::unique_lock<std::mutex> list_lock(mt_data);
                data_list.push(*infoObj);
                list_lock.unlock();

                // wait for promise
                std::chrono::milliseconds timespan(500); // or whatever
                std::this_thread::sleep_for(timespan);

                int wait_pro = infoObj->prom->get_future().get();

                if(wait_pro==1) {
                    string msg = "success";
                    *response << "HTTP/1.1 200 OK\r\n" << "Content-Length: " << msg.length() << "\r\n\r\n" << msg;
                } else{
                    string msg = "fail";
                    *response << "HTTP/1.1 200 OK\r\n" << "Content-Length: " << msg.length() << "\r\n\r\n" << msg;
                }
            }
            else{
                cout << "data corrupted!" << endl;
                string msg = "data corrupted!";
                *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << msg.length() << "\r\n\r\n" << msg;
                return -1;
            }
        }
        catch(const exception &e) {
            *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
            return -1;
        }
        return 0;
    };

    //DELETE
    http_server.resource[path]["DELETE"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
        try {
            //读取JSON串
            rapidjson::Document read_value;
            read_value.Parse(request->content.string().c_str());

            FaceModifyInfo *infoObj = new FaceModifyInfo;

            if (!read_value.HasParseError()) { //reader将Json字符串解析到read_value

                infoObj->method = 2;
                infoObj->person_id = read_value["person_id"].GetUint();
                infoObj->unix_time = read_value["unix_time"].GetUint();
                infoObj->image_num = read_value["image_num"].GetUint();
                std::promise<int> promise;
                infoObj->prom = &promise;

                std::unique_lock<std::mutex> list_lock(mt_data);
                data_list.push(*infoObj);
                list_lock.unlock();

                // wait for promise
                std::chrono::milliseconds timespan(500); // or whatever
                std::this_thread::sleep_for(timespan);

                int wait_pro = infoObj->prom->get_future().get();

                if(wait_pro==1) {
                    string msg = "success";
                    *response << "HTTP/1.1 200 OK\r\n" << "Content-Length: " << msg.length() << "\r\n\r\n" << msg;
                } else{
                    string msg = "fail";
                    *response << "HTTP/1.1 200 OK\r\n" << "Content-Length: " << msg.length() << "\r\n\r\n" << msg;
                }
            }
            else{
                cout << "data corrupted!" << endl;
                string msg = "data corrupted!";
                *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << msg.length() << "\r\n\r\n" << msg;
                return -1;
            }
        }
        catch(const exception &e) {
            *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
            return -1;
        }
        return 0;
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

int StopFaceModifyServer(){
    http_server.stop();
    return 0;
}

