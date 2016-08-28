#pragma once
#include <iostream>

namespace aliens { namespace formatters {

std::ostream& operator<<(std::ostream &oss, __uint128_t num128);

}} // aliens::formatters
