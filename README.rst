{Concurrent LRU Cache}
=====

.. All external links are here
.. image:: https://github.com/vsdmars/LN_LRU_Cache/actions/workflows/lruc_action.yaml/badge.svg?branch=master
  :target: https://github.com/vsdmars/LN_LRU_Cache/actions/workflows/lruc_action.yaml
.. _intel-tbb: https://github.com/oneapi-src/oneTBB
.. ;; And now we continue with the actual content

----

**{Concurrent LRU Cache}** is an header only concurrent safe LRU cache implementation.

----


Examples
--------

.. code:: c++

    #include <fmt/core.h>

    int main() {
      fmt::print("Hello, world!\n");
    }

.. code:: c++

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



.. code:: c++

    using MyCache = LRUC::ScalableLRUCache<int, string>;

    auto main(int argc, char* argv[]) -> int {
        MyCache lruc{42};

        lruc.insert(1, "I");
        lruc.insert(2, "Will");
        lruc.insert(3, "Coding");
        lruc.insert(4, "In");
        lruc.insert(5, "Go");
        lruc.insert(6, "Even Sooner :-)");

        MyCache::ConstAccessor ca;
        if (lruc.find(ca, 1)) {
            std::cout << *ca << std::endl;
        }
    }


Requirements
------------
`intel-tbb`_ version 5 or later



Contact
-------
**vsdmars<<at>>gmail.com**
