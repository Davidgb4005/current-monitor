cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 20)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(PARTITION_TABLE_CUSTOM ${CMAKE_CURRENT_SOURCE_DIR}/partitions.csv)
set(PARTITION_TABLE_OFFSET 0x8000)
set(PARTITION_TABLE_FILENAME "partitions.csv")

project(CurrentMonitor)

spiffs_create_partition_image(spiffs ${CMAKE_SOURCE_DIR}/spiffs FLASH_IN_PROJECT)