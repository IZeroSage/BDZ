#include "rational.hpp"
#include <numeric>
#include <sstream>
#include <limits>

static int SafeCastImpl(int64_t v) {
    if (v < std::numeric_limits<int>::min() || v > std::numeric_limits<int>::max()) {
        throw std::overflow_error("Rational overflow");
    }
    return static_cast<int>(v);
}

int Rational::SafeCast(int64_t value) {
    return SafeCastImpl(value);
}

Rational::Rational() : numer_(0), denom_(1) {}

Rational::Rational(int num) : numer_(num), denom_(1) {}

Rational::Rational(int num, int den) : numer_(num), denom_(den) {
    if (denom_ == 0) {
        throw RationalDivisionByZero();
    }
    Normalize();
}

int Rational::GetNumerator() const {
    return numer_;
}

int Rational::GetDenominator() const {
    return denom_;
}

void Rational::SetNumerator(int value) {
    numer_ = value;
    Normalize();
}

void Rational::SetDenominator(int value) {
    if (value == 0) {
        throw RationalDivisionByZero();
    }
    denom_ = value;
    Normalize();
}

void Rational::Normalize() {
    if (numer_ == 0) {
        denom_ = 1;
        return;
    }
    if (denom_ < 0) {
        numer_ = -numer_;
        denom_ = -denom_;
    }
    int64_t an = numer_;
    int64_t ad = denom_;
    int64_t g = std::gcd(std::llabs(an), std::llabs(ad));
    if (g == 0) {
        g = 1;
    }
    an /= g;
    ad /= g;
    numer_ = SafeCast(an);
    denom_ = SafeCast(ad);
}

Rational& Rational::operator+=(const Rational& rhs) {
    int64_t new_numer = static_cast<int64_t>(numer_) * rhs.denom_ +
                        static_cast<int64_t>(rhs.numer_) * denom_;
    int64_t new_denom = static_cast<int64_t>(denom_) * rhs.denom_;
    numer_ = SafeCast(new_numer);
    denom_ = SafeCast(new_denom);
    Normalize();
    return *this;
}

Rational& Rational::operator-=(const Rational& rhs) {
    int64_t new_numer = static_cast<int64_t>(numer_) * rhs.denom_ -
                        static_cast<int64_t>(rhs.numer_) * denom_;
    int64_t new_denom = static_cast<int64_t>(denom_) * rhs.denom_;
    numer_ = SafeCast(new_numer);
    denom_ = SafeCast(new_denom);
    Normalize();
    return *this;
}

Rational& Rational::operator*=(const Rational& rhs) {
    int64_t new_numer = static_cast<int64_t>(numer_) * rhs.numer_;
    int64_t new_denom = static_cast<int64_t>(denom_) * rhs.denom_;
    numer_ = SafeCast(new_numer);
    denom_ = SafeCast(new_denom);
    Normalize();
    return *this;
}

Rational& Rational::operator/=(const Rational& rhs) {
    if (rhs.numer_ == 0) {
        throw RationalDivisionByZero();
    }
    int64_t new_numer = static_cast<int64_t>(numer_) * rhs.denom_;
    int64_t new_denom = static_cast<int64_t>(denom_) * rhs.numer_;
    numer_ = SafeCast(new_numer);
    denom_ = SafeCast(new_denom);
    Normalize();
    return *this;
}

Rational Rational::operator+() const {
    return *this;
}

Rational Rational::operator-() const {
    return {-numer_, denom_};
}

Rational& Rational::operator++() {
    *this += Rational(1);
    return *this;
}

Rational Rational::operator++(int) {
    Rational tmp = *this;
    ++(*this);
    return tmp;
}

Rational& Rational::operator--() {
    *this -= Rational(1);
    return *this;
}

Rational Rational::operator--(int) {
    Rational tmp = *this;
    --(*this);
    return tmp;
}

Rational operator+(Rational lhs, const Rational& rhs) {
    return lhs += rhs;
}

Rational operator-(Rational lhs, const Rational& rhs) {
    return lhs -= rhs;
}

Rational operator*(Rational lhs, const Rational& rhs) {
    return lhs *= rhs;
}

Rational operator/(Rational lhs, const Rational& rhs) {
    return lhs /= rhs;
}

bool operator==(const Rational& lhs, const Rational& rhs) {
    return lhs.numer_ == rhs.numer_ && lhs.denom_ == rhs.denom_;
}

bool operator!=(const Rational& lhs, const Rational& rhs) {
    return !(lhs == rhs);
}

bool operator<(const Rational& lhs, const Rational& rhs) {
    return static_cast<int64_t>(lhs.numer_) * rhs.denom_ <
           static_cast<int64_t>(rhs.numer_) * lhs.denom_;
}

bool operator<=(const Rational& lhs, const Rational& rhs) {
    return lhs < rhs || lhs == rhs;
}

bool operator>(const Rational& lhs, const Rational& rhs) {
    return rhs < lhs;
}

bool operator>=(const Rational& lhs, const Rational& rhs) {
    return !(lhs < rhs);
}

std::ostream& operator<<(std::ostream& os, const Rational& obj) {
    if (obj.denom_ == 1) {
        os << obj.numer_;
    } else {
        os << obj.numer_ << '/' << obj.denom_;
    }
    return os;
}

std::istream& operator>>(std::istream& is, Rational& obj) {
    if (!(is >> std::ws)) {
        // Just to consume any leading whitespace
    }
    std::string s;
    if (!(is >> s)) {
        return is;
    }
    std::istringstream iss(s);
    int num = 0;
    int den = 1;
    char c = '\0';
    if (!(iss >> num)) {
        is.setstate(std::ios::failbit);
        return is;
    }
    if (iss >> c) {
        if (c != '/' || !(iss >> den)) {
            is.setstate(std::ios::failbit);
            return is;
        }
        if (den == 0) {
            throw RationalDivisionByZero();
        }
    }
    if ((iss >> std::ws) && !iss.eof()) {
        is.setstate(std::ios::failbit);
        return is;
    }
    obj = Rational(num, den);
    return is;
}
