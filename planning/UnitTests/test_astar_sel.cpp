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
            astar({ "",{ 1, 1 },{ 2, 2 } }, validate);
        }

        TEST_METHOD(getter)
        {
            function<bool(Node)> validate = [](Node const& n) {return true; };
            AStarSEL<EuclideanDistance<Node>, decltype(validate)> search;

            Assert::AreEqual(0u, search.last_run.max_q_size);
            Assert::AreEqual(0u, search.last_run.expansions.size());
            Assert::AreEqual(string{""}, search.last_run.final_path);
            Assert::IsTrue(0 == search.last_run.run_time);
            Assert::IsFalse(search.last_run.is_found);
        }

        TEST_METHOD(astar_algorithm)
        {
            {
                unordered_set<State> blokeds{ { 3, 2 }, { 5, 4 } };
                function<bool(Node)> validate = [&](Node const& n) {
                    auto is_not_bloked = none_of(blokeds.cbegin(), blokeds.cend(), [&](State s) {
                        return s == n.state();
                    });
                    return is_not_bloked && n.state().is_within_grid({ 0, 0 }, { 39, 39 });
                };
                AStarSEL<ManhattanDistance<Node>, decltype(validate)> astar;
                astar({ "", { 0, 0 }, { 8, 12 } }, validate);

                Assert::AreEqual(44u, astar.last_run.max_q_size);
                Assert::AreEqual(34u, astar.last_run.expansions.size());
                Assert::AreEqual(string{ "788887888688" }, astar.last_run.final_path);
                Assert::IsTrue(100 < (int)astar.last_run.run_time && 1000 > (int)astar.last_run.run_time);
                Assert::IsTrue(astar.last_run.is_found);
            }

            {
                unordered_set<State> blokeds{ { 3, 3 }, { 5, 5 } };
                function<bool(Node)> validate = [&](Node const& n) {
                    auto is_not_bloked = none_of(blokeds.cbegin(), blokeds.cend(), [&](State s) {
                        return s == n.state();
                    });
                    return is_not_bloked && n.state().is_within_grid({ 0, 0 }, { 39, 39 });
                };
                AStarSEL<ManhattanDistance<Node>, decltype(validate)> astar;
                astar({ "", { 0, 0 }, { 30, 30 } }, validate);

                Assert::AreEqual(119u, astar.last_run.max_q_size);
                Assert::AreEqual(75u, astar.last_run.expansions.size());
                Assert::AreEqual(string{ "8858887888888888888888888888888" }, astar.last_run.final_path);
                Assert::IsTrue(100 < (int)astar.last_run.run_time && 5000 > (int)astar.last_run.run_time);
                Assert::IsTrue(astar.last_run.is_found);
            }

            {//when goal is blocked, it's gonna be quite slow.
                unordered_set<State> blokeds{ { 3, 3 }, { 5, 5 }, { 9, 9 } };
                function<bool(Node)> validate = [&](Node const& n) {
                    auto is_not_bloked = none_of(blokeds.cbegin(), blokeds.cend(), [&](State s) {
                        return s == n.state();
                    });
                    return is_not_bloked && n.state().is_within_grid({ 0, 0 }, { 9, 9 });
                };
                AStarSEL<ManhattanDistance<Node>, decltype(validate)> astar;
                astar({ "", { 0, 0 }, { 9, 9 } }, validate);

                Assert::AreEqual(32u, astar.last_run.max_q_size);
                Assert::AreEqual(97u, astar.last_run.expansions.size());
                Assert::AreEqual(string{ }, astar.last_run.final_path);
                Assert::IsTrue(100 < (int)astar.last_run.run_time && 5000 > (int)astar.last_run.run_time);
                Assert::IsFalse(astar.last_run.is_found);
            }
        }
    };
}