#pragma once
#include <string>

namespace score { namespace fs {

bool readFileSync(const std::string &filePath, std::string &result);
std::string readFileSync(const std::string &filePath);
void writeFileSync(const std::string &filePath, const std::string &body);

}} // score::fs
