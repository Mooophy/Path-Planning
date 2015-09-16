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
        using Key = LpState::Key;

        TEST_METHOD(cost_function)
        {
            Assert::AreEqual(1, cost());
        }

        TEST_METHOD(infinity_function)
        {
            Assert::AreEqual(2147483647, infinity());
        }

        TEST_METHOD(key)
        {
            Key key{ 42, 99 };
            Assert::AreEqual(42, key.first);
            Assert::AreEqual(99, key.second);
            Assert::IsTrue(Key{ 1, 2 } < Key{ 2, 1 });
            Assert::IsTrue(Key{ 2, 1 } < Key{ 2, 2 });
            Assert::IsTrue(Key{ 2, 2 } == Key{ 2, 2 });
        }

        TEST_METHOD(lp_coordinate)
        {
            Coordinate c{ 42, 99 };
            Assert::AreEqual(42, c.x);
            Assert::AreEqual(99, c.y);
            Assert::IsTrue(Coordinate{ 1, 1 } == Coordinate{ 1, 1 });
            Assert::IsTrue(Coordinate{ 1, 2 } != Coordinate{ 1, 1 });

            //test neighbour
            {
                Coordinate c{ 1, 1 };
                decltype(c.neighbours()) expect = 
                {
                    { 0, 0 }, { 1, 0 }, { 2, 0 },
                    { 0, 1 },   /* */   { 2, 1 },
                    { 0, 2 }, { 1, 2 }, { 2, 2 }
                };

                for (auto i = 1; i != expect.size(); ++i)
                    Assert::IsTrue(expect[i] == c.neighbours()[i]);
            }
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
            Assert::AreEqual(3, ls.coordinate.x);
            Assert::AreEqual(4, ls.coordinate.y);
            Assert::AreEqual(6, ls.g);
            Assert::AreEqual(7, ls.r);

            Assert::IsTrue(Key{ 6, 6 } == ls.key(LpManhattanDistance{ { 39, 29 } }));
            Assert::IsTrue(Key{ 6, 6 } == ls.key(LpManhattanDistance{ { 6, 7 } }));
            Assert::IsTrue(Key{ 6, 6 } == ls.key(LpEuclideanDistance{ { 39, 29 } }));
            Assert::IsTrue(Key{ 6, 6 } == ls.key(LpEuclideanDistance{ { 6, 7 } }));
        }

        TEST_METHOD(matrix_class)
        {
            Matrix matrix{ 10, 10 };
            
            {//x = 0, y = 0
                Coordinate c = { 0, 0 };
                Assert::AreEqual(0, matrix.at(c).g);
                Assert::AreEqual(0, matrix.at(c).r);
                Assert::IsTrue(Coordinate{ 0, 0 } == matrix.at(c).coordinate);
                Assert::IsTrue(Key{ 0, 0 } == matrix.at(c).key(LpEuclideanDistance{ { 6, 7 } }));
            }

            {//x = 2, y = 4
                Coordinate c = { 2, 4 };
                Assert::AreEqual(0, matrix.at(c).g);
                Assert::AreEqual(0, matrix.at(c).r);
                Assert::IsTrue(Coordinate{ 0, 0 } == matrix.at(c).coordinate);
                Assert::IsTrue(Key{ 0, 0 } == matrix.at(c).key(LpEuclideanDistance{ { 6, 7 } }));
            }
        }
    };
}