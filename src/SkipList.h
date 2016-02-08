/**
 * Copyright (C) 2016, Wu Tao. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <functional>
#include <iterator>
#include <random>
#include <cassert>

#include "Disallowcopying.h"

namespace lessdb {

// SkipLists are a probabilistic balanced data structure.
// This implementation is based on the paper
// [Skip Lists: A Probabilistic Alternative to Balanced Trees] by William Pugh.
//
// NOTE: Allocated nodes are never deleted until the SkipList is destroyed.

// Compare takes two element keys as arguments and returns a bool.
// The expression comp(a,b), where comp is an object of this type
// and a and b are key values, shall return true if a is considered to go
// before b in the strict weak ordering the function defines.

template<class T, class Compare = std::less<T> >
class SkipList {
  __DISALLOW_COPYING__(SkipList);

  struct Node;

  static const unsigned MaxLevel = 12;

 public:

  struct Iterator {
   private:
    // intentionally copyable
    const Node *node_;

   public:

    explicit Iterator(const Node *n) :
        node_(n) {
    }

    // prefix
    T &operator++() {
      node_ = node_->Next(0);
      return node_->key;
    }

    // postfix
    T operator++(int) {
      T ret = node_->key;
      node_ = node_->Next(0);
      return ret;
    }

    const T &operator*() const {
      return node_->key;
    }

    const T *operator->() const {
      return &(node_->key);
    }

    friend bool operator==(const Iterator &x, const Iterator &y) noexcept {
      return x.node_ == y.node_;
    }

    friend bool operator!=(const Iterator &x, const Iterator &y) noexcept {
      return x.node_ == y.node_;
    }
  };

 public:

  explicit SkipList(const Compare &compare = Compare()) :
      distrib_(0, 3),
      head_(nullptr),
      height_(0),
      compare_(compare) {
  }

  ~SkipList() noexcept = default;

  Iterator Insert(const T &key);

//  Iterator Find(const T &key);

  bool Empty() const noexcept {
    return head_ == nullptr;
  }

  Iterator LowerBound(const T &key) const;

  Iterator UpperBound(const T &key) const;

//  bool Size() const noexcept;

  Iterator Begin() const noexcept {
    return Iterator(head_);
  }

  Iterator End() const noexcept {
    return Iterator(nullptr);
  }

  int GetHeight() const;

 private:

  int randomLevel();

  int random();

  bool KeyEq(const T &k1, const T &k2) {
    return compare_(k1, k2) && !compare_(k1, k2);
  }

 private:
  Node *const head_;
  int height_;
  Compare compare_;

  // random number ranges in [0, 3]
  std::default_random_engine gen_;
  std::uniform_int_distribution<int> distrib_;
};

template<class T, class Compare>
struct SkipList<T, Compare>::Node {
  const T key;
  Node *forward[];

  Node(const T &k, int height) :
      key(k),
      forward(new Node *[height]) {
  }

  Node *Next(int level) const {
    return forward[level];
  }

  void SetNext(Node *next, int level) {
    forward[level] = next;
  }
};

template<class T, class Compare>
inline int SkipList<T, Compare>::randomLevel() {
  int height = 1;
  while (height < MaxLevel && random() == 0) height++;
  return height;
}

template<class T, class Compare>
inline int SkipList<T, Compare>::random() {
  return distrib_(gen_);
}

template<class T, class Compare>
typename SkipList<T, Compare>::Iterator
SkipList<T, Compare>::Insert(const T &key) {
  Node *update[MaxLevel];
  int height = GetHeight() - 1;
  Node *x = head_;

  for (int level = height; level >= 1; level--) {
    Node *next = x->Next(level);
    while (next != nullptr && compare_(key, next->key)) {
      x = next;
      next = x->Next(level);
    }
    update[level] = x;
  }

  x = x->Next(0); // x >= key or x == nullptr
  if (x != nullptr && KeyEq(key, x->key)) {
    return Iterator(x);
  }

  int level = randomLevel();
  if (level > height_) {
    for (int i = height_; i < level; i++) {
      update[i] = head_;
    }
    height_ = level;
  }

  x = new Node(key, level);
  for (int i = 0; i < level; i++) {
    x->forward[i] = update[i]->forward[i];
    update[i]->forward[i] = x;
  }

  return Iterator(x);
}

template<class T, class Compare>
inline int SkipList<T, Compare>::GetHeight() const {
  return height_;
}

template<class T, class Compare>
typename SkipList<T, Compare>::Iterator
SkipList<T, Compare>::LowerBound(const T &key) const {
  Node *x = head_;
  int height = GetHeight() - 1;

  for (int level = height; level >= 0; level--) {
    Node *next = x->Next(level);
    while (next != nullptr && compare_(key, next->key)) {
      x = next;
      next = x->Next(level);
    }
    // next >= x
    if (level == 0)
      return Iterator(x);
  }
  assert(0);
}

template<class T, class Compare>
typename SkipList<T, Compare>::Iterator
SkipList<T, Compare>::UpperBound(const T &key) const {
  Node *x = head_;
  int height = GetHeight() - 1;

  for (int level = height; level >= 0; level--) {
    Node *next = x->Next(level);
    while (next != nullptr && compare_(key, next->key)) {
      x = next;
      next = x->Next(level);
    }
    // next >= x
    if (level == 0)
      return Iterator(next);
  }
  assert(0);
}

} // namespace lessdb