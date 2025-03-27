#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "double_list.hpp"
#include "exceptions.hpp"
#include <cmath>
#include <cstddef>
#include <iostream>
#include <iterator>

namespace sjtu {
template <class T> class deque {

private:
  // Internal block structure containing a double_list of elements
  struct block {
    double_list<T> data;
  };

  double_list<block> blocks; // List of blocks
  size_t total_size;         // Total number of elements in deque

  // Calculate minimum block size based on total elements
  size_t min_block_size() const {
    return std::max(size_t(100), (size_t)std::sqrt(total_size) / 3);
  }

  // Calculate maximum block size based on total elements
  size_t max_block_size() const {
    return std::max(size_t(200), (size_t)std::sqrt(total_size));
  }

  /**
   * Split a block if it exceeds maximum size
   * @param it Iterator pointing to the block to split
   */
  void split_block(typename double_list<block>::iterator it) {
    if (it->data.size() <= max_block_size())
      return;

    // Create new block with half the elements
    double_list<T> new_block_data;
    auto split_it = it->data.begin();
    for (int i = 0; i < it->data.size() / 2; i++) {
      split_it++;
    }
    new_block_data.splice(new_block_data.begin(), it->data, split_it,
                          it->data.end());

    block new_block;
    new_block.data = new_block_data;

    // Insert new block after current block
    auto next_it = ++it;
    blocks.insert(next_it, new_block);
  }

  /**
   * Merge adjacent blocks if their combined size is within limits
   * @param it Iterator pointing to the first block to merge
   */
  void merge_blocks(typename double_list<block>::iterator it) {
    if (it == blocks.end())
      return;
    auto next_it = it;
    ++next_it;
    if (next_it == blocks.end())
      return;
    if (it->data.size() + next_it->data.size() > max_block_size())
      return;

    // Move all elements from next block to current block
    it->data.splice(it->data.end(), next_it->data, next_it->data.begin(),
                    next_it->data.end());
    blocks.erase(next_it);
  }

public:
  class const_iterator;

  // Iterator class for deque
  class iterator {
    friend class deque;

  private:
    typename double_list<block>::iterator block_it; // Current block
    typename double_list<T>::iterator iter;         // Current element in block
    deque *dq;                                      // Pointer to deque

  public:
    iterator() {}
    iterator(typename double_list<T>::iterator iter,
             typename double_list<block>::iterator b_it, deque *dq)
        : iter(iter), block_it(b_it), dq(dq) {}

    // Prefix increment
    iterator &operator++() {
      if (iter != block_it->data.end()) {
        ++iter;
        if (iter == block_it->data.end()) {
          ++block_it;
          if (block_it != dq->blocks.end()) {
            iter = block_it->data.begin();
          } else {
            iter = typename double_list<T>::iterator();
          }
        }
      } else {
        ++block_it;
        if (block_it != dq->blocks.end()) {
          iter = block_it->data.begin();
        } else {
          iter = typename double_list<T>::iterator();
        }
      }
      return *this;
    }

    // Postfix increment
    iterator operator++(int) {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    // Prefix decrement
    iterator &operator--() {
      if (block_it == dq->blocks.end()) {
        --block_it;
        iter = block_it->data.end();
      }
      if (iter == block_it->data.begin()) {
        if (block_it != dq->blocks.begin()) {
          --block_it;
          iter = block_it->data.end();
        } else {
          throw invalid_iterator();
        }
      }
      --iter;
      return *this;
    }

    // Postfix decrement
    iterator operator--(int) {
      iterator tmp = *this;
      --(*this);
      return tmp;
    }

    // Dereference operator
    T &operator*() const { return *iter; }

    // Arrow operator
    T *operator->() const noexcept { return &(*iter); }

    /**
     * Addition operator for iterator arithmetic
     * @param n Number of positions to move forward
     * @return Iterator moved forward by n positions
     */
    iterator operator+(const int &n) const {
      if (n == 0)
        return *this;
      if (n < 0)
        return *this - (-n);

      iterator temp = *this;
      int remain = n;

      // Calculate elements remaining in current block
      int elements_to_end = 0;
      auto it_copy = temp.iter;
      auto block_end = temp.block_it->data.end();
      while (it_copy != block_end) {
        ++it_copy;
        ++elements_to_end;
      }

      // If can complete within current block
      if (remain < elements_to_end) {
        for (int i = 0; i < remain; i++)
          ++temp;
        return temp;
      }

      // Need to cross blocks
      remain -= elements_to_end;
      ++temp.block_it;

      // Check if reached end
      if (temp.block_it == temp.dq->blocks.end()) {
        temp.block_it = dq->blocks.end();
        temp.iter = typename double_list<T>::iterator();
        return temp;
      }

      // Skip whole blocks
      while (remain > 0 && temp.block_it != dq->blocks.end()) {
        int block_size = temp.block_it->data.size();

        if (remain >= block_size) {
          remain -= block_size;
          ++temp.block_it;

          if (temp.block_it == temp.dq->blocks.end()) {
            temp.block_it = dq->blocks.end();
            temp.iter = typename double_list<T>::iterator();
            return temp;
          }
        } else {
          break;
        }
      }

      // Final positioning within target block
      if (temp.block_it != temp.dq->blocks.end()) {
        temp.iter = temp.block_it->data.begin();
        for (int i = 0; i < remain; i++)
          ++temp;
      }

      return temp;
    }

    /**
     * Subtraction operator for iterator arithmetic
     * @param n Number of positions to move backward
     * @return Iterator moved backward by n positions
     */
    iterator operator-(const int &n) const {
      if (n == 0)
        return *this;
      if (n < 0)
        return *this + (-n);

      // Special case for end() iterator
      if (block_it == dq->blocks.end()) {
        if (dq->blocks.empty()) {
          return *this;
        }

        // Calculate from end backwards
        iterator end_copy = *this;
        --end_copy.block_it;
        end_copy.iter = end_copy.block_it->data.end();

        int remain = n;
        int elements_in_last_block = end_copy.block_it->data.size();

        // If can complete within last block
        if (remain <= elements_in_last_block) {
          end_copy.iter = end_copy.block_it->data.end();
          for (int i = 0; i < remain; i++) {
            --end_copy.iter;
          }
          return end_copy;
        }

        // Need to cross blocks
        remain -= elements_in_last_block;

        // Move to previous blocks
        while (remain > 0 && end_copy.block_it != dq->blocks.begin()) {
          --end_copy.block_it;
          int block_size = end_copy.block_it->data.size();

          if (remain > block_size) {
            remain -= block_size;
          } else {
            end_copy.iter = end_copy.block_it->data.end();
            for (int i = 0; i < remain; i++) {
              --end_copy.iter;
            }
            return end_copy;
          }
        }

        end_copy.iter = end_copy.block_it->data.begin();
        return end_copy;
      }

      iterator temp = *this;
      int remain = n;

      // Calculate elements from current position to block start
      int current_pos = 0;
      auto it_copy = temp.iter;
      auto block_begin = temp.block_it->data.begin();
      while (it_copy != block_begin) {
        --it_copy;
        ++current_pos;
      }

      // If can complete within current block
      if (remain <= current_pos) {
        for (int i = 0; i < remain; i++)
          --temp.iter;
        return temp;
      }

      // Need to cross blocks
      remain -= current_pos;

      // Move to previous blocks
      while (remain > 0 && temp.block_it != dq->blocks.begin()) {
        --temp.block_it;
        int block_size = temp.block_it->data.size();

        if (remain > block_size) {
          remain -= block_size;
        } else {
          temp.iter = temp.block_it->data.end();
          for (int i = 0; i < remain; i++) {
            --temp.iter;
          }
          return temp;
        }
      }

      // Position at first block's beginning
      temp.iter = temp.block_it->data.begin();
      return temp;
    }

    /**
     * Calculate distance between two iterators
     * @param rhs Iterator to subtract from this
     * @return Number of elements between iterators
     */
    int operator-(const iterator &rhs) const {
      if (dq != rhs.dq)
        throw invalid_iterator();

      // Both are end()
      if (block_it == dq->blocks.end() && rhs.block_it == dq->blocks.end())
        return 0;

      // Calculate this iterator's position
      int this_pos = 0;
      if (block_it != dq->blocks.end()) {
        auto curr_block = dq->blocks.begin();
        while (curr_block != block_it) {
          this_pos += curr_block->data.size();
          ++curr_block;
        }

        auto curr_iter = block_it->data.begin();
        while (curr_iter != iter) {
          ++this_pos;
          ++curr_iter;
        }
      } else {
        this_pos = dq->size();
      }

      // Calculate rhs iterator's position
      int rhs_pos = 0;
      if (rhs.block_it != dq->blocks.end()) {
        auto curr_block = dq->blocks.begin();
        while (curr_block != rhs.block_it) {
          rhs_pos += curr_block->data.size();
          ++curr_block;
        }

        auto curr_iter = rhs.block_it->data.begin();
        while (curr_iter != rhs.iter) {
          ++rhs_pos;
          ++curr_iter;
        }
      } else {
        rhs_pos = dq->size();
      }

      return this_pos - rhs_pos;
    }

    // Compound addition assignment
    iterator &operator+=(const int &n) {
      *this = *this + n;
      return *this;
    }

    // Compound subtraction assignment
    iterator &operator-=(const int &n) {
      *this = *this - n;
      return *this;
    }

    // Equality comparison
    bool operator==(const iterator &rhs) const {
      return block_it == rhs.block_it && iter == rhs.iter;
    }

    // Inequality comparison
    bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
  };

  // Const iterator class for deque
  class const_iterator {
    friend class deque;
    friend class iterator;

  private:
    typename double_list<block>::const_iterator block_it;
    typename double_list<T>::const_iterator iter;
    const deque *dq;

  public:
    const_iterator() {}
    const_iterator(typename double_list<T>::const_iterator iter,
                   typename double_list<block>::const_iterator b_it,
                   const deque *dq)
        : iter(iter), block_it(b_it), dq(dq) {}

    // Conversion from iterator to const_iterator
    const_iterator(const iterator &it)
        : iter(it.iter), block_it(it.block_it), dq(it.dq) {}

    // Prefix increment
    const_iterator &operator++() {
      if (iter != block_it->data.cend()) {
        ++iter;
        if (iter == block_it->data.cend()) {
          ++block_it;
          if (block_it != dq->blocks.cend()) {
            iter = block_it->data.cbegin();
          }
        }
      } else {
        ++block_it;
        if (block_it != dq->blocks.cend()) {
          iter = block_it->data.cbegin();
        }
      }
      return *this;
    }

    // Postfix increment
    const_iterator operator++(int) {
      const_iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    // Prefix decrement
    const_iterator &operator--() {
      if (block_it == dq->blocks.cend()) {
        --block_it;
        iter = block_it->data.cend();
      }
      if (iter == block_it->data.cbegin()) {
        if (block_it != dq->blocks.cbegin()) {
          --block_it;
          iter = block_it->data.cend();
        } else {
          throw invalid_iterator();
        }
      }
      --iter;
      return *this;
    }

    // Postfix decrement
    const_iterator operator--(int) {
      const_iterator tmp = *this;
      --(*this);
      return tmp;
    }

    // Dereference operator
    const T &operator*() const { return *iter; }

    // Arrow operator
    const T *operator->() const noexcept { return &(*iter); }

    // Addition operator (similar to iterator's version)
    const_iterator operator+(const int &n) const {
      if (n == 0)
        return *this;
      if (n < 0)
        return *this - (-n);

      const_iterator temp = *this;
      int remain = n;

      int elements_to_end = 0;
      auto it_copy = temp.iter;
      auto block_end = temp.block_it->data.cend();
      while (it_copy != block_end) {
        ++it_copy;
        ++elements_to_end;
      }

      if (remain < elements_to_end) {
        for (int i = 0; i < remain; i++)
          ++temp;
        return temp;
      }

      remain -= elements_to_end;
      ++temp.block_it;
      if (temp.block_it == temp.dq->blocks.cend()) {
        temp.block_it = dq->blocks.cend();
        temp.iter = typename double_list<T>::const_iterator();
        return temp;
      }

      while (remain > 0 && temp.block_it != dq->blocks.cend()) {
        int block_size = temp.block_it->data.size();

        if (remain >= block_size) {
          remain -= block_size;
          ++temp.block_it;

          if (temp.block_it == temp.dq->blocks.cend()) {
            temp.block_it = dq->blocks.cend();
            temp.iter = typename double_list<T>::const_iterator();
            return temp;
          }
        } else {
          break;
        }
      }

      if (temp.block_it != temp.dq->blocks.cend()) {
        temp.iter = temp.block_it->data.cbegin();
        for (int i = 0; i < remain; i++)
          ++temp;
      }

      return temp;
    }

    // Subtraction operator (similar to iterator's version)
    const_iterator operator-(const int &n) const {
      if (n == 0)
        return *this;
      if (n < 0)
        return *this + (-n);

      const_iterator begin = dq->cbegin();

      // Special case for end() iterator
      if (block_it == dq->blocks.cend()) {
        if (dq->blocks.empty()) {
          return *this;
        }

        const_iterator end_copy = *this;
        --end_copy.block_it;
        end_copy.iter = end_copy.block_it->data.cend();

        int remain = n;
        int elements_in_last_block = end_copy.block_it->data.size();

        if (remain <= elements_in_last_block) {
          end_copy.iter = end_copy.block_it->data.cend();
          for (int i = 0; i < remain; i++) {
            --end_copy.iter;
          }
          return end_copy;
        }

        remain -= elements_in_last_block;

        while (remain > 0 && end_copy.block_it != dq->blocks.cbegin()) {
          --end_copy.block_it;
          int block_size = end_copy.block_it->data.size();

          if (remain > block_size) {
            remain -= block_size;
          } else {
            end_copy.iter = end_copy.block_it->data.cend();
            for (int i = 0; i < remain; i++) {
              --end_copy.iter;
            }
            return end_copy;
          }
        }

        end_copy.iter = end_copy.block_it->data.cbegin();
        return end_copy;
      }

      const_iterator temp = *this;
      int remain = n;

      int current_pos = 0;
      auto it_copy = temp.iter;
      auto block_begin = temp.block_it->data.cbegin();
      while (it_copy != block_begin) {
        --it_copy;
        ++current_pos;
      }

      if (remain <= current_pos) {
        for (int i = 0; i < remain; i++)
          --temp.iter;
        return temp;
      }

      remain -= current_pos;

      while (remain > 0 && temp.block_it != dq->blocks.cbegin()) {
        --temp.block_it;
        int block_size = temp.block_it->data.size();

        if (remain > block_size) {
          remain -= block_size;
        } else {
          temp.iter = temp.block_it->data.cend();
          for (int i = 0; i < remain; i++) {
            --temp.iter;
          }
          return temp;
        }
      }

      temp.iter = temp.block_it->data.cbegin();
      return temp;
    }

    // Distance calculation (similar to iterator's version)
    int operator-(const const_iterator &rhs) const {
      if (dq != rhs.dq)
        throw invalid_iterator();

      if (block_it == dq->blocks.cend() && rhs.block_it == dq->blocks.cend())
        return 0;

      int this_pos = 0;
      if (block_it != dq->blocks.cend()) {
        auto curr_block = dq->blocks.cbegin();
        while (curr_block != block_it) {
          this_pos += curr_block->data.size();
          ++curr_block;
        }

        auto curr_iter = block_it->data.cbegin();
        while (curr_iter != iter) {
          ++this_pos;
          ++curr_iter;
        }
      } else {
        this_pos = dq->size();
      }

      int rhs_pos = 0;
      if (rhs.block_it != dq->blocks.cend()) {
        auto curr_block = dq->blocks.cbegin();
        while (curr_block != rhs.block_it) {
          rhs_pos += curr_block->data.size();
          ++curr_block;
        }

        auto curr_iter = rhs.block_it->data.cbegin();
        while (curr_iter != rhs.iter) {
          ++rhs_pos;
          ++curr_iter;
        }
      } else {
        rhs_pos = dq->size();
      }

      return this_pos - rhs_pos;
    }

    // Compound addition assignment
    const_iterator &operator+=(const int &n) {
      *this = *this + n;
      return *this;
    }

    // Compound subtraction assignment
    const_iterator &operator-=(const int &n) {
      *this = *this - n;
      return *this;
    }

    // Equality comparison
    bool operator==(const const_iterator &rhs) const {
      return block_it == rhs.block_it && iter == rhs.iter;
    }

    // Inequality comparison
    bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
  };

  // Default constructor
  deque() : total_size(0) {}

  // Copy constructor
  deque(const deque &other) : total_size(other.total_size) {
    blocks = other.blocks;
  }

  // Destructor
  ~deque() { clear(); }

  // Assignment operator
  deque &operator=(const deque &other) {
    if (this != &other) {
      total_size = other.total_size;
      blocks = other.blocks;
    }
    return *this;
  }

  /**
   * Access element at specified position with bounds checking
   * @param pos Position of element to access
   * @return Reference to element at position
   * @throw index_out_of_bound if pos is invalid
   */
  T &at(const size_t &pos) {
    if (pos >= total_size)
      throw index_out_of_bound();
    auto it = blocks.begin();
    size_t count = 0;
    while (it != blocks.end()) {
      if (count + it->data.size() > pos) {
        auto data_it = it->data.begin();
        for (size_t i = 0; i < pos - count; ++i) {
          ++data_it;
        }
        return *data_it;
      }
      count += it->data.size();
      ++it;
    }
    throw index_out_of_bound();
  }

  // Const version of at()
  const T &at(const size_t &pos) const {
    if (pos >= total_size)
      throw index_out_of_bound();
    auto it = blocks.cbegin();
    size_t count = 0;
    while (it != blocks.cend()) {
      if (count + it->data.size() > pos) {
        auto data_it = it->data.cbegin();
        for (size_t i = 0; i < pos - count; ++i) {
          ++data_it;
        }
        return *data_it;
      }
      count += it->data.size();
      ++it;
    }
    throw index_out_of_bound();
  }

  // Subscript operator
  T &operator[](const size_t &pos) { return at(pos); }

  // Const subscript operator
  const T &operator[](const size_t &pos) const { return at(pos); }

  /**
   * Access first element
   * @return Reference to first element
   * @throw container_is_empty if deque is empty
   */
  T &front() {
    if (total_size == 0)
      throw container_is_empty();
    return blocks.front().data.front();
  }

  // Const version of front()
  const T &front() const {
    if (total_size == 0)
      throw container_is_empty();
    return blocks.cfront().data.cfront();
  }

  /**
   * Access last element
   * @return Reference to last element
   * @throw container_is_empty if deque is empty
   */
  T &back() {
    if (total_size == 0)
      throw container_is_empty();
    return blocks.back().data.back();
  }

  // Const version of back()
  const T &back() const {
    if (total_size == 0)
      throw container_is_empty();
    return blocks.cback().data.cback();
  }

  // Get iterator to beginning
  iterator begin() {
    if (blocks.empty())
      return end();
    return iterator(blocks.begin()->data.begin(), blocks.begin(), this);
  }

  // Get const iterator to beginning
  const_iterator cbegin() const {
    return const_iterator(blocks.cbegin()->data.cbegin(), blocks.cbegin(),
                          this);
  }

  // Get iterator to end
  iterator end() {
    return iterator(typename double_list<T>::iterator(), blocks.end(), this);
  }

  // Get const iterator to end
  const_iterator cend() const {
    return const_iterator(typename double_list<T>::const_iterator(),
                          blocks.cend(), this);
  }

  // Check if deque is empty
  bool empty() const { return total_size == 0; }

  // Get number of elements
  size_t size() const { return total_size; }

  // Clear all elements
  void clear() {
    blocks.clear();
    total_size = 0;
  }

  /**
   * Insert element at specified position
   * @param pos Iterator before which to insert
   * @param value Element value to insert
   * @return Iterator pointing to inserted element
   * @throw invalid_iterator if pos is invalid
   */
  iterator insert(iterator pos, const T &value) {
    if (pos.dq != this)
      throw invalid_iterator();
    if (pos == begin()) {
      push_front(value);
      return begin();
    }
    if (pos == end()) {
      push_back(value);
      return --end();
    }

    int offset = pos - begin();

    auto new_data_iter = pos.block_it->data.insert(pos.iter, value);
    total_size++;

    // Split current block if exceeds max size
    split_block(pos.block_it);

    return begin() + offset;
  }

  /**
   * Erase element at specified position
   * @param pos Iterator to element to erase
   * @return Iterator following last removed element
   * @throw invalid_iterator if pos is invalid
   */
  iterator erase(iterator pos) {
    if (pos == end() || pos.dq != this)
      throw invalid_iterator();
    auto current_block = pos.block_it;
    auto next = current_block->data.erase(pos.iter);
    total_size--;

    if (current_block->data.empty()) {
      auto next_block = current_block;
      ++next_block;
      blocks.erase(current_block);
      if (next_block != blocks.end()) {
        return iterator(next_block->data.begin(), next_block, this);
      } else {
        return end();
      }
    } else {
      // Merge with previous block if too small
      if (current_block->data.size() < min_block_size() &&
          current_block != blocks.begin()) {
        auto prev_block = current_block;
        --prev_block;

        size_t prev_size = prev_block->data.size();
        size_t current_size = current_block->data.size();
        size_t offset = 0;
        for (auto it = current_block->data.begin(); it != next; it++)
          offset++;

        if (prev_block->data.size() + current_size <= max_block_size()) {
          merge_blocks(prev_block);
          // Recalculate iterator position after merge
          auto new_iter = prev_block->data.begin();
          for (auto i = 0; i < prev_size + offset; i++)
            new_iter++;
          next = new_iter;
          current_block = prev_block;
        }
      }

      if (next != current_block->data.end()) {
        return iterator(next, current_block, this);
      } else {
        auto next_block = current_block;
        ++next_block;
        if (next_block != blocks.end()) {
          return iterator(next_block->data.begin(), next_block, this);
        } else {
          return end();
        }
      }
    }
  }

  /**
   * Add element to the end
   * @param value Element value to add
   */
  void push_back(const T &value) {
    if (blocks.empty() || blocks.back().data.size() >= max_block_size()) {
      blocks.insert_tail(block{});
    }
    auto last_block = --blocks.end();
    last_block->data.insert_tail(value);
    total_size++;
    if (last_block->data.size() > max_block_size()) {
      split_block(last_block);
    }
  }

  /**
   * Remove last element
   * @throw container_is_empty if deque is empty
   */
  void pop_back() {
    if (empty())
      throw container_is_empty();
    auto last_block = --blocks.end();
    last_block->data.pop_back();
    total_size--;

    // Merge with previous block if too small
    if (last_block->data.empty()) {
      blocks.pop_back();
    } else if (last_block->data.size() < min_block_size() &&
               blocks.size() > 1) {
      auto prev_block = last_block;
      --prev_block;
      if (prev_block->data.size() + last_block->data.size() <=
          max_block_size()) {
        prev_block->data.splice(prev_block->data.end(), last_block->data,
                                last_block->data.begin(),
                                last_block->data.end());
        blocks.erase(last_block);
      }
    }
  }

  /**
   * Add element to the front
   * @param value Element value to add
   */
  void push_front(const T &value) {
    if (blocks.empty() || blocks.front().data.size() >= max_block_size()) {
      blocks.insert_head(block{});
    }
    auto first_block = blocks.begin();
    first_block->data.insert_head(value);
    total_size++;
    if (first_block->data.size() > max_block_size()) {
      split_block(first_block);
    }
  }

  /**
   * Remove first element
   * @throw container_is_empty if deque is empty
   */
  void pop_front() {
    if (empty()) {
      throw container_is_empty();
    }
    auto first_block = blocks.begin();
    first_block->data.pop_front();
    total_size--;

    // Merge with next block if too small
    if (first_block->data.empty()) {
      blocks.erase(first_block);
    } else if (first_block->data.size() < min_block_size() &&
               blocks.size() > 1) {
      auto next_block = first_block;
      ++next_block;
      if (next_block != blocks.end()) {
        if (first_block->data.size() + next_block->data.size() <=
            max_block_size()) {
          first_block->data.splice(first_block->data.end(), next_block->data,
                                   next_block->data.begin(),
                                   next_block->data.end());
          blocks.erase(next_block);
        }
      }
    }
  }
};

} // namespace sjtu

#endif
