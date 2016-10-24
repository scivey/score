#pragma once
#include <string>
#include "score/html/Node.h"

namespace score { namespace extract { namespace goose {

bool hasHighLinkDensity(const html::Node&, const std::string&);
bool hasHighLinkDensity(const html::Node&);

}}} // score::extract::goose
