#pragma once

#include <unordered_set>
#include "priority_queue.hpp"
#include "func.hpp"
#include "node.hpp"

using std::unordered_set;
using std::size_t;
using std::string;

namespace search
{
    template<typename Hfunc, typename Cfunc, typename RunningTime>
    class AStarSEL
    {
    private:
        using Q = PriorityQueue<Node, Less<Node, ManhattanDistance<Node>>>;
        Q _q;
        size_t _max_q_size;
        //unordered_set<Node> _expanded;
        string _final_path;
        RunningTime _running_time;
    };
}