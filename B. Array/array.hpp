#ifndef ARRAY_HPP
#define ARRAY_HPP
#define ARRAY_TRAITS_IMPLEMENTED


#include <stdexcept>

class ArrayOutOfRange : public std::out_of_range {
 public:
  ArrayOutOfRange() : std::out_of_range("ArrayOutOfRange") {
  }
};


template <typename T, size_t COUNT>
class Array {
  public:
  T arr_[COUNT];

  T& At(size_t ind) {
    if ((ind >= 0) && (ind < COUNT)) {
      return arr_[ind];
    }
    throw ArrayOutOfRange();
  }

  const T& At(size_t ind) const {
    if ((ind >= 0) && (ind < COUNT)) {
      return arr_[ind];
    }
    throw ArrayOutOfRange();
  }

  size_t Size() const {
    return COUNT;
  }

  bool Empty() const {
    return Size() == 0;
  }

  T& Back() {
    if (!Empty()) {
      return arr_[COUNT - 1];
    }
    throw ArrayOutOfRange();
  }

  const T& Back() const {
    if (!Empty()) {
      return arr_[COUNT - 1];
    }
    throw ArrayOutOfRange();
  }

  T& Front() {
    if (!Empty()) {
      return arr_[0];
    }
    throw ArrayOutOfRange();
  }

  const T& Front() const {
    if (!Empty()) {
      return arr_[0];
    }
    throw ArrayOutOfRange();
  }

  T* Data() {
    return arr_;
  }

  const T* Data() const {
    return arr_;
  }

  void Fill(const T& value) {
    for (size_t i = 0; i < COUNT; ++i) {
      arr_[i] = value;
    }
  }

  void Swap(Array<T, COUNT>& arr) {
    for (size_t i = 0; i < COUNT; ++i) {
      std::swap(arr_[i], arr.arr_[i]);
    }
  }

  T& operator[](size_t ind) {
    return At(ind);
  }

  const T& operator[](size_t ind) const {
    return At(ind);
  }
};
#endif // ARRAY_HPP //