#pragma once
#include <string>
#include "score/unicode/UnicodeBlock.h"

namespace score { namespace unicode {

uint32_t normalizeCodePoint(uint32_t codePoint, UnicodeBlock block);
uint32_t normalizeCodePoint(uint32_t codePoint);
bool isLetterPoint(uint32_t cp, UnicodeBlock uBlock);
bool isLetterPoint(uint32_t cp);
std::string basicUnicodeClean(const std::string&);


}} // score::unicode
