#include <thread>
#include <string>
#include <set>
#include <memory>
#include <glog/logging.h>
#include "score_async/EventContext.h"
#include "score_curl/EventCurler.h"
#include "score/html/HtmlDom.h"
#include "score/util/misc.h"
#include "score/url/URLView.h"

using namespace std;
using namespace score::html;
using score::url::URLView;
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
        if (isGoodLink(link) && links.count(link) == 0) {
          links.insert(link);
        }
      }
    });
  }
  return links;
}

std::set<std::string> extractLinksFromSameDomain(const string& domain, const string& html) {
  auto links = extractLinks(html);
  std::set<std::string> result;
  for (auto& link: links) {
    auto viewOpt = URLView::parse(link);
    if (!viewOpt.hasException()) {
      auto view = viewOpt.value();
      auto host = view.host();
      if (host.hasValue()) {
        string hostName = folly::to<std::string>(host.value());
        if (hostName != domain) {
          continue;
        }
        string toAdd = link;
        if (view.fragment().hasValue()) {
          auto ffs = view.fragmentOffset();
          if (ffs.hasValue()) {
            toAdd = link.substr(0, ffs.value());
          }
        }
        if (result.count(toAdd) == 0) {
          result.insert(toAdd);
        }
      }
    }
  }
  return result;
}


void tryLinks() {
  auto ctx = util::createShared<score::async::EventContext>();
  auto curler = util::createShared<score::curl::EventCurler>(ctx.get());
  curler->getURL("http://jezebel.com", [&curler](string result) {
    LOG(INFO) << "got result: " << result.substr(0, 200);
    auto links = extractLinksFromSameDomain("jezebel.com", result);
    for (auto link: links) {
      curler->getURL(link, [link](string result) {
        LOG(INFO) << "got: " << link;
      });
    }
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
