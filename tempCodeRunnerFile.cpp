void test3() {
  printf("test3: itetator operation            ");
  int num = q.size();
  for (int i = 1; i <= 1000; i++) {
    int t1 = rand() % num;
    int t2 = rand() % num;
    if (*(q.begin() + t1) != *(stl.begin() + t1)) {
      puts("Wrong Answer");
      return;
    }
    if (t2 && *(q.end() - t2) != *(stl.end() - t2)) {
      puts("Wrong Answer");
      return;
    }
    if ((q.begin() + t1) - (q.begin() + t2) != (t1 - t2)) {
      puts("Wrong Answer");
      return;
    }
  }
  if ((q.begin() + num) != q.end()) {
    puts("Wrong Answer");
    return;
  }
  if ((q.end() - num) != q.begin()) {
    puts("Wrong Answer");
    return;
  }
  bool flag = 0;
  sjtu::deque<T> other;
  try {
    int t = q.begin() - other.begin();
  } catch (...) {
    flag = 1;
  }
  if (!flag && need_to_check_throw) {
    puts("Wrong Answer");
    return;
  }
  it_q = q.begin();
  it_stl = stl.begin();
  for (int i = 1; i <= 10; i++) {
    int t = rand() % (num / 10);
    it_q += t;
    it_stl += t;
    if (*it_q != *it_stl) {
      puts("Wrong Answer");
      return;
    }
    if (it_q->num() != it_stl->num()) {
      puts("Wrong Answer");
      return;
    }
  }
  it_q = --q.end();
  it_stl = --stl.end();
  if (*it_q != *it_stl) {
    puts("Wrong Answer");
    return;
  }
  for (int i = 1; i < 10; i++) {
    int t = rand() % (num / 10);
    it_q -= t;
    it_stl -= t;
    if (*it_q != *it_stl) {
      puts("Wrong Answer");
      return;
    }
    it_q->change(t);
    ;
    it_stl->change(t);
    if (*it_q != *it_stl) {
      puts("Wrong Answer");
      return;
    }
  }
  if (!equal()) {
    puts("Wrong Answer");
    return;
  }
  if (!(q.begin() + 10 == q.begin() + 5 + 6 - 1)) {
    puts("Wrong Answer");
    return;
  }
  sjtu::deque<T> pp;
  if (q.end() == pp.end()) {
    puts("Wrong Answer");
    return;
  }

  int t = rand() % (q.size() - 1);
  it_q = q.begin() + t;
  it_stl = stl.begin() + t;
  const sjtu::deque<T>::iterator it_q_const(++it_q);
  const std::deque<T>::iterator it_stl_const(++it_stl);
  if (*it_q_const != *it_stl_const) {
    puts("Wrong Answer");
    return;
  }
  if (it_q_const->num() != it_stl_const->num()) {
    puts("Wrong Answer");
    return;
  }
  it_q_const->change(t);
  it_stl_const->change(t);
  if (!equal()) {
    puts("Wrong Answer");
    return;
  }
  puts("Accept");
}
