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
    template<typename Hfunc, typename Validate, typename Cfunc = Cost<Node>>
    class AStarSEL
    {
    public:

        using Q = PriorityQueue<Node, Less<Node, Hfunc>>;
        using Expansions = std::unordered_set<Node>;

        struct Result
        {
            size_t max_q_size;
            Expansions const& expansions;
            string const& final_path;
            long long run_time;
            bool is_found;
        };

        auto operator()(Validate validate) -> Result
        {
            reset();
            search(move(validate));
            return { _q, _max_q_size, _expansions, _final_path, _run_time, _is_found };
        }

    private:

        Q _q;
        size_t _max_q_size;
        Expansions _expansions;
        string _final_path;
        long long _run_time;
        bool _is_found;

        auto search(Validate && validate) -> void
        {

        }

        auto reset() -> void
        {
            _q.clear();
            _max_q_size = 0;
            _expansions.clear();
            _final_path.clear();
            _run_time = 0;
            _is_found = false;
        }
    };
}