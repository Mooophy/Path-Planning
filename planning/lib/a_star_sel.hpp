//
//  A* with strict expanded list
//
#pragma once

#include <unordered_set>
#include "priority_queue.hpp"
#include "func.hpp"
#include "node.hpp"
#include "timing.hpp"

using std::unordered_set;
using std::size_t;
using std::string;

namespace search
{
    template<typename Hfunc, typename Cfunc>
    class AStarSEL
    {
    public:
        AStarSEL()
            : _q{},
            _max_q_size{ 0u },
            _expansion{},
            _final_path{ "" },
            _run_time{ 0 }
        {   }

    private:

        using Q = PriorityQueue<Node, Less<Node, Hfunc>>;
        Q _q;
        size_t _max_q_size;
        unordered_set<Node> _expansion;
        string _final_path;
        long long _run_time;
    };
}