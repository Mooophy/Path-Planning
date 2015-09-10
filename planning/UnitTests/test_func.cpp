#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace search;

namespace UnitTests
{
    TEST_CLASS(test_func)
    {
    public:

        TEST_METHOD(dy_dx)
        {
            auto start = State{ 0u, 0u };
            auto goal = State{ 2u, 2u };
            Node node{ "8", start, goal };

            Assert::AreEqual(1, dy(node));
            Assert::AreEqual(1, dx(node));
        }

        TEST_METHOD(ManhattanDistance)
        {
            auto start = State{ 0u, 0u };
            auto goal = State{ 60u, 80u };
            Node node{ "8", start, goal };
            Assert::AreEqual(79, search::ManhattanDistance<Node>{}(node));
        }

        TEST_METHOD(EuclideanDinstance)
        {
            auto start = State{ 0u, 0u };
            auto goal = State{ 60u, 80u };
            Node node{ "8", start, goal };
            Assert::AreEqual(99, search::EuclideanDistance<Node>{}(node));
        }

        TEST_METHOD(Cost)
        {
            auto start = State{ 0u, 0u };
            auto goal = State{ 60u, 80u };
            Node node{ "854617", start, goal };
            Assert::AreEqual(6, search::Cost<Node>{}(node));
        }

        TEST_METHOD(Less)
        {
            auto start = State{ 0u, 0u };
            auto goal = State{ 60u, 80u };
            Node lhs{ "88", start, goal };
            Node rhs{ "81", start, goal };
            
            Assert::IsTrue(search::Less<Node, search::ManhattanDistance<Node>>{}(lhs, rhs));
        }
    };
}
