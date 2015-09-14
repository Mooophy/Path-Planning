#pragma once

#include "node.hpp"

namespace search
{
    //
    //  Lifelong planning 
    //
    namespace llp
    {
        struct Int
        {
            int i;
            bool inifinity;
        };

        struct State : public search::State
        {
            Int g;
            Int rhs;
        };
    }
}