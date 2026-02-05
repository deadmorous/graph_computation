/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "common/ring_buffer.hpp"

#include <gtest/gtest.h>

#include <vector>


TEST(RingBufferTest, PopFrontWorks) {
    common::RingBuffer<int> rb(3);
    rb.push_back(10);
    rb.push_back(20);
    rb.push_back(30);

    rb.pop_front();
    EXPECT_EQ(rb.size(), 2);
    EXPECT_EQ(rb.front(), 20);

    rb.pop_front();
    EXPECT_EQ(rb.front(), 30);

    rb.pop_front();
    EXPECT_TRUE(rb.empty());
}

TEST(RingBufferTest, PopFrontAfterWrap) {
    common::RingBuffer<int> rb(2);
    rb.push_back(1);
    rb.push_back(2);
    rb.push_back(3); // Overwrites 1. Buffer: [3, 2], head at index 1 (val 2)

    EXPECT_EQ(rb.front(), 2);
    rb.pop_front();
    EXPECT_EQ(rb.front(), 3);
}

TEST(RingBufferTest, ComplexTypeResourceRelease) {
    // Testing that pop_front/clear effectively "clears" the object
    static int destruct_count = 0;
    struct Probe {
        ~Probe() { if (active) destruct_count++; }
        bool active = false;
    };

    {
        common::RingBuffer<Probe> rb(5);
        rb.push_back(Probe{true});
        rb.push_back(Probe{true});
        rb.pop_front();
        // 1 from the pop, plus temporary copies created during push
        EXPECT_GT(destruct_count, 0);
    }
}

TEST(RingBufferTest, FullIteratorTraversal) {
    common::RingBuffer<int> rb(5);
    for(int i=0; i<8; ++i) rb.push_back(i); // 0..7, capacity 5 -> [3, 4, 5, 6, 7]

    std::vector<int> vals;
    for(auto it = rb.begin(); it != rb.end(); ++it) {
        vals.push_back(*it);
    }

    EXPECT_EQ(vals.size(), 5);
    EXPECT_EQ(vals[0], 3);
    EXPECT_EQ(vals[4], 7);
}

TEST(RingBufferTest, SegmentsEmptyOnPop) {
    common::RingBuffer<int> rb(3);
    rb.push_back(1);
    rb.push_back(2);
    rb.pop_front();
    rb.pop_front();

    EXPECT_TRUE(rb.first_segment().empty());
    EXPECT_TRUE(rb.second_segment().empty());
}

TEST(RingBufferTest, BackwardIteratorTraversal)
{
    common::RingBuffer<int> rb(5);
    for(int i=0; i<8; ++i) rb.push_back(i); // 0..7, capacity 5 -> [3, 4, 5, 6, 7]

    std::vector<int> vals;
    for(auto it = rb.end(); it != rb.begin();) {
        --it;
        vals.push_back(*it);
    }

    EXPECT_EQ(vals.size(), 5);
    EXPECT_EQ(vals[0], 7);
    EXPECT_EQ(vals[4], 3);
}

TEST(RingBufferTest, IndexAccess)
{
    common::RingBuffer<int> rb(5);
    for(int i=0; i<8; ++i) rb.push_back(i); // 0..7, capacity 5 -> [3, 4, 5, 6, 7]

    EXPECT_EQ(rb[0], 3);
    EXPECT_EQ(rb[2], 5);
    EXPECT_EQ(rb[4], 7);

    EXPECT_EQ(rb.begin()[2], 5);
    EXPECT_EQ((rb.begin()+1)[2], 6);
}
