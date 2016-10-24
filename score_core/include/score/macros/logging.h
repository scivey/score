#pragma once

#include <glog/logging.h>
#include <folly/Format.h>

#define SCORE_INFO(...) LOG(INFO) << folly::format(__VA_ARGS__)
