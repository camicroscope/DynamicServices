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
    std::string getSubjectId() const;
    std::string getAnalysisId() const;
    unsigned getWidth() const;
    unsigned getHeight() const;
    unsigned getX() const;
    unsigned getY() const;
    unsigned getW() const;
    unsigned getH() const;
    std::string getInputFormat() const;
    std::string getLocationIDPath() const;
    std::string getImagePath(const std::string& locPath) const;
    std::string getImageSource() const;
    ROI getRoiType() const;
    unsigned getNumAuxArgs() const;
    std::pair<std::string,std::string> getAuxArg(unsigned) const;
    Order(const Json::Value& value, unsigned W, unsigned H);
    Order(const Json::Value& value, const std::string& iips, const std::string& fmat);
    Order(const Order& orig);
	std::string getParamR() const;
	std::string getParamW() const;
	std::string getParamL() const;
	std::string getParamU() const;
	std::string getParamK() const;
	std::string getParamJ() const;
	std::string getParamM() const;
    virtual ~Order();
private:
    std::string id;
    std::string inHost;
    std::string inPort;
    std::string inPath;
    std::string caseId;
    std::string subjectId;
    std::string analysisId;
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
    std::vector<std::pair<std::string,std::string>> auxArgs;	
	std::string pr;
	std::string pw;
	std::string pl;
	std::string pu;
	std::string pk;		
	std::string pj;
	std::string pm;
};

#endif /* ORDER_HPP */
