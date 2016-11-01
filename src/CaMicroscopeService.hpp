//
//  CaMicroscopeService.hpp
//
//  Created by Amit on 1/22/16.
//  Copyright © 2016 Amit. All rights reserved.
//

#ifndef CaMicroscope_hpp
#define CaMicroscope_hpp

#include "AbstractDynamicService.hpp"
#include <stdio.h>
#include <memory>
#include <queue>
#include <mutex>
#include "Order.hpp"
#include <redox.hpp>
#include <unordered_map>
#include <future>

class CaMicroscopeService {
public:
    void init(const std::string& cfile);
    void post();
    void process();
private:
    redox::Redox publisher;
    redox::Redox statusUpdater;
    redox::Subscriber subscriber;
    redox::Subscriber annotationSubscriber;
    std::queue<std::unique_ptr<Order>> orderQ;
    std::unordered_map<std::string, std::unique_ptr<Order>> processedOrders;
    std::mutex orderQMutex;
    std::mutex processedOrderQMutex;
    std::string orderHost;
    std::string orderPort;
    std::string orderPath;
    std::string imageServer;
    std::string orderPubSubHost;
    std::string orderPubSubPort;
    std::string orderPubSubChannel;
    std::string postHost;
    std::string postPort;
    std::string postPath;
    std::string annotationPubSubHost;
    std::string annotationPubSubPort;
    std::string annotationPubSubChannel;
    std::string locHost;
    std::string locPort;
    std::string locPath;
    std::string orderKeyFile;
    std::string orderApiKey;
    std::thread tProcess;
    bool processed;
    unsigned orderPollTime;
    FORMAT format;
    std::string formatS;
    void processRedis(const std::string& jobID);
    void processOrder(std::unique_ptr<Order> ord);
    void processOrderRedis();
    std::string postToAnnotationServer(const std::string&);
    bool parseOrders(const std::string& jsonText);
    bool parseOrdersRedis(const std::string& jsonText);
    bool getJSON(const std::string& URL, std::string& jsonText);
    bool readConfigFile(const std::string& configFile);
    std::string readApiKey(const std::string& keyfile) const;
    std::string getKeyFromJson(const std::string& filename) const;
    std::string getURLFromJson(const std::string& jsonTextFile) const;
    void getImage(const std::string& url, const std::string& imName);
    static std::size_t fetchImage(void *ptr, size_t size, size_t nmemb, FILE *stream);
    std::string getFileLocation(const std::string& URL, const std::string& subID);
    std::pair<unsigned, unsigned> getWidthHeight(const std::string& URL, const std::string& subID);
    static std::size_t writer(char *data, size_t size, size_t nmemb, void* userp);
};

#endif /* CaMicroscope_hpp */
