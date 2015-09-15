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

        TEST_METHOD(infinity_function)
        {
            Assert::AreEqual(2147483647, infinity());
        }

        TEST_METHOD(key)
        {
            Key key{ 42, 99 };
            Assert::AreEqual(42, key.first);
            Assert::AreEqual(99, key.second);
        }

        TEST_METHOD(lp_coordinate)
        {
            Coordinate c{ 42, 99 };
            Assert::AreEqual(42, c.x);
            Assert::AreEqual(99, c.y);
            Assert::IsTrue(Coordinate{ 1, 1 } == Coordinate{ 1, 1 });
            Assert::IsTrue(Coordinate{ 1, 2 } != Coordinate{ 1, 1 });
        }

        TEST_METHOD(lp_manhattan)
        {
            Assert::AreEqual(39, LpManhattanDistance{ { 39, 39 } }({ 0, 0 }));
            Assert::AreEqual(39, LpManhattanDistance{ { 38, 39 } }({ 0, 0 }));
            Assert::AreEqual(38, LpManhattanDistance{ { 38, 38 } }({ 0, 0 }));
        }

        TEST_METHOD(lp_euclidean_distance)
        {
            Assert::AreEqual(100, LpEuclideanDistance{ { 60, 80 } }({ 0, 0 }));
            Assert::AreEqual(50, LpEuclideanDistance{ { 30, 40 } }({ 0, 0 }));
            Assert::AreEqual(14, LpEuclideanDistance{ { 10, 10 } }({ 0, 0 }));
        }

        TEST_METHOD(lp_state)
        {
            auto ls = LpState{ { 3, 4 }, 6, 7 };
            Assert::AreEqual(3, ls.c.x);
            Assert::AreEqual(4, ls.c.y);
            Assert::AreEqual(6, ls.g);
            Assert::AreEqual(7, ls.r);

            Assert::IsTrue(Key{ 6, 6 } == ls.key(LpManhattanDistance{ { 39, 29 } }));
            Assert::IsTrue(Key{ 6, 6 } == ls.key(LpManhattanDistance{ { 6, 7 } }));

            Assert::IsTrue(Key{ 6, 6 } == ls.key(LpEuclideanDistance{ { 39, 29 } }));
            Assert::IsTrue(Key{ 6, 6 } == ls.key(LpEuclideanDistance{ { 6, 7 } }));
        }
    };
}