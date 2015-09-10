#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace search;
using std::unordered_set;
using std::none_of;

namespace UnitTests
{
    TEST_CLASS(test_astar_sel)
    {
    public:

        TEST_METHOD(astar_sel_ctor)
        {
            function<bool(Node)> validate = [](Node const& n) {return true; };
            AStarSEL<ManhattanDistance<Node>, decltype(validate)> astar;
            astar({ 1, 1 }, { 2, 2 }, validate);
        }

        TEST_METHOD(getter)
        {
            function<bool(Node)> validate = [](Node const& n) {return true; };
            AStarSEL<EuclideanDistance<Node>, decltype(validate)> search;

            Assert::AreEqual(0u, search.data.max_q_size);
            Assert::AreEqual(0u, search.data.expansions.size());
            Assert::AreEqual(string{""}, search.data.final_path);
            Assert::IsTrue(0 == search.data.run_time);
            Assert::IsFalse(search.data.is_found);
        }

        TEST_METHOD(astar_algorithm)
        {
            unordered_set<State> blokeds{ { 3, 3 }, { 5, 5 } };
            function<bool(Node)> validate = [&](Node const& n) {
                auto is_not_bloked = none_of(blokeds.cbegin(), blokeds.cend(), [&](State s) {
                    return s != n.state();
                });
                auto is_within_grid = n.state().x <= 39 && n.state().x >= 0 && n.state().y <= 39 && n.state().y >= 0;

                return is_not_bloked && is_within_grid;
            };
            AStarSEL<ManhattanDistance<Node>, decltype(validate)> astar;
            astar({ 0, 0 }, { 5, 5 }, validate);

            Assert::AreEqual(string("88"), astar.data.final_path);
        }
    };
}