#ifndef VECTOR_HPP
#define VECTOR_HPP

#define VECTOR_MEMORY_IMPLEMENTED
#include <iostream>
#include <new>

template <class T>
class Vector {

 private:
  T* vec_ = nullptr;
  size_t capacity_ = 0;
  size_t size_ = 0;

 public:
  using ValueType = T;
  using Pointer = T*;
  using ConstPointer = const T*;
  using Reference = T&;
  using ConstReference = const T&;
  using SizeType = size_t;
  using Iterator = T*;
  using ConstIterator = const T*;
  using ReverseIterator = std::reverse_iterator<Iterator>;
  using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

  Vector() : vec_(nullptr), capacity_(0), size_(0) {
  }

  explicit Vector(size_t number) : vec_(nullptr), capacity_(0), size_(0) {
    if (number > 0) {
      vec_ = static_cast<T*>(::operator new(sizeof(T) * number));
      capacity_ = number;
      size_ = number;

      size_t i = 0;
      try {
        for (; i < size_; ++i) {
          new (&vec_[i]) T();
        }
      } catch (...) {
        // Уничтожаем созданные объекты
        for (size_t j = 0; j < i; ++j) {
          vec_[j].~T();
        }
        ::operator delete(vec_);
        vec_ = nullptr;
        capacity_ = 0;
        size_ = 0;
        throw;
      }
    }
  }

  explicit Vector(size_t size, const T& value) : vec_(nullptr), capacity_(0), size_(0) {
    if (size > 0) {
      vec_ = static_cast<T*>(::operator new(sizeof(T) * size));
      capacity_ = size;
      size_ = size;

      size_t i = 0;
      try {
        for (; i < size_; ++i) {
          new (&vec_[i]) T(value);
        }
      } catch (...) {
        for (size_t j = 0; j < i; ++j) {
          vec_[j].~T();
        }
        ::operator delete(vec_);
        vec_ = nullptr;
        capacity_ = 0;
        size_ = 0;
        throw;
      }
    }
  }

  template <class Iterator, class = std::enable_if_t<std::is_base_of_v<
                                std::forward_iterator_tag, typename std::iterator_traits<Iterator>::iterator_category>>>
  Vector(Iterator begin, Iterator end) : vec_(nullptr), capacity_(0), size_(0) {
    size_ = std::distance(begin, end);
    if (size_ > 0) {
      capacity_ = size_;
      vec_ = static_cast<T*>(::operator new(sizeof(T) * capacity_));

      size_t i = 0;
      try {
        for (auto j = begin; j != end; ++j, ++i) {
          new (&vec_[i]) T(*j);
        }
      } catch (...) {
        for (size_t j = 0; j < i; ++j) {
          vec_[j].~T();
        }
        ::operator delete(vec_);
        vec_ = nullptr;
        capacity_ = 0;
        size_ = 0;
        throw;
      }
    }
  }

  Vector(std::initializer_list<T> init) : Vector(init.begin(), init.end()) {
  }

  Vector(const Vector& other) : vec_(nullptr), capacity_(0), size_(0) {
    if (other.size_ > 0) {
      capacity_ = other.size_;
      vec_ = static_cast<T*>(::operator new(sizeof(T) * capacity_));

      size_t i = 0;
      try {
        for (; i < other.size_; ++i) {
          new (vec_ + i) T(other.vec_[i]);
        }
        size_ = other.size_;
      } catch (...) {
        for (size_t j = 0; j < i; ++j) {
          vec_[j].~T();
        }
        ::operator delete(vec_);
        vec_ = nullptr;
        capacity_ = 0;
        throw;
      }
    }
  }

  Vector(Vector&& other) noexcept : vec_(other.vec_), capacity_(other.capacity_), size_(other.size_) {
    other.vec_ = nullptr;
    other.capacity_ = 0;
    other.size_ = 0;
  }

  Vector& operator=(Vector&& other) noexcept {
    if (this != &other) {
      Clear();
      ::operator delete(vec_);

      vec_ = other.vec_;
      capacity_ = other.capacity_;
      size_ = other.size_;

      other.vec_ = nullptr;
      other.capacity_ = 0;
      other.size_ = 0;
    }
    return *this;
  }

  Vector& operator=(const Vector& second_vector) {
    Vector temp(second_vector);
    Swap(temp);
    return *this;
  }

  ~Vector() noexcept {
    for (size_t i = 0; i < size_; ++i) {
      vec_[i].~T();
    }
    if (vec_) {
      ::operator delete(vec_);
    }
  }

  size_t Size() const {
    return size_;
  }

  size_t Capacity() const {
    return capacity_;
  }

  bool Empty() const {
    return size_ == 0;
  }

  T& Front() {
    return vec_[0];
  }

  const T& Front() const {
    return vec_[0];
  }

  T& Back() {
    return vec_[size_ - 1];
  }

  const T& Back() const {
    return vec_[size_ - 1];
  }

  Pointer Data() {
    return vec_;
  }

  ConstPointer Data() const {
    return vec_;
  }

  void Swap(Vector& second_vector) {
    std::swap(vec_, second_vector.vec_);
    std::swap(capacity_, second_vector.capacity_);
    std::swap(size_, second_vector.size_);
  }

  void DeleteTrash(T* start, size_t from, size_t to) {
    for (size_t i = from; i < to; ++i) {
      start[i].~T();
    }
  }

  T* ReallocateD(size_t new_capacity, size_t new_size) {
    T* new_vec = static_cast<T*>(::operator new(sizeof(T) * new_capacity));
    size_t i = 0;
    try {
      for (; i < size_; ++i) {
        new (new_vec + i) T(static_cast<T&&>(vec_[i]));
      }
      for (; i < new_size; ++i) {
        new (new_vec + i) T();
      }
    } catch (...) {
      DeleteTrash(new_vec, 0, i);
      ::operator delete(new_vec);
      throw;
    }
    return new_vec;
  }

  T* ReallocateV(size_t new_capacity, size_t new_size, const T& value) {
    T* new_vec = static_cast<T*>(::operator new(sizeof(T) * new_capacity));
    size_t i = 0;
    try {
      for (; i < size_; ++i) {
        new (new_vec + i) T(static_cast<T&&>(vec_[i]));
      }
      for (; i < new_size; ++i) {
        new (new_vec + i) T(value);
      }
    } catch (...) {
      DeleteTrash(new_vec, 0, i);
      ::operator delete(new_vec);
      throw;
    }
    return new_vec;
  }

  void Resize(size_t new_size) {
    if (new_size > size_) {
      if (new_size > capacity_) {
        T* new_vec = ReallocateD(new_size, new_size);

        DeleteTrash(vec_, 0, size_);
        ::operator delete(vec_);

        vec_ = new_vec;
        capacity_ = new_size;
      } else {
        size_t i = size_;
        try {
          for (; i < new_size; ++i) {
            new (vec_ + i) T();
          }
        } catch (...) {
          DeleteTrash(vec_, size_, i);
          throw;
        }
      }
      size_ = new_size;
    } else if (new_size < size_) {
      DeleteTrash(vec_, new_size, size_);
      size_ = new_size;
    }
  }

  void Resize(size_t new_size, const T& value) {
    if (new_size > size_) {
      if (new_size > capacity_) {
        T* new_vec = ReallocateV(new_size, new_size, value);

        DeleteTrash(vec_, 0, size_);
        ::operator delete(vec_);

        vec_ = new_vec;
        capacity_ = new_size;
      } else {
        size_t i = size_;
        try {
          for (; i < new_size; ++i) {
            new (vec_ + i) T(value);
          }
        } catch (...) {
          DeleteTrash(vec_, size_, i);
          throw;
        }
      }
      size_ = new_size;
    } else if (new_size < size_) {
      DeleteTrash(vec_, new_size, size_);
      size_ = new_size;
    }
  }

  void Reserve(size_t new_capacity) {
    if (new_capacity <= capacity_) {
      return;
    }
    T* new_vec = static_cast<T*>(::operator new(sizeof(T) * new_capacity));
    size_t i = 0;
    try {
      for (; i < size_; ++i) {
        new (new_vec + i) T(std::move(vec_[i]));
      }
    } catch (...) {
      for (size_t j = 0; j < i; ++j) {
        new_vec[j].~T();
      }
      ::operator delete(new_vec);
      throw;
    }

    for (size_t j = 0; j < size_; ++j) {
      vec_[j].~T();
    }
    ::operator delete(vec_);

    vec_ = new_vec;
    capacity_ = new_capacity;
  }

  void ShrinkToFit() {
    if (size_ == 0) {
      ::operator delete(vec_);
      vec_ = nullptr;
      capacity_ = 0;
      return;
    }
    if (capacity_ > size_) {
      T* new_vec = static_cast<T*>(::operator new(sizeof(T) * size_));
      size_t i = 0;
      try {
        for (; i < size_; ++i) {
          new (new_vec + i) T(std::move(vec_[i]));
        }
      } catch (...) {
        for (size_t j = 0; j < i; ++j) {
          new_vec[j].~T();
        }
        ::operator delete(new_vec);
        throw;
      }

      for (size_t j = 0; j < size_; ++j) {
        vec_[j].~T();
      }
      ::operator delete(vec_);

      vec_ = new_vec;
      capacity_ = size_;
    }
  }

  void Clear() {
    for (size_t i = 0; i < size_; ++i) {
      vec_[i].~T();
    }
    size_ = 0;
  }

  void PushBack(const T& value) {
    if (size_ == capacity_) {
      size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
      T* new_vec = static_cast<T*>(::operator new(sizeof(T) * new_capacity));
      size_t i = 0;
      try {
        for (; i < size_; ++i) {
          new (new_vec + i) T(std::move(vec_[i]));
        }
        new (new_vec + size_) T(value);
      } catch (...) {
        for (size_t j = 0; j < i; ++j) {
          new_vec[j].~T();
        }
        ::operator delete(new_vec);
        throw;
      }
      for (size_t i = 0; i < size_; ++i) {
        vec_[i].~T();
      }
      ::operator delete(vec_);

      vec_ = new_vec;
      capacity_ = new_capacity;
      ++size_;
    } else {
      new (vec_ + size_) T(value);
      ++size_;
    }
  }

  void PushBack(T&& value) {
    if (size_ == capacity_) {
      size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
      T* new_vec = static_cast<T*>(::operator new(sizeof(T) * new_capacity));

      size_t i = 0;
      try {
        for (; i < size_; ++i) {
          new (new_vec + i) T(std::move(vec_[i]));
        }
        new (new_vec + size_) T(std::move(value));
      } catch (...) {
        for (size_t j = 0; j < i; ++j) {
          new_vec[j].~T();
        }
        ::operator delete(new_vec);
        throw;
      }
      for (size_t i = 0; i < size_; ++i) {
        vec_[i].~T();
      }
      ::operator delete(vec_);

      vec_ = new_vec;
      capacity_ = new_capacity;
      ++size_;
    } else {
      new (vec_ + size_) T(std::move(value));
      ++size_;
    }
  }

  void PopBack() {
    if (size_ > 0) {
      vec_[size_ - 1].~T();
      --size_;
    }
  }

  T& operator[](size_t ind) {
    return vec_[ind];
  }

  const T& operator[](size_t ind) const {
    return vec_[ind];
  }

  T& At(size_t ind) {
    if (ind >= size_) {
      throw std::out_of_range("Index out of range.");
    }
    return vec_[ind];
  }

  const T& At(size_t ind) const {
    if (ind >= size_) {
      throw std::out_of_range("Index out of range.");
    }
    return vec_[ind];
  }

  template <typename... Args>
  void EmplaceBack(Args&&... args) {
    if (size_ == capacity_) {
      size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
      T* new_data = static_cast<T*>(::operator new(sizeof(T) * new_capacity));

      size_t i = 0;
      try {
        for (; i < size_; ++i) {
          new (new_data + i) T(std::move(vec_[i]));
        }
        new (new_data + size_) T(std::forward<Args>(args)...);
      } catch (...) {
        for (size_t j = 0; j < i; ++j) {
          new_data[j].~T();
        }
        ::operator delete(new_data);
        throw;
      }

      for (size_t i = 0; i < size_; ++i) {
        vec_[i].~T();
      }
      ::operator delete(vec_);
      vec_ = new_data;
      capacity_ = new_capacity;
      ++size_;
    } else {
      new (vec_ + size_) T(std::forward<Args>(args)...);
      ++size_;
    }
  }

  Iterator begin() {  // NOLINT
    return vec_;
  }

  ConstIterator begin() const {  // NOLINT
    return vec_;
  }

  Iterator end() {  // NOLINT
    return vec_ + size_;
  }

  ConstIterator end() const {  // NOLINT
    return vec_ + size_;
  }

  ConstIterator cbegin() const {  // NOLINT
    return vec_;
  }

  ConstIterator cend() const {  // NOLINT
    return vec_ + size_;
  }

  ReverseIterator rbegin() {  // NOLINT
    return std::reverse_iterator<T*>(end());
  }

  ConstReverseIterator rbegin() const {  // NOLINT
    return std::reverse_iterator<const T*>(end());
  }

  ReverseIterator rend() {  // NOLINT
    return std::reverse_iterator<T*>(begin());
  }

  ConstReverseIterator rend() const {  // NOLINT
    return std::reverse_iterator<const T*>(begin());
  }

  ConstReverseIterator crbegin() const {  // NOLINT
    return std::reverse_iterator<const T*>(cend());
  }

  ConstReverseIterator crend() const {  // NOLINT
    return std::reverse_iterator<const T*>(cbegin());
  }

  friend bool operator==(const Vector& l, const Vector& r) {
    bool flag = true;
    if (r.size_ != l.size_) {
      flag = false;
    } else {
      for (size_t i = 0; i < l.size_; ++i) {
        if (l.vec_[i] != r.vec_[i]) {
          flag = false;
          break;
        }
      }
    }
    return flag;
  }

  friend bool operator!=(const Vector& l, const Vector& r) {
    return !(l == r);
  }

  friend bool operator<(const Vector& l, const Vector& r) {
    size_t size = 0;
    if (r.size_ > l.size_) {
      size = l.size_;
    } else {
      size = r.size_;
    }

    if (l.size_ < r.size_) {
      size = l.size_;
    } else {
      size = r.size_;
    }
    for (size_t i = 0; i < size; ++i) {
      if (l.vec_[i] > r.vec_[i]) {
        return false;
      }
      if (l.vec_[i] < r.vec_[i]) {
        return true;
      }
    }
    return l.size_ < r.size_;
  }

  friend bool operator>(const Vector& l, const Vector& r) {
    return (r < l);
  }

  friend bool operator<=(const Vector& l, const Vector& r) {
    return !(l > r);
  }

  friend bool operator>=(const Vector& l, const Vector& r) {
    return !(l < r);
  }
};

#endif  // VECTOR_HPP //