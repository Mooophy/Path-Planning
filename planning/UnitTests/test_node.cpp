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
            Node node{ "1123" };
            Assert::AreEqual(std::string{ "1123" }, node.path());
        }

        TEST_METHOD(coordinate)
        {
            Node node{ "45782316" };
            auto start = Node::Coordinate{ 1u, 1u };
            Assert::IsTrue(Node::Coordinate{ 1u, 1u } == node.coordinate(start));
        }

    };
}