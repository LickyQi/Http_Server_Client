#include "http_server.h"

using namespace std;

void decode_json(){
    for(;;) {
        std::chrono::milliseconds timespan(500); // or whatever
        std::this_thread::sleep_for(timespan);
        // Base64 解码
        DecodedJsonObj *jsonObj = new DecodedJsonObj;

        int decode_flag = ReadDecodedJsonObj(jsonObj);
        if (decode_flag == 0) {
            //输出解码
            cout << "Decode the JsonObj!" << endl;
        } else if(decode_flag == -1){
            cout << "Can't get JsonObj!" << endl;
        }
        else if(decode_flag == -2){
            cout << "End Server!" << endl;
            ReleaseDecodedJsonObj(jsonObj);
            break;
        }
    }
}

int main() {

    char * port = "1080";
    char * url= "127.0.0.1/Face";

    if(StartServer(port,url)!=0)
        return 0;

    std::thread t1(decode_json);
    StopServer(port,url);

    t1.join();

    return 0;
}
