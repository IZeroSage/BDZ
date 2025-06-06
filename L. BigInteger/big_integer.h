#ifndef BIG_INTEGER_HPP
#define BIG_INTEGER_HPP
#define BIG_INTEGER_DIVISION_IMPLEMENTED

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include <cstdint>

class BigIntegerOverflow : public std::runtime_error {
 public:
  BigIntegerOverflow() : std::runtime_error("BigIntegerOverflow") {
  }
};

class BigIntegerDivisionByZero : public std::runtime_error {
 public:
  BigIntegerDivisionByZero() : std::runtime_error("BigIntegerDivisionByZero") {
  }
};

class BigInteger {
 private:
  using Digit = uint32_t;
  static constexpr Digit kBase = 1000000000;

  std::vector<Digit> m_digits_;
  bool m_negative_;

  void NormalizeDigits();
  void ReserveCapacity(size_t required);
  void Decrement();
  void AddWithSign(const BigInteger& second, bool subtract);
  bool AbsLess(const BigInteger& second) const;

 public:
  BigInteger();
  BigInteger(int val);  // NOLINT
  explicit BigInteger(uint32_t value);
  explicit BigInteger(int64_t value);
  explicit BigInteger(const std::string& input);
  explicit BigInteger(const char* str);

  bool IsNegative() const;
  explicit operator bool() const;

  BigInteger operator+() const;
  BigInteger operator-() const;

  BigInteger operator+(const BigInteger& rhs) const;
  BigInteger operator-(const BigInteger& rhs) const;
  BigInteger operator*(const BigInteger& rhs) const;
  BigInteger operator/(const BigInteger& denom) const;
  BigInteger operator%(const BigInteger& denom) const;

  BigInteger& operator+=(const BigInteger& addend);
  BigInteger& operator-=(const BigInteger& subtrahend);
  BigInteger& operator*=(const BigInteger& factor);
  BigInteger& operator/=(const BigInteger& denom);
  BigInteger& operator%=(const BigInteger& denom);

  BigInteger& operator++();
  BigInteger operator++(int);
  BigInteger& operator--();
  BigInteger operator--(int);

  bool operator==(const BigInteger& rhs) const;
  bool operator!=(const BigInteger& rhs) const;
  bool operator<(const BigInteger& rhs) const;
  bool operator<=(const BigInteger& rhs) const;
  bool operator>(const BigInteger& rhs) const;
  bool operator>=(const BigInteger& rhs) const;

  friend std::istream& operator>>(std::istream& in, BigInteger& num);
  friend std::ostream& operator<<(std::ostream& out, const BigInteger& num);
};

#endif  // BIG_INTEGER_HPP //