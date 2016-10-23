//
//  main.cpp
//  CaMicroscopeSDK
//
//  Created by Amit on 1/22/16.
//  Copyright © 2016 Amit. All rights reserved.
//

#include <iostream>
#include <string>
#include <fstream>
#include <curl/curl.h>
#include <json/json.h>
#include <json/reader.h>
#include "CaMicroscopeService.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
    // insert code here...
    if (argc != 2) {
        cout << "\nMissing argument: config.json.\n";
        exit(1);
    }
    std::string cfile(argv[1]);
    CaMicroscopeService cma;
    cma.init(cfile);
    //cma.process();
    return 0;
}





   

