#pragma once
#include <string>
#include <vector>
#include "score/Try.h"
#include "score/nlp/Language.h"
#include "score/exceptions/ScoreError.h"

namespace score { namespace nlp {

SCORE_DECLARE_EXCEPTION(LanguageDetectionError, exceptions::ScoreError);
SCORE_DECLARE_EXCEPTION(UnreliableDetection, LanguageDetectionError);

struct LanguageDetectionOptions {
  bool isPlainText {true};
  bool acceptUnreliableResult {false};
};

score::Try<Language> detectLanguage(const std::string&,
    const LanguageDetectionOptions& = LanguageDetectionOptions());

}} // score::nlp
