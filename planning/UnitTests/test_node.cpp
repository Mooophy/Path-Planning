#include "stdafx.h"
#include "CppUnitTest.h"
#include "../lib/node.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
    using namespace search;
    TEST_CLASS(test_node)
    {
    public:

        //  0,0     0,1     0,2     ||  1   2   3
        //  1,0     1,1     1,2     ||  4       5
        //  2,0     2,1     2,2     ||  6   7   8
        TEST_METHOD(goes)
        {
            auto const c = Node::Coordinate{ 1u, 1u };

            Assert::IsTrue(Node::Coordinate{ 0u, 0u } == Node::goes.at('1')(c));
            Assert::IsTrue(Node::Coordinate{ 0u, 1u } == Node::goes.at('2')(c));
            Assert::IsTrue(Node::Coordinate{ 0u, 2u } == Node::goes.at('3')(c));
            Assert::IsTrue(Node::Coordinate{ 1u, 0u } == Node::goes.at('4')(c));
            Assert::IsTrue(Node::Coordinate{ 1u, 2u } == Node::goes.at('5')(c));
            Assert::IsTrue(Node::Coordinate{ 2u, 0u } == Node::goes.at('6')(c));
            Assert::IsTrue(Node::Coordinate{ 2u, 1u } == Node::goes.at('7')(c));
            Assert::IsTrue(Node::Coordinate{ 2u, 2u } == Node::goes.at('8')(c));
        }

        TEST_METHOD(node_ctor)
        {
            auto start = Node::Coordinate{ 1u, 1u };
            auto goal = Node::Coordinate{ 2u, 2u };
            Node node{ "1123", start, goal };
            Assert::AreEqual(std::string{ "1123" }, node.path());
        }

        TEST_METHOD(coordinate)
        {
            auto start = Node::Coordinate{ 1u, 1u };
            auto goal = Node::Coordinate{ 2u, 2u };
            Node node{ "45782316", start, goal };
            Assert::IsTrue(Node::Coordinate{ 1u, 1u } == node.coordinate());
        }

        TEST_METHOD(children)
        {
            auto start = Node::Coordinate{ 1u, 1u };
            auto goal = Node::Coordinate{ 2u, 2u };
            Node node{ "", start, goal };

            struct Validate
            {
                auto operator()(Node node) const -> bool
                {
                    auto y = node.coordinate().y;
                    auto x = node.coordinate().x;
                    return y >= 0 && y <= 39 && x >= 0 && x <= 39;
                }
            };

            auto actual = node.children<Validate>();
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
    };
}