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
      if (ptr == nullptr)
        throw "invalid";
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

  iterator begin() { return iterator(this, head); }
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
