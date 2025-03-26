std::pair<bool, double> iteratorInsertPersistenceChecker() {
  std::deque<Int> a;
  sjtu::deque<Int> b;
  std::deque<Int>::iterator itA;
  sjtu::deque<Int>::iterator itB;
  itA = a.end();
  itB = b.end();
  timer.init();
  for (int i = 0, delta; i < N; i++) {
    int tmp = rand();
    delta = itA - a.begin();
    if (b.begin() + delta != itB) {
      std::cout << "here1 ";
      return std::make_pair(false, 0);
    }
    if (rand() % 2) {
      if (itA != a.begin()) {
        int offset = rand() % (itA - a.begin());
        itA -= offset;
        itB -= offset;
      }
    } else {
      if (itA != a.end()) {
        int offset = rand() % (a.end() - itA);
        itA += offset;
        itB += offset;
      }
    }
    delta = itA - a.begin();
    if (b.begin() + delta != itB) {
      std::cout << "here4 ";
      return std::make_pair(false, 0);
    }

    itA = a.insert(itA, tmp);
    DEBUG = true;
    itB = b.insert(itB, tmp);
    DEBUG = false;
    delta = itA - a.begin();
    count++;
    if (b.begin() + delta != itB) {
      std::cout << count << std::endl;
      return std::make_pair(false, 0);
    }

    if (*itA != *itB) {
      std::cout << "here3 ";
      return std::make_pair(false, 0);
    }
  }

  timer.stop();
  if (!isEqual(a, b)) {
    return std::make_pair(false, 0);
  } else {
    return std::make_pair(true, timer.getTime());
  }
}
