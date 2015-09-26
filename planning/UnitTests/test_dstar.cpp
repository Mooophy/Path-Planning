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
            Cells bad_cells{ { 1, 0 },{ 2, 0 },{ 3, 0 },{ 4, 0 },{ 1, 2 },{ 2, 2 },{ 3, 2 },{ 4, 2 } };
            DStarCore dstar{ 6, 4,{ 0, 3 },{ 5, 0 }, "manhattan", bad_cells };

            Assert::AreEqual(0u, dstar.max_q_size);
            Assert::AreEqual(0u, dstar.expansions.size());
            Assert::IsTrue(0 == dstar.run_time);
        }
    };
}