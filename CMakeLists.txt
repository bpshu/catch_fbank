cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(catch_fbank VERSION 0.1)

include_directories(
${CMAKE_CURRENT_SOURCE_DIR}/src
)
set(CMAKE_VERBOSE_MAKEFILE on)

include(FetchContent)
include(ExternalProject)
set(FETCHCONTENT_QUIET off)
get_filename_component(fc_base "fc_base" REALPATH BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(FETCHCONTENT_BASE_DIR ${fc_base})

if(NOT MSVC)
  # Keep the same with openfst, -fPIC or -fpic
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -fPIC -g")
endif()

# third_party: gflags
FetchContent_Declare(gflags
  URL      http://duiwai-deploy.oss-cn-hangzhou.aliyuncs.com/asr-service/third-party/gflags/v2.2.1.zip
  URL_HASH SHA256=4e44b69e709c826734dbbbd5208f61888a2faf63f239d73d8ba0011b2dccc97a
)
FetchContent_MakeAvailable(gflags)
include_directories(${gflags_BINARY_DIR}/include)

# third_party: glog
FetchContent_Declare(glog
  URL      http://duiwai-deploy.oss-cn-hangzhou.aliyuncs.com/asr-service/third-party/glog/v0.4.0.zip
  URL_HASH SHA256=9e1b54eb2782f53cd8af107ecf08d2ab64b8d0dc2b7f5594472f3bd63ca85cdc
)
FetchContent_MakeAvailable(glog)
include_directories(${glog_SOURCE_DIR}/src ${glog_BINARY_DIR})

# utils
add_library(utils STATIC
  src/utils/utils.cc
)
target_link_libraries(utils PUBLIC glog)

# frontend
add_library(frontend STATIC
  src/frontend/feature_pipeline.cc
  src/frontend/fft.cc
)
target_link_libraries(frontend PUBLIC utils)

add_executable(catch_fbank_multithread_main bin/catch_fbank_multithread_main.cc)
target_link_libraries(catch_fbank_multithread_main PUBLIC frontend gflags)

add_executable(catch_fbank_multithread_main_2nd bin/catch_fbank_multithread_main_2nd.cc)
target_link_libraries(catch_fbank_multithread_main_2nd PUBLIC frontend gflags)