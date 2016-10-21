#include <gumbo.h>
#include "score_html/GumboVectorWrapper.h"
#include "score_html/Node.h"
#include "score/util/misc.h"

using namespace std;

namespace score { namespace html {

using NodeVec = Node::NodeVector;
using NodeIter = Node::NodeVector::Iterator;

NodeVec::NodeVector(GumboVectorWrapper wrapper): wrapper_(wrapper){}

NodeVec::NodeVector(const GumboVector *vec): wrapper_(vec){}

NodeVec::NodeVector(const NodeVec &vec): wrapper_(vec.wrapper_){}

NodeVec::NodeVector(){}

Node::Node(const GumboNode *node): node_(node){}

Node::Node(const Node &other): node_(other.node_){}

Node::Node(): node_(nullptr){};

NodeVec Node::children() const {
  if (!good()) {
    return NodeVec();
  }
  return NodeVec(&node_->v.element.children);
}

Node Node::parent() const {
  return Node(node_->parent);
}

bool Node::good() const {
  return node_ != nullptr;
}

Node::operator bool() const {
  return good();
}

bool Node::isText() const {
  if (!good()) {
    return false;
  }
  return node_->type == GUMBO_NODE_TEXT;
}

bool Node::isWhitespace() const {
  if (!good()) {
    return false;
  }
  return node_->type == GUMBO_NODE_WHITESPACE;
}

bool Node::isElement() const {
  if (!good()) {
    return false;
  }
  return node_->type == GUMBO_NODE_ELEMENT;
}

size_t Node::childCount() const {
  if (!isElement()) {
    return 0;
  }
  auto offspring = children();
  return offspring.size();
}

bool Node::hasChildren() const {
  return childCount() > 0;
}

Tag Node::getTag() const {
  if (!isElement()) {
    return Tag::UNKNOWN;
  }
  return tagOfGumboTag(node_->v.element.tag);
}

const std::string& Node::getTagString() const {
  return stringOfTag(getTag());
}

bool Node::hasTag(Tag tag) const {
  return getTag() == tag;
}

bool Node::hasTag(GumboTag tag) const {
  if (!isElement()) {
    return false;
  }
  return node_->v.element.tag == tag;
}

const GumboNode* Node::getGumboNode() const {
  return node_;
}

Node NodeVec::at(size_t idx) const {
  if (idx >= size()) {
    return Node();
  }
  return Node(wrapper_.at(idx));
}

Node NodeVec::operator[](size_t idx) const {
  return at(idx);
}

NodeIter NodeVec::begin() {
  return NodeIter(wrapper_, 0);
}

NodeIter NodeVec::end() {
  return NodeIter(wrapper_, size());
}

size_t NodeVec::size() const {
  return wrapper_.size();
}

NodeIter::Iterator(GumboVectorWrapper wrapper, size_t idx)
  : wrapper_(wrapper), idx_(idx){}

NodeIter& NodeIter::operator++() {
  ++idx_;
  return *this;
}

NodeIter NodeIter::operator++(int) {
  NodeIter result(wrapper_, idx_);
  ++result;
  return result;
}

bool NodeIter::operator!=(const NodeIter &other) const {
  return idx_ != other.idx_;
}

Node NodeIter::operator*() {
  return Node(wrapper_[idx_]);
}

void Node::dfs(Node::filter_visitor filterFn,
    Node::escape_visitor mainFn,
    Node::escape_func escaper,
    const bool &keepGoing) const {
  if (!good()) {
    return;
  }
  mainFn(*this, escaper);
  if (!keepGoing) {
    return;
  }
  if (!hasChildren()) {
    return;
  }
  for (auto child: children()) {
    if (filterFn(child)) {
      child.dfs(filterFn, mainFn, escaper, keepGoing);
      if (!keepGoing) {
        break;
      }
    }
  }
}

void Node::dfs(Node::filter_visitor filterFn,
    Node::escape_visitor mainFn) const {
  bool keepGoing = true;
  Node::escape_func escaper([&keepGoing](){
    keepGoing = false;
  });
  dfs(filterFn, mainFn, escaper, keepGoing);
}

void Node::dfs(Node::escape_visitor mainFn) const {
  dfs(util::always<Node>, mainFn);
}

void Node::dfs(Node::non_escape_visitor mainFn) const {
  dfs([mainFn](const Node& node, Node::escape_func) {
    mainFn(node);
  });
}

bool Node::hasParent() const {
  return !!node_->parent;
}

bool Node::refEquals(const Node &other) const {
  return node_ == other.node_;
}

bool Node::refEquals(const GumboNode *other) const {
  return node_ == other;
}

bool Node::walkSiblings(Node::escape_visitor mainFn) const {
  if (!hasParent()) {
    return false;
  }
  bool keepGoing = true;
  Node::escape_func escaper([&keepGoing](){
    keepGoing = false;
  });
  auto parentNode = parent();
  if (parentNode.childCount() == 1) {
    return false;
  }
  for (auto child: parentNode.children()) {
    mainFn(child, escaper);
    if (!keepGoing) {
      break;
    }
  }
  return true;
}

GumboAttribute* Node::getGumboAttribute(const string &attrName) const {
  GumboAttribute *attr = nullptr;
  if (!good()) {
    return attr;
  }
  attr = gumbo_get_attribute(&node_->v.element.attributes, attrName.c_str());
  return attr;
}

bool Node::hasAttr(const string &attrName) const {
  return !!getGumboAttribute(attrName);
}

bool Node::getAttr(const string &attrName, string &result) const {
  auto attr = getGumboAttribute(attrName);
  if (!attr) {
    return false;
  }
  result = attr->value;
  return true;
}

string Node::getAttr(const string &attrName) const {
  string result = "";
  getAttr(attrName, result);
  return result;
}

size_t Node::getText(ostringstream &oss) const {
  if (isText()) {
    auto startPos = oss.tellp();
    oss << node_->v.text.text;
    auto endPos = oss.tellp();
    if (startPos < 0 || endPos <= startPos) {
      return 0;
    }
    return endPos - startPos;
  }
  if (isElement()) {
    if (hasTag(Tag::SCRIPT) || hasTag(Tag::STYLE) || (!hasChildren())) {
      return 0;
    }
    size_t i = 0;
    size_t appended = 0;
    size_t last = childCount() - 1;
    for (auto child: children()) {
      size_t childAdded = child.getText(oss);
      appended += childAdded;
      if (i < last && childAdded > 0) {
        oss << " ";
        appended++;
      }
    }
    return appended;
  }
  return 0;
}

string Node::getText() const {
  ostringstream oss;
  getText(oss);
  return oss.str();
}

Node Node::dfFindFirst(Node::filter_visitor choosePred,
    Node::filter_visitor recursePred) const {
  if (!good()) {
    return Node();
  }
  Node result;
  dfs(recursePred,
    [choosePred, &result](const Node& node, Node::escape_func escaper) {
      if (choosePred(node)) {
        result = node;
        escaper();
      }
    }
  );
  return result;
}

Node Node::dfFindFirst(Node::filter_visitor choosePred) const {
  return dfFindFirst(choosePred, util::always<Node>);
}

uintptr_t Node::getGumboUintPtr() const {
  return (uintptr_t) getGumboNode();
}

bool Node::operator!=(const Node &other) const {
  return getGumboUintPtr() != other.getGumboUintPtr();
}

bool Node::operator<(const Node &other) const {
  return getGumboUintPtr() < other.getGumboUintPtr();
}

bool Node::nodeIsElement(const Node &node) {
  return node.isElement();
}

bool Node::nodeIsText(const Node &node) {
  return node.isText();
}

bool Node::nodeIsWhitespace(const Node &node) {
  return node.isWhitespace();
}

bool Node::nodeHasTag(Tag tag, const Node &node) {
  return node.hasTag(tag);
}

function<bool (const Node&)> Node::nodeHasTag(Tag tag) {
  return [tag](const Node& node) {
    return Node::nodeHasTag(tag, node);
  };
}

}} // score::html
