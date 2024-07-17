#include <gtest/gtest.h>
#include "queue.h"  // Adjust according to your file structure

// Test case for pushing elements into the queue
TEST(QueueTest, PushTest) {
    Queue<int> q(3);
    q.Push(1);
    q.Push(2);
    q.Push(3);
    EXPECT_EQ(q.Count(), 3);

    // Check if the queue drops the oldest element when full
    q.Push(4);
    EXPECT_EQ(q.Count(), 3);
    EXPECT_EQ(q.Pop(), 2);
    EXPECT_EQ(q.Pop(), 3);
    EXPECT_EQ(q.Pop(), 4);
}

// Test case for popping elements from the queue
TEST(QueueTest, PopTest) {
    Queue<int> q(2);
    q.Push(1);
    q.Push(2);
    EXPECT_EQ(q.Pop(), 1);
    EXPECT_EQ(q.Pop(), 2);
}

// Test case for popping elements with timeout
TEST(QueueTest, PopWithTimeoutTest) {
    Queue<int> q(2);
    q.Push(1);
    EXPECT_EQ(q.PopWithTimeout(1000), 1);

    // Expect timeout exception
    try {
        q.PopWithTimeout(500);
        FAIL() << "Expected std::runtime_error";
    } catch (const std::runtime_error& e) {
        EXPECT_EQ(std::string(e.what()), "Timeout while waiting to pop element");
    } catch (...) {
        FAIL() << "Expected std::runtime_error";
    }
}

// Test case for checking the queue size and count
TEST(QueueTest, SizeAndCountTest) {
    Queue<int> q(2);
    EXPECT_EQ(q.Size(), 2);
    EXPECT_EQ(q.Count(), 0);
    q.Push(1);
    EXPECT_EQ(q.Count(), 1);
    q.Push(2);
    EXPECT_EQ(q.Count(), 2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
