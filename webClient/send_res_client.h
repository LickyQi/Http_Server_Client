//
// Created by 李琦 on 2020/3/25.
//

#ifndef WEBCLIENT_SEND_RES_CLIENT_H
#define WEBCLIENT_SEND_RES_CLIENT_H

#include <string>
#include <iostream>

using namespace std;

/*
功    能：
	初始化人脸识别结果上传模块。
函数声明：
	int InitFaceClient(char* url, char* port);
运行方式：
	阻塞式运行，不可并发调用。
参    数：
	port 服务端监听端口号
	url url地址
返回值：
	初始化正常时，返回 0；否则异常返回 -1。
*/
int InitFaceClient(char* url, char* port);

/*
功   能：
	释放人脸识别结果上传模块资源。
函数声明：
	int ReleaseFaceClient();
运行方式：
	阻塞式运行
参	数：

返 回 值：
	0：正常， -1：异常
注释：
	应释放模块中的各种资源。
*/
int ReleaseFaceClient();

/*
功    能：
	发送人脸识别比对结果。
函数声明：
	int SendFaceRes(string cameraid, string c_x, string c_y, string c_h, string c_w,
    string c_age, string c_gender, string c_threshold, string totalimgbase64, string imgbase64,
    string stranger, string faceid, string timestap, string datetime);
运行方式：
	阻塞式运行，需支持多线程调用，保证线程安全。
参    数：
	cameraid	摄像头id(token)
        c_x	边框x
        c_y	边框y
        c_h	边框h
        c_w	边框w
        c_age	年龄
        c_gender	性别
        c_threshold	置信度
        totalimgbase64	加框后的完整图像base64数据
        imgbase64	人脸base64数据
        stranger	是否为陌生人
        faceid	人脸ID
        timestap	相对帧数
        datetime	上传时间(绝对时间)，服务器时间

返回值：
	发送成功时，返回 0；否则异常返回 -1。
*/
int SendFaceRes(string cameraid, string c_x, string c_y, string c_h,
                string c_w, string c_age, string c_gender, string c_threshold,
                string totalimgbase64, string imgbase64, string stranger, string faceid,
                string timestap, string datetime);

/*
功    能：
	初始化车牌识别结果上传模块。
函数声明：
	int InitPlateClient(char* url, char* port);
运行方式：
	阻塞式运行，不可并发调用。
参    数：
	port 服务端监听端口号
	url url地址
返回值：
	初始化正常时，返回 0；否则异常返回 -1。
*/
int InitPlateClient(char* url, char* port);

/*
功   能：
	释放车牌识别结果上传模块资源。
函数声明：
	int ReleasePlateClient();
运行方式：
	阻塞式运行
参	数：

返 回 值：
	0：正常， -1：异常
注释：
	应释放模块中的各种资源。
*/
int ReleasePlateClient();

/*
功    能：
	发送车牌识别结果。
函数声明：
	int SendPlateRes(string cameraid, string c_x, string c_y, string c_h,
    string c_w, string c_threshold, string totalimgbase64,
    string imgbase64, string plate_id);
运行方式：
	阻塞式运行，需支持多线程调用，保证线程安全。
参    数：
	cameraid	摄像头id(token)
	c_x	边框x
	c_y	边框y
	c_h	边框h
	c_w	边框w
	c_threshold	置信度
	totalimgbase64	加框后的完整图像base64数据
	imgbase64	车牌base64数据
	plate_id	车牌ID

返回值：
	发送成功时，返回 0；否则异常返回 -1。
*/
int SendPlateRes(string cameraid, string c_x, string c_y, string c_h,
        string c_w, string c_threshold, string totalimgbase64,
        string imgbase64, string plate_id);

#endif //WEBCLIENT_SEND_RES_CLIENT_H
