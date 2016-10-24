#pragma once
#include "score/html/vendored/gumbo-parser/gumbo.h"
#include <functional>
#include <string>
#include <sstream>
#include "score/html/detail/GumboVectorWrapper.h"
#include "score/html/Tag.h"
#include "score/io/string_utils.h"


namespace score { namespace html {

class Node {
 protected:
  const GumboNode *node_ {nullptr};
 public:
  using GumboVectorWrapper = detail::GumboVectorWrapper;
  Node(const GumboNode*);
  Node(const Node &other);
  Node();
  const GumboNode* getGumboNode() const;
  bool good() const;
  operator bool() const;
  size_t childCount() const;
  size_t childElementCount() const;
  bool hasChildren() const;
  bool isElement() const;
  bool isText() const;
  bool isWhitespace() const;
  bool hasTag(Tag tag) const;
  bool hasTag(GumboTag tag) const;
  Tag getTag() const;
  const std::string& getTagString() const;
  bool hasParent() const;
  Node parent() const;
  bool refEquals(const Node &other) const;
  bool refEquals(const GumboNode *other) const;
 protected:
  GumboAttribute* getGumboAttribute(const std::string &attrName) const;
 public:
  bool hasAttr(const std::string &attrName) const;
  bool getAttr(const std::string &attrName, std::string& result) const;
  std::string getAttr(const std::string &attrName) const;

  size_t getText(std::ostringstream &oss) const;
  std::string getText() const;
  std::string getTrimmedText() const;

  class NodeVector {
   protected:
    GumboVectorWrapper wrapper_;
   public:
    NodeVector();
    NodeVector(GumboVectorWrapper wrapper);
    NodeVector(const GumboVector *vec);
    NodeVector(const NodeVector &vec);

    size_t size() const;
    Node at(size_t idx) const;
    Node operator[](size_t idx) const;

    class Iterator {
     protected:
      GumboVectorWrapper wrapper_;
      size_t idx_;
     public:
      Iterator(GumboVectorWrapper wrapper, size_t idx);
      bool operator!=(const Iterator &other) const;
      Iterator& operator++();
      Iterator operator++(int);
      Node operator*();
    };

    Iterator begin();
    Iterator end();
  };

  NodeVector children() const;
  using filter_visitor = std::function<bool (const Node&)>;
  using non_escape_visitor = std::function<void (const Node&)>;
  using escape_func = std::function<void()>;
  using escape_visitor = std::function<void(const Node&, escape_func)>;

 protected:
  void dfs(filter_visitor, escape_visitor, escape_func, const bool &keepGoing) const;
  uintptr_t getGumboUintPtr() const;

 public:
  void dfs(filter_visitor, escape_visitor) const;
  void dfs(escape_visitor) const;
  void dfs(non_escape_visitor) const;
  Node dfFindFirst(filter_visitor choosePred, filter_visitor recursePred) const;
  Node dfFindFirst(filter_visitor choosePred) const;
  bool walkSiblings(escape_visitor) const;
  bool operator!=(const Node &other) const;
  bool operator<(const Node &other) const;

  static bool nodeIsElement(const Node&);
  static bool nodeIsText(const Node&);
  static bool nodeIsWhitespace(const Node&);
  static bool nodeHasTag(Tag, const Node&);

  static std::function<bool (const Node&)> nodeHasTag(Tag);
};

}} // score::html
