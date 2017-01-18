//
//  CaMicroscopeService.cpp
//
//  Created by Amit on 1/22/16.
//  Copyright © 2016 Amit. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <curl/curl.h>
#include <json/json.h>
#include <json/reader.h>
#include "CaMicroscopeService.hpp"
#include "AbstractDynamicService.hpp"
#include <thread>
#include <stdlib.h>
#include <sys/signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdexcept>

using namespace std;

void CaMicroscopeService::init(const string& cfile) {
    if (readConfigFile(cfile)) {
        std::mutex mtx;
        orderApiKey = readApiKey(orderKeyFile);
        cout << "\napi_key: " << orderApiKey;
        //cout << "\nPubSub: " << orderPubSubHost << ":" << stoi(orderPubSubPort) << endl;
        if (!publisher.connect(orderPubSubHost,stoi(orderPubSubPort)) || !subscriber.connect(orderPubSubHost,stoi(orderPubSubPort))) {
        //if (!publisher.connect() || !subscriber.connect()) {
            cout << "\nSomething went wrong with redis! Exiting";
            exit(1);
        }

        if (!annotationSubscriber.connect(annotationPubSubHost, stoi(annotationPubSubPort))) {
            cout << "\nSomething went wrong with annotation redis! Exiting";
            exit(1);
        }

        auto got_message = [&](const string& topic, const string & msg) {
            cout << "\n" << topic << ": " << msg << endl;
            Json::Value values;
            Json::Reader reader;
            bool parsed = reader.parse(msg, values);
            if (!parsed) {
                std::cout << "\nFailed to parse " << msg << " " << reader.getFormattedErrorMessages();
            }
            else {
                string jobID =  values.get("id", "default value").asString();
                cout << "\nProcessing job: " << jobID;
                processRedis(jobID);
            }
        };
        auto got_annotation_message = [&](const string& topic, const string & msg) {
            cout << "\n" << topic << ": " << msg << endl;
            Json::Value values;
            Json::Reader reader;
            bool parsed = reader.parse(msg, values);
            if (!parsed) {
                std::cout << "\nFailed to parse " << msg << " " << reader.getFormattedErrorMessages();
            }
            else {
                string event = values.get("event", "default value").asString();
                cout << "\nEvent: " << event;
                if (event == "complete") {
                    string jobID =  values.get("id", "default value").asString();
                    cout << "\nInside complete block; Looking for id: " << jobID <<"\n";
                    mtx.lock();
                    auto pJob = processedOrders.find(jobID);
                    if (pJob != processedOrders.end()) {
                        unique_ptr<Order> order = move(pJob->second);
                        cout << "\norder id fetch: " << order->getOrderId();
                        string rcmd = "q:job:" + order->getOrderId();
                        cout << "\nJob: " << order->getOrderId() << " competed";
                        try {
                            redox::Command<int>& c = publisher.commandSync<int>({"HSET", rcmd, "state", "\"complete\""});
                            if(!c.ok()) {
                                 cerr << "Error while communicating with redis" << c.status() << endl;
                            }
                            c.free();
                        } catch (runtime_error& e) {
                            cerr << "send_message: Exception in redox: " << e.what() << endl;
                        }
                    }
                    mtx.unlock();
                }
            }
        };

        auto subscribed = [](const string & topic) {
            cout << "\n> Subscribed to " << topic << endl;
        };

        auto unsubscribed = [](const string & topic) {
            cout << "\n> Unsubscribed from " << topic << endl;
        };

        cout << "\nDone setting up Redis connections" << endl;
        subscriber.subscribe("q:events", got_message, subscribed, unsubscribed);
        annotationSubscriber.subscribe("q:events", got_annotation_message, subscribed, unsubscribed);
        tProcess = std::thread(&CaMicroscopeService::processOrderRedis, this);
        //std::thread tProcess(&CaMicroscope::processOrderRedis, this);
        while(1) {
            this_thread::sleep_for(chrono::seconds(2));
        }
    } else {
        cout << "\nExiting";
        exit(1);
    }
}

std::string CaMicroscopeService::readApiKey(const string& keyfile) const {
    ifstream input(keyfile);
    Json::Value values;
    Json::Reader reader;
    bool parsed = reader.parse(input, values);
    if (!parsed) {
        std::cout << "Failed to parse " << keyfile << " " << reader.getFormattedErrorMessages();
        return NULL;
    }
    return values.get("api_key", "default value").asString();
}

void CaMicroscopeService::processOrderRedis() {
    this_thread::sleep_for(chrono::seconds(5));
    while (1) {
        while (!orderQ.empty()) {
            cout << "\nqsize:" << orderQ.size();
            orderQMutex.lock();
            unique_ptr<Order> order = move(orderQ.front());
            orderQ.pop();
            orderQMutex.unlock();
            //string locP = order->getLocationIDPath() + "&api_key=" + orderApiKey;
            auto fut = async(launch::async, &CaMicroscopeService::processOrder, this, move(order));
            //processOrder(move(order));
        }
        this_thread::sleep_for(chrono::seconds(2));
    }
}

void CaMicroscopeService::processOrder(unique_ptr<Order> order) {
    string rcmd = "q:job:" + order->getOrderId();
    try {
        redox::Command<int>& c = publisher.commandSync<int>({"HSET", rcmd, "state", "start"});
        if(!c.ok()) {
            cerr << "Error while communicating with redis" << c.status() << endl;
        }
        c.free();
    } catch (runtime_error& e) {
        cerr << "send_message: Exception in redox: " << e.what() << endl;
    }
    string locP = locHost + ":" + locPort + locPath + "?TCGAId=" + order->getCaseId() + "&api_key=" + orderApiKey;
    string jsTxt;
    cout << "\nqsize:" << orderQ.size();
    cout << "\norder_processed:";
    order->print();
    cout << "\nlocationPath:" << locP;
    getJSON(locP, jsTxt);
    cout << "\nloc json: " << jsTxt;
    Json::Value values;
    Json::Reader reader;
    bool parsed = reader.parse(jsTxt, values);
    if (!parsed) {
        std::cout << "\n\nFailed to parse jsonText " << reader.getFormattedErrorMessages();
        //return false;
    }
    string loc = values[0].get("file-location", "").asString();
    cout << "\nfile-location:" << loc;

    string cmd;
    string imName = order->getOrderId() + "." + order->getInputFormat();

    cout << "\n\nImage source: " << order->getImageSource();

    if (order->getImageSource() == "image_server") {
      string imURL = order->getImagePath(loc);
      cout << "\n\nImageURL: " << imURL;
      string outDir = "images/" + order->getOrderId();
      cmd = "mkdir " + outDir;
      int ret = system(cmd.c_str());
      getImage(imURL, (outDir + "/" + imName));
      //cmd = "/bin/sh algo1.sh images images img " + imName + " " + order->getOrderId();
      cmd = "mainSegmentFeatures -i /images/" + imName 
		+ " -z " + outDir + "/output.zip -o " + outDir +
		+ " -t img -c " + order->getCaseId() + " -p " + order->getSubjectId() 
		+ " -a " + order->getAnalysisId() + " -s " + to_string(order->getX()) + "," 
                + to_string(order->getY())  
		+ " -b " + to_string(order->getW()) + "," + to_string(order->getH())  
		+ " -d " + to_string(order->getW()) + "," + to_string(order->getH());
      for (auto i=0;i<order->getNumAuxArgs();i++) {
	cmd = cmd + " -" + order->getAuxArg(i).first
		+ " " + order->getAuxArg(i).second;
      }
    }
    else {
	cout << "\nOnly source:image_server supported currently" <<endl; 
/*        size_t found = loc.find_last_of("//");
        cout << " path: " << loc.substr(0,found) << '\n';
        cout << " file: " << loc.substr(found+1) << '\n';
        string ipath = loc.substr(0,found);
        string ifile = loc.substr(found+1);
      
        switch (order->getRoiType()) {
         case FULL:  cmd = "/bin/sh algo1.sh " + ipath + " images wsi " + ifile + " " + order->getOrderId();
                    break;

         case TILE:  cmd = "/bin/sh algo1.sh " + ipath + " images onetile " + ifile + " " + order->getOrderId() 
			+ " " + to_string(order->getX())
			+ " " + to_string(order->getY())
			+ " " + to_string(order->getW())
			+ " " + to_string(order->getH());
                    break;
         default:  cout << "\n\nROI type:" << order->getRoiType() << " isn't supported. Skipping." << endl;
                  return;
      }*/
    }


    cout << "\nProcess command: " << cmd;
    //cout << "\nProcess command: " << "/bin/sh algo1.sh " << imName  << " " << order->getOrderId();
    int ret = system(cmd.c_str());

    string annotationsServerPath = postHost + ":" + postPort + postPath;
    string postCmd = "curl -X POST -F " + order->getCaseId() + "=xyz -F zip=@"
		+ "/tmp/" + order->getOrderId() + "/output.zip " 
                + annotationsServerPath + "/submitZipOrder";
    /*string postCmd = "curl -v " + annotationsServerPath + " -F mask=@"
            + "$(echo $(ls images/" + order->getOrderId() +
            +"_mpp_*-seg.png))" + " -F case_id=" + order->getCaseId()
            + " -F execution_id=ganesh:algo1 -F width=" + to_string(order->getWidth())
            + " -F " + "height=" + to_string(order->getHeight())
            + " -F " + "x=" + to_string(order->getX())
            + " -F " + "y=" + to_string(order->getY());
    */
    cout << "\npost: " << postCmd <<endl;
    //system(postCmd.c_str());
    string annId = postToAnnotationServer(postCmd);
    order->setAnnotationId(annId);
    processedOrderQMutex.lock();
    processedOrders.insert(std::make_pair(order->getAnnotationId(),move(order)));
    processedOrderQMutex.unlock();
    try {
        if(!ret && annId!="") {
            redox::Command<int>& c = publisher.commandSync<int>({"HSET", rcmd, "state", "complete"});
            if(!c.ok()) {
                cerr << "Error while communicating with redis" << c.status() << endl;
            }
            c.free();
        } else {
            redox::Command<int>& c = publisher.commandSync<int>({"HSET", rcmd, "state", "failed"});
            if(!c.ok()) {
                cerr << "Error while communicating with redis" << c.status() << endl;
            }
            c.free();
        }
    } catch (runtime_error& e) {
        cerr << "send_message: Exception in redox: " << e.what() << endl;
    }
}

string CaMicroscopeService::postToAnnotationServer(const string& cmd) {
    string jobID="";
    char buffer[128];
    std::string result = "";
    cout << "\ncommand:" << cmd.c_str();
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    cout << "\nResponse from annotation server:\n" << result << endl;
    Json::Value values;
    Json::Reader reader;
    bool parsed = reader.parse(result, values);
    if (!parsed) {
        std::cout << "\nFailed to parse " << " " << reader.getFormattedErrorMessages();
    }
    else {
        jobID =  values["Job"].get("id", "default value").asString();
    }
    return jobID;
}

void CaMicroscopeService::processRedis(const string& jobID) {
    cout << "\n\nProcessing: "<< jobID <<"\n";
    string jsonText;
    string url = orderHost + ":" + orderPort + orderPath + jobID;
    cout << url << "\n";
    if (getJSON(url, jsonText)) {
        cout << "\n\njsonText: " << jsonText;
        parseOrdersRedis(jsonText);
    }
}

bool CaMicroscopeService::parseOrdersRedis(const string& jsonText) {
    Json::Value values;
    Json::Reader reader;
    bool parsed = reader.parse(jsonText, values);
    if (!parsed) {
        std::cout << "\n\nFailed to parse jsonText " << reader.getFormattedErrorMessages();
        return false;
    }
    unique_ptr<Order> order(new Order(values, imageServer, formatS));
    order->print();
    orderQMutex.lock();
    orderQ.push(move(order));
    orderQMutex.unlock();
    return true;
}


void CaMicroscopeService::post() {
}

bool CaMicroscopeService::readConfigFile(const string& configFile) {
    ifstream input(configFile);
    Json::Value values;
    Json::Reader reader;
    bool parsed = reader.parse(input, values);
    if (!parsed) {
        std::cout << "Failed to parse " << configFile << " " << reader.getFormattedErrorMessages();
        return false;
    }
    cout<<"Inputfile: "<<configFile<<endl;
    orderPubSubHost = values["orders"]["redis"].get("host", "default value").asString();
    orderPubSubPort = values["orders"]["redis"].get("port", "default value").asString();
    orderPubSubChannel = values["orders"]["redis"].get("channel", "default value").asString();
    orderHost = values["orders"]["kue"].get("host", "default value").asString();
    orderPort = values["orders"]["kue"].get("port", "default value").asString();
    orderPath = values["orders"]["kue"].get("path", "default value").asString();
    locHost = values["images"]["location"].get("host", "default value").asString();
    locPort = values["images"]["location"].get("port", "default value").asString();
    locPath = values["images"]["location"].get("path", "default value").asString();
    imageServer = values["images"]["imageServer"].get("url", "default value").asString();
    formatS = values["images"]["imageServer"].get("format", "default value").asString();
    postHost = values["annotations"]["server"].get("host", "default value").asString();
    postPort = values["annotations"]["server"].get("port", "default value").asString();
    postPath = values["annotations"]["server"].get("path", "default value").asString();
    annotationPubSubHost = values["annotations"]["redis"].get("host", "default value").asString();
    annotationPubSubPort = values["annotations"]["redis"].get("port", "default value").asString();
    annotationPubSubChannel = values["annotations"]["redis"].get("channel", "default value").asString();
    orderKeyFile = values["images"].get("api_key_file", "default value").asString();
    cout << "\nDone reading Config file" << endl;
    return true;
}

size_t CaMicroscopeService::fetchImage(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}

void CaMicroscopeService::getImage(const string& url, const string& imName) {
    //getOSInfo(uid);
    CURL *curl;
    FILE *fp;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
        fp = fopen(imName.c_str(), "wb");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fetchImage);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
    }
}

string CaMicroscopeService::getKeyFromJson(const string& jsonTextFile) const {
    string key;
    ifstream input(jsonTextFile.c_str());
    Json::Value values;
    Json::Reader reader;
    bool parsed = reader.parse(input, values);
    if (!parsed) {
        std::cout << "Failed to parse" << reader.getFormattedErrorMessages();
        return NULL;
    }
    key = values.get("api_key", "").asString();
    //cout << key;
    return key;
}

bool CaMicroscopeService::getJSON(const string& URL, string& jsonText) {
    CURL *curl;
    CURLcode res;
    string readBuffer;
    //ostringstream readBuffer;
    struct curl_slist *headers = NULL;
    bool retFlag = false;
    curl_slist_append(headers, "Accept: application/json");
    curl_slist_append(headers, "Content-Type: application/json");
    curl_slist_append(headers, "charsets: utf-8");
    curl = curl_easy_init();
    cout << "\n" << URL.c_str();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        //cout << "\n\n\njsontext: " << oss.str();
        if (CURLE_OK == res) {
            char *ct;
            res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
            if ((CURLE_OK == res) && ct) {
                retFlag = true;
                //jsonText = readBuffer.str();
                jsonText = readBuffer;
            }
        }
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    return retFlag;
}

size_t CaMicroscopeService::writer(char *data, size_t size, size_t nmemb, void* userp) {
    //(*(ostringstream *)userp) << data;
    (*(string *) userp).append(data, size * nmemb);
    return size * nmemb;
}
