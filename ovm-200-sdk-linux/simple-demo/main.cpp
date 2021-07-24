#include "ovs_sdk.h"

#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{

    cout<<"main"<<endl;
    
    string devid;
    cout<<OVS::InitSDK(devid)<<endl;

    return 0;
}
