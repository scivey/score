#pragma once

#include <typeinfo>
#include <string>

namespace score {

std::string demangle(const std::type_info& type);

} // score
