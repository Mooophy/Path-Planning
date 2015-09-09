#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace search;

namespace UnitTests
{
    TEST_CLASS(test_astar_sel)
    {
    public:

        TEST_METHOD(astar_sel_ctor)
        {
            function<bool(Node)> validate = [](Node const& n) {return true; };
            AStarSEL<Heuristc<Node>, decltype(validate)> astar;
        }

        TEST_METHOD(latest)
        {
            function<bool(Node)> validate = [](Node const& n) {return true; };
            AStarSEL<Heuristc<Node>, decltype(validate)> search;

            Assert::AreEqual(0u, search.data.max_q_size);
            Assert::AreEqual(0u, search.data.expansions.size());
            Assert::AreEqual(string{""}, search.data.final_path);
            Assert::IsTrue(0 == search.data.run_time);
            Assert::IsFalse(search.data.is_found);
        }

    };
}