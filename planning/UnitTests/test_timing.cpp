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
                long long record = 0;
                {
                    Timing recording{ record };
                    for (auto i = 0; i--;);
                }
                Assert::IsTrue(record < 1ll);
            }

            {//0ms
                long long record = 0;
                {
                    Timing recording{ record };
                    for (auto i = 9999; i--;);
                }
                Assert::IsTrue(record < 1ll);
            }

            {//2ms
                long long record = 0;
                {
                    Timing recording{ record };
                    for (auto i = 999999; --i;);
                }
                Assert::IsTrue(record > 0);
            }

            {//24ms
                long long record = 0;
                {
                    Timing recording{ record };
                    for (auto i = 9999999; --i;);
                }
                Assert::IsTrue(record > 9);
            }
        }
    };
}