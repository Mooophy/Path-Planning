//
//  A* with strict expanded list
//
#pragma once

#include <unordered_set>
#include "priority_queue.hpp"
#include "func.hpp"
#include "node.hpp"
#include "timing.hpp"

using std::size_t;
using std::string;
using std::move;

namespace search
{
    template<typename Hfunc, typename ValidateFunc, typename Cfunc = Cost<Node>>
    class AStarSEL
    {
    public:

        using Q = PriorityQueue<Node, Less<Node, Hfunc>>;
        using Expansions = std::unordered_set<Node>;

        struct Get
        {
            Q q;
            size_t const& max_q_size;
            Expansions const& expansions;
            string const& final_path;
            long long const& run_time;
            bool const& is_found;
        } const data;

        AStarSEL()
            : data{ _q, _max_q_size, _expansions, _final_path, _run_time, _is_found }
        {
            reset();
        }

        auto operator()(ValidateFunc validate) -> void
        {
            reset();
            search(move(validate));
        }

    private:

        Q _q;
        size_t _max_q_size;
        Expansions _expansions;
        string _final_path;
        long long _run_time;
        bool _is_found;

        auto search(ValidateFunc && validate) -> void
        {

        }

        auto reset() -> void
        {
            _q.reset();
            _max_q_size = 0;
            _expansions.clear();
            _final_path.clear();
            _run_time = 0;
            _is_found = false;
        }
    };
}