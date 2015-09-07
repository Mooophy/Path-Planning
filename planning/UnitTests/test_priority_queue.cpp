#include "stdafx.h"
#include "CppUnitTest.h"
#include "../lib/priority_queue.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
    using namespace search;
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

    };
}