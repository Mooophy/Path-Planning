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
using std::unordered_set;

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
        using Expansions = unordered_set<State>;

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
        //
        //  Data members
        //
        Q _q;
        size_t _max_q_size;
        Expansions _expansions;
        string _final_path;
        long long _run_time;
        bool _is_found;
        //
        //  Core part
        //
        auto search(State start, State goal, ValidateFunc validate) -> bool
        {
            for (_q.push({ "", start, goal }); true; _max_q_size = max(_max_q_size, _q.size()))
            {
                if (_q.empty()) return _is_found = false;

                auto curr = _q.top(); _q.pop();
                if (goal == curr.state()) return _is_found = true;
                if (expansions_contain_state_of(curr))  continue;
                expand(curr);

                for (auto const& child : curr.children(validate))
                {
                    if (expansions_contain_state_of(child))  continue;

                    function<bool(Node)> is_same_state = [&](Node const& node) {
                        return node.state() == child.state();
                    };
                    if (!_q.any(is_same_state))
                        _q.push(child);
                    else
                        _q.update_with_if(child, is_same_state);
                }
            }
        }
        //
        //  helper for method search
        //
        auto expansions_contain_state_of(Node const& n) const -> bool
        {
            auto const& e = _expansions;
            return any_of(e.cbegin(), e.cend(), [&](State s) {return s == n.state(); });
        }
        //
        //  helper for method search
        //
        auto expand(Node const& n) -> void
        {
            _expansions.insert(n.state());
        }
        //
        //  reset all data members
        //
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