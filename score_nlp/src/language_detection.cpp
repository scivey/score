#include "score/nlp/language_detection.h"
#include "score/nlp/vendored/cld2/compact_lang_det.h"
#include "score/nlp/vendored/cld2/internal/lang_script.h"
#include "score/util/try_helpers.h"

using namespace std;

namespace score { namespace nlp {

Try<score::nlp::Language> detectLanguage(const string& text, const LanguageDetectionOptions& options) {
  bool isReliable {false};
  auto result = CLD2::DetectLanguage(
    text.c_str(), text.size(),
    options.isPlainText, &isReliable
  );
  if (!isReliable && !options.acceptUnreliableResult) {
    return util::makeTryFailure<Language, UnreliableDetection>(
      "CLD2 couldn't reliably guess a language for this text."
    );
  }
  auto cld2LangCode = CLD2::LanguageCode(result);
  auto scoreLang = score::nlp::languageFromCode(cld2LangCode);
  return util::makeTrySuccess<score::nlp::Language>(scoreLang);
}

}} // score::nlp
