//
// Created by 李琦 on 2020/3/30.
//

#ifndef WENSERVER_WITH_AVX_UTIL_H
#define WENSERVER_WITH_AVX_UTIL_H

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <chrono>

std::string Mat2Base64String(cv::Mat mat);
std::vector<char> Base64String2CharVec(std::string base64_str);


#endif //WENSERVER_WITH_AVX_UTIL_H
