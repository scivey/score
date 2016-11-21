#include <gtest/gtest.h>
#include <string>
#include "score/nlp/Language.h"
#include "score/nlp/language_detection.h"


using namespace score::nlp;
using namespace std;


TEST(TestLanguageDetection, TestShortStrings) {
  vector<pair<string, Language>> toTest {
    {"this is a test", Language::EN},
    {"no me gusta la biblioteca", Language::ES},
    {
      "Le travail éloigne de nous trois grands maux: l'ennui, le vice et le besoin.",
      Language::FR
    },
    {
      "Der Alte verliert eines der größten Menschenrechte: er wird nicht mehr von seines Gleichen beurteilt.",
      Language::DE
    }
  };
  for (const auto& item: toTest) {
    auto detected = detectLanguage(item.first);
    EXPECT_FALSE(detected.hasException());
    EXPECT_EQ(item.second, detected.value());
  }
}
