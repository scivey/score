#include "score/html/HTMLDoc.h"
#include "score/html/Node.h"
#include "score/html/Tag.h"
#include "score/html/MetaTags.h"

#include <map>
#include <string>
using namespace std;

namespace score { namespace html {

MetaTags::MetaTags(std::map<std::string, std::string> vals)
  : tagValues_(std::move(vals)) {}

MetaTags MetaTags::extract(const Node &startingNode) {
  map<string, string> tags;
  string metaName;
  string metaVal;
  startingNode.dfs([&tags, &metaName, &metaVal](const Node &node) {
    if (!node.hasTag(Tag::META)) {
      return;
    }
    metaName = "";
    metaVal = "";
    if (node.getAttr("property", metaName)) {
      if (node.getAttr("content", metaVal)) {
        tags.insert(std::make_pair(metaName, metaVal));
      }
    } else if (node.getAttr("name", metaName)) {
      if (node.getAttr("content", metaVal)) {
        tags.insert(std::make_pair(metaName, metaVal));
      }
    } else if (node.getAttr("itemprop", metaName)) {
      if (metaName == "datePublished" && node.getAttr("datetime", metaVal)) {
        tags.insert(std::make_pair(metaName, metaVal));
      }
    }
  });
  return MetaTags(std::move(tags));
}

static const string nothing = "";


const string& MetaTags::getFirstOrNothing(
    const vector<string> &tagNames) const {
  for (auto &name: tagNames) {
    auto found = tagValues_.find(name);
    if (found != tagValues_.end()) {
      return found->second;
    }
  }
  return nothing;
}

static const vector<string> pubdateTags {
  "rnews:datePublished",
  "article:published_time",
  "OriginalPublicationDate",
  "datePublished"
};

const string& MetaTags::getPublishDate() const {
  return getFirstOrNothing(pubdateTags);
}

static const vector<string> titleTags {
  "og:title",
  "headline"
};

const string& MetaTags::getTitle() const {
  return getFirstOrNothing(titleTags);
}

bool MetaTags::has(const string &key) const {
  return tagValues_.count(key) > 0;
}

const string& MetaTags::get(const string &key) const {
  auto found = tagValues_.find(key);
  if (found != tagValues_.end()) {
    return found->second;
  }
  return nothing;
}

size_t MetaTags::size() const {
  return tagValues_.size();
}

}} // text::html
