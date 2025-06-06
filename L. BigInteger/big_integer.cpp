#include "big_integer.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <cstdint>

void BigInteger::NormalizeDigits() {
  while (!m_digits_.empty() && m_digits_.back() == 0) {
    m_digits_.pop_back();
  }
  if (m_digits_.empty()) {
    m_digits_.push_back(0);
    m_negative_ = false;
  }
}

void BigInteger::ReserveCapacity(size_t required) {
  if (m_digits_.size() < required) {
    m_digits_.resize(required, 0);
  }
}

void BigInteger::Decrement() {
  if (m_digits_.empty()) {
    m_digits_.push_back(1);
    m_negative_ = true;
    return;
  }
  size_t index = 0;
  while (index < m_digits_.size() && m_digits_[index] == 0) {
    m_digits_[index] = kBase - 1;
    index++;
  }
  if (index < m_digits_.size()) {
    m_digits_[index]--;
  } else {
    m_digits_.push_back(1);
    m_negative_ = true;
  }
  NormalizeDigits();
}

void BigInteger::AddWithSign(const BigInteger& second, bool subtract) {
  if (second.m_digits_.empty()) {
    return;
  }
  size_t max_length = 0;
  max_length = std::max(m_digits_.size(), second.m_digits_.size());
  ReserveCapacity(max_length + 1);
  std::vector<Digit> other_copy = second.m_digits_;
  if (other_copy.size() < max_length + 1) {
    other_copy.resize(max_length + 1, 0);
  }
  int64_t carry_value = 0;
  for (size_t pos = 0; pos <= max_length; ++pos) {
    auto term1 = static_cast<int64_t>(m_digits_[pos]);
    int64_t term2 = subtract ? -static_cast<int64_t>(other_copy[pos]) : static_cast<int64_t>(other_copy[pos]);
    int64_t total = term1 + term2 + carry_value;
    if (total >= 0) {
      m_digits_[pos] = static_cast<Digit>(total % kBase);
      carry_value = total / kBase;
    } else {
      m_digits_[pos] = static_cast<Digit>((total + kBase) % kBase);
      carry_value = -1;
    }
  }
  if (carry_value < 0) {
    m_negative_ = !m_negative_;
    carry_value = 0;
    for (size_t pos = 0; pos <= max_length; ++pos) {
      int64_t diff_val = -static_cast<int64_t>(m_digits_[pos]) + carry_value;
      if (diff_val >= 0) {
        m_digits_[pos] = static_cast<Digit>(diff_val % kBase);
        carry_value = diff_val / kBase;
      } else {
        m_digits_[pos] = static_cast<Digit>((diff_val + kBase) % kBase);
        carry_value = -1;
      }
    }
  } else if (carry_value > 0) {
    throw BigIntegerOverflow{};
  }
  NormalizeDigits();
}

BigInteger::BigInteger() : m_digits_{}, m_negative_(false) {
}

BigInteger::BigInteger(int val) : BigInteger(static_cast<int64_t>(val)) {
}

BigInteger::BigInteger(uint32_t value) : m_digits_{}, m_negative_(false) {
  if (value == 0) {
    m_digits_.push_back(0);
  } else {
    while (value > 0) {
      m_digits_.push_back(static_cast<Digit>(value % kBase));
      value /= kBase;
    }
  }
}

BigInteger::BigInteger(int64_t value) : m_digits_{}, m_negative_(value < 0) {
  value = value < 0 ? -value : value;
  if (value == 0) {
    m_digits_.push_back(0);
  } else {
    while (value > 0) {
      m_digits_.push_back(static_cast<Digit>(value % kBase));
      value /= kBase;
    }
  }
}

BigInteger::BigInteger(const std::string& input) : m_digits_{}, m_negative_(false) {
  if (input.empty()) {
    *this = BigInteger(0);
    return;
  }
  size_t start_index = 0;
  if (input[0] == '-') {
    m_negative_ = true;
    start_index = 1;
  } else if (input[0] == '+') {
    start_index = 1;
  }
  if (start_index >= input.size()) {
    throw std::invalid_argument("Invalid number format");
  }
  size_t non_zero_index = start_index;
  while (non_zero_index < input.size() && input[non_zero_index] == '0') {
    non_zero_index++;
  }
  if (non_zero_index == input.size()) {
    *this = BigInteger(0);
    return;
  }
  for (size_t i = non_zero_index; i < input.size(); ++i) {
    if (!std::isdigit(input[i])) {
      throw std::invalid_argument("Non-digit character found");
    }
  }
  std::string clean_num = input.substr(non_zero_index);
  if (clean_num.length() > 30000) {
    throw BigIntegerOverflow();
  }
  size_t num_length = clean_num.size();
  size_t position = num_length;
  while (position > 0) {
    size_t segment_start = (position > 9) ? position - 9 : 0;
    size_t segment_length = (position > 9) ? 9 : position;
    std::string segment = clean_num.substr(segment_start, segment_length);
    try {
      m_digits_.push_back(static_cast<Digit>(std::stoull(segment)));
    } catch (const std::out_of_range&) {
      throw BigIntegerOverflow();
    }
    position -= segment_length;
  }
  NormalizeDigits();
}

BigInteger::BigInteger(const char* str) {
  if (!str || *str == '\0') {
    *this = BigInteger(0);
    return;
  }
  std::string s(str);
  *this = BigInteger(s);
}

bool BigInteger::IsNegative() const {
  return m_negative_;
}

BigInteger BigInteger::operator+() const {
  return {*this};
}

BigInteger BigInteger::operator-() const {
  BigInteger result = *this;
  if (!result.m_digits_.empty()) {
    result.m_negative_ = !m_negative_;
  }
  return result;
}

BigInteger BigInteger::operator+(const BigInteger& rhs) const {
  BigInteger result = *this;
  result += rhs;
  return result;
}

BigInteger BigInteger::operator-(const BigInteger& rhs) const {
  BigInteger result = *this;
  result -= rhs;
  return result;
}

BigInteger BigInteger::operator*(const BigInteger& rhs) const {
  if (m_digits_.empty() || rhs.m_digits_.empty()) {
    return {0};
  }
  size_t total_size = 0;
  total_size = m_digits_.size() + rhs.m_digits_.size();
  if (total_size > 30000 / 9 + 2) {
    throw BigIntegerOverflow{};
  }
  BigInteger product;
  product.m_negative_ = m_negative_ != rhs.m_negative_;
  product.m_digits_.resize(m_digits_.size() + rhs.m_digits_.size(), 0);
  for (size_t i_index = 0; i_index < m_digits_.size(); ++i_index) {
    uint64_t carry = 0;
    size_t j_index = 0;
    while (j_index < rhs.m_digits_.size() || carry != 0) {
      uint64_t current = product.m_digits_[i_index + j_index] + carry;
      if (j_index < rhs.m_digits_.size()) {
        current += static_cast<uint64_t>(m_digits_[i_index]) * rhs.m_digits_[j_index];
      }
      if (current >= kBase) {
        product.m_digits_[i_index + j_index] = static_cast<Digit>(current % kBase);
        carry = current / kBase;
        if ((j_index >= rhs.m_digits_.size() - 1) && carry > 0 && (i_index + j_index + 1 >= 30000 / 9)) {
          throw BigIntegerOverflow{};
        }
      } else {
        product.m_digits_[i_index + j_index] = static_cast<Digit>(current);
        carry = 0;
      }
      j_index++;
    }
  }
  product.NormalizeDigits();
  if (product.m_digits_.size() > 30000 / 9 + 1) {
    throw BigIntegerOverflow{};
  }
  return product;
}

BigInteger BigInteger::operator/(const BigInteger& denom) const {
  if (denom == BigInteger(0)) {
    throw BigIntegerDivisionByZero();
  }
  bool result_sign = (m_negative_ != denom.m_negative_);
  BigInteger dividend = *this;
  BigInteger divisor = denom;
  dividend.m_negative_ = false;
  divisor.m_negative_ = false;
  if (dividend.AbsLess(divisor)) {
    return {0};
  }
  BigInteger quotient(0);
  BigInteger current_value(0);
  int i_index = static_cast<int>(dividend.m_digits_.size()) - 1;
  while (i_index >= 0) {
    current_value = current_value * BigInteger(kBase);
    current_value += BigInteger(static_cast<uint32_t>(dividend.m_digits_[i_index]));
    Digit low_bound = 0;
    Digit high_bound = kBase - 1;
    Digit mid_val = 0;
    Digit best_fit = 0;
    while (low_bound <= high_bound) {
      mid_val = low_bound + (high_bound - low_bound) / 2;
      BigInteger trial = divisor * BigInteger(static_cast<uint32_t>(mid_val));
      if (trial <= current_value) {
        best_fit = mid_val;
        low_bound = mid_val + 1;
      } else {
        high_bound = mid_val - 1;
      }
    }
    quotient = quotient * BigInteger(kBase) + BigInteger(static_cast<uint32_t>(best_fit));
    current_value -= divisor * BigInteger(static_cast<uint32_t>(best_fit));
    i_index--;
  }
  quotient.m_negative_ = result_sign;
  quotient.NormalizeDigits();
  return quotient;
}

BigInteger BigInteger::operator%(const BigInteger& denom) const {
  if (denom == BigInteger(0)) {
    throw BigIntegerDivisionByZero();
  }
  return *this - (*this / denom) * denom;
}

BigInteger& BigInteger::operator/=(const BigInteger& denom) {
  return *this = *this / denom;
}

BigInteger& BigInteger::operator%=(const BigInteger& denom) {
  return *this = *this % denom;
}

BigInteger& BigInteger::operator+=(const BigInteger& addend) {
  if (addend.m_digits_.empty()) {
    return *this;
  }
  if (m_negative_ != addend.m_negative_) {
    if (AbsLess(addend)) {
      BigInteger temp = addend;
      temp -= *this;
      *this = temp;
      m_negative_ = addend.m_negative_;
    } else {
      *this -= -addend;
    }
    return *this;
  }
  size_t max_digits = 0;
  max_digits = std::max(m_digits_.size(), addend.m_digits_.size());
  m_digits_.resize(max_digits + 1, 0);
  uint64_t carry_val = 0;
  for (size_t digit_index = 0; digit_index <= max_digits; ++digit_index) {
    uint64_t sum_val = carry_val;
    if (digit_index < m_digits_.size()) {
      sum_val += m_digits_[digit_index];
    }
    if (digit_index < addend.m_digits_.size()) {
      sum_val += addend.m_digits_[digit_index];
    }
    m_digits_[digit_index] = static_cast<Digit>(sum_val % kBase);
    carry_val = sum_val / kBase;
  }
  if (carry_val > 0) {
    throw BigIntegerOverflow();
  }
  NormalizeDigits();
  return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& subtrahend) {
  if (*this == subtrahend) {
    *this = BigInteger(0);
    return *this;
  }
  if (m_negative_ != subtrahend.m_negative_) {
    return *this += -subtrahend;
  }
  bool this_smaller = AbsLess(subtrahend);
  if (this_smaller) {
    BigInteger temp = subtrahend;
    temp -= *this;
    *this = temp;
    m_negative_ = !m_negative_;
  } else {
    int64_t borrow = 0;
    for (size_t digit_pos = 0; digit_pos < m_digits_.size(); ++digit_pos) {
      auto minuend = static_cast<int64_t>(m_digits_[digit_pos]);
      int64_t subtrahend_val =
          (digit_pos < subtrahend.m_digits_.size()) ? static_cast<int64_t>(subtrahend.m_digits_[digit_pos]) : 0;
      int64_t diff_val = minuend - subtrahend_val - borrow;
      if (diff_val < 0) {
        diff_val += kBase;
        borrow = 1;
      } else {
        borrow = 0;
      }
      m_digits_[digit_pos] = static_cast<Digit>(diff_val);
    }
  }
  NormalizeDigits();
  return *this;
}

BigInteger& BigInteger::operator*=(const BigInteger& factor) {
  size_t result_size = 0;
  result_size = m_digits_.size() + factor.m_digits_.size();
  if (result_size > 30000 / 9 + 2) {
    throw BigIntegerOverflow{};
  }
  *this = *this * factor;
  return *this;
}

BigInteger& BigInteger::operator++() {
  *this += BigInteger(1);
  return *this;
}

BigInteger BigInteger::operator++(int) {
  BigInteger temp = *this;
  ++(*this);
  return temp;
}

BigInteger& BigInteger::operator--() {
  if (m_negative_) {
    for (size_t i = 0; i < m_digits_.size(); ++i) {
      if (m_digits_[i] == kBase - 1) {
        m_digits_[i] = 0;
      } else {
        m_digits_[i]++;
        return *this;
      }
    }
    m_digits_.push_back(1);
  } else {
    if (m_digits_.empty() || (m_digits_.size() == 1 && m_digits_[0] == 0)) {
      *this = BigInteger(1);
      m_negative_ = true;
      return *this;
    }
    size_t pos = 0;
    while (pos < m_digits_.size() && m_digits_[pos] == 0) {
      m_digits_[pos] = kBase - 1;
      pos++;
    }
    if (pos < m_digits_.size()) {
      m_digits_[pos]--;
    }
  }
  NormalizeDigits();
  return *this;
}

BigInteger BigInteger::operator--(int) {
  BigInteger temp = *this;
  --(*this);
  return temp;
}

BigInteger::operator bool() const {
  return !m_digits_.empty() && (m_digits_.size() > 1 || m_digits_[0] != 0);
}

bool BigInteger::operator==(const BigInteger& rhs) const {
  if (m_negative_ != rhs.m_negative_) {
    return false;
  }
  if (m_digits_.empty()) {
    return rhs.m_digits_.empty() || (rhs.m_digits_.size() == 1 && rhs.m_digits_[0] == 0);
  }
  if (rhs.m_digits_.empty()) {
    return m_digits_.size() == 1 && m_digits_[0] == 0;
  }
  if (m_digits_.size() != rhs.m_digits_.size()) {
    return false;
  }
  for (size_t i = 0; i < m_digits_.size(); ++i) {
    if (m_digits_[i] != rhs.m_digits_[i]) {
      return false;
    }
  }
  return true;
}

bool BigInteger::operator!=(const BigInteger& rhs) const {
  return !(*this == rhs);
}

bool BigInteger::operator<(const BigInteger& rhs) const {
  if (m_negative_ != rhs.m_negative_) {
    return m_negative_;
  }
  if (m_digits_.size() != rhs.m_digits_.size()) {
    return (m_negative_) ? m_digits_.size() > rhs.m_digits_.size() : m_digits_.size() < rhs.m_digits_.size();
  }
  for (size_t i = m_digits_.size(); i-- > 0;) {
    if (m_digits_[i] != rhs.m_digits_[i]) {
      return (m_negative_) ? m_digits_[i] > rhs.m_digits_[i] : m_digits_[i] < rhs.m_digits_[i];
    }
  }
  return false;
}

bool BigInteger::operator<=(const BigInteger& rhs) const {
  return (*this < rhs) || (*this == rhs);
}

bool BigInteger::operator>(const BigInteger& rhs) const {
  return !(*this <= rhs);
}

bool BigInteger::operator>=(const BigInteger& rhs) const {
  return !(*this < rhs);
}

bool BigInteger::AbsLess(const BigInteger& second) const {
  size_t this_size = m_digits_.size();
  size_t other_size = second.m_digits_.size();
  while (this_size > 1 && m_digits_[this_size - 1] == 0) {
    this_size--;
  }
  while (other_size > 1 && second.m_digits_[other_size - 1] == 0) {
    other_size--;
  }
  if (this_size != other_size) {
    return this_size < other_size;
  }
  for (size_t i = this_size; i-- > 0;) {
    if (m_digits_[i] != second.m_digits_[i]) {
      return m_digits_[i] < second.m_digits_[i];
    }
  }
  return false;
}

std::istream& operator>>(std::istream& in, BigInteger& num) {
  std::string str_val;
  in >> str_val;
  num = BigInteger(str_val);
  return in;
}

std::ostream& operator<<(std::ostream& out, const BigInteger& num) {
  if (num.m_digits_.empty()) {
    out << "0";
    return out;
  }
  if (num.m_negative_) {
    out << "-";
  }
  out << num.m_digits_.back();
  for (auto it = num.m_digits_.rbegin() + 1; it != num.m_digits_.rend(); ++it) {
    out.width(9);
    out.fill('0');
    out << *it;
  }
  return out;
}