#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace search;

namespace UnitTests
{
    TEST_CLASS(test_timing)
    {
    public:
        TEST_METHOD(timing_ctor)
        {
            {//0ms
                int record = 0;
                {
                    Timing<int> recording{ record };
                    for (auto i = 0; i--;);
                }
                Assert::AreEqual(0, record);
            }

            {//0ms
                int record = 0;
                {
                    Timing<int> recording{ record };
                    for (auto i = 9999; i--;);
                }
                Assert::AreEqual(0, record);
            }

            {//2ms
                int record = 0;
                {
                    Timing<int> recording{ record };
                    for (auto i = 999999; --i;);
                }
                Assert::IsTrue(record > 0);
            }

            {//24ms
                int record = 0;
                {
                    Timing<int> recording{ record };
                    for (auto i = 9999999; --i;);
                }
                Assert::IsTrue(record > 9);
            }
        }
    };
}