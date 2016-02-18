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

#include <folly/Varint.h>

#include "DataView.h"
#include "WriteBatch.h"
#include "InternalKey.h"

namespace lessdb {

class WriteBatch::Impl {

  static constexpr size_t kCountSize = sizeof(uint32_t);
  static constexpr size_t kSeqSize = sizeof(uint64_t);
  static constexpr size_t kHeaderSize = kSeqSize + kCountSize;

 public:

  Impl() {
    bytes_.reserve(kHeaderSize);
  }

  inline int Count() {
    return DataView(&bytes_[kSeqSize]).ReadNum<int>();
  }

  inline void SetCount(int count) {
    DataView(&bytes_[kSeqSize]).WriteNum(count);
  }

  inline void PutRecord(const Slice &key, const Slice &value) {
    SetCount(Count() + 1); // count++
    bytes_.push_back(static_cast<char>(kTypeValue));
    AppendVarString(key);
    AppendVarString(value);
  }

  inline void DeleteRecord(const Slice &key) {
    SetCount(Count() - 1); // count--
    bytes_.push_back(static_cast<char>(kTypeDeletion));
    AppendVarString(key);
  }

  inline void AppendVarInt(std::string *res, uint64_t v) {
    uint8_t buf[kCountSize];
    res->append(reinterpret_cast<char *>(buf), folly::encodeVarint(v, buf));
  }

  inline void AppendVarString(const Slice &str) {
    AppendVarInt(&bytes_, str.Len());
    bytes_.append(str.RawData(), str.Len());
  }

 private:

  // bytes_ is the internal representation of WriteBatch.
  //
  // bytes_ :=
  //    sequence: fixed64
  //    count: fixed32
  //    data: record[count]
  // record :=
  //    kTypeValue varstring varstring         |
  //    kTypeDeletion varstring
  // varstring :=
  //    len: varint32
  //    data: uint8[len]
  std::string bytes_;
};

WriteBatch::WriteBatch() :
    pImpl_(new Impl()) {
}

void WriteBatch::Put(Slice key, Slice value) {
  pImpl_->PutRecord(key, value);
}

void WriteBatch::Delete(Slice key) {
  pImpl_->DeleteRecord(key);
}

} // namespace lessdb
