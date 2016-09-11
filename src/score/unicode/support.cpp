#include "score/io/UTF8View.h"
#include "score/unicode/UnicodeBlock.h"
#include "score/unicode/support.h"
#include "score/vendored/utf8/utf8.h"
#include "score/macros.h"
#include <string>
#include <map>

using namespace std;
namespace utf8 = score::vendored::utf8;
using score::io::UTF8View;

namespace score { namespace unicode {

uint32_t normalizeBasicLatinCodePoint(uint32_t codePoint) {
  // A = 65
  // Z = 90
  // a - A = 32
  // z - Z = 32, etc
  if (codePoint >= 65 && codePoint <= 90) {
    codePoint += 32;
  }
  return codePoint;
}

uint32_t normalizeLatin1SupplementCodePoint(uint32_t codePoint) {
  if ((codePoint >= 192 && codePoint <= 214) || (codePoint >= 216 && codePoint <= 221)) {
    codePoint += 32;
  }
  return codePoint;
}

uint32_t normalizeLatinExtendedACodePoint(uint32_t codePoint) {
  // 383 is the small long S,
  // the only one in latin extended A without a capital version
  if (codePoint == 383) {
    return codePoint;
  }
  if (codePoint < 311) {
    // up to 311, latin A alternates between
    // upper and lower case versions with
    // uppers on even numbers.
    if (!(codePoint % 2)) {
      codePoint += 1;
    }
  }
  return codePoint;
}

uint32_t normalizeCodePoint(uint32_t codePoint, UnicodeBlock block) {
  switch (block) {
    case UnicodeBlock::BASIC_LATIN : return normalizeBasicLatinCodePoint(codePoint);
    case UnicodeBlock::LATIN_1_SUPPLEMENT : return normalizeLatin1SupplementCodePoint(codePoint);
    case UnicodeBlock::LATIN_EXTENDED_A : return normalizeLatinExtendedACodePoint(codePoint);
    default: return codePoint;
  }
}

uint32_t normalizeCodePoint(uint32_t codePoint) {
  auto block = getUnicodeBlock(codePoint);
  return normalizeCodePoint(codePoint, block);
}

bool isBasicLatinLetter(uint32_t cp) {
  // 0-31: misc whitespace chars, hex tablet things

  // 33-64: misc punc, digits, more misc punc
  // 33 = '!''
  // 54 = '6'
  // 64 = '@'

  if (cp <= 64) {
    return false;
  }
  if (cp <= 96 && cp > 90) {
    // lbracket, backslash, etc
    return false;
  }

  // 122 = 'z'
  if (cp > 122) {
    SDCHECK(cp <= 127);
    return false;
  }

  // remaining characters are letters
  return true;
}

bool isLatin1SupplementLetter(uint32_t cp) {
  // LATIN_1_SUPPLEMENT starts with a bunch of punctuation.
  // 191 is upside-down question mark
  if (cp <= 191) {
    return false;
  }

  // 215 = multiplication symbol
  // 247 = division symbol
  if (cp == 215 || cp == 247) {
    return false;
  }

  return true;
}

bool isMathematicalAlphanumericSymbolLetter(uint32_t cp) {
  // 120782 is a 0 character. everything before that in
  // this block is a letter (sort of)
  return cp < 120782;
}

bool isLetterPoint(uint32_t cp, UnicodeBlock uBlock) {
  switch (uBlock) {
    case UnicodeBlock::BASIC_LATIN : return isBasicLatinLetter(cp);
    case UnicodeBlock::LATIN_1_SUPPLEMENT : return isLatin1SupplementLetter(cp);
    case UnicodeBlock::LATIN_EXTENDED_A : return true;
    case UnicodeBlock::LATIN_EXTENDED_B : return true;
    case UnicodeBlock::LATIN_EXTENDED_C : return true;
    case UnicodeBlock::LATIN_EXTENDED_D : return true;
    case UnicodeBlock::LATIN_EXTENDED_E : return true;
    case UnicodeBlock::LATIN_EXTENDED_ADDITIONAL : return true;
    case UnicodeBlock::GENERAL_PUNCTUATION : return false;

    case UnicodeBlock::ARROWS : return false;
    case UnicodeBlock::BLOCK_ELEMENTS : return false;
    case UnicodeBlock::BOX_DRAWING : return false;
    case UnicodeBlock::BRAILLE_PATTERNS : return false;
    case UnicodeBlock::COMBINING_DIACRITICAL_MARKS : return false;
    case UnicodeBlock::COMBINING_DIACRITICAL_MARKS_EXTENDED : return false;
    case UnicodeBlock::CONTROL_PICTURES : return false;
    case UnicodeBlock::DINGBATS : return false;
    case UnicodeBlock::DOMINO_TILES : return false;
    case UnicodeBlock::EMOTICONS : return false;
    case UnicodeBlock::ENCLOSED_ALPHANUMERICS : return false;
    case UnicodeBlock::GEOMETRIC_SHAPES : return false;
    case UnicodeBlock::IPA_EXTENSIONS : return false;
    case UnicodeBlock::MATHEMATICAL_OPERATORS : return false;
    case UnicodeBlock::MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A : return false;
    case UnicodeBlock::MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B : return false;
    case UnicodeBlock::MISCELLANEOUS_SYMBOLS : return false;
    case UnicodeBlock::MISCELLANEOUS_SYMBOLS_AND_ARROWS : return false;
    case UnicodeBlock::MISCELLANEOUS_TECHNICAL : return false;
    case UnicodeBlock::MUSICAL_SYMBOLS : return false;
    case UnicodeBlock::NUMBER_FORMS : return false;
    case UnicodeBlock::SPACING_MODIFIER_LETTERS : return false;
    case UnicodeBlock::SUPERSCRIPTS_AND_SUBSCRIPTS : return false;
    case UnicodeBlock::SUPPLEMENTAL_MATHEMATICAL_OPERATORS : return false;
    case UnicodeBlock::SUPPLEMENTAL_PUNCTUATION : return false;

    default: return true;
  }
}

bool isLetterPoint(uint32_t cp) {
  auto uBlock = getUnicodeBlock(cp);
  return isLetterPoint(cp, uBlock);
}

static const map<uint32_t, uint32_t> singleUnicodeReplacements {
  {8220, 34}, // unicode lquote to quote
  {8221, 34}, // unicode rquote to quote
  {8216, 39}, // unicode l singlequote to singlequote
  {8217, 39}, // unicode r singlequote to singlequote
  {8291, 32}, // a space-like thing with a real space
  {8229, 32}, // a space-like thing with a real space
  {160,  32}, // a space-like thing with a real space
  {8226, 45}, // bullet with dash
  {9679, 45}, // large bullet with dash
  {8210, 45}, // long dash with dash
  {8211, 45}, // long dash with dash
  {8212, 45}, // long dash with dash
  {8213, 45}, // long dash with dash
  {8275, 45}, // long dash with dash
  {11834, 45}, // long dash with dash
  {11835, 45} // long dash with dash
};

static const uint32_t SPACE = 32;
static const uint32_t CARRIAGE_RETURN = 13;
static const uint32_t NEW_LINE = 10;

string basicClean(const string &text) {
  string result;
  result.reserve(text.size());
  auto iter = std::back_inserter(result);
  UTF8View view(text);
  array<uint32_t, 2> prevPoints;
  for (size_t i = 0; i < 2; i++) {
    prevPoints[i] = 0;
  }
  for (auto cp: view) {
    if (cp == 160 || (cp > 8000 && cp < 12000)) {
      auto replacement = singleUnicodeReplacements.find(cp);
      if (replacement != singleUnicodeReplacements.end()) {
        cp = replacement->second;
      }
    }
    if (prevPoints[1] == CARRIAGE_RETURN && cp != NEW_LINE) {
      iter = utf8::unchecked::append(NEW_LINE, iter);
      prevPoints[0] = prevPoints[1];
      prevPoints[1] = NEW_LINE;
    }
    if (cp == SPACE) {
      if (prevPoints[1] != SPACE) {
        iter = utf8::unchecked::append(cp, iter);
      }
    } else if (cp == NEW_LINE) {
      if (prevPoints[0] != NEW_LINE) {
        iter = utf8::unchecked::append(cp, iter);
      }
    } else {
      iter = utf8::unchecked::append(cp, iter);
    }
    prevPoints[0] = prevPoints[1];
    prevPoints[1] = cp;
  }
  return result;
}

}} // score::unicode

