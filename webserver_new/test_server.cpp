#include "http_server.h"

using namespace std;

void decode_json(){
    for(;;) {
        std::chrono::milliseconds timespan(500); // or whatever
        std::this_thread::sleep_for(timespan);
        // Base64 解码
        DecodedJsonObj *jsonObj = ReadDecodedJsonObj();
        if (jsonObj == NULL) {
            //输出解码
            cout << "Can't get JsonObj!" << endl;
        } else if((jsonObj != NULL) && (jsonObj->version == -1)){
            cout << "End Server!" << endl;
            ReleaseDecodedJsonObj(jsonObj);
            break;
        }
        else {
            cout << "Decode the JsonObj!" << endl;
        }

    }
}

int main() {

    int port = 9877;
    string resource_path= "/anasys/facerecognizeservice";

    if(StartServer(port,resource_path)!=0)
        return 0;

    std::thread t1(decode_json);
    //StopServer(port,url);

    t1.join();

    return 0;
}
