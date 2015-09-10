#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace search;

namespace UnitTests
{
    TEST_CLASS(test_node)
    {
    public:
        //  0,0     0,1     0,2     ||  1   2   3
        //  1,0     1,1     1,2     ||  4       5
        //  2,0     2,1     2,2     ||  6   7   8
        TEST_METHOD(goes)
        {
            auto const c = State{ 1u, 1u };

            Assert::IsTrue(State{ 0u, 0u } == GOES.at('1')(c));
            Assert::IsTrue(State{ 0u, 1u } == GOES.at('2')(c));
            Assert::IsTrue(State{ 0u, 2u } == GOES.at('3')(c));
            Assert::IsTrue(State{ 1u, 0u } == GOES.at('4')(c));
            Assert::IsTrue(State{ 1u, 2u } == GOES.at('5')(c));
            Assert::IsTrue(State{ 2u, 0u } == GOES.at('6')(c));
            Assert::IsTrue(State{ 2u, 1u } == GOES.at('7')(c));
            Assert::IsTrue(State{ 2u, 2u } == GOES.at('8')(c));
        }

        TEST_METHOD(node_ctor)
        {
            auto start = State{ 1u, 1u };
            auto goal = State{ 2u, 2u };
            Node node{ "1123", start, goal };
            Assert::AreEqual(std::string{ "1123" }, node.path());
        }

        TEST_METHOD(State_to_string)
        {
            State c = { 42, 99 };
            Assert::AreEqual(string{ "[42,99]" }, c.to_string());
        }

        TEST_METHOD(Node_to_string)
        {
            auto start = State{ 1u, 1u };
            auto goal = State{ 2u, 2u };
            Node node{ "1123", start, goal };
            Assert::AreEqual(std::string{ "[1,1][1123][2,2]" }, node.to_string());
        }

        TEST_METHOD(coordinate)
        {
            auto start = State{ 1u, 1u };
            auto goal = State{ 2u, 2u };
            Node node{ "45782316", start, goal };
            Assert::IsTrue(State{ 1u, 1u } == node.state());
        }

        TEST_METHOD(children)
        {
            auto start = State{ 1u, 1u };
            auto goal = State{ 2u, 2u };
            Node node{ "", start, goal };

            struct Validate
            {
                auto operator()(Node node) const -> bool
                {
                    auto y = node.state().y;
                    auto x = node.state().x;
                    return y >= 0 && y <= 39 && x >= 0 && x <= 39;
                }
            };

            auto actual = node.children<Validate>(Validate{});
            auto expect = Node::Children
            {
                Node{ "1", start, goal },
                Node{ "2", start, goal },
                Node{ "3", start, goal },
                Node{ "4", start, goal },
                Node{ "5", start, goal },
                Node{ "6", start, goal },
                Node{ "7", start, goal },
                Node{ "8", start, goal },
            };

            for (auto i = 0u; i != 1u; ++i)
                Assert::IsTrue(expect[i] == expect[i]);
        }

        TEST_METHOD(hash)
        {
            auto start = State{ 1u, 1u };
            auto goal = State{ 2u, 2u };
            Node node{ "87654321", start, goal };

            Assert::AreEqual(1681822733u, node.hash());
            Assert::AreEqual(1681822733u, std::hash<Node>{}(node));
            
            std::unordered_set<Node> uset{ node };
            Assert::AreEqual(1u, uset.size());
        }
    };
}