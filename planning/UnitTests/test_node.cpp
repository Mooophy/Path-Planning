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

        TEST_METHOD(goes)
        {
            auto const c = Node::Coordinate{ 1u, 1u };
            Assert::IsTrue(Node::Coordinate{ 0u, 0u } == Node::goes.at('1')(c));
            //

            //  not finish yet
            //
        }

        TEST_METHOD(node_ctor)
        {
            Node node{ "1123" };
        }

    };
}