{Concurrent LRU Cache}
=====

.. All external links are here
.. image:: https://github.com/vsdmars/LN_LRU_Cache/actions/workflows/lruc_action.yaml/badge.svg?branch=master
  :target: https://github.com/vsdmars/LN_LRU_Cache/actions/workflows/lruc_action.yaml
.. image:: https://app.codacy.com/project/badge/Grade/8613cff7339947578b7a8897535a1f88
  :target: https://www.codacy.com/gh/vsdmars/LN_LRU_Cache/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=vsdmars/LN_LRU_Cache&amp;utm_campaign=Badge_Grade
.. image:: https://img.shields.io/badge/license-MIT-blue.svg
  :target: https://raw.githubusercontent.com/vsdmars/LN_LRU_Cache/master/LICENSE
.. _intel-tbb: https://github.com/oneapi-src/oneTBB
.. ;; And now we continue with the actual content

----

**{Concurrent LRU Cache}** is an header only concurrent safe LRU cache supports generic types with simple API.

----

LRUCache and Scaled LRUCache
----------------------------
Concurrent LRUCache provides thread-safe access with defined size limit.

find() : concurrent access to cache with specified key and returns value.

insert() : insert key with value.

erase() : evict cache with specified key.

capacity() : capacity of the cache.

size() : current cache size.

clear() : evict all cache entries.

For heavy concurrent insert/evict load, scaled-lru cache is provided.


Examples
--------
**LRU Cache** (`run <https://godbolt.org/z/Y6he8z9Gf>`_)

.. code:: c++

    #include <lrucache.h>

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
        if (lruc.find(ca, 5)) {
            std::cout << *ca << std::endl;
        }

        lruc.erase(2);
        if (!lruc.find(ca, 2)) {
            std::cout << "2 is gone." << std::endl;
        }
    }

**Scaled LRU Cache**

.. code:: c++

    #include <scale-lrucache.h>

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
