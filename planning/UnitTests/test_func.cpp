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

        TEST_METHOD(ManhattanDistance)
        {
            auto start = Coordinate{ 0u, 0u };
            auto goal = Coordinate{ 60u, 80u };
            Node node{ "8", start, goal };
            Assert::AreEqual(79, search::ManhattanDistance<Node>{}(node));
        }

        TEST_METHOD(EuclideanDinstance)
        {
            auto start = Coordinate{ 0u, 0u };
            auto goal = Coordinate{ 60u, 80u };
            Node node{ "8", start, goal };
            Assert::AreEqual(99, search::EuclideanDinstance<Node>{}(node));
        }
    };
}
