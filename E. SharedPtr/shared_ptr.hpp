#ifndef SHARED_PTR_H
#define SHARED_PTR_H
#define WEAK_PTR_IMPLEMENTED

#include <stdexcept>
#include <cstddef>
#include <utility>

struct BadWeakPtr : public std::exception {
  const char* what() const noexcept override {
    return "BadWeakPtr";
  }
};

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;

template <typename T>
struct RefCounter {
  std::size_t strong_count_ = 1;
  std::size_t weak_count_ = 0;
};

template <typename T>
class SharedPtr {
 private:
  T* ptr_ = nullptr;
  RefCounter<T>* ref_ = nullptr;

  void ToTrash() {
    delete ptr_;
    ptr_ = nullptr;
    if (ref_->weak_count_ == 0) {
      delete ref_;
      ref_ = nullptr;
    }
  }

  void Release() {
    if ((ref_) && (--ref_->strong_count_ == 0)) {
      ToTrash();
    }
  }

 public:
  SharedPtr() = default;

  explicit SharedPtr(T* ptr) : ptr_(ptr) {
    if (ptr) {
      ref_ = new RefCounter<T>();
    }
  }

  SharedPtr(const SharedPtr& second_ptr) : ptr_(second_ptr.ptr_), ref_(second_ptr.ref_) {
    if (ref_) {
      ++ref_->strong_count_;
    }
  }

  SharedPtr(SharedPtr&& second_ptr) noexcept : ptr_(second_ptr.ptr_), ref_(second_ptr.ref_) {
    second_ptr.ptr_ = nullptr;
    second_ptr.ref_ = nullptr;
  }

  explicit SharedPtr(const WeakPtr<T>& weak_count) {
    if (!weak_count.ref_ || weak_count.ref_->strong_count_ == 0) {
      throw BadWeakPtr{};
    }
    ptr_ = weak_count.ptr_;
    ref_ = weak_count.ref_;
    ++ref_->strong_count_;
  }

  ~SharedPtr() {
    Release();
  }

  void Reset(T* ptr = nullptr) {
    Release();
    if (ptr) {
      ptr_ = ptr;
      ref_ = new RefCounter<T>();
      return;
    }
    ptr_ = nullptr;
    ref_ = nullptr;
  }

  void Swap(SharedPtr& second_ptr) {
    std::swap(ptr_, second_ptr.ptr_);
    std::swap(ref_, second_ptr.ref_);
  }

  T* Get() const {
    return ptr_;
  }

  std::size_t UseCount() const {
    return ref_ ? ref_->strong_count_ : 0;
  }

  SharedPtr& operator=(const SharedPtr& second_ptr) {
    if (this != &second_ptr) {
      Release();
      ptr_ = second_ptr.ptr_;
      ref_ = second_ptr.ref_;
      if (ref_) {
        ++ref_->strong_count_;
      }
    }
    return *this;
  }

  SharedPtr& operator=(SharedPtr&& second_ptr) noexcept {
    if (this != &second_ptr) {
      Release();
      ref_ = second_ptr.ref_;
      ptr_ = second_ptr.ptr_;
      second_ptr.ref_ = nullptr;
      second_ptr.ptr_ = nullptr;
    }
    return *this;
  }

  T& operator*() const {
    return *ptr_;
  }

  T* operator->() const {
    return ptr_;
  }

  explicit operator bool() const {
    return ptr_ != nullptr;
  }

  friend class WeakPtr<T>;
};

template <typename T>
class WeakPtr {
 private:
  T* ptr_ = nullptr;
  RefCounter<T>* ref_ = nullptr;

  void ToTrash() {
    delete ref_;
    ref_ = nullptr;
  }

  void Release() {
    if ((ref_) && (--ref_->weak_count_ == 0 && ref_->strong_count_ == 0)) {
      ToTrash();
    }
  }

 public:
  WeakPtr() = default;

  WeakPtr(const SharedPtr<T>& shared) : ptr_(shared.ptr_), ref_(shared.ref_) { // NOLINT
    if (ref_) {
      ++ref_->weak_count_;
    }
  }

  WeakPtr(const WeakPtr& second_ptr) : ptr_(second_ptr.ptr_), ref_(second_ptr.ref_) {
    if (ref_) {
      ++ref_->weak_count_;
    }
  }

  WeakPtr(WeakPtr&& second_ptr) noexcept : ptr_(second_ptr.ptr_), ref_(second_ptr.ref_) {
    second_ptr.ptr_ = nullptr;
    second_ptr.ref_ = nullptr;
  }

  ~WeakPtr() {
    Release();
  }

  void Reset() {
    Release();
    ptr_ = nullptr;
    ref_ = nullptr;
  }

  void Swap(WeakPtr& second_ptr) {
    std::swap(ptr_, second_ptr.ptr_);
    std::swap(ref_, second_ptr.ref_);
  }

  std::size_t UseCount() const {
    return ref_ ? ref_->strong_count_ : 0;
  }

  bool Expired() const {
    return !ref_ || ref_->strong_count_ == 0;
  }

  SharedPtr<T> Lock() const {
    if (!Expired()) {
      return SharedPtr<T>(*this);
    }
    return SharedPtr<T>();
  }

  WeakPtr& operator=(const WeakPtr& second_ptr) {
    if (this != &second_ptr) {
      Release();
      ptr_ = second_ptr.ptr_;
      ref_ = second_ptr.ref_;
      if (ref_) {
        ++ref_->weak_count_;
      }
    }
    return *this;
  }

  WeakPtr& operator=(WeakPtr&& second_ptr) noexcept {
    if (this != &second_ptr) {
      Release();
      ptr_ = second_ptr.ptr_;
      ref_ = second_ptr.ref_;
      second_ptr.ptr_ = nullptr;
      second_ptr.ref_ = nullptr;
    }
    return *this;
  }

  WeakPtr& operator=(const SharedPtr<T>& shared) {
    if (ref_ != shared.ref_) {
      Release();
      ptr_ = shared.ptr_;
      ref_ = shared.ref_;
      if (ref_) {
        ++ref_->weak_count_;
      }
    }
    return *this;
  }

  friend class SharedPtr<T>;
};

template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
  return SharedPtr<T>(new T(std::forward<Args>(args)...));
}

#endif  // SHARED_PTR_H