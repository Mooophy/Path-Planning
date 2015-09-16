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
        using Key = LpAstar::Key;

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

        TEST_METHOD(lp_heuristics)
        {
            Assert::AreEqual(6, HeuristcFuncs{}.at("manhattan")({ 3, 4 }, { 9, 9 }));
            Assert::AreEqual(5, HeuristcFuncs{}.at("manhattan")({ 4, 4 }, { 9, 9 }));

            Assert::AreEqual(5, HeuristcFuncs{}.at("euclidean")({ 6, 5 }, { 9, 9 }));
            Assert::AreEqual(1, HeuristcFuncs{}.at("euclidean")({ 8, 8 }, { 9, 9 }));
        }

        TEST_METHOD(lp_key)
        {
            auto ls = LpState{ { 3, 4 }, 6, 7 };
            Assert::IsTrue(Key{ 6, 6 } == Key{ ls, HeuristcFuncs{}.at("manhattan"), { 39, 39 } });
            Assert::IsTrue(Key{ 6, 6 } == Key{ ls, HeuristcFuncs{}.at("manhattan"), { 4, 4 } });
        }

        TEST_METHOD(lp_state)
        {
            auto ls = LpState{ { 3, 4 }, 6, 7 };
            Assert::AreEqual(3, ls.coordinate.x);
            Assert::AreEqual(4, ls.coordinate.y);
            Assert::AreEqual(6, ls.g);
            Assert::AreEqual(7, ls.r);
            Assert::IsTrue(ls == LpState{ { 3, 4 }, 6, 7 });
        }

        TEST_METHOD(matrix_class)
        {
            Matrix matrix{ 10, 10 };
            
            {//x = 0, y = 0
                Coordinate c = { 0, 0 };
                Assert::AreEqual(0, matrix.at(c).g);
                Assert::AreEqual(0, matrix.at(c).r);
                Assert::IsTrue(Coordinate{ 0, 0 } == matrix.at(c).coordinate);
            }

            {//x = 2, y = 4
                Coordinate c = { 2, 4 };
                Assert::AreEqual(0, matrix.at(c).g);
                Assert::AreEqual(0, matrix.at(c).r);
                Assert::IsTrue(Coordinate{ 0, 0 } == matrix.at(c).coordinate);
            }
        }

        TEST_METHOD(lp_astar)
        {
            LpAstar lpastar{ 40, 40, { 19, 29 }, "manhattan" };
            
            {//test priority queue
                Assert::AreEqual(2u, lpastar.heuristics.size());
            }
        }
    };
}