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

#include "Slice.h"

namespace lessdb {

inline bool operator==(const Slice &lhs, const Slice &rhs) {
  return lhs.Compare(rhs) == 0;
}

inline std::ostream &operator<<(std::ostream &stream, const Slice &s) {
  return stream << s.RawData();
}

inline int Slice::Compare(const Slice &rhs) const {
  size_t min_size = std::min(rhs.len_, len_);
  int ret = 0;

  // It's illegal to pass nullptr to memcmp.
  if (str_ && rhs.str_)
    ret = memcmp(str_, rhs.str_, len_);

  if (ret == 0) {
    // Use len_ - rhs.len_ as returned value may cause overflow
    // of size_t type length. Therefore +1, -1 are returned here.
    if (len_ < rhs.len_) return -1;
    else if (len_ > rhs.len_) return 1;
  }
  return ret;
}

} // namespace lessdb
