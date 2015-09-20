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
        TEST_METHOD(cost_function)
        {
            Assert::AreEqual(1, cost());
        }

        TEST_METHOD(infinity_function)
        {
            Assert::AreEqual(10'000, huge());
        }

        TEST_METHOD(key)
        {
            Key key{ 42, 99 };
            Assert::AreEqual(42, key.fst);
            Assert::AreEqual(99, key.snd);
            Assert::IsTrue(Key{ 1, 2 } < Key{ 2, 1 });
            Assert::IsTrue(Key{ 2, 1 } < Key{ 2, 2 });
            Assert::IsTrue(Key{ 2, 2 } == Key{ 2, 2 });
        }

        TEST_METHOD(lp_cell)
        {
            Cell c{ 42, 99 };
            Assert::AreEqual(42, c.row);
            Assert::AreEqual(99, c.col);
            Assert::IsTrue(Cell{ 1, 1 } == Cell{ 1, 1 });
            Assert::IsTrue(Cell{ 1, 2 } != Cell{ 1, 1 });
            Assert::AreEqual(string{ "[r=42,c=99]" }, c.to_string());
            Assert::AreEqual(69382224u, c.to_hash());

            {//to confirm hash works
                Cells blockeds;
                blockeds.insert({ 1, 2 });
                blockeds.insert({ 1, 2 });
                blockeds.insert({ 1, 3 });
                Assert::AreEqual(2u, blockeds.size());
            }
        }

        TEST_METHOD(lp_heuristics)
        {
            Assert::AreEqual(4, HEURISTICS.at("manhattan")({ 1, 3 }, { 5, 0 }));
            Assert::AreEqual(5, HEURISTICS.at("manhattan")({ 0, 2 }, { 5, 0 }));

            Assert::AreEqual(5, HEURISTICS.at("euclidean")({ 6, 5 }, { 9, 9 }));
            Assert::AreEqual(1, HEURISTICS.at("euclidean")({ 8, 8 }, { 9, 9 }));
        }

        TEST_METHOD(lp_state)
        {
            auto ls = LpState{ { 3, 4 }, 6, 7, 1, false };
            Assert::AreEqual(3, ls.cell.row);
            Assert::AreEqual(4, ls.cell.col);
            Assert::AreEqual(6, ls.g);
            Assert::AreEqual(7, ls.r);
            Assert::IsTrue(ls == LpState{ { 3, 4 }, 6, 7, 1 });
            Assert::IsTrue(ls == LpState{ { 3, 4 }, 6, 7, 1, false });
            Assert::IsFalse(ls.bad);
            Assert::AreEqual(string{ "{[r=3,c=4]|g:6|r:7|h:1|b:f}" }, ls.to_string());
        }

        TEST_METHOD(matrix_class)
        {
            Matrix matrix{ 9, 8 };
            Assert::AreEqual(9u, matrix.rows());
            Assert::AreEqual(8u, matrix.cols());
            Assert::AreEqual(1'0000, huge());

            {
                Cell c = { 2, 4 };
                Assert::AreEqual(huge(), matrix.at(c).g);
                Assert::AreEqual(huge(), matrix.at(c).r);
                Assert::IsTrue(c == matrix.at(c).cell);
            }

            {
                Cell c = { 4, 2 };
                Assert::AreEqual(huge(), matrix.at(c).g);
                Assert::AreEqual(huge(), matrix.at(c).r);
                Assert::IsTrue(c == matrix.at(c).cell);
            }

            {//for testing to_string
                Matrix matrix{ 2, 2 };
                string expect = "{[r=0,c=0]|g:10000|r:10000|h:0|b:f}{[r=0,c=1]|g:10000|r:10000|h:0|b:f}+++{[r=1,c=0]|g:10000|r:10000|h:0|b:f}{[r=1,c=1]|g:10000|r:10000|h:0|b:f}+++";
                Assert::AreEqual(expect, matrix.to_string());
            }
        }

        TEST_METHOD(matrix_of_lpastar)
        {
            //case from pdf file
            Cells bad_cells{ { 1, 0 },{ 2, 0 },{ 3, 0 },{ 4, 0 },{ 1, 2 },{ 2, 2 },{ 3, 2 },{ 4, 2 } };
            LpAstarCore lpa{ 6, 4, { 0, 3 }, { 5, 0 }, "manhattan", bad_cells };

            //test bad marking
            for (auto cell : bad_cells)
                Assert::AreEqual(true, lpa.matrix.at(cell).bad);
            for (auto r = 0; r != lpa.matrix.rows(); ++r)
                for (auto c = 0; c != lpa.matrix.cols(); ++c)
                    if(bad_cells.count({ r, c }) == 0)
                        Assert::AreEqual(false, lpa.matrix.at({ r, c }).bad);

            //test h value marking
            Assert::AreEqual(5, lpa.matrix.at({ 0, 2 }).h);
            Assert::AreEqual(4, lpa.matrix.at({ 1, 3 }).h);
        }

        TEST_METHOD(priority_queue_of_lpastar)
        {
            {//pushing order 1
                Cells bad_cells{ { 1, 0 },{ 2, 0 },{ 3, 0 },{ 4, 0 },{ 1, 2 },{ 2, 2 },{ 3, 2 },{ 4, 2 } };
                LpAstarCore lpa{ 6, 4, { 0, 3 }, { 5, 0 }, "manhattan", bad_cells };
                lpa.q.push({ 0, 2 });
                lpa.q.push({ 1, 3 });
                Assert::IsTrue(Cell{ 1, 3 } == lpa.q.top());
            }

            {//pushing order 2
                Cells bad_cells{ { 1, 0 },{ 2, 0 },{ 3, 0 },{ 4, 0 },{ 1, 2 },{ 2, 2 },{ 3, 2 },{ 4, 2 } };
                LpAstarCore lpa{ 6, 4,{ 0, 3 },{ 5, 0 }, "manhattan", bad_cells };
                lpa.q.push({ 1, 3 });
                lpa.q.push({ 0, 2 });
                Assert::IsTrue(Cell{ 1, 3 } == lpa.q.top());
            }
        }

        TEST_METHOD(run_of_lpastar)
        {
            //initial planning
            Cells bad_cells{ { 1, 0 },{ 2, 0 },{ 3, 0 },{ 4, 0 },{ 1, 2 },{ 2, 2 },{ 3, 2 },{ 4, 2 } };
            LpAstarCore lpa{ 6, 4, { 0, 3 }, { 5, 0 }, "manhattan", bad_cells };

            Assert::AreEqual(0u, lpa.max_q_size);
            Assert::AreEqual(0u, lpa.expansions.size());
            Assert::IsTrue(0 == lpa.run_time);
            Assert::AreEqual(string{}, lpa.path);

            lpa.plan();
            string expect{ "{[r=0,c=0]|g:10000|r:3|h:5|b:f}{[r=0,c=1]|g:10000|r:2|h:5|b:f}{[r=0,c=2]|g:1|r:1|h:5|b:f}{[r=0,c=3]|g:0|r:0|h:5|b:f}+++{[r=1,c=0]|g:10000|r:10000|h:4|b:t}{[r=1,c=1]|g:2|r:2|h:4|b:f}{[r=1,c=2]|g:10000|r:10000|h:4|b:t}{[r=1,c=3]|g:1|r:1|h:4|b:f}+++{[r=2,c=0]|g:10000|r:10000|h:3|b:t}{[r=2,c=1]|g:3|r:3|h:3|b:f}{[r=2,c=2]|g:10000|r:10000|h:3|b:t}{[r=2,c=3]|g:2|r:2|h:3|b:f}+++{[r=3,c=0]|g:10000|r:10000|h:2|b:t}{[r=3,c=1]|g:4|r:4|h:2|b:f}{[r=3,c=2]|g:10000|r:10000|h:2|b:t}{[r=3,c=3]|g:3|r:3|h:3|b:f}+++{[r=4,c=0]|g:10000|r:10000|h:1|b:t}{[r=4,c=1]|g:5|r:5|h:1|b:f}{[r=4,c=2]|g:10000|r:10000|h:2|b:t}{[r=4,c=3]|g:10000|r:4|h:3|b:f}+++{[r=5,c=0]|g:6|r:6|h:0|b:f}{[r=5,c=1]|g:10000|r:6|h:1|b:f}{[r=5,c=2]|g:10000|r:6|h:2|b:f}{[r=5,c=3]|g:10000|r:10000|h:3|b:f}+++" };
            Assert::AreEqual(expect, lpa.matrix.to_string());

            Assert::AreEqual(6u, lpa.max_q_size);
            Assert::AreEqual(10u, lpa.expansions.size());
            Assert::IsTrue(0 < lpa.run_time && lpa.run_time < 30);
            //Assert::AreEqual(string{}, lpa.path); // working here

            //replanning
            Cells cells_to_toggle{ { 3, 1 } };
            lpa.replan(cells_to_toggle);
            Assert::AreEqual(true, lpa.matrix.at({ 3, 1 }).bad);
            {
                string expect = "{[r=0,c=0]|g:10000|r:3|h:5|b:f}{[r=0,c=1]|g:2|r:2|h:5|b:f}{[r=0,c=2]|g:1|r:1|h:5|b:f}{[r=0,c=3]|g:0|r:0|h:5|b:f}+++{[r=1,c=0]|g:10000|r:10000|h:4|b:t}{[r=1,c=1]|g:2|r:2|h:4|b:f}{[r=1,c=2]|g:10000|r:10000|h:4|b:t}{[r=1,c=3]|g:1|r:1|h:4|b:f}+++{[r=2,c=0]|g:10000|r:10000|h:3|b:t}{[r=2,c=1]|g:3|r:3|h:3|b:f}{[r=2,c=2]|g:10000|r:10000|h:3|b:t}{[r=2,c=3]|g:2|r:2|h:3|b:f}+++{[r=3,c=0]|g:10000|r:10000|h:2|b:t}{[r=3,c=1]|g:10000|r:10000|h:2|b:t}{[r=3,c=2]|g:10000|r:10000|h:2|b:t}{[r=3,c=3]|g:3|r:3|h:3|b:f}+++{[r=4,c=0]|g:10000|r:10000|h:1|b:t}{[r=4,c=1]|g:6|r:6|h:1|b:f}{[r=4,c=2]|g:10000|r:10000|h:2|b:t}{[r=4,c=3]|g:4|r:4|h:3|b:f}+++{[r=5,c=0]|g:7|r:7|h:0|b:f}{[r=5,c=1]|g:6|r:6|h:1|b:f}{[r=5,c=2]|g:5|r:5|h:2|b:f}{[r=5,c=3]|g:10000|r:5|h:3|b:f}+++";
                Assert::AreEqual(expect, lpa.matrix.to_string());

                Assert::AreEqual(6u, lpa.max_q_size);
                Assert::AreEqual(6u, lpa.expansions.size());
                Assert::IsTrue(0 < lpa.run_time && lpa.run_time < 30);
            }
        }
    };
}