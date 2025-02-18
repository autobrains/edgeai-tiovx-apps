cmake_minimum_required(VERSION 3.10.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/common.cmake)

set(SRC_FILES
    aewb_logger_sender.c
    aewb_logger_receiver.c
    aewb_debug_utils.c
    ../ae_params/ae_params.cpp # we add this here to avoid passing an extra lib to Cartica-Renesas
    ../aewb_debug_mode/periodic_fixed_exposure_gain_switch.c # we add this here to avoid passing an extra lib to Cartica-Renesas
    )


build_lib(aewb_logger      # Named argument: library name
          SHARED           # Named argument: library type
          0.1.0            # Named argument: library version
          # The following all will be clubbed under ${ARGN}          
          ${SRC_FILES})

set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})

set(AE_PARAMs_HEADERS ../ae_params/ae_params.h)
install(FILES ${AE_PARAMs_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/ae_params)

FILE(GLOB AEWB_LOG_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
install(FILES ${AEWB_LOG_HDRS} DESTINATION ${INCLUDE_INSTALL_DIR}/aewb_logger)

set(CMAKE_BUILD_TYPE Debug)

build_app_no_install(aewb_logger_test
          aewb_logger_test.c)

build_app_no_install(ae_params_test
          ../ae_params/ae_params_test.c)          
