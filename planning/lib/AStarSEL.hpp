#pragma once

#include <unordered_set>
#include "priority_queue.hpp"

using std::unordered_set;
using std::size_t;
using std::string;

namespace search
{
    template<typename Hfunc, typename Cfunc, typename RuningTime>
    class AStarSEL
    {
    private:
        PriorityQueue<Node> _q;
        size_t _max_q_size;
        unordered_set<Node> _expanded;
        string final_path_;
        RuningTime _running_time;
    };
}