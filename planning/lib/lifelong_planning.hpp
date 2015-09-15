#pragma once

#include "node.hpp"
using namespace search;
using std::make_pair;
using std::min;

namespace search
{
    //
    //  Lifelong planning 
    //
    namespace lp
    {
        struct LpState
        {
            State state;
            int g, r;
            using Key = std::pair<int, int>;

            //
            //  Calculate Key(s)
            //
            template<typename Hfunc>
            auto key(Hfunc h) const -> Key
            {
                return make_pair(min(g, r + h(x, y), min(g, r)));
            }
        };
    }
}