/**
 * @author shchang
 */

#pragma once
#include "lrucache.h"

#include <cmath>
#include <limits>
#include <memory>

namespace LRUC {

template <class TKey, class TValue, class THash = tbb::tbb_hash_compare<TKey>>
class ScalableLRUCache final {
private:
  using Shard = LRUCache<TKey, TValue, THash>;
  using ShardPtr = std::unique_ptr<Shard>;

  std::vector<ShardPtr> shards_;
  const size_t cache_size_;
  size_t shard_count_;

private:
  /**
   * shard returns a Shard (LRUCache instance) based on key.
   */
  Shard& shard(const TKey& key);

public:
  using ConstAccessor = typename Shard::ConstAccessor;

  /**
   * size: ScalableLRUCache capacity. And each internal LRUCache's capacity can be changed at runtime TODO(shchang)
   * shard_count: shard count.
   */
  explicit ScalableLRUCache(size_t size, size_t shard_count = 0);

  ~ScalableLRUCache() noexcept { clear(); }

  ScalableLRUCache(const ScalableLRUCache&) = delete;
  ScalableLRUCache& operator=(const ScalableLRUCache&) = delete;

  size_t erase(const TKey& key);

  bool find(ConstAccessor& caccessor, const TKey& key);

  bool insert(const TKey& key, const TValue& value);

  void clear() noexcept;

  long long size() const;
  int size(size_t shard_idx) const;

  long long capacity() const;
  int capacity(size_t shard_idx) const;

  size_t shardCount() const;
};

// ---- private member functions ----
template <class TKey, class TValue, class THash>
typename ScalableLRUCache<TKey, TValue, THash>::Shard& ScalableLRUCache<TKey, TValue, THash>::shard(const TKey& key) {
  THash hashObj{};
  // lower 16 bits counted as hash key
  constexpr int shift = std::numeric_limits<size_t>::digits - 16;

  // According to intel TBB doc:
  // Good performance depends on having good pseudo-randomness in the low-order bits of the hash code.
  size_t h = (hashObj.hash(key) >> shift) % shard_count_;

  return *shards_[h];
}
// ---- private member functions end ----

template <class TKey, class TValue, class THash>
ScalableLRUCache<TKey, TValue, THash>::ScalableLRUCache(size_t size, size_t shard_count)
    : cache_size_(size), shard_count_(shard_count > 0 ? shard_count : std::thread::hardware_concurrency()) {
  const size_t bucket_count =
      static_cast<size_t>(std::ceil(std::log2(shard_count_ / std::thread::hardware_concurrency()) + 0.5));

  size_t cap = cache_size_ / shard_count_;
  size_t modular = cache_size_ % shard_count_;

  for (size_t i = 0; i < shard_count_; i++) {
    shards_.emplace_back(std::make_unique<Shard>(i != 0 ? cap : (cap + modular), bucket_count));
  }
}

template <class TKey, class TValue, class THash>
size_t ScalableLRUCache<TKey, TValue, THash>::erase(const TKey& key) {
  return shard(key).erase(key);
}

template <class TKey, class TValue, class THash>
bool ScalableLRUCache<TKey, TValue, THash>::find(ConstAccessor& caccessor, const TKey& key) {
  return shard(key).find(caccessor, key);
}

template <class TKey, class TValue, class THash>
bool ScalableLRUCache<TKey, TValue, THash>::insert(const TKey& key, const TValue& value) {
  return shard(key).insert(key, value);
}

template <class TKey, class TValue, class THash>
void ScalableLRUCache<TKey, TValue, THash>::clear() noexcept {
  for (size_t i = 0; i < shard_count_; i++) {
    shards_[i]->clear();
  }
}

template <class TKey, class TValue, class THash>
long long ScalableLRUCache<TKey, TValue, THash>::size() const {
  long long size = 0;
  for (size_t i = 0; i < shard_count_; i++) {
    size += shards_[i]->size();
  }
  return size;
}

template <class TKey, class TValue, class THash>
int ScalableLRUCache<TKey, TValue, THash>::size(size_t shard_idx) const {
  if (shard_idx < shard_count_) {
    return shards_[shard_idx]->size();
  }

  return 0;
}

template <class TKey, class TValue, class THash>
long long ScalableLRUCache<TKey, TValue, THash>::capacity() const {
  long long size = 0;
  for (size_t i = 0; i < shard_count_; i++) {
    size += shards_[i]->capacity();
  }

  return size;
}

template <class TKey, class TValue, class THash>
int ScalableLRUCache<TKey, TValue, THash>::capacity(size_t shard_idx) const {
  if (shard_idx < shard_count_) {
    return shards_[shard_idx]->capacity();
  }

  return 0;
}

template <class TKey, class TValue, class THash>
size_t ScalableLRUCache<TKey, TValue, THash>::shardCount() const {
  return shard_count_;
}
}  // namespace LRUC
