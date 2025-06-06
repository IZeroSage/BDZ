#ifndef MATRIX_HPP
#define MATRIX_HPP

#define MATRIX_SQUARE_MATRIX_IMPLEMENTED

#include <iostream>
#include <stdexcept>
#include <algorithm>

class MatrixIsDegenerateError : public std::runtime_error {
 public:
  MatrixIsDegenerateError() : std::runtime_error("MatrixIsDegenerateError") {
  }
};

class MatrixOutOfRange : public std::out_of_range {
 public:
  MatrixOutOfRange() : std::out_of_range("MatrixOutOfRange") {
  }
};

template <typename ValType, size_t Rows, size_t Cols>
class Matrix {
 public:
  ValType values[Rows][Cols];

  size_t RowsNumber() const noexcept {
    return Rows;
  }
  
  size_t ColumnsNumber() const noexcept {
    return Cols;
  }

  ValType& operator()(size_t row, size_t col) {
    return values[row][col];
  }
  
  const ValType& operator()(size_t row, size_t col) const {
    return values[row][col];
  }

  ValType& At(size_t row, size_t col) {
    if (row >= Rows || col >= Cols) {
      throw MatrixOutOfRange();
    }
    return values[row][col];
  }
  
  const ValType& At(size_t row, size_t col) const {
    if (row >= Rows || col >= Cols) {
      throw MatrixOutOfRange();
    }
    return values[row][col];
  }

  Matrix& operator+=(const Matrix& second) {
    for (size_t r = 0; r < Rows; ++r) {
      for (size_t c = 0; c < Cols; ++c) {
        values[r][c] += second.values[r][c];
      }
    }
    return *this;
  }

  Matrix& operator-=(const Matrix& second) {
    for (size_t r = 0; r < Rows; ++r) {
      for (size_t c = 0; c < Cols; ++c) {
        values[r][c] -= second.values[r][c];
      }
    }
    return *this;
  }

  Matrix& operator*=(const ValType& scalar) {
    for (auto& row : values) {
      for (auto& elem : row) {
        elem *= scalar;
      }
    }
    return *this;
  }

  Matrix& operator/=(const ValType& scalar) {
    for (auto& row : values) {
      for (auto& elem : row) {
        elem /= scalar;
      }
    }
    return *this;
  }

  template <size_t OtherCols>
  Matrix<ValType, Rows, OtherCols>& operator*=(const Matrix<ValType, Cols, OtherCols>& rhs) {
    *this = *this * rhs;
    return *this;
  }

  void FillFromStream(std::istream& is) {
    for (size_t r = 0; r < Rows; ++r) {
      for (size_t c = 0; c < Cols; ++c) {
        is >> values[r][c];
      }
    }
  }

  friend Matrix operator+(Matrix lhs, const Matrix& rhs) {
    return lhs += rhs;
  }
  
  friend Matrix operator-(Matrix lhs, const Matrix& rhs) {
    return lhs -= rhs;
  }
  
  friend Matrix operator*(Matrix lhs, const ValType& scalar) {
    return lhs *= scalar;
  }
  
  friend Matrix operator*(const ValType& scalar, Matrix rhs) {
    return rhs *= scalar;
  }
  
  friend Matrix operator/(Matrix lhs, const ValType& scalar) {
    return lhs /= scalar;
  }

  template <size_t OtherCols>
  friend Matrix<ValType, Rows, OtherCols> operator*(const Matrix& lhs, const Matrix<ValType, Cols, OtherCols>& rhs) {
    Matrix<ValType, Rows, OtherCols> res{};
    for (size_t r = 0; r < Rows; ++r) {
      for (size_t c = 0; c < OtherCols; ++c) {
        ValType sum{};
        for (size_t k = 0; k < Cols; ++k) {
          sum += lhs.values[r][k] * rhs.values[k][c];
        }
        res.values[r][c] = sum;
      }
    }
    return res;
  }

  friend bool operator==(const Matrix& m1, const Matrix& m2) {
    for (size_t r = 0; r < Rows; ++r) {
      for (size_t c = 0; c < Cols; ++c) {
        if (m1.values[r][c] != m2.values[r][c]) {
          return false;
        }
      }
    }
    return true;
  }

  friend bool operator!=(const Matrix& m1, const Matrix& m2) {
    return !(m1 == m2);
  }
};

template <typename T, size_t R, size_t C>
std::ostream& operator<<(std::ostream& os, const Matrix<T, R, C>& m) {
  for (size_t r = 0; r < R; ++r) {
    for (size_t c = 0; c < C; ++c) {
      if (c) {
        os << ' ';
      }
      os << m.values[r][c];
    }
    os << '\n';
  }
  return os;
}

template <typename T, size_t R, size_t C>
std::istream& operator>>(std::istream& is, Matrix<T, R, C>& m) {
  m.FillFromStream(is);
  return is;
}

template <typename T, size_t R, size_t C>
Matrix<T, C, R> GetTransposed(const Matrix<T, R, C>& m) {
  Matrix<T, C, R> result;
  for (size_t r = 0; r < R; ++r) {
    for (size_t c = 0; c < C; ++c) {
      result(c, r) = m(r, c);
    }
  }
  return result;
}

template <typename T, size_t N>
void Transpose(Matrix<T, N, N>& m) {
  for (size_t r = 0; r < N; ++r) {
    for (size_t c = r + 1; c < N; ++c) {
      std::swap(m(r, c), m(c, r));
    }
  }
}

template <typename T, size_t N>
T Trace(const Matrix<T, N, N>& m) {
  T trace = T();
  for (size_t i = 0; i < N; ++i) {
    trace += m(i, i);
  }
  return trace;
}

template <typename T, size_t N>
T Determinant(const Matrix<T, N, N>& m) {
  if constexpr (N == 1) {
    return m(0, 0);
  } else if constexpr (N == 2) {
    return m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0);
  } else {
    T det = T();
    for (size_t j = 0; j < N; ++j) {
      Matrix<T, N-1, N-1> minor;
      for (size_t r = 1; r < N; ++r) {
        size_t minor_c = 0;
        for (size_t c = 0; c < N; ++c) {
          if (c == j) {
            continue;
          }
          minor(r-1, minor_c) = m(r, c);
          minor_c++;
        }
      }
      T sign = (j % 2) ? -1 : 1;
      det += sign * m(0, j) * Determinant(minor);
    }
    return det;
  }
}

template <typename T, size_t N>
Matrix<T, N, N> GetInversed(const Matrix<T, N, N>& m) {
  T det = Determinant(m);
  if (det == T()) {
    throw MatrixIsDegenerateError();
  }
  
  Matrix<T, N, N> inv;
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < N; ++j) {
      Matrix<T, N-1, N-1> minor;
      size_t minor_r = 0;
      for (size_t r = 0; r < N; ++r) {
        if (r == i) {
          continue;
        }
        size_t minor_c = 0;
        for (size_t c = 0; c < N; ++c) {
          if (c == j) {
            continue;
          }
          minor(minor_r, minor_c) = m(r, c);
          minor_c++;
        }
        minor_r++;
      }
      T sign = ((i + j) % 2) ? -1 : 1;
      inv(j, i) = sign * Determinant(minor) / det;
    }
  }
  return inv;
}

template <typename T, size_t N>
void Inverse(Matrix<T, N, N>& m) {
  m = GetInversed(m);
}

#endif  // MATRIX_HPP //