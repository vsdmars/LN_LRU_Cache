/**
 * @author shchang
 */

#pragma once

#include <atomic>
#include <functional>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

namespace LRUC {

template <typename TKey, typename TValue, typename THash = std::hash<TKey>, typename TKeyEqual = std::equal_to<TKey>>
class LRUClockCache final {
private:
  // type defs
  using HashMap = std::unordered_map<TKey, size_t, THash, TKeyEqual>;
  using Mutex = std::shared_mutex;
  using CharVector = std::vector<std::atomic<char>>;
  using KeyVector = std::vector<TKey>;
  using ValueVector = std::vector<TValue>;
  using Optional = std::optional<TValue>;

private:
  Mutex mutex_;
  HashMap hash_map_;
  KeyVector keyBuf_;
  ValueVector valueBuf_;
  CharVector surviveBuf_;
  const size_t capacity_;
  size_t cur_idx_;
  size_t evict_idx_;

public:
  explicit LRUClockCache(size_t size);

  ~LRUClockCache() noexcept { clear(); }

  LRUClockCache(const LRUClockCache& other) = delete;
  LRUClockCache& operator=(const LRUClockCache&) = delete;

  typename HashMap::size_type size() const { return hash_map_.size(); }
  constexpr size_t capacity() const noexcept { return capacity_; }

  void clear() noexcept;
  size_t erase(const TKey& key);
  Optional find(const TKey& key);
  bool insert(const TKey& key, const TValue& value);
};

template <typename TKey, typename TValue, typename THash, typename TKeyEqual>
LRUClockCache<TKey, TValue, THash, TKeyEqual>::LRUClockCache(size_t size)
    : surviveBuf_(size), capacity_(size), cur_idx_(0), evict_idx_(capacity_ / 2) {
  hash_map_.reserve(capacity_);
  keyBuf_.resize(capacity_);
  valueBuf_.resize(capacity_);
}

template <typename TKey, typename TValue, typename THash, typename TKeyEqual>
void LRUClockCache<TKey, TValue, THash, TKeyEqual>::clear() noexcept {
  hash_map_.clear();
}

template <typename TKey, typename TValue, typename THash, typename TKeyEqual>
size_t LRUClockCache<TKey, TValue, THash, TKeyEqual>::erase(const TKey& key) {
  std::unique_lock lock(mutex_);
  return hash_map_.erase(key);
}

template <typename TKey, typename TValue, typename THash, typename TKeyEqual>
typename LRUClockCache<TKey, TValue, THash, TKeyEqual>::Optional LRUClockCache<TKey, TValue, THash, TKeyEqual>::find(
    const TKey& key) {
  std::shared_lock lock(mutex_);
  if (auto it = hash_map_.find(key); it != hash_map_.end()) {
    surviveBuf_[it->second] = 1;
    return valueBuf_[it->second];
  } else {
    return {};
  }
}

template <typename TKey, typename TValue, typename THash, typename TKeyEqual>
bool LRUClockCache<TKey, TValue, THash, TKeyEqual>::insert(const TKey& key, const TValue& value) {
  {
    std::shared_lock lock(mutex_);
    if (auto it = hash_map_.find(key); it != hash_map_.end()) {
      return false;
    }
  }

  std::unique_lock lock(mutex_);
  // signed; use -1
  long long victim_idx = -1;

  while (victim_idx == -1) {
    if (surviveBuf_[cur_idx_] > 0) {
      surviveBuf_[cur_idx_] = 0;
    }

    cur_idx_++;
    if (cur_idx_ >= capacity_) {
      cur_idx_ = 0;
    }

    if (surviveBuf_[evict_idx_] == 0) {
      victim_idx = static_cast<decltype(victim_idx)>(evict_idx_);
    }

    evict_idx_++;
    if (evict_idx_ >= capacity_) {
      evict_idx_ = 0;
    }
  }

  hash_map_.erase(keyBuf_[static_cast<size_t>(victim_idx)]);

  keyBuf_[static_cast<size_t>(victim_idx)] = key;
  valueBuf_[static_cast<size_t>(victim_idx)] = value;
  surviveBuf_[static_cast<size_t>(victim_idx)] = 0;
  hash_map_.emplace(key, static_cast<size_t>(victim_idx));

  return true;
}

}  // namespace LRUC
