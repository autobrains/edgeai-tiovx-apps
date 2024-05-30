cmake_minimum_required(VERSION 3.10.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/common.cmake)

set(SRC_FILES
    aewb_logger_sender.c
    aewb_logger_receiver.c
    )

build_lib(aewb_logger      # Named argument: library name
          SHARED           # Named argument: library type
          0.1.0            # Named argument: library version
          # The following all will be clubbed under ${ARGN}
          ${SRC_FILES})


set(CMAKE_BUILD_TYPE Debug)
set(TEST_SRC_FILES
    aewb_logger_test.c
    )

build_app_no_install(aewb_logger_test
          ${TEST_SRC_FILES})
