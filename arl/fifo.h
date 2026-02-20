#pragma once 

#include <cassert>
#include <atomic>
#include <optional>
#include <vector>
#include <memory>
#include <new>

#if 0 //def __cpp_lib_hardware_interference_size
  using std::hardware_constructive_interference_size;
  using std::hardware_destructive_interference_size;
#else 
  constexpr size_t hardware_constructive_interference_size = 64;
  constexpr size_t hardware_destructive_interference_size = 64;
#endif

template <class T, class alloc = std::allocator<T>>
class fifo_t {
  static_assert(std::atomic<size_t>::is_always_lock_free);

  using traits = std::allocator_traits<alloc>;
  alloc Alloc;

  size_t mCapacity;

  alignas(hardware_destructive_interference_size) std::atomic<size_t> mPushCursor{};
  alignas(hardware_destructive_interference_size) size_t mCachedPushCursor{};

  alignas(hardware_destructive_interference_size) std::atomic<size_t> mPopCursor{};
  alignas(hardware_destructive_interference_size) size_t mCachedPopCursor{};

  //std::byte padding[hardware_destructive_interference_size - sizeof(size_t)];

  T* mData;

  auto full(size_t push, size_t pop) const { return (push - pop) == mCapacity; }
  auto empty(size_t push, size_t pop) const { return push == pop; }
  auto element(size_t elem) { return mData + (elem & (mCapacity - 1)); }

  public:
  explicit fifo_t(size_t size) 
    : mCapacity(size)
  {
    assert(size > 0);
    assert(!(size & (size - 1)));

    mData = traits::allocate(Alloc, mCapacity);
  }

  ~fifo_t() {
    while (!empty()) {
      auto e = pop();
    }
    traits::deallocate(Alloc, mData, mCapacity);
  }

  auto capacity() const { return mCapacity; }
  auto size() const { return mPushCursor - mPopCursor; }
  auto empty() const { return empty(mPushCursor, mPopCursor); }
  auto full() const { return full(mPushCursor, mPopCursor); }

  bool push(const T& value) {
    auto push = mPushCursor.load(std::memory_order_relaxed);

    if (full(push, mPopCursor)) {
      mPopCursor = mPopCursor.load(std::memory_order_acquire);
      if (full(push, mPopCursor)) return false;
    }

    traits::construct(Alloc, element(push), value);

    mPushCursor.store(push + 1, std::memory_order_release);

    return true;
  }

  bool push(T&& value) {
    auto push = mPushCursor.load(std::memory_order_relaxed);

    if (full(push, mPopCursor)) {
      mPopCursor = mPopCursor.load(std::memory_order_acquire);
      if (full(push, mPopCursor)) return false;
    }

    traits::construct(Alloc, element(push), std::move(value));

    mPushCursor.store(push + 1, std::memory_order_release);

    return true;
  }

  std::optional<T> pop() {
    auto pop = mPopCursor.load(std::memory_order_relaxed);

    if (empty(mCachedPushCursor, pop)) {
      mCachedPushCursor= mPushCursor.load(std::memory_order_acquire);
      if (empty(mCachedPushCursor, pop)) return {};
    }

    auto value = std::move(*element(pop));

    mPopCursor.store(pop + 1, std::memory_order_release);

    return value;
  }

}; 

