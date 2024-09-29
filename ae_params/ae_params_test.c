#ifdef NDEBUG
#error "make sure cmake CMAKE_BUILD_TYPE is set to Debug, otherwise assert will be nop"
#endif

#include "ae_params.h"


void good_single_range() {
    ae_params_t params;
    int ret = ae_params_get_params_from_yaml("./ae_params/test_yaml/good_single_range.yaml", &params);   
    assert(ret==1);

    ae_params_dump(&params);
    
    assert(params.dyn_params.targetBrightnessRange.min==1);
    assert(params.dyn_params.targetBrightnessRange.max==2);
    assert(params.dyn_params.targetBrightness==3);
    assert(params.dyn_params.threshold==4);
    assert(params.dyn_params.enableBlc==5);
    assert(params.dyn_params.exposureTimeStepSize==6);
    
    int i=0;
    assert(params.dyn_params.exposureTimeRange[i].min==7);
    assert(params.dyn_params.exposureTimeRange[i].max==8);
    assert(params.dyn_params.analogGainRange[i].min==9);
    assert(params.dyn_params.analogGainRange[i].max==10);
    assert(params.dyn_params.digitalGainRange[i].min==11);
    assert(params.dyn_params.digitalGainRange[i].max==12);

    assert(params.dyn_params.numAeDynParams==1);
}

void good_multi_range() {
    ae_params_t params;
    int ret = ae_params_get_params_from_yaml("./ae_params/test_yaml/good_multi_range.yaml", &params);

    assert(ret==1);
    
    assert(params.dyn_params.targetBrightnessRange.min==1);
    assert(params.dyn_params.targetBrightnessRange.max==2);
    assert(params.dyn_params.targetBrightness==3);
    assert(params.dyn_params.threshold==4);
    assert(params.dyn_params.enableBlc==5);
    assert(params.dyn_params.exposureTimeStepSize==6);

    int i=0;
    assert(params.dyn_params.exposureTimeRange[i].min==7);
    assert(params.dyn_params.exposureTimeRange[i].max==8);
    assert(params.dyn_params.analogGainRange[i].min==9);
    assert(params.dyn_params.analogGainRange[i].max==10);
    assert(params.dyn_params.digitalGainRange[i].min==11);
    assert(params.dyn_params.digitalGainRange[i].max==12);

    i=1;
    assert(params.dyn_params.exposureTimeRange[i].min==20);
    assert(params.dyn_params.exposureTimeRange[i].max==21);
    assert(params.dyn_params.analogGainRange[i].min==22);
    assert(params.dyn_params.analogGainRange[i].max==23);
    assert(params.dyn_params.digitalGainRange[i].min==24);
    assert(params.dyn_params.digitalGainRange[i].max==25);    

    assert(params.dyn_params.numAeDynParams==2);
}

void missing_root_attr1() {
    ae_params_t params;
    int ret = ae_params_get_params_from_yaml("./ae_params/test_yaml//missing_root_attr1.yaml", &params);
    assert(ret==0);
    assert(params.dyn_params.targetBrightnessRange.min==0);
}

void missing_root_attr2() {
    ae_params_t params;
    int ret = ae_params_get_params_from_yaml("./ae_params/test_yaml//missing_root_attr2.yaml", &params);
    assert(ret==0);
    assert(params.dyn_params.targetBrightnessRange.min==0);
}

void missing_range_attr() {
    ae_params_t params;
    int ret = ae_params_get_params_from_yaml("./ae_params/test_yaml//missing_range_attr.yaml", &params);
    assert(ret==0);
    assert(params.dyn_params.targetBrightnessRange.min==0);
}

void file_doesnt_exist() {
    ae_params_t params;
    int ret = ae_params_get_params_from_yaml("./ae_params/test_yaml//non_existing.yaml", &params);    
    assert(ret==0);
    assert(params.dyn_params.targetBrightnessRange.min==0);
}

void illegal_yaml() {
    ae_params_t params;
    int ret = ae_params_get_params_from_yaml("./ae_params/test_yaml//illegal_yaml.yaml", &params);    
    assert(ret==0);
    assert(params.dyn_params.targetBrightnessRange.min==0);
}

void wrong_key_type() {
    ae_params_t params;
    int ret = ae_params_get_params_from_yaml("./ae_params/test_yaml//wrong_key_type.yaml", &params);    
    assert(ret==0);
    assert(params.dyn_params.targetBrightnessRange.min==0);
}

int main() {
    printf("\ngood_single_range\n");
    good_single_range();

    printf("\ngood_multi_range\n");
    good_multi_range();

    printf("\nmissing_root_attr1\n");
    missing_root_attr1();

    printf("\nmissing_root_attr2\n");
    missing_root_attr2();

    printf("\nmissing_range_attr\n");
    missing_range_attr();

    printf("\nfile_doesnt_exist\n");
    file_doesnt_exist();

    printf("\nillegal_yaml\n");
    illegal_yaml();

    printf("\nwrong_key_type\n");
    wrong_key_type();
}
