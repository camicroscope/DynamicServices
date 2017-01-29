/*
 * File:   Order.cpp
 * Author: amit
 *
 * Created on May 13, 2016, 11:51 PM
 */

#include <iostream>
#include <locale>
#include "Order.hpp"

using namespace std;


FORMAT stringToEnumFormat(const string& s) {
    string S = s;
    for (auto & c: S) c = toupper(c);
    if (S == "JPEG" || S == "JPG") {
        return JPEG;
    }
    else if (S == "MASK") {
        return mask;
    }
    return format_not_found;
}

string enumToStringFormat(const FORMAT& f) {
    if (f == JPEG) {
        return "JPEG";
    }
    else if (f == mask) {
        return "mask";
    }
    return "format_not_found";
}

void Order::print() const {
    //cout << "\n\nOrder: " << id;
    //cout << "\n" << inHost;
    //cout << "\n" << inPort;
    //cout << "\n" << inPath;
    cout << "\n" << caseId;
    cout << "\n" << x;
    cout << "\n" << y;
    cout << "\n" << w;
    cout << "\n" << h;
    cout << "\n" << enumToStringFormat(inFormat);
    cout << "\n" << iipServer;
    cout << "\n" << imageSource;
    cout << "\nArgs:";
    for(auto kv:auxArgs){
        cout << " -" << kv.first << " " << kv.second;
    }
    //cout << "\n" << outHost;
    //cout << "\n" << outPort;
    //cout << "\n" << outPath;
    //cout << "\n" << outFormat;
    cout << endl;
}

Order::Order(const Json::Value& value, unsigned W, unsigned H) {
    width = W;
    height = H;
    id = value["_id"].get("$oid", "").asString();
    inHost = value["input"].get("host", "").asString();
    inPort = value["input"].get("port", "").asString();
    inPath = value["input"].get("path", "").asString();
    caseId = value["input"].get("case_id", "").asString();
    imageSource = value["data"]["order"]["image"].get("source","image_server").asString();//.tolower();
    string fullImage = value["data"]["order"]["roi"].get("full", "false").asString();//.tolower();
    string squareImage = value["data"]["order"]["roi"].get("full", "false").asString();//.tolower();
    string pctImage = value["data"]["order"]["roi"].get("full", "false").asString();//.tolower();
    if (fullImage == "true")
        roiType = ROI::FULL;
    else if (squareImage == "true")
        roiType = ROI::SQUARE;
    else if (pctImage == "true")
        roiType = ROI::PCT;
    else roiType = ROI::TILE;
    x = stoi(value["input"].get("x", "0").asString());
    y = stoi(value["input"].get("y", "0").asString());
    w = stoi(value["input"].get("w", "0").asString());
    h = stoi(value["input"].get("h", "0").asString());
    inFormat = stringToEnumFormat(value["input"].get("format", "format_not_found").asString());
    iipServer = value["input"].get("iipServer", "").asString();
    outFormat = stringToEnumFormat(value["output"].get("format", "format_not_found").asString());
    outHost = value["output"].get("host", "").asString();
    outPort = value["output"].get("port", "").asString();
    outPath = value["output"].get("path", "").asString();
    processed = false;
}

Order::Order(const Json::Value& value, const string& iips, const string& fmat) {
    //inHost = host;
    //inPort = port;
    //inPath = path;
    iipServer = iips;
    inFormat = stringToEnumFormat(fmat);
    id = value.get("id", "").asString();
    analysisId = value["data"].get("execution_id", "").asString();
    auto params = value["data"]["order"]["parameters"];
    for(Json::Value::iterator it = params.begin(); it !=params.end(); ++it) {
        Json::Value ky = it.key();
        Json::Value val = (*it);
        auxArgs.push_back(make_pair<string,string>(ky.asString(), val.asString()));
    }
    width = stoi(value["data"]["order"]["image"].get("width", "0").asString());
    height = stoi(value["data"]["order"]["image"].get("height", "0").asString());
    caseId = value["data"]["order"]["image"].get("case_id", "").asString();
    subjectId = value["data"]["order"]["image"].get("subject_id", "").asString();
    imageSource = value["data"]["order"]["image"].get("source","image_server").asString();//.tolower();
    string fullImage = value["data"]["order"]["roi"].get("full", "false").asString();//.tolower();
    string squareImage = value["data"]["order"]["roi"].get("full", "false").asString();//.tolower();
    string pctImage = value["data"]["order"]["roi"].get("full", "false").asString();//.tolower();
    if (fullImage == "true")
        roiType = ROI::FULL;
    else if (squareImage == "true")
        roiType = ROI::SQUARE;
    else if (pctImage == "true")
        roiType = ROI::PCT;
    else roiType = ROI::TILE;
    x = stoi(value["data"]["order"]["roi"].get("x", "0").asString());
    y = stoi(value["data"]["order"]["roi"].get("y", "0").asString());
    w = stoi(value["data"]["order"]["roi"].get("w", "0").asString());
    h = stoi(value["data"]["order"]["roi"].get("h", "0").asString());

	pr = value["data"]["order"].get("pr", "1").asString();
	pw = value["data"]["order"].get("pw", "1").asString();
	pl = value["data"]["order"].get("pl", "1").asString();
	pu = value["data"]["order"].get("pu", "1").asString();
	pk = value["data"]["order"].get("pk", "1").asString();
	pj = value["data"]["order"].get("pj", "N").asString();
	pm = value["data"]["order"].get("pm", "0.25").asString();
    //inFormat = stringToEnumFormat(value["input"].get("format", "format_not_found").asString());
    //outFormat = stringToEnumFormat(value["output"].get("format", "format_not_found").asString());
    processed = false;
}

string Order::getParamR() const{
	return pr;
}
string Order::getParamW() const{
	return pw;
}
string Order::getParamL() const{
	return pl;
}
string Order::getParamU() const{
	return pu;
}
string Order::getParamK() const{
	return pk;
}
string Order::getParamJ() const{
	return pj;
}
string Order::getParamM() const{
	return pm;
}

string Order::getLocationIDPath() const {
    return (inHost + ":" + inPort + inPath +"?TCGAId=" + caseId);
}

string Order::getImagePath(const string& locPath) const {
    return (iipServer + locPath + "/" + to_string(x) + ","
            + to_string(y) + "," + to_string(w) + "," + to_string(h) + "/full/0/default.jpg");
}

string Order::getOrderId() const {
    return id;
}

string Order::getAnnotationId() const {
    return annotationId;
}

void Order::setAnnotationId(const string& annId) {
    annotationId = annId;
}

string Order::getInputFormat() const {
    return enumToStringFormat(inFormat);
}

string Order::getCaseId() const {
    return caseId;
}

string Order::getSubjectId() const {
    return subjectId;
}

string Order::getAnalysisId() const {
    return analysisId;
}

string Order::getImageSource() const {
    return imageSource;
}

ROI Order::getRoiType() const {
    return roiType;
}

unsigned Order::getWidth() const {
    return width;
}

unsigned Order::getHeight() const {
    return height;
}

unsigned Order::getW() const {
    return w;
}

unsigned Order::getH() const {
    return h;
}

unsigned Order::getX() const {
    return x;
}

unsigned Order::getY() const {
    return y;
}

unsigned Order::getNumAuxArgs() const {
    return auxArgs.size();
}

pair<string,string> Order::getAuxArg(unsigned i) const {
    return auxArgs[i];
}

bool Order::isProcessed() const {
    return processed;
}

void Order::setProcessed(bool p) {
    processed = p;
}

Order::Order(const Order& orig) {
}

Order::~Order() {
}
