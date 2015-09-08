#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace search;

namespace UnitTests
{
    //using namespace search;
    TEST_CLASS(test_func)
    {
    public:

        TEST_METHOD(dy_dx)
        {
            auto start = Coordinate{ 0u, 0u };
            auto goal = Coordinate{ 2u, 2u };
            Node node{ "8", start, goal };

            Assert::AreEqual(1, dy(node));
            Assert::AreEqual(1, dx(node));
        }

    };
}
