#ifndef RATIONAL_HPP
#define RATIONAL_HPP

#include <iostream>
#include <stdexcept>
#include <cstdint>

class RationalDivisionByZero : public std::runtime_error {
public:
  RationalDivisionByZero() : std::runtime_error("RationalDivisionByZero") {}
};

class Rational {
public:
  Rational();
  Rational(int numerator); // NOLINT
  Rational(int numerator, int denominator); // NOLINT

  int GetNumerator() const;
  int GetDenominator() const;

  void SetNumerator(int value);
  void SetDenominator(int value);

  Rational& operator+=(const Rational& rhs);
  Rational& operator-=(const Rational& rhs);
  Rational& operator*=(const Rational& rhs);
  Rational& operator/=(const Rational& rhs);

  Rational operator+() const;
  Rational operator-() const;

  Rational& operator++();    // prefix
  Rational operator++(int);  // postfix
  Rational& operator--();    // prefix
  Rational operator--(int);  // postfix

  friend Rational operator+(Rational lhs, const Rational& rhs);
  friend Rational operator-(Rational lhs, const Rational& rhs);
  friend Rational operator*(Rational lhs, const Rational& rhs);
  friend Rational operator/(Rational lhs, const Rational& rhs);

  friend bool operator==(const Rational& lhs, const Rational& rhs);
  friend bool operator!=(const Rational& lhs, const Rational& rhs);
  friend bool operator<(const Rational& lhs, const Rational& rhs);
  friend bool operator<=(const Rational& lhs, const Rational& rhs);
  friend bool operator>(const Rational& lhs, const Rational& rhs);
  friend bool operator>=(const Rational& lhs, const Rational& rhs);

  friend std::ostream& operator<<(std::ostream& os, const Rational& obj);
  friend std::istream& operator>>(std::istream& is, Rational& obj);

private:
  int numer_ = 0;
  int denom_ = 1;

  void Normalize();
  static int SafeCast(int64_t value);
};

#endif // RATIONAL_HPP //