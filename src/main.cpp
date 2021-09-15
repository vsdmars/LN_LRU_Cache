#include "../include/lrucache.h"

#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

using MyCache = LRUC::LRUCache<int, string>;

auto main(int argc, char* argv[]) -> int {
  MyCache lruc{42};

  lruc.insert(1, "I");
  lruc.insert(2, "Will");
  lruc.insert(3, "Coding");
  lruc.insert(4, "In");
  lruc.insert(5, "Rust");
  lruc.insert(6, "Soon :-)");

  MyCache::ConstAccessor ca;
  if (lruc.find(ca, 1)) {
    std::cout << *ca << std::endl;
  }
}
