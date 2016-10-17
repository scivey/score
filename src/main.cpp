#include <thread>
#include <string>
#include <set>
#include <memory>
#include <glog/logging.h>
#include "score_async/EventContext.h"
#include "score_curl/EventCurler.h"
#include "score/html/HtmlDom.h"
#include "score/util/misc.h"

using namespace std;
using namespace score::html;
namespace util = score::util;

bool isGoodLink(const std::string &link) {
  if (link.size() < 3) {
    return false;
  }
  if (link.find("?") == 0 || link.find("#") == 0) {
    return false;
  }
  if (link.find("mailto:") == 0) {
    return false;
  }
  return true;

}

set<string> extractLinks(const std::string& data) {
  auto dom = score::html::HtmlDom::create(data);
  std::set<string> links;
  auto body = dom.root().dfFindFirst(Node::nodeHasTag(Tag::BODY));
  if (body) {
    LOG(INFO) << body.getText();
    body.dfs([&links](const Node& node) {
      if (node.isElement() && node.hasTag(Tag::A) && node.hasAttr("href")) {
        auto link = node.getAttr("href");
        if (link.size() < 3) {
          return;
        }
        if (link.find("?") == 0) {
          return;
        }
        if (links.count(link) == 0) {
          links.insert(link);
        }
      }
    });
  }
  return links;
}

void tryLinks() {
  auto ctx = util::createShared<score::async::EventContext>();
  auto curler = util::createShared<score::curl::EventCurler>(ctx.get());
  curler->getURL("http://jezebel.com", [](string result) {
    LOG(INFO) << "got result: " << result.substr(0, 200);
    auto links = extractLinks(result);
    for (auto link: links) {
      LOG(INFO) << "link: " << link;
    }
    LOG(INFO) << "here. " << links.size();
  });
  for (;;) {
    ctx->getBase()->runForever();
  }
}

int main() {
  google::InstallFailureSignalHandler();
  LOG(INFO) << "begin";
  tryLinks();
  LOG(INFO) << "end.";
}
