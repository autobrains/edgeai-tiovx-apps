cmake_minimum_required(VERSION 3.10.0)

project(edgeai-tiovx-apps)

add_subdirectory(aewb_logger)
# ae_params not added here, it's built with aewb_logger
# aewb_debug_mode not added here, it's built with aewb_logger
add_subdirectory(modules)
add_subdirectory(tests)
add_subdirectory(apps)
add_subdirectory(docs)
