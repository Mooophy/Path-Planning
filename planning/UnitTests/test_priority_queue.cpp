#include "stdafx.h"
#include "CppUnitTest.h"
//#include "../lib/priority_queue.hpp"
//#include "../lib/node.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace search;

namespace UnitTests
{
    TEST_CLASS(test_priority_queue)
    {
    public:
        TEST_METHOD(pq_parent)
        {
            auto seq = { 5, 6, 9, 3, 2, 7 };
            auto p = parent(seq.begin(), seq.begin());
            Assert::IsTrue(seq.begin() == parent(seq.begin(), seq.begin() + 1));
            Assert::IsTrue(seq.begin() == parent(seq.begin(), seq.begin() + 2));
        }

        TEST_METHOD(pq_left_child)
        {
            auto seq = { 5, 6, 9, 3, 2, 7 };
            auto left = left_child(seq.begin(), seq.end(), seq.begin());
            Assert::IsTrue(left == seq.begin() + 1);
        }

        TEST_METHOD(pq_right_child)
        {
            auto seq = { 5, 6, 9, 3, 2, 7 };
            auto right = right_child(seq.begin(), seq.end(), seq.begin());
            Assert::IsTrue(right == seq.begin() + 2);
        }

        TEST_METHOD(pq_heapify)
        {
            auto seq = std::vector < int >{ 5, 6, 9, 3, 2, 7 };
            heapify(seq.begin(), seq.end(), seq.begin(), std::greater<int>());
            std::vector<int> expect = { 9, 6, 7, 3, 2, 5 };
            for (auto i = 0u; i != seq.size(); ++i)
                Assert::AreEqual(expect[i], seq[i]);
        }

        TEST_METHOD(pq_build_heap)
        {
            auto seq = std::vector < int >{ 4, 1, 3, 2, 16, 9, 10, 14, 8, 7 };
            build_heap(seq.begin(), seq.end(), std::greater<int>());
            std::vector<int> expect = { 16, 14, 10, 8, 7, 9, 3, 2, 4, 1 };
            for (auto i = 0u; i != seq.size(); ++i)
                Assert::AreEqual(expect[i], seq[i]);
        }

        TEST_METHOD(pq_default_ctor)
        {
            //test with int
            {
                using Less = decltype(std::less<int>{});
                auto pq = PriorityQueue<int, Less>{ { 4, 6, 7, 1 }, Less{} };
                Assert::AreEqual(4u, pq.size());
                Assert::AreEqual(1, pq.top());  pq.pop();
                Assert::AreEqual(4, pq.top());  pq.pop();
                Assert::AreEqual(6, pq.top());  pq.pop();
                Assert::AreEqual(7, pq.top());  pq.pop();
                Assert::AreEqual(0u,  pq.size());
            }

            //test with node
            {
                using Compare = std::function<bool(Node, Node)>;
            }
        }
    };
}