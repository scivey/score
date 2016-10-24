#pragma once
#include <initializer_list>

namespace score {

template<typename TMap>
class DefaultValueMap {
 public:
  using key_type = typename TMap::key_type;
  using mapped_type = typename TMap::mapped_type;
  using value_type = typename TMap::value_type;
  using size_type = typename TMap::size_type;
  using difference_type = typename TMap::difference_type;
  using key_compare = typename TMap::key_compare;
  using value_compare = typename TMap::value_compare;
  using allocator_type = typename TMap::allocator_type;
  using reference = typename TMap::reference;
  using const_reference = typename TMap::const_reference;
  using pointer = typename TMap::pointer;
  using const_pointer = typename TMap::const_pointer;
  using iterator = typename TMap::iterator;
  using const_iterator = typename TMap::const_iterator;
  using reverse_iterator = typename TMap::reverse_iterator;
  using const_reverse_iterator = typename TMap::const_reverse_iterator;

 protected:
  mapped_type defaultVal_;
  TMap underlying_;
 public:
  explicit DefaultValueMap(mapped_type defaultVal)
    : defaultVal_(defaultVal) {}

  DefaultValueMap(const DefaultValueMap &other)
    : defaultVal_(other.defaultVal_), underlying_(other.underlying_) {}

  DefaultValueMap(DefaultValueMap &&other)
    : defaultVal_(other.defaultVal_),
      underlying_(std::move(other.underlying_)) {}

  DefaultValueMap(mapped_type defaultVal, const TMap &other)
    : defaultVal_(defaultVal), underlying_(other) {}

  DefaultValueMap(mapped_type defaultVal, TMap &&other)
    : defaultVal_(defaultVal),
      underlying_(std::move(other)) {}

  DefaultValueMap(mapped_type defaultVal,
      std::initializer_list<value_type> init)
    : defaultVal_(defaultVal),
      underlying_(std::move(init)) {}

  DefaultValueMap& operator=(const DefaultValueMap &other) {
    underlying_ = other.underlying_;
    defaultVal_ = other.defaultVal_;
    return *this;
  }

  DefaultValueMap& operator=(const TMap &other) {
    underlying_ = other;
    return *this;
  }

  DefaultValueMap& operator=(DefaultValueMap &&other) {
    underlying_ = std::move(other.underlying_);
    defaultVal_ = other.defaultVal_;
    return *this;
  }

  DefaultValueMap& operator=(TMap &&other) {
    underlying_ = other;
    return *this;
  }

  DefaultValueMap& operator=(std::initializer_list<value_type> initVals) {
    underlying_ = initVals;
    return *this;
  }

  bool has(const key_type &key) const {
    return count(key) > 0;
  }

  size_type count(const key_type &key) const {
    return underlying_.count(key);
  }

  mapped_type value_at(const key_type &key) const {
    if (has(key)) {
      return at(key);
    }
    return defaultVal_;
  }

  mapped_type at(const key_type &key) const {
    return value_at(key);
  }

  mapped_type& at(const key_type &key) {
    if (!has(key)) {
      underlying_.insert(std::make_pair(
        key, defaultVal_
      ));
    }
    return underlying_.at(key);
  }

  mapped_type& operator[](const key_type &key) {
    return at(key);
  }

  mapped_type operator[](const key_type &key) const {
    return value_at(key);
  }

  std::pair<iterator, bool> insert(const value_type &value) {
    return underlying_.insert(value);
  }

  template<typename P>
  std::pair<iterator, bool> insert(P &&value) {
    return underlying_.insert(std::move(value));
  }

  template<typename InputIt>
  void insert(InputIt first, InputIt last) {
    underlying_.insert(first, last);
  }

  void insert(std::initializer_list<value_type> ilist) {
    underlying_.insert(ilist);
  }

  iterator insert(const_iterator hint, const value_type &value) {
    return underlying_.insert(hint, value);
  }

  iterator find(const key_type &key) {
    auto found = underlying_.find(key);
    if (found != underlying_.end()) {
      return found;
    }
    underlying_.insert(std::make_pair(key, defaultVal_));
    return underlying_.find(key);
  }

  const_iterator find(const key_type &key) const {
    return underlying_.find(key);
  }

  std::pair<iterator, iterator> equal_range(const key_type &key) {
    return underlying_.equal_range(key);
  }

  std::pair<const_iterator, const_iterator> equal_range(
      const key_type &key) const {
    return underlying_.equal_range(key);
  }

  size_type erase(const key_type &key) {
    return underlying_.erase(key);
  }

  iterator erase(const_iterator pos) {
    return underlying_.erase(pos);
  }

  iterator erase(const_iterator first, const_iterator last) {
    return underlying_.erase(first, last);
  }

  iterator insert(const_iterator hint, value_type &&value) {
    return underlying_.insert(hint, std::move(value));
  }

  template<typename ...Args>
  std::pair<iterator, bool> emplace(Args&& ...args) {
    return underlying_.emplace(std::forward<Args>(args)...);
  }

  template<typename ...Args>
  std::pair<iterator, bool> emplace_hint(const_iterator hint,
      Args&& ...args) {
    return underlying_.emplace_hint(hint,
      std::forward<Args>(args)...
    );
  }


  iterator begin() {
    return underlying_.begin();
  }

  const_iterator begin() const {
    return underlying_.begin();
  }

  const_iterator cbegin() const {
    return underlying_.cbegin();
  }

  iterator end() {
    return underlying_.end();
  }

  const_iterator end() const {
    return underlying_.end();
  }

  const_iterator cend() const {
    return underlying_.cend();
  }

  size_type size() const {
    return underlying_.size();
  }

  size_type max_size() const {
    return underlying_.max_size();
  }

  bool empty() const {
    return underlying_.empty();
  }

  void swap(DefaultValueMap &other) {
    underlying_.swap(other.underlying_);
  }

  void swap(TMap &other) {
    underlying_.swap(other);
  }

  void clear() noexcept {
    underlying_.clear();
  }

  key_compare key_comp() const {
    return underlying_.key_comp();
  }

  value_compare value_comp() const {
    return underlying_.value_comp();
  }

  allocator_type get_allocator() const {
    return underlying_.get_allocator();
  }

  iterator lower_bound(const key_type &key) {
    return underlying_.lower_bound(key);
  }

  const_iterator lower_bound(const key_type &key) const {
    return underlying_.lower_bound(key);
  }

  iterator upper_bound(const key_type &key) {
    return underlying_.upper_bound(key);
  }

  const_iterator upper_bound(const key_type &key) const {
    return underlying_.upper_bound(key);
  }
};

}