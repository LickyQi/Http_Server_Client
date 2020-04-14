//
// Created by 李琦 on 2020/3/30.
//

#include "util.h"
#include "libbase64.h"

using namespace std;
using namespace cv;

string Mat2Base64String(cv::Mat mat)
{
    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    compression_params.push_back(95);

    std::vector<uchar> data_encode;
    imencode(".jpg", mat, data_encode, compression_params);

    size_t out_max_size = data_encode.size() * 4 / 3 + 1;
    char *encode_out = (char *)malloc(out_max_size * sizeof(char));
    size_t out_size = 0;
    auto start = std::chrono::high_resolution_clock::now();
    base64_encode((const char*)&data_encode[0], data_encode.size(), encode_out, &out_size, BASE64_FORCE_AVX);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = duration_cast<chrono::milliseconds>(end - start);
    cout << "inference taken : " << duration.count() << " ms" << endl;
    //cout << "out_size: " << out_size << endl;
    string encode_str(encode_out, out_size);
    free(encode_out);
    return encode_str;
}

vector<char> Base64String2CharVec(string base64_str)
{
    size_t encode_str_len = base64_str.length();
    size_t decode_max_size = encode_str_len * 3 / 4 + 1;
    char *decode_out = (char *)malloc(decode_max_size * sizeof(char));
    size_t decode_out_size = 0;
    base64_decode(base64_str.data(), encode_str_len, decode_out, &decode_out_size, BASE64_FORCE_AVX);
    vector<char> res(decode_out, decode_out + decode_out_size);
    free(decode_out);
    return res;
}
