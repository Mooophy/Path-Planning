#pragma once

#include "priority_queue.hpp"
#include "timing.hpp"
#include "helpers.hpp"

namespace search
{
    //
    //  Lifelong planning 
    //
    namespace lp
    {
        struct Key
        {
            const int fst, snd;

            Key(int fst, int snd)
                : fst{ fst }, snd{ snd }
            {   }
            Key(LpState const& s)
                : Key{ min(s.g, s.r) + s.h, min(s.g, s.r) } // i.e. CalculateKey in paper
            {   }

            friend auto operator== (Key l, Key r)
            {
                return l.fst == r.fst && l.snd == r.snd;
            }
            friend auto operator < (Key l, Key r)
            {
                return (l.fst < r.fst) || (l.fst == r.fst && l.snd < r.snd);
            }
        };
        //
        //  Lifelong A*
        //
        class LpAstarCore
        {
            //
            //  Algorithm
            //

            auto validate(Cell c) const
            {
                return c.row >= 0 && c.row < (int)matrix.rows() && c.col >= 0 && c.col < (int)matrix.cols();
            }
            auto build_path() const
            {
                string inverse_path;
                for (auto c = goal; c != start; )
                {
                    for (auto direction = '1'; direction != '9'; ++direction)
                    {
                        auto neighbour = DIRECTIONS.at(direction)(c);
                        if (validate(neighbour) && (at(neighbour).g + cost() == at(c).g))
                        {
                            inverse_path.push_back(direction);
                            c = neighbour;
                            break;
                        }
                    }
                }
                string path;
                for (auto i = inverse_path.crbegin(); i != inverse_path.crend(); ++i)
                    path.push_back(*i);
                for (auto& ch : path)
                    ch = 2 * '0' + 9 - ch;

                return path;
            }
            auto valid_neighbours_of(Cell c) const
            {
                Cells neighbours;
                for (auto direction = '1'; direction != '9'; ++direction)
                {
                    auto n = DIRECTIONS.at(direction)(c);
                    if (validate(n))
                        neighbours.insert(n);
                }
                return neighbours;
            }

            auto initialize()
            {
                q.reset();
                at(start).r = 0;
                q.push(start);
            }
            auto update_vertex(LpState& s)
            {
                if (s.cell != start)
                {
                    auto minimum = huge();
                    for (auto neighbour : valid_neighbours_of(s.cell))
                        minimum = min(minimum, (at(neighbour).g + cost()));
                    s.r = minimum;
                }
                q.remove(s.cell);
                if (s.g != s.r) q.push(s.cell);
            }
            auto update_neighbours_of(Cell cell)
            {
                for (auto neighbour : valid_neighbours_of(cell))
                    if (!at(neighbour).bad)
                        update_vertex(at(neighbour));
            }
            auto compute_shortest_path()
            {
                Timing t{ run_time };
                while (!q.empty() && (Key{ at(q.top()) } < Key{ at(goal) } || at(goal).r != at(goal).g))
                {
                    auto c = q.pop();
                    if (at(c).g > at(c).r)
                        at(c).g = at(c).r;
                    else
                        at(c).g = huge(), update_vertex(at(c));
                    update_neighbours_of(c);
                    {
                        max_q_size = max(max_q_size, q.size());
                        expansions.insert(c);
                    }
                }
                path = build_path();
            }
            //
            //  helpers
            //
            auto at(Cell c) const -> LpState const&
            {
                return matrix.at(c);
            }
            auto at(Cell c) -> LpState&
            {
                return matrix.at(c);
            }
            auto mark_bad_cells(Cells const& bad_cells)
            {
                for (auto c : bad_cells) at(c).bad = true;
            }
            auto mark_h_values()
            {
                auto mark_h = [this](Cell c) { at(c).h = hfunc(c, goal); };
                matrix.each_cell(mark_h);
            }
            auto reset_statistics()
            {
                run_time = max_q_size = 0;
                expansions.clear(), path.clear();
            }

        public:
            //
            //  Constructor
            //
            LpAstarCore(unsigned rows, unsigned cols, Cell start, Cell goal, string heuristic, Cells const& bad_cells) :
                matrix{ rows, cols },
                start{ start },
                goal{ goal },
                hfunc{ HEURISTICS.at(heuristic) },
                q{ [this](Cell l, Cell r) { return Key{ at(l) } < Key{ at(r) }; } }
            {
                mark_bad_cells(bad_cells);
                mark_h_values();
                reset_statistics();
            }

            auto plan()
            {
                reset_statistics();
                initialize();
                compute_shortest_path();
            }
            auto replan(Cells const& cells_to_toggle = {})
            {
                reset_statistics();
                for (auto c : cells_to_toggle)
                {
                    at(c).bad = !at(c).bad;
                    if (!at(c).bad)
                        update_vertex(at(c));
                    else
                        at(c).g = at(c).r = huge();
                    update_neighbours_of(c);
                }
                compute_shortest_path();
            }
            //
            //  data members
            //
            Matrix matrix;
            Cell const start, goal;
            function<int(Cell, Cell)> const hfunc;
            PriorityQueue < Cell, function<bool(Cell, Cell)> > q;
            //
            //  statistics
            //
            size_t max_q_size;
            Cells expansions;
            string path;
            long long run_time;
        };
    }
}//end of namespace search