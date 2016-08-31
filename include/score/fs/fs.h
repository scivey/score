#pragma once
#include <string>

namespace score { namespace fs {

bool readFileSync(const std::string &filePath, std::string &result);

}} // score::fs
