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


// Specialize YAML::convert for ae_params_t
template<> struct convert<ae_params_t> {
    static Node encode(const ae_params_t& rhs) {
        Node node;

        node["targetBrightnessRange"] = rhs.dyn_params.targetBrightnessRange;
        node["targetBrightness"] = rhs.dyn_params.targetBrightness;
        node["threshold"] = rhs.dyn_params.threshold;
        node["enableBlc"] = rhs.dyn_params.enableBlc;  
        node["exposureTimeStepSize"] = rhs.dyn_params.exposureTimeStepSize;

        for (uint32_t i=0; i<rhs.dyn_params.numAeDynParams; i+=1) {
            node["ranges"][i]["exposureTimeRange"] = rhs.dyn_params.exposureTimeRange[i];
            node["ranges"][i]["analogGainRange"] = rhs.dyn_params.analogGainRange[i];
            node["ranges"][i]["digitalGainRange"] = rhs.dyn_params.digitalGainRange[i];
        }

        node["cur_y_from_cc_pixels"] = rhs.cur_y_from_cc_pixels;
        
        return node;
    }

    static bool decode(const Node& node, ae_params_t& rhs) {
        bool ret = true;

        if(!node.IsMap()) {
            return false;
        }

        rhs.dyn_params.targetBrightnessRange = node["targetBrightnessRange"].as<IssAeRange>();
        rhs.dyn_params.targetBrightness = node["targetBrightness"].as<uint32_t>();
        rhs.dyn_params.threshold = node["threshold"].as<uint32_t>();
        rhs.dyn_params.enableBlc = node["enableBlc"].as<uint32_t>();
        rhs.dyn_params.exposureTimeStepSize = node["exposureTimeStepSize"].as<uint32_t>();         

        rhs.dyn_params.numAeDynParams = node["ranges"].size();
        int range_index = 0;

        for (const auto& range_node : node["ranges"]) {     
            rhs.dyn_params.exposureTimeRange[range_index] = range_node["exposureTimeRange"].as<IssAeRange>();
            rhs.dyn_params.analogGainRange[range_index] = range_node["analogGainRange"].as<IssAeRange>();
            rhs.dyn_params.digitalGainRange[range_index] = range_node["digitalGainRange"].as<IssAeRange>();
            
            range_index += 1;
        }

        rhs.cur_y_from_cc_pixels = node["cur_y_from_cc_pixels"].as<uint8_t>();
        
        return ret;
    } //struct convert<ae_params_t>
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

int ae_params_get_params_from_yaml(const char *path, ae_params_t *p_params)
{
    try {
        YAML::Node config = YAML::LoadFile(path);
        *p_params = config.as<ae_params_t>();
    }
    catch (const std::runtime_error& e) {
        LOG(ERROR) << "ae_params_get_params_from_yaml " << e.what() << std::endl;
        return 0;
    }

    return 1;
}

int ae_params_get(ae_params_t *p_params)
{
    int ret = 0;
    memset(p_params, 0, sizeof(ae_params_t));

    std::string path = ae_params_get_path();
    if (path=="") {
        LOG(ERROR) << "ae_params_get() path==NULL";
        return 0;
    }

    LOG(INFO) << "path " << path;

    ret = ae_params_get_params_from_yaml(path.c_str(), p_params);
    if (ret==0) {
        memset(p_params, 0, sizeof(ae_params_t));
        return 0;
    }

    ae_params_dump(p_params);

    google::FlushLogFiles(google::INFO);

    return 1;
}

void ae_params_dump(ae_params_t *p_params)
{
    LOG(INFO) << "targetBrightnessRange.min " << p_params->dyn_params.targetBrightnessRange.min;
    LOG(INFO) << "targetBrightnessRange.max " << p_params->dyn_params.targetBrightnessRange.max;
    LOG(INFO) << "targetBrightness " << p_params->dyn_params.targetBrightness;
    LOG(INFO) << "threshold " << p_params->dyn_params.threshold;
    LOG(INFO) << "enableBlc " << p_params->dyn_params.enableBlc;
    LOG(INFO) << "exposureTimeStepSize " << p_params->dyn_params.exposureTimeStepSize;

    for (uint32_t i=0; i<p_params->dyn_params.numAeDynParams; i+=1) {
        LOG(INFO) << "range index " << i;
        LOG(INFO) << "\texposureTimeRange.min " << p_params->dyn_params.exposureTimeRange[i].min;
        LOG(INFO) << "\texposureTimeRange.max " << p_params->dyn_params.exposureTimeRange[i].max;
        LOG(INFO) << "\tanalogGainRange.min " << p_params->dyn_params.analogGainRange[i].min;
        LOG(INFO) << "\tanalogGainRange.max " << p_params->dyn_params.analogGainRange[i].max;
        LOG(INFO) << "\tdigitalGainRange.min " << p_params->dyn_params.digitalGainRange[i].min;
        LOG(INFO) << "\tdigitalGainRange.max " << p_params->dyn_params.digitalGainRange[i].max;
    }

    LOG(INFO) << "\tcur_y_from_cc_pixels " << p_params->cur_y_from_cc_pixels;
}

bool is_glog_en = false;

__attribute__((constructor))
void init_library(void) {
    if (!is_glog_en) {
        google::InitGoogleLogging("ae_params"); 
        is_glog_en = true;
    }
}
