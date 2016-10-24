#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include "score/extract/goose/TextCleaner.h"
#include "score/html/Node.h"

using namespace std;
using namespace score::html;

namespace score { namespace extract { namespace goose {

namespace detail {

  const vector<string> badClasses = {
    "^side$|combx|retweet|mediaarticlerelated|menucontainer|",
    "navbar|storytopbar-bucket|utility-bar|inline-share-tools",
    "|comment|PopularQuestions|contact|foot|footer|Footer|footnote",
    "|cnn_strycaptiontxt|cnn_html_slideshow|cnn_strylftcntnt",
    "|^links$|meta$|shoutbox|sponsor",
    "|tags|socialnetworking|socialNetworking|cnnStryHghLght",
    "|cnn_stryspcvbx|^inset$|pagetools|post-attributes",
    "|welcome_form|contentTools2|the_answers",
    "|communitypromo|runaroundLeft|subscribe|vcard|articleheadings",
    "|date|^print$|popup|author-dropdown|tools|socialtools|byline",
    "|konafilter|KonaFilter|breadcrumbs|^fn$|wp-caption-text",
    "|legende|ajoutVideo|timestamp|js_replies"
  };

  regex getBadClassesRegex() {
    ostringstream oss;
    for (auto &elem: badClasses) {
      oss << elem;
    }
    string reggie = oss.str();
    regex bad_re(reggie);
    return bad_re;
  }

  static const set<Tag> badTagTypes {
    Tag::STYLE, Tag::SCRIPT, Tag::EM
  };

} // detail

bool TextCleaner::isBadTextNode(const Node &node) {
    if (!node.isElement()) {
        return false;
    }
    if (detail::badTagTypes.count(node.getTag()) > 0) {
      return true;
    }
    string attr;
    if (node.getAttr("id", attr) && regex_search(attr, badClasses_)) {
      return true;
    }
    if (node.getAttr("class", attr) && regex_search(attr, badClasses_)) {
      return true;
    }
    return false;
}

TextCleaner::TextCleaner() {
  badClasses_ = detail::getBadClassesRegex();
}

size_t TextCleaner::getText(const Node &node, ostringstream &oss) {
  if (node.isText()) {
    return node.getText(oss);
  } else if (node.isElement()) {
    if (!isBadTextNode(node)) {
      size_t appended = 0;
      for (auto child: node.children()) {
        size_t childAppended = getText(child, oss);
        appended += childAppended;
        if (childAppended > 0) {
          oss << " ";
          appended++;
        }
      }
      if (node.hasTag(Tag::P)) {
        oss << "\n\n";
        appended += 2;
      } else if (node.hasTag(Tag::BR)) {
        oss << "\n";
        appended += 1;
      }
      return appended;
    }
  }
  return 0;
}


string TextCleaner::getText(const Node &node) {
  ostringstream oss;
  getText(node, oss);
  return oss.str();
}

}}} // score::extract::goose

