//
//  A* with strict expanded list
//
#pragma once

#include <functional>
#include <unordered_set>
#include "priority_queue.hpp"
#include "func.hpp"
#include "node.hpp"
#include "timing.hpp"

using std::size_t;
using std::string;
using std::move;
using std::max;
using std::any_of;

namespace search
{
    //
    //  Hfunc : heuristic function, (Node) -> h value
    //  ValidateFunc : unary predicate, check if a node is valid, (Node) -> bool
    //  Cfunc : cost function, (Node) -> c value
    //
    template<typename Hfunc, typename ValidateFunc, typename Cfunc = Cost<Node>>
    class AStarSEL
    {
    public:

        using Q = PriorityQueue<Node, Less<Node, Hfunc>>;
        using Expansions = std::unordered_set<State>;

        struct Get
        {
            Q q;
            size_t const& max_q_size;
            Expansions const& expansions;
            string const& final_path;
            long long const& run_time;
            bool const& is_found;
        } const last_run;

        AStarSEL()
            : last_run{ _q, _max_q_size, _expansions, _final_path, _run_time, _is_found }
        {
            reset();
        }

        auto operator()(State start, State goal, ValidateFunc validate) -> void
        {
            reset();
            search(start, goal, validate);
        }

    private:

        Q _q;
        size_t _max_q_size;
        Expansions _expansions;
        string _final_path;
        long long _run_time;
        bool _is_found;

        auto search(State start, State goal, ValidateFunc validate) -> bool
        {
            for (_q.push({ "", start, goal }); true; _max_q_size = max(_max_q_size, _q.size()))
            {
                if (_q.empty()) break;

                auto curr = _q.top(); _q.pop();
                if (goal == curr.state()) return _is_found = true;

                // is really needed? need refactoring anyway.
                if (any_of(_expansions.cbegin(), _expansions.cend(), [&](State const& state) {
                    return state == curr.state(); 
                }))
                    continue;
                _expansions.insert(curr.state());

                for (auto const& child : curr.children(validate))
                {
                    //handle expansions
                    if (any_of(_expansions.cbegin(), _expansions.cend(), [&](State state) {
                        return state == child.state();
                    }))
                        continue;
                    //handle q
                    function<bool(Node)> is_same_state = [&](Node const& node) {
                        return node.state() == child.state();
                    };

                    if (!_q.any_of(is_same_state))
                        _q.push(child);
                    else
                        _q.update_with_if(child, is_same_state);
                }
            }
            return false;
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