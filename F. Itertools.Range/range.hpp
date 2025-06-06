#ifndef RANGE_HPP
#define RANGE_HPP
#define REVERSE_RANGE_IMPLEMENTED

#include <stdexcept>

class ArrayOutOfRange : public std::out_of_range {
public:
  ArrayOutOfRange() : std::out_of_range("ArrayOutOfRange") {}
};

class Iterator {
private:
  int data_ = 0;
  int step_ = 0;

public:
  Iterator(int data, int step) : data_(data), step_(step) {}

  int operator*() const { return data_; }

  Iterator &operator++() {
    data_ += step_;
    return *this;
  }

  bool operator!=(const Iterator &other) const {
    if (step_ > 0) {
      return data_ < other.data_;
    }
    return data_ > other.data_;
  }
};

class Range {
private:
  int begin_ = 0;
  int end_ = 0;
  int step_ = 1;
  bool flag_ = false;

public:
  explicit Range(int end) : Range(0, end, 1) {}
  Range(int begin, int end) : Range(begin, end, 1) {}

  Range(int begin, int end, int step) : begin_(begin), end_(end), step_(step) {
    if (step == 0) {
      flag_ = true;
      begin_ = end_ = 0;
      step_ = 1;
    } else if ((step < 0 && begin <= end) || (step > 0 && begin >= end)) {
      flag_ = true;
    }
  }

  Iterator begin() const { // NOLINT
    if (flag_) {
      return {0, step_};
    }
    return {begin_, step_};
  }

  Iterator end() const { // NOLINT
    if (flag_) {
      return {0, step_};
    }
    return {end_, step_};
  }

  Iterator rbegin() const { // NOLINT
    if (flag_) {
      return {0, -step_};
    }
    int delta = end_ - begin_;
    int count = delta / step_;
    if (delta % step_ != 0) {
      ++count;
    }

    int last = begin_ + (count - 1) * step_;
    return {last, -step_};
  }

  Iterator rend() const { // NOLINT
    if (flag_) {
      return {0, -step_};
    }
    return {begin_ - step_, -step_};
  }
};

#endif // RANGE_HPP //
