#include <gtest/gtest.h>
#include <utils/thread-pool.hpp>
#include <cstring>

#include <iostream>

using namespace utils;

TEST(ThreadPool, UpdateArrayInParallel)
{
    int MAX = 10;
    int arr[MAX] = {0};
    int expected[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

    {
        ThreadsPool pool(6);
        for (int i = 0; i < MAX; i++)
        {
            pool.Enqueue([&arr, i]()
                         { arr[i] = (i + 1) * 10; });
        }
    }

    EXPECT_TRUE(std::memcmp(arr, expected, MAX) == 0);
}