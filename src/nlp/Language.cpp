#include "score/nlp/Language.h"
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

namespace score { namespace nlp {

std::string stringOfLanguage(Language lang) {
  switch(lang) {
    case Language::AF : return "af";
    case Language::AR : return "ar";
    case Language::BG : return "bg";
    case Language::BN : return "bn";
    case Language::CA : return "ca";
    case Language::CS : return "cs";
    case Language::CY : return "cy";
    case Language::DA : return "da";
    case Language::DE : return "de";
    case Language::EL : return "el";
    case Language::EN : return "en";
    case Language::ES : return "es";
    case Language::ET : return "et";
    case Language::FA : return "fa";
    case Language::FI : return "fi";
    case Language::FR : return "fr";
    case Language::GU : return "gu";
    case Language::HE : return "he";
    case Language::HI : return "hi";
    case Language::HR : return "hr";
    case Language::HU : return "hu";
    case Language::ID : return "id";
    case Language::IT : return "it";
    case Language::JA : return "ja";
    case Language::KN : return "kn";
    case Language::KO : return "ko";
    case Language::LT : return "lt";
    case Language::LV : return "lv";
    case Language::MK : return "mk";
    case Language::ML : return "ml";
    case Language::MR : return "mr";
    case Language::NE : return "ne";
    case Language::NL : return "nl";
    case Language::NO : return "no";
    case Language::PA : return "pa";
    case Language::PL : return "pl";
    case Language::PT : return "pt";
    case Language::RO : return "ro";
    case Language::RU : return "ru";
    case Language::SK : return "sk";
    case Language::SL : return "sl";
    case Language::SO : return "so";
    case Language::SQ : return "sq";
    case Language::SV : return "sv";
    case Language::SW : return "sw";
    case Language::TA : return "ta";
    case Language::TE : return "te";
    case Language::TH : return "th";
    case Language::TL : return "tl";
    case Language::TR : return "tr";
    case Language::UK : return "uk";
    case Language::UNKNOWN : return "UNKNOWN";
    case Language::UR : return "ur";
    case Language::VI : return "vi";
    case Language::ZH_CN : return "zh_cn";
    case Language::ZH_TW : return "zh_tw";
    default : return "NOT_RECOGNIZED";
  }
}



std::string englishNameOfLanguage(Language lang) {
  switch (lang) {
    case Language::AR : return "Arabic";
    case Language::DE : return "German";
    case Language::EN : return "English";
    case Language::ES : return "Spanish";
    case Language::FR : return "French";
    case Language::IT : return "Italian";
    case Language::RU : return "Russian";
    default           : return "UNKNOWN";
  }
}

static const unordered_map<string, Language> codeToLanguageMap {
  {"af", Language::AF},
  {"ar", Language::AR},
  {"bg", Language::BG},
  {"bn", Language::BN},
  {"ca", Language::CA},
  {"cs", Language::CS},
  {"cy", Language::CY},
  {"da", Language::DA},
  {"de", Language::DE},
  {"el", Language::EL},
  {"en", Language::EN},
  {"es", Language::ES},
  {"et", Language::ET},
  {"fa", Language::FA},
  {"fi", Language::FI},
  {"fr", Language::FR},
  {"gu", Language::GU},
  {"he", Language::HE},
  {"hi", Language::HI},
  {"hr", Language::HR},
  {"hu", Language::HU},
  {"id", Language::ID},
  {"it", Language::IT},
  {"ja", Language::JA},
  {"kn", Language::KN},
  {"ko", Language::KO},
  {"lt", Language::LT},
  {"lv", Language::LV},
  {"mk", Language::MK},
  {"ml", Language::ML},
  {"mr", Language::MR},
  {"ne", Language::NE},
  {"nl", Language::NL},
  {"no", Language::NO},
  {"pa", Language::PA},
  {"pl", Language::PL},
  {"pt", Language::PT},
  {"ro", Language::RO},
  {"ru", Language::RU},
  {"sk", Language::SK},
  {"sl", Language::SL},
  {"so", Language::SO},
  {"sq", Language::SQ},
  {"sv", Language::SV},
  {"sw", Language::SW},
  {"ta", Language::TA},
  {"te", Language::TE},
  {"th", Language::TH},
  {"tl", Language::TL},
  {"tr", Language::TR},
  {"uk", Language::UK},
  {"unknown", Language::UNKNOWN},
  {"ur", Language::UR},
  {"vi", Language::VI},
  {"zh_cn", Language::ZH_CN},
  {"zh_tw", Language::ZH_TW}
};


Language languageFromCode(const std::string &code) {
  auto found = codeToLanguageMap.find(code);
  if (found == codeToLanguageMap.end()) {
    return Language::UNKNOWN;
  }
  return found->second;
}

static const vector<Language> allLanguages {
  Language::AF,
  Language::AR,
  Language::BG,
  Language::BN,
  Language::CA,
  Language::CS,
  Language::CY,
  Language::DA,
  Language::DE,
  Language::EL,
  Language::EN,
  Language::ES,
  Language::ET,
  Language::FA,
  Language::FI,
  Language::FR,
  Language::GU,
  Language::HE,
  Language::HI,
  Language::HR,
  Language::HU,
  Language::ID,
  Language::IT,
  Language::JA,
  Language::KN,
  Language::KO,
  Language::LT,
  Language::LV,
  Language::MK,
  Language::ML,
  Language::MR,
  Language::NE,
  Language::NL,
  Language::NO,
  Language::PA,
  Language::PL,
  Language::PT,
  Language::RO,
  Language::RU,
  Language::SK,
  Language::SL,
  Language::SO,
  Language::SQ,
  Language::SV,
  Language::SW,
  Language::TA,
  Language::TE,
  Language::TH,
  Language::TL,
  Language::TR,
  Language::UK,
  Language::UNKNOWN,
  Language::UR,
  Language::VI,
  Language::ZH_CN,
  Language::ZH_TW
};

const vector<Language>& getAllLanguages() {
  return allLanguages;
}

}} // score::nlp