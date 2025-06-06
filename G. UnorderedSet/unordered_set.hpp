#ifndef unordered_set_h
#define unordered_set_h

#include <vector>
#include <forward_list>
#include <iterator>
#include <iostream>

template <typename T>
class UnorderedSet {

  std::vector<std::forward_list<T>> data_;
  int n_items_ = 0;

 public:
  UnorderedSet() = default;

  UnorderedSet(const UnorderedSet& set) = default;

  UnorderedSet(UnorderedSet&& set) noexcept : data_(std::move(set.data_)), n_items_(set.n_items_) {
    set.n_items_ = 0;
    set.data_.clear();
  }

  explicit UnorderedSet(size_t count) {
    data_ = std::vector<std::forward_list<T>>(count);
  }

  UnorderedSet(typename std::forward_list<T>::iterator begin, typename std::forward_list<T>::iterator end) {
    auto elem_cnt = std::distance(begin, end);
    data_ = std::vector<std::forward_list<T>>(elem_cnt);

    for (auto it = begin; it != end; ++it) {
      Insert(*it);
    }
  }

  UnorderedSet& operator=(const UnorderedSet& set) = default;

  UnorderedSet& operator=(UnorderedSet&& set) noexcept {
    if (this != &set) {
      data_ = std::move(set.data_);
      n_items_ = set.n_items_;
      set.Clear();
    }
    return *this;
  }

  size_t Size() const {
    return n_items_;
  }

  bool Empty() const {
    return Size() == 0;
  }

  void Clear() {
    data_.clear();
    n_items_ = 0;
  }

  size_t BucketCount() const {
    return data_.size();
  }

  size_t BucketSize(size_t ind) const {
    if (ind >= BucketCount()) {
      return 0;
    }
    return std::distance(data_[ind].begin(), data_[ind].end());
  }

  size_t Bucket(const T& key) const {
    return std::hash<T>{}(key) % BucketCount();
  }

  double LoadFactor() const {
    if (n_items_ == 0) {
      return 0;
    }
    return static_cast<double>(Size()) / BucketCount();
  }

  void Insert(const T& key) {
    if (Find(key)) {
      return;
    }
    if (BucketCount() == 0) {
      Rehash(1);
    }
    if (std::abs(LoadFactor() - 1.0) < 0.001) {
      Rehash(BucketCount() * 2);
    }
    auto ind = Bucket(key);
    data_[ind].push_front(key);
    ++n_items_;
  }

  void Insert(T&& key) {
    if (Find(key)) {
      return;
    }

    if (BucketCount() == 0) {
      Rehash(1);
    }
    if (std::abs(LoadFactor() - 1.0) < 0.001) {
      Rehash(BucketCount() * 2);
    }
    auto ind = Bucket(key);
    data_[ind].push_front(key);
    ++n_items_;
  }

  bool Find(const T& key) const {
    if (BucketCount() == 0) {
      return false;
    }
    auto ind = Bucket(key);
    for (auto it = data_[ind].begin(); it != data_[ind].end(); ++it) {
      if (key == *it) {
        return true;
      }
    }
    return false;
  }

  void Erase(const T& key) {
    if (Find(key)) {
      auto ind = Bucket(key);
      data_[ind].remove(key);
      --n_items_;
    }
  }

  void Rehash(size_t new_bucket_count) {
    if (new_bucket_count == BucketCount() || new_bucket_count < static_cast<size_t>(n_items_)) {
      return;
    }

    std::vector<std::forward_list<T>> new_data(new_bucket_count);
    for (auto bc : data_) {
      for (auto k : bc) {
        size_t ind = std::hash<T>{}(k) % new_bucket_count;
        new_data[ind].push_front(k);
      }
    }
    data_ = new_data;
  }

  void Reserve(size_t new_bucket_count) {
    if (new_bucket_count > BucketCount()) {
      Rehash(new_bucket_count);
    }
  }
};
#endif