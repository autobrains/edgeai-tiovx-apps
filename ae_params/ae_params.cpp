#include "ae_params.h"
#include <filesystem>
#include <iostream>
#include <yaml-cpp/yaml.h>
#include <stdexcept>
#include <glog/logging.h>


namespace fs = std::filesystem;

namespace YAML {
    
// Specialize YAML::convert for IssAeRange
template<> struct convert<IssAeRange> {
    static Node encode(const IssAeRange& rhs) {
        Node node;
        node["min"] = rhs.min;
        node["max"] = rhs.max;
        return node;
    }


    static bool decode(const Node& node, IssAeRange& rhs) {
        if(!node.IsMap()) {
            return false;
        }

        if (!node["min"]) {
            return false;
        }

        rhs.min = node["min"].as<int>();

        if (!node["max"]) {
            return false;
        }
                
        rhs.max = node["max"].as<int>();

        return true;
    }
};


// Specialize YAML::convert for IssAeDynamicParams
template<> struct convert<IssAeDynamicParams> {
    static Node encode(const IssAeDynamicParams& rhs) {
        Node node;

        node["targetBrightnessRange"] = rhs.targetBrightnessRange;
        node["targetBrightness"] = rhs.targetBrightness;
        node["threshold"] = rhs.threshold;
        node["enableBlc"] = rhs.enableBlc;  
        node["exposureTimeStepSize"] = rhs.exposureTimeStepSize;

        for (uint32_t i=0; i<rhs.numAeDynParams; i+=1) {
            node["ranges"][i]["exposureTimeRange"] = rhs.exposureTimeRange[i];
            node["ranges"][i]["analogGainRange"] = rhs.analogGainRange[i];
            node["ranges"][i]["digitalGainRange"] = rhs.digitalGainRange[i];
        }
        
        return node;
    }

    static bool decode(const Node& node, IssAeDynamicParams& rhs) {
        bool ret = true;

        if(!node.IsMap()) {
            return false;
        }

        rhs.targetBrightnessRange = node["targetBrightnessRange"].as<IssAeRange>();
        rhs.targetBrightness = node["targetBrightness"].as<uint32_t>();
        rhs.threshold = node["threshold"].as<uint32_t>();
        rhs.enableBlc = node["enableBlc"].as<uint32_t>();
        rhs.exposureTimeStepSize = node["exposureTimeStepSize"].as<uint32_t>();         

        rhs.numAeDynParams = node["ranges"].size();
        int range_index = 0;

        for (const auto& range_node : node["ranges"]) {     
            rhs.exposureTimeRange[range_index] = range_node["exposureTimeRange"].as<IssAeRange>();
            rhs.analogGainRange[range_index] = range_node["analogGainRange"].as<IssAeRange>();
            rhs.digitalGainRange[range_index] = range_node["digitalGainRange"].as<IssAeRange>();
            
            range_index += 1;
        }
        
        return ret;
    } //struct convert<IssAeDynamicParams>
};

} //namespace YAML

std::string ae_params_get_path() {
    const char *path1 = std::getenv("AE_PARAMS_PATH");
    std::string path2 = "./ae_params.yaml";
    std::string path3 = "/home/root/app/imx728/dcc_3856x2176/ae_params.yaml";

    if (path1!=NULL and fs::exists(path1)) {
        return path1;
    }

    if (fs::exists(path2)) {
        return path2;
    }

    if (fs::exists(path3)) {
        return path3;
    }

    return "";
}

int ae_params_get_params_from_yaml(const char *path, IssAeDynamicParams *p_params)
{
    try {
        YAML::Node config = YAML::LoadFile(path);
        *p_params = config.as<IssAeDynamicParams>();
    }
    catch (const std::runtime_error& e) {
        LOG(ERROR) << "ae_params_get_params_from_yaml " << e.what() << std::endl;
        return 0;
    }

    return 1;
}

int ae_params_get(IssAeDynamicParams *p_params)
{
    int ret = 0;
    memset(p_params, 0, sizeof(IssAeDynamicParams));

    std::string path = ae_params_get_path();
    if (path=="") {
        LOG(ERROR) << "ae_params_get() path==NULL";
        return 0;
    }

    LOG(INFO) << "path " << path;

    ret = ae_params_get_params_from_yaml(path.c_str(), p_params);
    if (ret==0) {
        memset(p_params, 0, sizeof(IssAeDynamicParams));
        return 0;
    }

    ae_params_dump(p_params);

    google::FlushLogFiles(google::INFO);

    return 1;
}

void ae_params_dump(IssAeDynamicParams *p_ae_dynPrms)
{
    LOG(INFO) << "targetBrightnessRange.min " << p_ae_dynPrms->targetBrightnessRange.min;
    LOG(INFO) << "targetBrightnessRange.max " << p_ae_dynPrms->targetBrightnessRange.max;
    LOG(INFO) << "targetBrightness " << p_ae_dynPrms->targetBrightness;
    LOG(INFO) << "threshold " << p_ae_dynPrms->threshold;
    LOG(INFO) << "enableBlc " << p_ae_dynPrms->enableBlc;
    LOG(INFO) << "exposureTimeStepSize " << p_ae_dynPrms->exposureTimeStepSize;

    for (uint32_t i=0; i<p_ae_dynPrms->numAeDynParams; i+=1) {
        LOG(INFO) << "range index " << i;
        LOG(INFO) << "\texposureTimeRange.min " << p_ae_dynPrms->exposureTimeRange[i].min;
        LOG(INFO) << "\texposureTimeRange.max " << p_ae_dynPrms->exposureTimeRange[i].max;
        LOG(INFO) << "\tanalogGainRange.min " << p_ae_dynPrms->analogGainRange[i].min;
        LOG(INFO) << "\tanalogGainRange.max " << p_ae_dynPrms->analogGainRange[i].max;
        LOG(INFO) << "\tdigitalGainRange.min " << p_ae_dynPrms->digitalGainRange[i].min;
        LOG(INFO) << "\tdigitalGainRange.max " << p_ae_dynPrms->digitalGainRange[i].max;
    }
}

bool is_glog_en = false;

__attribute__((constructor))
void init_library(void) {
    if (!is_glog_en) {
        google::InitGoogleLogging("ae_params"); 
        is_glog_en = true;
    }
}
