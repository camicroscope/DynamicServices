/*
 * File:   Order.hpp
 * Author: amit
 *
 * Created on May 13, 2016, 11:51 PM
 */

#ifndef ORDER_HPP
#define ORDER_HPP

#include <string>
#include <json/json.h>
#include <json/reader.h>

enum FORMAT {JPEG, mask, format_not_found};
enum ROI {FULL, SQUARE, PCT, TILE};

class CaMicroscopeService;
class Order {
public:
    void print() const;
    bool isProcessed() const;
    void setProcessed(bool p);
    std::string getOrderId() const;
    std::string getAnnotationId() const;
    void setAnnotationId(const std::string& annId);
    std::string getCaseId() const;
    unsigned getWidth() const;
    unsigned getHeight() const;
    unsigned getX() const;
    unsigned getY() const;
    std::string getInputFormat() const;
    std::string getLocationIDPath() const;
    std::string getImagePath(const std::string& locPath) const;
    std::string getImageSource() const;
    ROI getRoiType() const;
    Order(const Json::Value& value, unsigned W, unsigned H);
    Order(const Json::Value& value, const std::string& iips, const std::string& fmat);
    Order(const Order& orig);
    virtual ~Order();
private:
    std::string id;
    std::string inHost;
    std::string inPort;
    std::string inPath;
    std::string caseId;
    std::string annotationId;
    unsigned x;
    unsigned y;
    unsigned w;
    unsigned h;
    unsigned width;
    unsigned height;
    FORMAT inFormat;
    std::string iipServer;
    std::string outHost;
    std::string outPort;
    std::string outPath;
    FORMAT outFormat;
    ROI roiType;
    std::string imageSource;
    bool processed;
};

#endif /* ORDER_HPP */
