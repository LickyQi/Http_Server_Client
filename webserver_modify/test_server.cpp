#include "face_modify_server.h"

using namespace std;

int face_det(FaceModifyInfo *jsonObj){
    jsonObj->prom->set_value(0);
    return 0;
}

int read_json(FaceModifyInfo *jsonObj){

    for(;;) {
        std::chrono::milliseconds timespan(500); // or whatever
        std::this_thread::sleep_for(timespan);
        if(ReadFaceModifyInfo(jsonObj)==0){
            cout << jsonObj->person_id << endl;

            std::chrono::milliseconds timespan(500); // or whatever
            std::this_thread::sleep_for(timespan);
            std::thread t2(face_det,jsonObj);
            t2.join();
        }
    }
}

int main() {

    int port = 9877;
    string resource_path= "/anasys/facerecognizeservice";

    if(StartFaceModifyServer(port,resource_path)!=0)
        return 0;

    FaceModifyInfo *infoObj = new FaceModifyInfo;
    std::thread t1(read_json,infoObj);

    t1.join();

    return 0;
}
