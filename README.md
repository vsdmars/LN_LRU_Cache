Cmake cmd:
$ cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_FIND_DEBUG_MODE=ON -G Ninja ../
$ ninja

Requirement:
LRU cache with confined size
Thread-safe (lock-free or short lock period with mutex thread affinity)
Low latency in write
Fast in read
Cache size dynamic tuning(smart tuning) [Phase II]
Generic, supports key/value in hashable types
Batch update LRU cache to reduce locking to next level. [Phase II]

To fulfil request 1) is straightforward, we will use traditional implement technique by using two data structures wrapped in our LRU cache type, i.e a concurrent, node-level lock implement of hashmap, a STL thread safe level(i.e Thread-compatible) linked list with caller applying lock to update the list.

2), by using Intel TBB’s concurrent_hash_map’s scoped lock, which separates read/write with different lock levels on a single node instead of using coarse-grained synchronization.

3), to slake heavy concurrent insert/evict load on a concurrent hash map, we use the technique of sharding. With multiple concurrent_hash_map_s to mitigate lock-wait queuing on a single hashmap.

4), lookup(find), first check if the key(IP) exists by using hash masking(like bloomfilter); if not, return early, else apply try lock to update internal LRU linked list, could fail to update if acquire lock failed, this to prevent blocking.

5), considered in phase II, using one dedicated thread to check the evicting rate of each of the sub LRU caches(shared LRU cache), increase its size while rate goes up, and decrease its size after a cool down period. The cool down period acts as purging based on statistics, as a side-track like LFU cache but based on time expiration.
Tunable config: 1. Upper bound memory usage, 2. Evicting rate, 3. Cool down period/Expiration period.

6), C++ template

7), considered in phase II, ringbuffer/cicular buffer per thread(thread affinity set) for batch updating. By thread affinity we could avoid the ringbuffer being ping-ponged among CPU cache, and only context switch on the same CPU core. Batch update to avoid locking in our LRU cache type thus avoid lock being ping-ponged among CPU cores. The size of the batch should be based on CPU cache size for better prefetching.

Operations:
Sharding: decide which concurrent map this key belongs to.
Find : lookup entry in the concurrent_hash_map by key
Insert : create entry and map it to key
Update LRU list : Push front the node in the LRU list indicates recently accessed.
Evict : based on LRU cache size, remove nodes at the end of the list and the linked entries from the concurrent hash map, one key per call.
Purge : unlink unused nodes in the LRU list.

Functions:
Insert:
Create a LRU list node.
If the key already exists during the insert in the concurrent hash map, return false and delete the created LRU list node.
Check LRU cache size, evict if necessary after insert successfully to the LRU cache(have not updated the LRU list yet in this stage).
Update LRU list with current insertion node.
For existing keys a reinsert would not update the LRU list for cache efficiency.

Batch insert:
Create LRU list nodes.
Preserve any keys inserted successfully.
Check LRU cache size, evict if necessary after insert successfully to the LRU cache.(have not updated the LRU list yet in this stage).
Update LRU list with current insertion nodes.

Find:
Look up the key in LRU cache.
If cache hit:
Tries to update LRU list, could fail but OK.
Else:
hash masking, returns early with no value.

Reference:
http://vsdmars.blogspot.com/2020/06/godesign-high-through-put-low.html
https://tech.ebayinc.com/engineering/high-throughput-thread-safe-lru-caching/
