#pragma once
#include <type_traits>

// inspired (mostly stolen) from seastar.
// see:  https://github.com/scylladb/seastar/blob/master/core/shared_ptr.hh
// this is adapted from the second (the least "lightweight") of the two
// single threaded shared_ptr implementations they have there.

// this is a shared pointer type that can only be used from a single thread.
// it doesn't use atomic operations to incr/decr refcounts.
// For objects that never need to travel between threads but do need a
// refcount, this saves on a lot of cache traffic.
// For objects that do need to travel between threads, don't be stupid.

namespace score { namespace mem {

struct types {
  using shared_counter_t = size_t;
};

template<typename T>
class st_shared_ptr;

template<typename T>
class enable_st_shared_from_this;

template<typename T>
st_shared_ptr<T> make_st_shared(T&);

template<typename T>
st_shared_ptr<T> make_st_shared(T&&);

template<typename T, typename... Args>
st_shared_ptr<T> make_st_shared(Args&&...);

template<typename T, typename U>
st_shared_ptr<T> static_pointer_cast(const st_shared_ptr<U>&);

template<typename T, typename U>
st_shared_ptr<T> dynamic_pointer_cast(const st_shared_ptr<U>&);

template<typename T, typename U>
st_shared_ptr<T> const_pointer_cast(const st_shared_ptr<U>&);


struct st_shared_ptr_count_base {
  virtual ~st_shared_ptr_count_base() = default;
  typename types::shared_counter_t count {0};
};

template<typename T>
struct st_shared_ptr_count_for: st_shared_ptr_count_base {
  T data;
  template<typename ...Types>
  st_shared_ptr_count_for(Types&&... args)
    : data(std::forward<Types>(args)...){}
};

template<typename T>
class enable_st_shared_from_this: private st_shared_ptr_count_base {
 public:
  using value_type = T;
  using pointer = T*;

  st_shared_ptr<T> shared_from_this();
  st_shared_ptr<const T> shared_from_this() const;

  template<typename U>
  friend class st_shared_ptr;

  template<typename U, bool esft>
  friend struct st_shared_ptr_make_helper;

};


template<typename T>
class st_shared_ptr {
  mutable st_shared_ptr_count_base *base_ {nullptr};
  mutable typename std::remove_const<T>::type *data_ {nullptr};
 public:
  using value_type = T;
  using non_const_value = typename std::remove_const<T>::type;
  using pointer = non_const_value*;
 private:
  explicit st_shared_ptr(st_shared_ptr_count_for<T>* base) noexcept
    : base_(base), data_(&base->data) {
    base_->count++;
  }
  st_shared_ptr(st_shared_ptr_count_base *base, T *data) noexcept
    : base_(base), data_(data) {
    if (base_) {
      base_->count++;
    }
  }
  explicit st_shared_ptr(enable_st_shared_from_this<non_const_value> *other) noexcept
    : base_(other), data_(static_cast<T*>(other)) {
    if (base_) {
      base_->count++;
    }
  }
 public:
  st_shared_ptr() noexcept = default;
  st_shared_ptr(std::nullptr_t) noexcept : st_shared_ptr(){}
  st_shared_ptr(const st_shared_ptr& other) noexcept
    : base_(other.base_), data_(other.data_) {
    if (base_) {
      base_->count++;
    }
  }
  st_shared_ptr(st_shared_ptr&& other) noexcept
    : base_(other.base_), data_(other.data_) {
    other.base_ = nullptr;
    other.data_ = nullptr;
  }

  template<typename U,
    typename std::enable_if<std::is_base_of<T, U>::value, U>::type>
  st_shared_ptr(const st_shared_ptr<U>& other) noexcept
    : base_(other.base_), data_(other.data_) {
    if (base_) {
      base_->count++;
    }
  }

  template<typename U,
    typename std::enable_if<std::is_base_of<T, U>::value, U>::type>
  st_shared_ptr(st_shared_ptr<U>&& other) noexcept
    : base_(other.base_), data_(other.data_) {
    other.base_ = nullptr;
    other.data = nullptr;
  }

  ~st_shared_ptr() {
    if (base_) {
      base_->count--;
      if (base_->count == 0) {
        delete base_;
      }
    }
  }

  st_shared_ptr& operator=(const st_shared_ptr& other) noexcept {
    if (this != &other) {
      this->~st_shared_ptr();
      new (this) st_shared_ptr(other);
    }
    return *this;
  }

  st_shared_ptr& operator=(st_shared_ptr&& other) noexcept {
    if (this != &other) {
      this->~st_shared_ptr();
      new (this) st_shared_ptr(std::move(other));
    }
    return *this;
  }
  st_shared_ptr& operator=(std::nullptr_t) noexcept {
    return *this = st_shared_ptr();
  }

  template<typename U,
    typename std::enable_if<std::is_base_of<T, U>::value, U>::type>
  st_shared_ptr& operator=(const st_shared_ptr<U>& other) noexcept {
    if (*this != other) {
      this->~st_shared_ptr();
      new (this) st_shared_ptr(other);
    }
    return *this;
  }

  template<typename U,
    typename std::enable_if<std::is_base_of<T, U>::value, U>::type>
  st_shared_ptr& operator=(st_shared_ptr<U>&& other) noexcept {
    if (*this != other) {
      this->~st_shared_ptr();
      new (this) st_shared_ptr(std::move(other));
    }
    return *this;
  }

  explicit operator bool() const noexcept {
    return data_;
  }

  T& operator*() const noexcept {
    return *data_;
  }
  T* operator->() const noexcept {
    return data_;
  }
  T* get() const noexcept {
    return data_;
  }
  typename types::shared_counter_t useCount() const noexcept {
    if (base_) {
      return base_->count;
    }
    return 0;
  }

  template<bool esft>
  struct make_helper;

  template<typename U, typename... Types>
  friend st_shared_ptr<U> make_st_shared(Types&&...);

  template<typename U>
  friend st_shared_ptr<U> make_st_shared(U&&);

  template<typename U>
  friend st_shared_ptr<U> make_st_shared(U&);

  template<typename V, typename U>
  friend st_shared_ptr<V> static_pointer_cast(const st_shared_ptr<U>&);

  template<typename V, typename U>
  friend st_shared_ptr<V> dynamic_pointer_cast(const st_shared_ptr<U>&);

  template<typename V, typename U>
  friend st_shared_ptr<V> const_pointer_cast(const st_shared_ptr<U>&);

  template<bool efst, typename... Types>
  static st_shared_ptr make(Types&&...);

  template<typename U>
  friend class enable_st_shared_from_this;

  template<typename U, bool esft>
  friend struct st_shared_ptr_make_helper;

  template<typename U>
  friend class st_shared_ptr;

};


template<typename U, bool efst>
struct st_shared_ptr_make_helper;


template<typename T>
struct st_shared_ptr_make_helper<T, false> {
  template<typename ...Types>
  static st_shared_ptr<T> make(Types&& ...args) {
    return st_shared_ptr<T>(new st_shared_ptr_count_for<T>(
      std::forward<Types>(args)...
    ));
  }
};



template<typename T>
struct st_shared_ptr_make_helper<T, true> {
  template<typename ...Types>
  static st_shared_ptr<T> make(Types&& ...args) {
    auto ptr = new T(std::forward<Types>(args)...);
    return st_shared_ptr<T>(ptr, ptr);
  }
};


template<typename T, typename ...Types>
st_shared_ptr<T> make_st_shared(Types&&... args) {
  using helper = st_shared_ptr_make_helper<
    T, std::is_base_of<st_shared_ptr_count_base, T>::value
  >;
  return helper::make(std::forward<Types>(args)...);
}

template<typename T>
st_shared_ptr<T> make_st_shared(T&& ref) {
  using helper = st_shared_ptr_make_helper<
    T, std::is_base_of<st_shared_ptr_count_base, T>::value
  >;
  return helper::make(std::forward<T>(ref));
}


template<typename T, typename U>
st_shared_ptr<T> static_pointer_cast(const st_shared_ptr<U>& p) {
  return st_shared_ptr<T>(p.base_, static_cast<T*>(p.data_));
}

template<typename T, typename U>
st_shared_ptr<T> dynamic_pointer_cast(const st_shared_ptr<U>& p) {
  auto q = dynamic_cast<T*>(p.base_);
  if (q) {
    return st_shared_ptr<T>(p.base_, q);
  }
  return st_shared_ptr<T>(nullptr, q);
}

template<typename T, typename U>
st_shared_ptr<T> const_pointer_cast(const st_shared_ptr<U>& p) {
  return st_shared_ptr<T>(p.base_, const_cast<T*>(p.data_));
}


template<typename T>
st_shared_ptr<T> enable_st_shared_from_this<T>::shared_from_this() {
  auto unconst = reinterpret_cast<enable_st_shared_from_this<
    typename std::remove_const<T>::type
  >*>(this);
  return st_shared_ptr<T>(unconst);
}

template<typename T>
st_shared_ptr<const T> enable_st_shared_from_this<T>::shared_from_this() const {
  auto esft = const_cast<enable_st_shared_from_this*>(this);
  auto unconst = reinterpret_cast<enable_st_shared_from_this<
    typename std::remove_const<T>::type
  >*>(esft);
  return st_shared_ptr<const T>(unconst);
}

template<typename T, typename U>
bool operator==(const st_shared_ptr<T>& x, const st_shared_ptr<U>& y) {
  return x.get() == y.get();
}

template<typename T, typename U>
bool operator!=(const st_shared_ptr<T>& x, const st_shared_ptr<U>& y) {
  return x.get() != y.get();
}

template<typename T, typename U>
bool operator<(const st_shared_ptr<T>& x, const st_shared_ptr<U>& y) {
  return x.get() < y.get();
}

template<typename T, typename U>
bool operator<=(const st_shared_ptr<T>& x, const st_shared_ptr<U>& y) {
  return x.get() <= y.get();
}

template<typename T, typename U>
bool operator>(const st_shared_ptr<T>& x, const st_shared_ptr<U>& y) {
  return x.get() > y.get();
}

template<typename T, typename U>
bool operator>=(const st_shared_ptr<T>& x, const st_shared_ptr<U>& y) {
  return x.get() >= y.get();
}


template<typename T>
bool operator==(const st_shared_ptr<T>& x, std::nullptr_t) {
  return x.get() == nullptr;
}

template<typename T>
bool operator!=(const st_shared_ptr<T>& x, std::nullptr_t) {
  return x.get() != nullptr;
}

template<typename T>
bool operator<(const st_shared_ptr<T>& x, std::nullptr_t) {
  return x.get() < nullptr;
}

template<typename T>
bool operator<=(const st_shared_ptr<T>& x, std::nullptr_t) {
  return x.get() <= nullptr;
}

template<typename T>
bool operator>(const st_shared_ptr<T>& x, std::nullptr_t) {
  return x.get() > nullptr;
}

template<typename T>
bool operator>=(const st_shared_ptr<T>& x, std::nullptr_t) {
  return x.get() >= nullptr;
}

template<typename T>
bool operator==(std::nullptr_t, const st_shared_ptr<T>& y) {
  return nullptr == y.get();
}

template<typename T>
bool operator!=(std::nullptr_t, const st_shared_ptr<T>& y) {
  return nullptr != y.get();
}


template<typename T>
bool operator<(std::nullptr_t, const st_shared_ptr<T>& y) {
  return nullptr < y.get();
}

template<typename T>
bool operator<=(std::nullptr_t, const st_shared_ptr<T>& y) {
  return nullptr <= y.get();
}

template<typename T>
bool operator>(std::nullptr_t, const st_shared_ptr<T>& y) {
  return nullptr > y.get();
}

template<typename T>
bool operator>=(std::nullptr_t, const st_shared_ptr<T>& y) {
  return nullptr >= y.get();
}

}} // score::mem


// <-- ENTER GLOBAL SCOPE
template<typename T>
static inline std::ostream& operator<<(std::ostream& oss,
    const score::mem::st_shared_ptr<T>& ptr) {
  if (!ptr) {
    return oss << "nullptr";
  }
  return oss << *ptr;
}
// EXIT GLOBAL SCOPE -->

namespace std {

template<typename T>
struct hash<score::mem::st_shared_ptr<T>>: private hash<T*> {
  size_t operator()(const score::mem::st_shared_ptr<T>& ptr) const {
    return hash<T*>::operator()(ptr.get());
  }
};

}