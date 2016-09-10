#pragma once
#include <string>
#include <vector>

#include "score/hashing/util.h"

namespace score { namespace nlp {

enum class Language {
  AF,
  AR,
  BG,
  BN,
  CA,
  CS,
  CY,
  DA,
  DE,
  EL,
  EN,
  ES,
  ET,
  FA,
  FI,
  FR,
  GU,
  HE,
  HI,
  HR,
  HU,
  ID,
  IT,
  JA,
  KN,
  KO,
  LT,
  LV,
  MK,
  ML,
  MR,
  NE,
  NL,
  NO,
  PA,
  PL,
  PT,
  RO,
  RU,
  SK,
  SL,
  SO,
  SQ,
  SV,
  SW,
  TA,
  TE,
  TH,
  TL,
  TR,
  UK,
  UNKNOWN,
  UR,
  VI,
  ZH_CN,
  ZH_TW
};

std::string stringOfLanguage(Language lang);

std::string englishNameOfLanguage(Language lang);

Language languageFromCode(const std::string&);

const std::vector<Language>& getAllLanguages();

}} // score::nlp


namespace std {
  template<>
  struct hash<score::nlp::Language> {
    size_t operator()(const score::nlp::Language &val) const {
      return score::hashing::hashEnum<score::nlp::Language>(val);
    }
  };
}
