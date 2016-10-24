#pragma once

#include <glog/logging.h>
#include "score/format.h"

#define SCORE_INFO(...) LOG(INFO) << score::format(__VA_ARGS__)
