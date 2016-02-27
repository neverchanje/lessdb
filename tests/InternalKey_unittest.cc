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

#include <gtest/gtest.h>

#include "InternalKey.h"
#include "Comparator.h"

using namespace lessdb;

TEST(Basic, InternalKeyComparator) {
  ASSERT_EQ(ByteWiseComparator()->Name(), "lessdb.ByteWiseComparator");
  const Comparator *comparator = InternalKeyComparator(ByteWiseComparator());
  ASSERT_EQ(comparator->Name(), "lessdb.InternalKeyComparator");
}

TEST(Basic, InternalKey) {
  InternalKeyBuf ibuf("", 0, kTypeValue);
  InternalKey ikey(ibuf.Data());
  ASSERT_EQ(ikey.user_key, "");
  ASSERT_EQ(ikey.sequence, 0);
  ASSERT_EQ(ikey.type, kTypeValue);

  ibuf = InternalKeyBuf("abc", 10, kTypeDeletion);
  ikey = InternalKey(ibuf.Data());
  ASSERT_EQ(ikey.user_key, "abc");
  ASSERT_EQ(ikey.sequence, 10);
  ASSERT_EQ(ikey.type, kTypeDeletion);
}