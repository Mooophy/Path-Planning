#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace search::lp;
using namespace search;

namespace UnitTests
{
    TEST_CLASS(test_lifelong_planning)
    {
    public:

        TEST_METHOD(LpState_struct)
        {
            LpState ls = { { 42, 99 }, 66, 67 };
            Assert::AreEqual(42, ls.state.y);
            Assert::AreEqual(99, ls.state.x);
            Assert::AreEqual(66, ls.g);
            Assert::AreEqual(67, ls.r);
        }

    };
}