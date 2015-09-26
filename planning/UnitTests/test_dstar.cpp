#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace search::ds;
using namespace search;

namespace UnitTests
{
    TEST_CLASS(test_dstar)
    {
    public:
        TEST_METHOD(dstar_initial_plan)
        {
            //initial planning
            Cells bad_cells{ { 1, 1 }, { 2, 1 } };
            DStarCore dstar{ 5, 3, { 1, 0 },{ 4, 2 }, "manhattan", bad_cells };

            Assert::AreEqual(0u, dstar.max_q_size);
            Assert::AreEqual(0u, dstar.expansions.size());
            Assert::IsTrue(0 == dstar.run_time);

            dstar.initial_plan();
            string expect = "{[r=0,c=0]|g:10000|r:4|h:1|b:f}{[r=0,c=1]|g:10000|r:4|h:1|b:f}{[r=0,c=2]|g:10000|r:10000|h:2|b:f}+++{[r=1,c=0]|g:3|r:3|h:0|b:f}{[r=1,c=1]|g:10000|r:10000|h:1|b:t}{[r=1,c=2]|g:10000|r:10000|h:2|b:f}+++{[r=2,c=0]|g:2|r:2|h:1|b:f}{[r=2,c=1]|g:10000|r:10000|h:1|b:t}{[r=2,c=2]|g:10000|r:2|h:2|b:f}+++{[r=3,c=0]|g:10000|r:2|h:2|b:f}{[r=3,c=1]|g:1|r:1|h:2|b:f}{[r=3,c=2]|g:1|r:1|h:2|b:f}+++{[r=4,c=0]|g:10000|r:2|h:3|b:f}{[r=4,c=1]|g:10000|r:1|h:3|b:f}{[r=4,c=2]|g:0|r:0|h:3|b:f}+++";
            Assert::AreEqual(expect, dstar.matrix.to_string()); //exactly same as the given example.
        }
    };
}