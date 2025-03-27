#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"
#include <cmath>
#include <cstddef>
#include <iostream>
#include <iterator>
namespace sjtu {
template <class T> class double_list {
private:
  struct node {
    T val;
    node *prev;
    node *next;
    node(const T &val, node *prev = nullptr, node *next = nullptr)
        : val(val), prev(prev), next(next) {}
  };
  int sizee;

public:
  node *head;
  node *tail;
  double_list() : sizee(0), head(nullptr), tail(nullptr) {}
  double_list(int sizee, node *head, node *tail)
      : sizee(sizee), head(head), tail(tail) {}
  // 深拷贝的拷贝构造函数
  double_list(const double_list &other)
      : head(nullptr), tail(nullptr), sizee(0) {
    node *current = other.head;
    while (current != nullptr) {
      insert_tail(current->val);
      current = current->next;
    }
  }

  // 赋值运算符重载
  double_list<T> &operator=(const double_list<T> &other) {
    if (this == &other) {
      return *this;
    }
    clear();
    node *current = other.head;
    while (current != nullptr) {
      insert_tail(current->val);
      current = current->next;
    }
    return *this;
  }
  ~double_list() { clear(); }
  class iterator {
  public:
    double_list *dl;
    node *ptr;

    iterator(double_list *dl = nullptr, node *ptr = nullptr)
        : dl(dl), ptr(ptr) {}
    iterator(node *node) : ptr(node) {}
    iterator(const iterator &t) {
      if (this == &t)
        return;
      dl = t.dl;
      ptr = t.ptr;
    }
    ~iterator() = default;
    /**
     * iter++
     */
    iterator operator++(int) {
      iterator tmp = *this;
      if (ptr == nullptr)
        throw "invalid";
      ptr = ptr->next;
      return tmp;
    }
    /**
     * ++iter
     */
    iterator &operator++() {
      if (ptr == nullptr) {
        if (dl == nullptr || dl->tail == nullptr)
          throw "invalid";
      }
      ptr = ptr->next;
      return *this;
    }
    /**
     * iter--
     */
    iterator &operator--() {
      if (ptr == nullptr) {
        if (dl == nullptr || dl->tail == nullptr) {
          throw "invalid";
        }
        ptr = dl->tail;
        return *this;
      }
      if (ptr == dl->head) {
        throw "invalid";
      }
      ptr = ptr->prev;
      return *this;
    }

    iterator operator--(int) {
      iterator temp = *this;
      if (ptr == nullptr) {
        if (dl == nullptr || dl->tail == nullptr) {
          throw "invalid";
        }
        ptr = dl->tail;
        return temp;
      }
      if (ptr == dl->head) {
        throw "invalid";
      }
      ptr = ptr->prev;
      return temp;
    }

    iterator operator+(int n) {
      iterator temp = *this;
      if (n < 0) {
        for (int i = 0; i < -n; i++) {
          if (temp.ptr == nullptr)
            throw "invalid";
          temp.ptr = temp.ptr->prev;
        }
      } else {
        for (int i = 0; i < n; i++) {
          if (temp.ptr == nullptr)
            throw "invalid";
          temp.ptr = temp.ptr->next;
        }
      }
      return temp;
    }
    /**
     * if the iter didn't point to a value
     * throw " invalid"
     */
    T &operator*() const {
      if (ptr == nullptr)
        throw "invalid";
      return ptr->val;
    }
    /**
     * other operation
     */
    T *operator->() const noexcept { return &(ptr->val); }
    bool operator==(const iterator &rhs) const {
      if (this == &rhs)
        return true;
      if (dl != rhs.dl)
        return false;
      return ptr == rhs.ptr;
    }

    bool operator!=(const iterator &rhs) const {
      if (this == &rhs)
        return false;

      if (dl != rhs.dl)
        return true;
      return ptr != rhs.ptr;
    }
  };
  //  double_list also uses const interator, for the linked_hashmap
  class const_iterator {
  public:
    const double_list *dl;
    node *ptr;

    const_iterator(const double_list *dl = nullptr, node *ptr = nullptr)
        : dl(dl), ptr(ptr) {}
    const_iterator(const const_iterator &t) = default;
    ~const_iterator() = default;

    const_iterator operator++(int) {
      const_iterator tmp = *this;
      if (ptr == nullptr)
        throw "invalid";
      ptr = ptr->next;
      return tmp;
    }

    const_iterator &operator++() {
      if (ptr == nullptr)
        throw "invalid";
      ptr = ptr->next;
      return *this;
    }

    const_iterator operator--(int) {
      const_iterator temp = *this;
      if (ptr == nullptr)
        throw "invalid";
      if (ptr == dl->head)
        throw "invalid";
      ptr = ptr->prev;
      return temp;
    }

    const_iterator &operator--() {
      if (ptr == nullptr) {
        if (dl == nullptr || dl->tail == nullptr) {
          throw "invalid";
        }
        ptr = dl->tail;
        return *this;
      }
      if (ptr == dl->head) {
        throw "invalid";
      }
      ptr = ptr->prev;
      return *this;
    }

    const_iterator operator+(int n) const {
      const_iterator temp = *this;
      if (n < 0) {
        for (int i = 0; i < -n; i++) {
          if (temp.ptr == nullptr)
            throw "invalid";
          temp.ptr = temp.ptr->prev;
        }
      } else {
        for (int i = 0; i < n; i++) {
          if (temp.ptr == nullptr)
            throw "invalid";
          temp.ptr = temp.ptr->next;
        }
      }
      return temp;
    }

    const T &operator*() const {
      if (ptr == nullptr)
        throw "invalid";
      return ptr->val;
    }

    const T *operator->() const noexcept {
      // if (ptr == nullptr)
      // throw "invalid";
      return &(ptr->val);
    }

    bool operator==(const const_iterator &rhs) const { return ptr == rhs.ptr; }

    bool operator!=(const const_iterator &rhs) const { return ptr != rhs.ptr; }
  };
  // begin() and end()
  const_iterator cbegin() const { return const_iterator(this, head); }

  const_iterator cend() const { return const_iterator(this, nullptr); }

  iterator begin() {
    // if (head == nullptr)
    // return iterator(this, nullptr);
    return iterator(this, head);
  }
  iterator end() { return iterator(this, nullptr); }

  iterator get_tail() const { return iterator(tail); }
  iterator get_head() const { return iterator(head); }
  // const_iterator cget_tail() const { return const_iterator(tail); }
  iterator erase(iterator pos) {
    if (pos.dl != this || pos.ptr == nullptr)
      return end();

    node *node_to_delete = pos.ptr;
    node *next_node = node_to_delete->next;

    if (node_to_delete->prev) {
      node_to_delete->prev->next = node_to_delete->next;
    } else {
      head = node_to_delete->next;
    }

    if (node_to_delete->next) {
      node_to_delete->next->prev = node_to_delete->prev;
    } else {
      tail = node_to_delete->prev;
    }

    delete node_to_delete;
    node_to_delete = nullptr;
    sizee--;

    return iterator(this, next_node);
  }
  iterator insert(iterator pos, const T &val) {
    if (pos.dl != this)
      throw "invalid";

    if (pos.ptr == nullptr) {
      if (tail == nullptr) {
        insert_tail(val);
        return iterator(this, tail);
      }
      node *new_node = new node(val, tail, nullptr);
      tail->next = new_node;
      tail = new_node;
      sizee++;
      return iterator(this, new_node);
    }

    node *new_node = new node(val, pos.ptr->prev, pos.ptr);
    if (pos.ptr->prev)
      pos.ptr->prev->next = new_node;
    else
      head = new_node;

    pos.ptr->prev = new_node;
    sizee++;
    return iterator(this, new_node);
  }

  T &front() { return head->val; }
  const T &cfront() const { return head->val; }
  T &back() { return tail->val; }
  const T &cback() const { return tail->val; }

  /**
   * the following are operations of double list
   */
  void insert_head(const T &val) {
    node *new_node = new node(val, nullptr, head);
    if (head != nullptr)
      head->prev = new_node;
    head = new_node;
    if (tail == nullptr)
      tail = head;
    sizee++;
  }
  void insert_tail(const T &val) {
    node *new_node = new node(val, tail, nullptr);
    if (tail != nullptr)
      tail->next = new_node;
    tail = new_node;
    if (head == nullptr)
      head = tail;
    sizee++;
  }
  void delete_head() {
    if (head == nullptr)
      return;
    node *temp = head;
    head = head->next;
    if (head != nullptr)
      head->prev = nullptr;
    delete temp;
    temp = nullptr;
    sizee--;
  }
  void delete_tail() {
    if (tail == nullptr)
      return;
    node *temp = tail;
    tail = tail->prev;
    if (tail != nullptr)
      tail->next = nullptr;
    delete temp;
    temp = nullptr;
    sizee--;
  }

  int size() { return sizee; }
  int size() const { return sizee; }

  bool empty() const { return sizee == 0; }
  void clear() {
    node *current = head;
    while (current != nullptr) {
      node *temp = current;
      current = current->next;
      delete temp;
      temp = nullptr;
    }
    head = tail = nullptr;
    sizee = 0;
  }

  void splice(double_list &other) {
    if (other.empty())
      return;

    if (empty()) {
      head = other.head;
      tail = other.tail;
      sizee = other.sizee;
    } else {
      tail->next = other.head;
      other.head->prev = tail;
      tail = other.tail;
      sizee += other.sizee;
    }

    other.head = other.tail = nullptr;
    other.sizee = 0;
  }

  void splice(iterator pos, double_list &other, iterator it) {
    if (pos.dl != this || it.dl != &other || it.ptr == nullptr)
      return;

    node *n = it.ptr;

    if (n->prev)
      n->prev->next = n->next;
    else
      other.head = n->next;
    if (n->next)
      n->next->prev = n->prev;
    else
      other.tail = n->prev;

    other.sizee--;

    if (pos.ptr) {
      n->next = pos.ptr;
      n->prev = pos.ptr->prev;
      if (pos.ptr->prev)
        pos.ptr->prev->next = n;
      else
        head = n;
      pos.ptr->prev = n;
    } else {
      n->next = nullptr;
      n->prev = tail;
      if (tail)
        tail->next = n;
      else
        head = n;
      tail = n;
    }

    sizee++;
  }

  void splice(iterator pos, double_list &other, iterator first, iterator last) {
    if (pos.dl != this || first.dl != &other || last.dl != &other)
      return;

    node *start = first.ptr;
    node *end = last.ptr ? last.ptr->prev : other.tail;

    if (!start || !end)
      return;

    if (start->prev)
      start->prev->next = last.ptr;
    else
      other.head = last.ptr;

    if (last.ptr)
      last.ptr->prev = start->prev;
    else
      other.tail = start->prev;

    if (pos.ptr) {
      start->prev = pos.ptr->prev;
      end->next = pos.ptr;
      if (pos.ptr->prev)
        pos.ptr->prev->next = start;
      else
        head = start;
      pos.ptr->prev = end;
    } else {
      start->prev = tail;
      end->next = nullptr;
      if (tail)
        tail->next = start;
      else
        head = start;
      tail = end;
    }

    int count = 0;
    for (node *cur = start; cur != end->next; cur = cur->next)
      count++;
    sizee += count;
    other.sizee -= count;
  }

  void pop_back() {
    if (tail == nullptr)
      return;
    node *temp = tail;
    tail = tail->prev;
    if (tail)
      tail->next = nullptr;
    else
      head = nullptr;
    delete temp;
    temp = nullptr;
    sizee--;
  }

  void pop_front() {
    if (head == nullptr)
      return;
    node *temp = head;
    head = head->next;
    if (head)
      head->prev = nullptr;
    else
      tail = nullptr;
    delete temp;
    temp = nullptr;
    sizee--;
  }
};

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

      // Special case: if both are end()
      if (block_it == dq->blocks.end() && rhs.block_it == dq->blocks.end())
        return 0;

      // We'll use a different approach:
      // Calculate the absolute position of each iterator from the beginning,
      // then take the difference

      // Calculate position of this iterator
      int this_pos = 0;
      if (block_it != dq->blocks.end()) {
        auto curr_block = dq->blocks.begin();
        while (curr_block != block_it) {
          this_pos += curr_block->data.size();
          ++curr_block;
        }

        // Add offset within current block
        auto curr_iter = block_it->data.begin();
        while (curr_iter != iter) {
          ++this_pos;
          ++curr_iter;
        }
      } else {
        // This is end(), so its position is the total size
        this_pos = dq->size();
      }

      // Calculate position of rhs iterator
      int rhs_pos = 0;
      if (rhs.block_it != dq->blocks.end()) {
        auto curr_block = dq->blocks.begin();
        while (curr_block != rhs.block_it) {
          rhs_pos += curr_block->data.size();
          ++curr_block;
        }

        // Add offset within current block
        auto curr_iter = rhs.block_it->data.begin();
        while (curr_iter != rhs.iter) {
          ++rhs_pos;
          ++curr_iter;
        }
      } else {
        // rhs is end(), so its position is the total size
        rhs_pos = dq->size();
      }

      // Return the difference in positions
      return this_pos - rhs_pos;
    }

    /*
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
*/
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

    const_iterator(const iterator &it)
        : iter(it.iter), block_it(it.block_it), dq(it.dq) {}
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

      // Special case: if both are end()
      if (block_it == dq->blocks.cend() && rhs.block_it == dq->blocks.cend())
        return 0;

      // Calculate position of this iterator
      int this_pos = 0;
      if (block_it != dq->blocks.cend()) {
        auto curr_block = dq->blocks.cbegin();
        while (curr_block != block_it) {
          this_pos += curr_block->data.size();
          ++curr_block;
        }

        // Add offset within current block
        auto curr_iter = block_it->data.cbegin();
        while (curr_iter != iter) {
          ++this_pos;
          ++curr_iter;
        }
      } else {
        // This is end(), so its position is the total size
        this_pos = dq->size();
      }

      // Calculate position of rhs iterator
      int rhs_pos = 0;
      if (rhs.block_it != dq->blocks.cend()) {
        auto curr_block = dq->blocks.cbegin();
        while (curr_block != rhs.block_it) {
          rhs_pos += curr_block->data.size();
          ++curr_block;
        }

        // Add offset within current block
        auto curr_iter = rhs.block_it->data.cbegin();
        while (curr_iter != rhs.iter) {
          ++rhs_pos;
          ++curr_iter;
        }
      } else {
        // rhs is end(), so its position is the total size
        rhs_pos = dq->size();
      }

      // Return the difference in positions
      return this_pos - rhs_pos;
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

  T &operator[](const size_t &pos) { return at(pos); }
  const T &operator[](const size_t &pos) const { return at(pos); }

  T &front() {
    if (total_size == 0)
      throw container_is_empty();
    return blocks.front().data.front();
  }

  const T &front() const {
    if (total_size == 0)
      throw container_is_empty();
    return blocks.cfront().data.cfront();
  }

  T &back() {
    if (total_size == 0)
      throw container_is_empty();
    return blocks.back().data.back();
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