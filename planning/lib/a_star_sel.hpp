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
        } const data;

        AStarSEL()
            : data{ _q, _max_q_size, _expansions, _final_path, _run_time, _is_found }
        {
            reset();
        }

        auto operator()(ValidateFunc validate) -> void
        {
            reset();
            _is_found = search(move(validate));
        }

    private:

        Q _q;
        size_t _max_q_size;
        Expansions _expansions;
        string _final_path;
        long long _run_time;
        bool _is_found;

        auto search(State start, State goal, ValidateFunc && validation) -> bool
        {
            for (_q.push({ "", start, goal }); true; _max_q_size = max(_max_q_size, _q.size()))
            {
                if (_q.empty())
                    return false;

                auto curr = _q.top(); _q.pop();
                if (goal == curr.state())
                {
                    _is_found = true;
                    return true;
                }

                // is really needed? need refactoring anyway.
                if (any_of(_expansions.cbegin(), _expansions, [&](Node const& node) {
                    return node.state() == curr.state(); 
                }))
                    continue;

                _expansions.insert(curr.state());

                for (auto const& child : curr.children(validation))
                {
                    //handle expansions
                    if (any_of(_expansions.cbegin(), _expansions, [&](Node const& node) {
                        return node.state() == child.state();
                    }))
                        continue;

                    //handle q
                    struct Functor
                    {
                        Node const value;
                        //  to see if same state
                        auto operator()(Node const& node) const -> bool
                        {
                            return node.state() == value.state();
                        }
                    };

                    _q.update_if(Functor{ child });
                }
            }
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