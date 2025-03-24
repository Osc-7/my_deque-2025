#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>
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

public:
  int size;
  node *head;
  node *tail;
  double_list() : size(0), head(nullptr), tail(nullptr) {}
  double_list(int size, node *head, node *tail)
      : size(size), head(head), tail(tail) {}
  // 深拷贝的拷贝构造函数
  double_list(const double_list<T> &other)
      : size(0), head(nullptr), tail(nullptr) {
    for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
      insert_tail(*it);
    }
  }
  // 赋值运算符重载
  double_list<T> &operator=(const double_list<T> &other) {
    if (this != &other) {
      clear();
      for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
        insert_tail(*it);
      }
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
      if (*this == dl->end())
        throw "invalid";
      ptr = ptr->next;
      return tmp;
    }
    /**
     * ++iter
     */
    iterator &operator++() {
      if (ptr == nullptr)
        throw "invalid";
      if (*this == dl->end())
        throw "invalid";
      ptr = ptr->next;
      return *this;
    }
    /**
     * iter--
     */
    // 前置 operator--
    iterator &operator--() {
      if (ptr == nullptr) {
        // 处理 end() 迭代器的情况
        if (dl == nullptr || dl->tail == nullptr) {
          throw "invalid"; // 链表为空，无法--
        }
        ptr = dl->tail;
        return *this;
      }
      if (ptr == dl->head) {
        throw "invalid"; // 已经是 begin()
      }
      ptr = ptr->prev;
      return *this;
    }

    // 后置 operator--
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
    const double_list *dl; // 关联的双向链表
    node *ptr;             // 当前指向的节点

    const_iterator(const double_list *dl = nullptr, node *ptr = nullptr)
        : dl(dl), ptr(ptr) {}
    const_iterator(const const_iterator &t) = default;
    ~const_iterator() = default;

    const_iterator operator++(int) {
      const_iterator tmp = *this;
      if (ptr == nullptr)
        throw "invalid";
      if (*this == dl->cend())
        throw "invalid";
      ptr = ptr->next;
      return tmp;
    }

    const_iterator &operator++() {
      if (ptr == nullptr)
        throw "invalid";
      if (*this == dl->cend())
        throw "invalid";
      ptr = ptr->next;
      return *this;
    }

    const_iterator operator--(int) {
      const_iterator temp = *this;
      if (ptr == nullptr)
        throw "invalid";
      if (*this == dl->cbegin())
        throw "invalid";
      ptr = ptr->prev;
      return temp;
    }

    const_iterator &operator--() {
      if (ptr == nullptr)
        throw "invalid";
      if (*this == dl->cbegin())
        throw "invalid";
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
      if (ptr == nullptr)
        throw "invalid";
      return &(ptr->val);
    }

    bool operator==(const const_iterator &rhs) const { return ptr == rhs.ptr; }

    bool operator!=(const const_iterator &rhs) const { return ptr != rhs.ptr; }
  };
  // begin() and end()
  const_iterator cbegin() const { return const_iterator(this, head); }

  const_iterator cend() const { return const_iterator(this, nullptr); }

  iterator begin() {
    if (head == nullptr)
      return iterator(this, nullptr);
    return iterator(this, head);
  }
  iterator end() { return iterator(this, nullptr); }

  iterator get_tail() const { return iterator(tail); }
  iterator erase(iterator pos) {
    if (pos.ptr == nullptr)
      return end();
    node *p = pos.ptr;
    node *prev = p->prev;
    node *next = p->next;

    if (prev)
      prev->next = next;
    else
      head = next;
    if (next)
      next->prev = prev;
    else
      tail = prev;

    delete p; // 释放被删除节点的内存
    size--;   // 更新链表长度

    return (next) ? iterator(this, next) : end();
  }

  iterator insert(iterator pos, const T &val) {
    if (pos.ptr == nullptr) {
      if (tail == nullptr) {
        insert_tail(val);
        return iterator(this, tail);
      }
      node *new_node = new node(val, tail, nullptr);
      tail->next = new_node;
      tail = new_node;
      size++;
      return iterator(this, new_node);
    }

    node *new_node = new node(val, pos.ptr->prev, pos.ptr);
    if (pos.ptr->prev)
      pos.ptr->prev->next = new_node;
    else
      head = new_node;

    pos.ptr->prev = new_node;
    size++;
    return iterator(this, new_node);
  }

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
    size++;
  }
  void insert_tail(const T &val) {
    node *new_node = new node(val, tail, nullptr);
    if (tail != nullptr)
      tail->next = new_node;
    tail = new_node;
    if (head == nullptr)
      head = tail;
    size++;
  }
  void delete_head() {
    if (head == nullptr)
      return;
    node *temp = head;
    head = head->next;
    if (head != nullptr)
      head->prev = nullptr;
    delete temp;
    size--;
  }
  void delete_tail() {
    if (tail == nullptr)
      return;
    node *temp = tail;
    tail = tail->prev;
    if (tail != nullptr)
      tail->next = nullptr;
    delete temp;
    size--;
  }

  bool empty() const {
    if (size == 0)
      return true;
    return false;
  }
  void clear() {
    node *cur = head;
    while (cur) {
      node *tmp = cur;
      cur = cur->next;
      delete tmp;
    }
    size = 0;
    head = tail = nullptr;
  }
};

template <class T> class deque {

private:
  struct block {
    double_list<T> data;
    block *prev;
    block *next;

    block(block *prev = nullptr, block *next = nullptr)
        : prev(prev), next(next) {}
  };
  block *head;
  block *tail;

  size_t total_size;
  size_t min_block_size() const {
    return std::max(size_t(2), (size_t)std::sqrt(total_size) / 2);
  }
  size_t max_block_size() const {
    return std::max(size_t(4), (size_t)std::sqrt(total_size) * 2);
  }

  void split_block(block *cur) {
    if (cur->data.size <= max_block_size())
      return; // 没超出 max_block_size，无需拆分

    block *new_block = new block(cur, cur->next);
    if (cur->next)
      cur->next->prev = new_block;
    cur->next = new_block;
    if (tail == cur)
      tail = new_block; // 如果 cur 是 tail，更新 tail 指向新块

    // 把 cur 后半部分的数据移动到 new_block
    size_t move_size = cur->data.size / 2;
    auto it = cur->data.begin();
    for (size_t i = 0; i < move_size; i++)
      ++it; // 定位到拆分点

    while (it != cur->data.end()) {
      new_block->data.insert_tail(*it);
      it = cur->data.erase(it); // 移除 cur 里已经移动的数据
    }
  }

  void merge_blocks(block *cur) {
    if (!cur || !cur->next)
      return; // cur 或 cur->next 为空，不能合并

    block *next_block = cur->next;
    if (cur->data.size + next_block->data.size > max_block_size())
      return; // 不能合并

    // 把 next_block 的数据移动到 cur
    auto it = next_block->data.begin();
    while (it != next_block->data.end()) {
      cur->data.insert_tail(*it);
      it++;
    }

    // 从链表中移除 next_block
    cur->next = next_block->next;
    if (next_block->next)
      next_block->next->prev = cur;
    else
      tail = cur; // 如果 next_block 是 tail，更新 tail

    delete next_block; // 释放内存
  }

public:
  class const_iterator;
  class iterator {
    friend class deque;

  private:
    block *current;
    typename double_list<T>::iterator iter;
    deque *dq;

  public:
    iterator(block *cur_block = nullptr,
             typename double_list<T>::iterator it =
                 typename double_list<T>::iterator(),
             deque *parent_dq = nullptr)
        : current(cur_block), iter(it), dq(parent_dq) {}

    iterator(const iterator &other)
        : current(other.current), iter(other.iter), dq(other.dq) {}

    iterator &operator++() {
      ++iter;
      if (current) {
        if (iter == current->data.end()) {
          if (current->next) {
            current = current->next;
            iter = current->data.begin();
          } else {
            current = nullptr;
            iter = typename double_list<T>::iterator();
          }
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
      if (current && iter == current->data.begin() && current->prev) {
        current = current->prev;
        iter = current->data.get_tail();
      } else {
        --iter;
      }
      return *this;
    }

    iterator operator--(int) {
      iterator tmp = *this;
      --(*this);
      return tmp;
    }

    T &operator*() const { return *iter; }
    T *operator->() const noexcept { return &(*iter); }

    iterator operator+(const int &n) const {
      if (n == 0)
        return *this;
      if (n < 0)
        return *this - (-n);

      iterator res = *this;
      block *cur_block = res.current;
      typename double_list<T>::iterator cur_iter = res.iter;

      int remain = n;

      // 处理当前块中的元素
      if (cur_block) {
        auto it = cur_iter;
        auto end = cur_block->data.end();
        int current_block_remain = 0;
        while (it != end) {
          ++it;
          ++current_block_remain;
        }

        if (remain < current_block_remain) {
          it = cur_iter;
          for (int i = 0; i < remain; ++i) {
            ++it;
          }
          res.iter = it;
          return res;
        } else {
          remain -= current_block_remain;
          cur_block = cur_block->next;
        }
      }

      // 跨越多个块
      while (cur_block && remain >= cur_block->data.size) {
        remain -= cur_block->data.size;
        cur_block = cur_block->next;
      }

      if (!cur_block) {
        // 如果已经到达末尾，返回end()
        return iterator(nullptr, typename double_list<T>::iterator(), dq);
      }

      // 处理最后一个块内的元素
      auto it = cur_block->data.begin();
      for (int i = 0; i < remain; ++i) {
        ++it;
      }

      return iterator(cur_block, it, dq);
    }

    iterator operator-(const int &n) const { return *this + (-n); }

    int operator-(const iterator &rhs) const {
      if (dq != rhs.dq)
        throw invalid_iterator();
      int dist = 0;
      iterator temp = rhs;
      while (temp.current != current || temp.iter != iter) {
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
      return current == rhs.current && iter == rhs.iter;
    }

    bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
  };

  class const_iterator {
    friend class deque;

  private:
    const block *current;
    typename double_list<T>::const_iterator iter;
    const deque *dq;

  public:
    const_iterator(const block *cur_block = nullptr,
                   typename double_list<T>::const_iterator it =
                       typename double_list<T>::const_iterator(),
                   const deque *parent_dq = nullptr)
        : current(cur_block), iter(it), dq(parent_dq) {}

    const_iterator(const iterator &other)
        : current(other.current), iter(other.iter), dq(other.dq) {}

    const_iterator &operator++() {
      ++iter;
      if (iter == current->data.cend() && current->next) {
        current = current->next;
        iter = current->data.cbegin();
      }
      return *this;
    }

    const_iterator operator++(int) {
      const_iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    const_iterator &operator--() {
      if (iter == current->data.cbegin() && current->prev) {
        current = current->prev;
        iter = current->data.get_tail();
      } else {
        --iter;
      }
      return *this;
    }

    const_iterator operator--(int) {
      const_iterator tmp = *this;
      --(*this);
      return tmp;
    }

    const T &operator*() const { return *iter; }
    const T *operator->() const noexcept { return &(*iter); }

    bool operator==(const const_iterator &rhs) const {
      return current == rhs.current && iter == rhs.iter;
    }
    bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
  };

  /**
   * constructors.
   */
  deque() : head(nullptr), tail(nullptr), total_size(0) {}
  deque(const deque &other) : head(nullptr), tail(nullptr), total_size(0) {
    block *cur = other.head;
    while (cur != nullptr) {
      block *new_block = new block();
      new_block->data = cur->data;
      new_block->prev = tail;

      if (tail != nullptr) {
        tail->next = new_block;
      } else {
        head = new_block;
      }

      tail = new_block;
      cur = cur->next;
    }

    total_size = other.total_size;
  }

  /**
   * deconstructor.
   */
  ~deque() { clear(); }

  /**
   * assignment operator.
   */
  deque &operator=(const deque &other) {
    if (this == &other) {
      return *this;
    }

    clear();

    block *cur = other.head;
    while (cur != nullptr) {
      block *new_block = new block();
      new_block->data = cur->data;
      new_block->prev = tail;

      if (tail != nullptr) {
        tail->next = new_block;
      } else {
        head = new_block;
      }

      tail = new_block;
      cur = cur->next;
    }

    total_size = other.total_size;

    return *this;
  }
  /**
   * access a specified element with bound checking.
   * throw index_out_of_bound if out of bound.
   */
  T &at(const size_t &pos) {
    if (pos >= total_size)
      throw index_out_of_bound();
    block *cur = head;
    size_t idx = pos;
    while (cur) {
      if (idx < cur->data.size) {
        auto it = cur->data.begin();
        for (size_t i = 0; i < idx; i++) {
          it++;
        }

        return *it;
      }
      idx -= cur->data.size;
      cur = cur->next;
    }
    throw index_out_of_bound();
  }
  const T &at(const size_t &pos) const {
    if (pos >= total_size)
      throw index_out_of_bound();
    block *cur = head;
    size_t idx = pos;
    while (cur) {
      if (idx < cur->data.size) {
        auto it = cur->data.begin();
        for (size_t i = 0; i < idx; i++) {
          it++;
        }
        return *it;
      }
      idx -= cur->data.size;
      cur = cur->next;
    }
    throw index_out_of_bound();
  }
  T &operator[](const size_t &pos) {
    block *cur = head;
    size_t idx = pos;
    while (cur) {
      if (idx < cur->data.size) {
        auto it = cur->data.begin();
        for (size_t i = 0; i < idx; i++) {
          it++;
        }
        return *it;
      }
      idx -= cur->data.size;
      cur = cur->next;
    }
    throw index_out_of_bound();
  }
  const T &operator[](const size_t &pos) const {
    block *cur = head;
    size_t idx = pos;
    while (cur) {
      if (idx < cur->data.size) {
        auto it = cur->data.begin();
        for (size_t i = 0; i < idx; i++) {
          it++;
        }
        return *it;
      }
      idx -= cur->data.size;
      cur = cur->next;
    }
    throw index_out_of_bound();
  }

  /**
   * access the first element.
   * throw container_is_empty when the container is empty.
   */
  const T &front() const {
    if (total_size == 0)
      throw container_is_empty();
    return *(head->data.begin());
  }

  /**
   * access the last element.
   * throw container_is_empty when the container is empty.
   */
  const T &back() const {
    if (total_size == 0)
      throw container_is_empty();
    return *(tail->data.get_tail());
  }

  /**
   * return an iterator to the beginning.
   */
  iterator begin() {
    return iterator(
        head, head ? head->data.begin() : typename double_list<T>::iterator(),
        this);
  }
  const_iterator cbegin() const {
    return const_iterator(head,
                          head ? head->data.cbegin()
                               : typename double_list<T>::const_iterator(),
                          this);
  }

  /**
   * return an iterator to the end.
   */
  iterator end() {
    return iterator(nullptr, typename double_list<T>::iterator(), this);
  }
  const_iterator cend() const {
    return const_iterator(nullptr, typename double_list<T>::const_iterator(),
                          this);
  }

  /**
   * check whether the container is empty.
   */
  bool empty() const {
    return (total_size == 0) ? true : false;
  } // 好蠢，但是我喜欢。

  /**
   * return the number of elements.
   */
  size_t size() const { return total_size; }

  /**
   * clear all contents.
   */
  void clear() {
    while (head != nullptr) {
      block *tmp = head;
      head = head->next;
      tmp->data.clear();
      delete tmp;
    }
    tail = nullptr;
    total_size = 0;
  }

  /**
   * insert value before pos.
   * return an iterator pointing to the inserted value.
   * throw if the iterator is invalid or it points to a wrong place.
   */
  iterator insert(iterator pos, const T &value) {
    if (empty()) {
      if (pos != end())
        throw invalid_iterator();
      // 初始化第一个块
      allocateInitialBlock(value);
      return begin();
    }

    // 处理插入到 begin()
    if (pos == begin()) {
      push_front(value);
      return begin();
    }

    // 处理插入到 end()
    if (pos == end()) {
      push_back(value);
      return --end(); // 安全递减，因为 deque 非空
    }

    // 常规插入检查
    if (!pos.current || pos.iter == pos.current->data.end())
      throw invalid_iterator();

    // 执行插入并维护结构
    auto inserted_iter = pos.current->data.insert(pos.iter, value);
    total_size++;

    if (pos.current->data.size > max_block_size())
      split_block(pos.current);

    return iterator(pos.current, inserted_iter, this);
  }

  // 辅助方法：在deque为空时初始化第一个块
  void allocateInitialBlock(const T &value) {
    block *new_block = new block();
    new_block->data.insert_tail(value);
    head = tail = new_block;
    total_size = 1;
  }

  /**
   * remove the element at pos.
   * return an iterator pointing to the following element. if pos points to
   * the last element, return end(). throw if the container is empty,
   * the iterator is invalid, or it points to a wrong place.
   */
  iterator erase(iterator pos) {
    if (!pos.current || total_size == 0)
      throw container_is_empty();
    pos.iter = pos.current->data.erase(pos.iter);
    total_size--;
    if (pos.current->data.size < min_block_size() && pos.current->next) {
      merge_blocks(pos.current);
    }
    if (pos.iter == pos.current->data.end() && pos.current->next)
      return iterator(pos.current->next, pos.current->next->data.begin(), this);
    return pos;
  }

  /**
   * add an element to the end.
   */
  void push_back(const T &value) {
    if (!tail) {
      allocateInitialBlock(value);
      return;
    }

    tail->data.insert_tail(value);
    total_size++;

    if (tail->data.size > max_block_size())
      split_block(tail);
  }

  /**
   * remove the last element.
   * throw when the container is empty.
   */
  void pop_back() {
    if (!tail || total_size == 0)
      throw container_is_empty();
    tail->data.delete_tail();
    total_size--;
    if (tail->data.size < min_block_size() && tail->prev) {
      merge_blocks(tail->prev);
    }
  }

  /**
   * insert an element to the beginning.
   */

  void push_front(const T &value) {
    if (!head) {
      allocateInitialBlock(value);
      return;
    }

    head->data.insert_head(value);
    total_size++;

    if (head->data.size > max_block_size())
      split_block(head);
  }

  /**
   * remove the first element.
   * throw when the container is empty.
   */
  void pop_front() {
    if (!head || total_size == 0)
      throw container_is_empty();
    head->data.delete_head();
    total_size--;
    if (head->data.size < min_block_size() && head->next) {
      merge_blocks(head);
    }
  }
};

} // namespace sjtu

#endif
