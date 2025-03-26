#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "double_list.hpp"
#include "exceptions.hpp"
#include <cstddef>
#include <iostream>
#include <iterator>
bool DEBUG = false;
static int counnt = 0;
namespace sjtu {
template <class T> class deque {

private:
  struct block {
    double_list<T> data;
  };

  double_list<block> blocks;

  size_t total_size;
  size_t min_block_size() const {
    return std::max(size_t(2), (size_t)std::sqrt(total_size) / 2);
  }
  size_t max_block_size() const {
    return std::max(size_t(4), (size_t)std::sqrt(total_size) * 2);
  }

  void rebalance() {
    if (blocks.empty())
      return;

    auto it = blocks.begin();
    while (it != blocks.end()) {
      if (it->data.size() > max_block_size()) {
        split_block(it);
      } else if (it->data.size() < min_block_size()) {
        merge_blocks(it);
      }
      ++it;
    }
  }

  void split_block(typename double_list<block>::iterator it) {
    if (it->data.size() <= max_block_size())
      return;

    double_list<T> new_block_data;
    auto split_it = it->data.begin();
    for (int i = 0; i < it->data.size() / 2; i++) {
      split_it++;
    }
    new_block_data.splice(new_block_data.begin(), it->data, split_it,
                          it->data.end());

    block new_block;
    new_block.data = new_block_data;

    auto next_it = ++it;
    blocks.insert(next_it, new_block);
  }

  void merge_blocks(typename double_list<block>::iterator it) {
    if (it == blocks.end())
      return;
    auto next_it = it;
    ++next_it;
    if (next_it == blocks.end())
      return;
    if (it->data.size() + next_it->data.size() > max_block_size())
      return;
    it->data.splice(it->data.end(), next_it->data, next_it->data.begin(),
                    next_it->data.end());
    blocks.erase(next_it);
  }

public:
  class const_iterator;
  class iterator {
    friend class deque;

  private:
    typename double_list<block>::iterator block_it;
    typename double_list<T>::iterator iter;
    deque *dq;

  public:
    iterator() {}
    iterator(typename double_list<T>::iterator iter,
             typename double_list<block>::iterator b_it, deque *dq)
        : iter(iter), block_it(b_it), dq(dq) {}

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

    iterator operator++(int) {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }

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

    iterator operator--(int) {
      iterator tmp = *this;
      --(*this);
      return tmp;
    }

    T &operator*() const { return *iter; }
    T *operator->() const noexcept { return &(*iter); }

    iterator operator+(int n) const {
      iterator temp = *this;
      if (n >= 0) {
        while (n-- > 0)
          ++temp;
      } else {
        while (n++ < 0)
          --temp;
      }
      return temp;
    }
    /*
    iterator operator+(const int &n) const {
      if (n == 0)
        return *this;

      iterator temp = *this;
      int remain = n;
      while (remain > 0) {
        int block_remaining = 0;
        for (auto it = temp.iter; it != temp.block_it->data.end(); it++)
          block_remaining++;
        if (remain < block_remaining) {
          for (int i = 0; i < remain; i++) {
            ++temp.iter;
          }
          break;
        } else {
          remain -= block_remaining;
          ++temp.block_it;
          if (temp.block_it != dq->blocks.end()) {
            temp.iter = temp.block_it->data.begin();
          } else {
            throw index_out_of_bound();
          }
        }
      }
      return temp;
    }
    */
    iterator operator-(int n) const { return *this + (-n); }

    int operator-(const iterator &rhs) const {
      if (dq != rhs.dq)
        throw invalid_iterator();
      int dist = 0;
      iterator temp = rhs;
      while (temp != *this) {
        ++temp;
        ++dist;
      }
      return dist;
    }

    iterator &operator+=(const int &n) {
      *this = *this + n;
      return *this;
    }

    iterator &operator-=(const int &n) {
      *this = *this - n;

      return *this;
    }

    bool operator==(const iterator &rhs) const {
      return block_it == rhs.block_it && iter == rhs.iter;
    }

    bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
  };

  class const_iterator {
    friend class deque;

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

    const_iterator operator++(int) {
      const_iterator tmp = *this;
      ++(*this);
      return tmp;
    }

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

    const_iterator operator--(int) {
      const_iterator tmp = *this;
      --(*this);
      return tmp;
    }

    const T &operator*() const { return *iter; }
    const T *operator->() const noexcept { return &(*iter); }

    const_iterator operator+(int n) const {
      const_iterator temp = *this;
      if (n >= 0) {
        while (n-- > 0)
          ++temp;
      } else {
        while (n++ < 0)
          --temp;
      }
      return temp;
    }

    const_iterator operator-(int n) const { return *this + (-n); }

    int operator-(const const_iterator &rhs) const {
      if (dq != rhs.dq)
        throw invalid_iterator();

      int dist = 0;
      auto lhs_block = block_it;
      auto rhs_block = rhs.block_it;
      auto lhs_iter = iter;
      auto rhs_iter = rhs.iter;

      if (lhs_block == rhs_block) {
        // 在同一个 block 里，直接计算迭代器差值
        while (rhs_iter != lhs_iter) {
          ++rhs_iter;
          ++dist;
        }
        return dist;
      }

      auto temp_block = rhs_block;
      auto temp_iter = rhs_iter;

      while (temp_iter != temp_block->data.cend()) {
        ++temp_iter;
        ++dist;
      }

      // 遍历中间完整的 block
      ++temp_block;
      while (temp_block != lhs_block) {

        dist += temp_block->data.sizee;
        ++temp_block;
      }

      // 计算 `lhs_block` 头部到 `lhs_iter` 之间的距离
      temp_iter = lhs_block->data.cbegin();
      while (temp_iter != lhs_iter) {
        ++temp_iter;
        ++dist;
      }

      return dist;
    }

    const_iterator &operator+=(const int &n) {
      *this = *this + n;
      return *this;
    }

    const_iterator &operator-=(const int &n) {
      *this = *this - n;
      return *this;
    }

    bool operator==(const const_iterator &rhs) const {
      return block_it == rhs.block_it && iter == rhs.iter;
    }
    bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
  };

  deque() : total_size(0) {}
  deque(const deque &other) : total_size(other.total_size) {
    blocks = other.blocks;
  }

  ~deque() { clear(); }

  deque &operator=(const deque &other) {
    if (this != &other) {
      total_size = other.total_size;
      blocks = other.blocks;
    }
    return *this;
  }

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

  const T &at(const size_t &pos) const {
    if (pos >= total_size)
      throw index_out_of_bound();
    auto it = blocks.cbegin();
    size_t count = 0;
    while (it != blocks.cend()) {
      if (count + it->data.sizee > pos) {
        auto data_it = it->data.cbegin();
        for (size_t i = 0; i < pos - count; ++i) {
          ++data_it;
        }
        return *data_it;
      }
      count += it->data.sizee;
      ++it;
    }
    throw index_out_of_bound();
  }

  T &operator[](const size_t &pos) { return at(pos); }
  const T &operator[](const size_t &pos) const { return at(pos); }

  const T &front() const {
    if (total_size == 0)
      throw container_is_empty();
    return blocks.cfront().data.cfront();
  }

  const T &back() const {
    if (total_size == 0)
      throw container_is_empty();
    return blocks.cback().data.cback();
  }

  iterator begin() {
    if (blocks.empty())
      return end();
    return iterator(blocks.begin()->data.begin(), blocks.begin(), this);
  }

  const_iterator cbegin() const {
    return const_iterator(blocks.cbegin()->data.cbegin(), blocks.cbegin(),
                          this);
  }

  iterator end() {
    return iterator(typename double_list<T>::iterator(), blocks.end(), this);
  }

  const_iterator cend() const {
    return const_iterator(typename double_list<T>::const_iterator(),
                          blocks.cend(), this);
  }

  bool empty() const { return total_size == 0; }

  size_t size() const { return total_size; }

  void clear() {
    blocks.clear();
    total_size = 0;
  }

  iterator insert(iterator pos, const T &value) {
    if (DEBUG)
      counnt++;
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

    // if (DEBUG) {
    // std::cout << "Before rebalance: " << &(*inserted) << std::endl;
    //}
    rebalance();
    // if (DEBUG) {
    // std::cout << "After rebalance: " << &(*inserted) << std::endl;
    // std::cout << counnt << std::endl;
    //}

    return begin() + offset;
  }

  /*
  void allocateInitialblock(const T &value) {
    block new_block;
    new_block.data.insert_tail(value);
    blocks.insert_tail(new_block);
    total_size = 1;
  }
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
          // 合并后，prev_block的data包含了原current_block的数据
          // 计算新的迭代器位置
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

  void push_back(const T &value) {
    if (blocks.empty() || blocks.back().data.size() >= max_block_size()) {
      blocks.insert_tail(block{});
    }
    auto last_block = blocks.get_tail();
    last_block->data.insert_tail(value);
    total_size++;
    rebalance();
  }

  void pop_back() {
    if (empty())
      throw container_is_empty();
    auto last_block = blocks.get_tail();
    last_block->data.pop_back();
    total_size--;
    if (last_block->data.empty()) {
      blocks.pop_back();
    }
    rebalance();
  }

  void push_front(const T &value) {
    if (blocks.empty() || blocks.front().data.size() >= max_block_size()) {
      blocks.insert_head(block{});
    }
    auto first_block = blocks.begin();
    first_block->data.insert_head(value);
    total_size++;

    rebalance();
  }

  void pop_front() {
    if (empty())
      throw container_is_empty();
    auto first_block = blocks.begin();
    first_block->data.pop_front();
    total_size--;
    if (first_block->data.empty()) {
      blocks.pop_front();
    }
    rebalance();
  }
};

} // namespace sjtu

#endif
