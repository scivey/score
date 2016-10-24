#pragma once
#include <iostream>
#include <sstream>

namespace score { namespace formatters {

std::ostream& operator<<(std::ostream &oss, __uint128_t num128);

}} // score::formatters
