//
//  TestCaMicroscopeService.cpp
//
//  Created by Amit on 6/11/16.
//  Copyright © 2016 Amit. All rights reserved.
//

#include <iostream>
#include <string>
#include <fstream>
#include <curl/curl.h>
#include <json/json.h>
#include <json/reader.h>
#include "CaMicroscopeService.hpp"
#include "Order.hpp"

using namespace std;

void testOrderJson(const string& jsonTextFile) {
    string key;
    ifstream input(jsonTextFile.c_str());
    Json::Value values;
    Json::Reader reader;
    bool parsed = reader.parse(input, values);
    if (!parsed) {
        std::cout << "Failed to parse " << jsonTextFile << "\n" << reader.getFormattedErrorMessages();
    }
    Order ord(values,"testserver");
    ord.print();
}
int main() {
    testOrderJson("order.json");
    return 0;
}





   

