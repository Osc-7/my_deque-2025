#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

namespace sjtu {

template <class T> class deque {
public:
  class const_iterator;
  class iterator {
  private:
    /**
     * add data members.
     * just add whatever you want.
     */
  public:
    /**
     * return a new iterator which points to the n-next element.
     * if there are not enough elements, the behaviour is undefined.
     * same for operator-.
     */
    iterator operator+(const int &n) const {}
    iterator operator-(const int &n) const {}

    /**
     * return the distance between two iterators.
     * if they point to different vectors, throw
     * invaild_iterator.
     */
    int operator-(const iterator &rhs) const {}
    iterator &operator+=(const int &n) {}
    iterator &operator-=(const int &n) {}

    /**
     * iter++
     */
    iterator operator++(int) {}
    /**
     * ++iter
     */
    iterator &operator++() {}
    /**
     * iter--
     */
    iterator operator--(int) {}
    /**
     * --iter
     */
    iterator &operator--() {}

    /**
     * *it
     */
    T &operator*() const {}
    /**
     * it->field
     */
    T *operator->() const noexcept {}

    /**
     * check whether two iterators are the same (pointing to the same
     * memory).
     */
    bool operator==(const iterator &rhs) const {}
    bool operator==(const const_iterator &rhs) const {}
    /**
     * some other operator for iterators.
     */
    bool operator!=(const iterator &rhs) const {}
    bool operator!=(const const_iterator &rhs) const {}
  };

  class const_iterator {
    /**
     * it should has similar member method as iterator.
     * you can copy them, but with care!
     * and it should be able to be constructed from an iterator.
     */
  };

  /**
   * constructors.
   */
  deque() {}
  deque(const deque &other) {}

  /**
   * deconstructor.
   */
  ~deque() {}

  /**
   * assignment operator.
   */
  deque &operator=(const deque &other) {}

  /**
   * access a specified element with bound checking.
   * throw index_out_of_bound if out of bound.
   */
  T &at(const size_t &pos) {}
  const T &at(const size_t &pos) const {}
  T &operator[](const size_t &pos) {}
  const T &operator[](const size_t &pos) const {}

  /**
   * access the first element.
   * throw container_is_empty when the container is empty.
   */
  const T &front() const {}
  /**
   * access the last element.
   * throw container_is_empty when the container is empty.
   */
  const T &back() const {}

  /**
   * return an iterator to the beginning.
   */
  iterator begin() {}
  const_iterator cbegin() const {}

  /**
   * return an iterator to the end.
   */
  iterator end() {}
  const_iterator cend() const {}

  /**
   * check whether the container is empty.
   */
  bool empty() const {}

  /**
   * return the number of elements.
   */
  size_t size() const {}

  /**
   * clear all contents.
   */
  void clear() {}

  /**
   * insert value before pos.
   * return an iterator pointing to the inserted value.
   * throw if the iterator is invalid or it points to a wrong place.
   */
  iterator insert(iterator pos, const T &value) {}

  /**
   * remove the element at pos.
   * return an iterator pointing to the following element. if pos points to
   * the last element, return end(). throw if the container is empty,
   * the iterator is invalid, or it points to a wrong place.
   */
  iterator erase(iterator pos) {}

  /**
   * add an element to the end.
   */
  void push_back(const T &value) {}

  /**
   * remove the last element.
   * throw when the container is empty.
   */
  void pop_back() {}

  /**
   * insert an element to the beginning.
   */
  void push_front(const T &value) {}

  /**
   * remove the first element.
   * throw when the container is empty.
   */
  void pop_front() {}
};

} // namespace sjtu

#endif
