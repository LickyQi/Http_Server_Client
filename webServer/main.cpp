#include "http_server.h"

using namespace std;

void decode_json(){

    for(;;) {
        std::chrono::milliseconds timespan(1000); // or whatever
        std::this_thread::sleep_for(timespan);
        // Base64 解码
        DecodedJsonObj *jsonObj = ReadDecodedJsonObj();
        if (jsonObj == NULL) {
            //输出解码
            cout << "Can't get JsonObj!" << endl;
        } else if(jsonObj->version == -1){
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

    char * port = "9877";
    char * url= "127.0.0.1";

    if(StartServer(port,url)!=0)
        return 0;

    std::thread t1(decode_json);
    StopServer(port,url);

    t1.join();

    return 0;
}
