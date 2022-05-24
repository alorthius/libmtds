// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <gtest/gtest.h>
#include "mtds/details/tagged_ptr.hpp"

constexpr unsigned NUM_OPERATIONS = 1 << 14;  // Tag overflow limit

class TaggedPtrTest : public ::testing::Test {};

TEST_F(TaggedPtrTest, PtrWorks) {
    for (unsigned i = 0; i < NUM_OPERATIONS; ++i) {
        int* ptr = new int{};
        mtds::tagged_ptr::TaggedPtr<int> p{ptr};
        EXPECT_EQ(p.ptr(), ptr);
        delete ptr;
    }
}

TEST_F(TaggedPtrTest, TagWorks) {
    for (unsigned tag = 0; tag < NUM_OPERATIONS; ++tag) {
        mtds::tagged_ptr::TaggedPtr<int> p{nullptr, tag};
        ASSERT_EQ(p.tag(), tag);
    }
}

TEST_F(TaggedPtrTest, PtrAndTagWork) {
    for (unsigned tag = 0; tag < NUM_OPERATIONS; ++tag) {
        int* ptr = new int{};
        mtds::tagged_ptr::TaggedPtr<int> p{ptr, tag};
        ASSERT_EQ(p.ptr(), ptr);
        delete ptr;
        ASSERT_EQ(p.tag(), tag);
    }
}