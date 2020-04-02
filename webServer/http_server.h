//
// Created by 李琦 on 2020-03-20.
//

#ifndef WEBSERVER_HTTP_SERVER_H
#define WEBSERVER_HTTP_SERVER_H

#include <string>
#include <thread>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <fstream>

struct CropImg {
    // 数据类型
    uint32_t type;
    // 边框x
    uint32_t c_x;
    // 边框y
    uint32_t c_y;
    // 边框h
    uint32_t c_h;
    // 边框w
    uint32_t c_w;
    // 置信度
    uint32_t c_threshold;
    // 抠图base64编码数据
    std::string img_data;
};

struct DecodedJsonObj {
    // 协议版本
    uint32_t version;
    // 设备ip
    std::string ai_ip;
    // 设备MAC
    std::string ai_mac;
    // 摄像头设备ID
    std::string camera_id;
    // 摄像头通道ID
    std::string channel_id;
    // unix时间戳
    uint32_t unix_time;
    // 原图base64编码数据
    std::string total_img;
    // 数据块数组
    struct CropImg *crop_imgs;
    // 数据块数组长度
    uint32_t crop_img_num;

    // override new and delete to use memory pool
    void* operator new(size_t size);
    void operator delete(void* p);
};

/*
功    能：
	读取接收并解析出的DecodedJsonObj的接口。
函数声明：
	int ReadDecodedJsonObj(DecodedJsonObj *obj);
运行方式：
	阻塞式运行，需支持多线程调用，保证线程安全。
参    数：
	obj 读取到的DecodedJsonObj对象的地址
返回值：
	读取成功时，返回 0；否则异常返回 -1。
*/
int ReadDecodedJsonObj(DecodedJsonObj *obj);

/*
功    能：
	释放DecodedJsonObj的接口。
函数声明：
	int ReleaseDecodedJsonObj(DecodedJsonObj *obj);
运行方式：
	阻塞式运行，需支持多线程调用，保证线程安全。
参    数：
	obj 待释放的DecodedJsonObj对象的地址
返回值：
	释放成功时，返回 0；否则异常返回 -1。
*/
int ReleaseDecodedJsonObj(DecodedJsonObj *obj);

/*
 功    能：
	处理监听数据。
函数声明：
	int Datalistening(char *port,char* url);
运行方式：
	阻塞式运行，不可并发调用。
参    数：
	port    监听端口号
    url     url地址
返回值：
	初始化正常时，返回 0；否则异常返回 -1。
 */
int DataListening(char *port,char* url);

/*
功    能：
	初始化http监听服务的接口。
函数声明：
	int StartServer(char *port,char* url);
运行方式：
	阻塞式运行，不可并发调用。
参    数：
	port    监听端口号
    url     url地址
    error_code    返回状态码
返回值：
	初始化正常时，返回 0；否则异常返回 -1。
*/

/** Http */
int StartServer(char* port, char* host);

/** TCP
int StartServer(char *port,char* url){

    std::mutex mts;
    std::lock_guard<std::mutex> lk(mts);

    try
    {
        boost::asio::io_context io_context;
        tcp::acceptor a(io_context, tcp::endpoint(boost::asio::ip::address_v4::from_string(url),
                                                  std::atoi(port)), true);

        for (;;)
        {
            char data_arr[200000];

            boost::system::error_code error;
            size_t length = a.accept().read_some(boost::asio::buffer(data_arr), error);

            if (error == boost::asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error

            // Listening Results

            // std::cout.write(data_arr,length);
            memcpy(data,data_arr,length+1);
            cout << "T1" <<endl;
            cout << data << endl;

            //Reply
//            char reply[max_length]="Hello";
//            size_t reply_length = std::strlen(reply);
//            std::cout << "Reply is: ";
//            std::cout.write(reply, reply_length);
//            std::cout << "\n";
//
//            boost::asio::write(sock, boost::asio::buffer(reply, reply_length));

        }
        return 0;
    }
    catch (std::exception& e)
    {
        std::cerr << "Start Exception: " << e.what() << "\n";
        return -1;
    }
}
*/

/*
功   能：
	终止http监听服务的接口。
函数声明：
	int StopServer();
运行方式：
	阻塞型
参	数：
 	port    监听端口号
    url     url地址
返 回 值：
	0：正常， -1：异常
注释：
	应释放模块中的各种资源。
*/
int StopServer(char *port,char* url);

#endif //WEBSERVER_HTTP_SERVER_H
