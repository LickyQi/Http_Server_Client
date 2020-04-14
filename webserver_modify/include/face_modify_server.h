//
// Created by 李琦 on 2020/4/13.
//

#ifndef WEBSERVER_MODIFY_FACE_MODIFY_SERVER_H
#define WEBSERVER_MODIFY_FACE_MODIFY_SERVER_H
#include <future>
#include <string>
#include <opencv2/opencv.hpp>
#include <vector>

struct FaceModifyInfo {
    // 动作, 0表示添加，1表示修改，2表示删除
    uint32_t method;
    // unix时间戳
    uint32_t unix_time;
    // 人的id
    int person_id;
    // 现有人脸图像数量
    int image_num;
    // 人脸图像
    std::vector<cv::Mat> face_imgs;
    // 异步调用获取响应，修改成功时响应结果为0，失败时响应结果为1
    std::promise<int> *prom;

    // override new and delete to use memory pool
    void* operator new(size_t size);
    void operator delete(void* p);
};

/*
功    能：
	初始化http监听服务的接口。
函数声明：
	int StartFaceModifyServer(int port, string resource_path);
运行方式：
	非阻塞式运行，不可并发调用。
参    数：
	port 监听端口号
	resource_path 资源地址
返回值：
	初始化正常时，返回 0；否则异常返回 -1。
*/
int StartFaceModifyServer(int port, std::string resource_path);

/*
功   能：
	终止http监听服务的接口。
函数声明：
	int StopFaceModifyServer();
运行方式：
	阻塞型，不可并发调用
参	数：

返 回 值：
	0：正常， -1：异常
注释：
	应释放模块中的各种资源。
*/
int StopFaceModifyServer();

/*
功    能：
	读取接收并解析出的FaceModifyInfo的接口。
函数声明：
	ReadFaceModifyInfo(FaceModifyInfo *info);
运行方式：
	阻塞式运行，需支持多线程调用，保证线程安全。
参    数：
        info 将解析出的信息赋值给info
返回值：
	读取成功时，返回 0；否则返回 -1。
*/
int ReadFaceModifyInfo(FaceModifyInfo *info);

#endif //WEBSERVER_MODIFY_FACE_MODIFY_SERVER_H
