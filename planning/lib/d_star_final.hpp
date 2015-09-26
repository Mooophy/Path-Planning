#pragma once

#include "helpers.hpp"
#include "priority_queue.hpp"

using std::pair;
using std::make_pair;


namespace search
{
    namespace ds
    {
        struct OldKeys : public unordered_map<Cell, Key, Cell::Hasher>
        {
            auto update_with(pair<Cell, Key> cell_key_pair)
            {
                erase(cell_key_pair.first);
                insert(cell_key_pair);
            }
        };

        class DStarCore
        {
            //
            //  Algorithm
            //

            auto validate(Cell c) const
            {
                return c.row >= 0 && c.row < (int)matrix.rows() && c.col >= 0 && c.col < (int)matrix.cols();
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
                km = at(goal).r = 0;
                q.push(goal);
            }
            auto update_vertex(LpState& s)
            {
                if (s.cell != goal)
                {
                    auto minimum = huge();
                    for (auto neighbour : valid_neighbours_of(s.cell))
                        minimum = min(minimum, (at(neighbour).g + cost()));
                    s.r = minimum;
                }
                q.remove(s.cell);
                if (s.g != s.r)
                    q.push(s.cell), old_keys.update_with(make_pair(s.cell, Key{ s, km }));
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
            auto mark_h_values_with(Cell terminal)
            {
                auto mark_h = [=](Cell c) { at(c).h = hfunc(c, terminal); };
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
            DStarCore(unsigned rows, unsigned cols, Cell start, Cell goal, string heuristic, Cells const& bad_cells) :
                matrix{ rows, cols },
                start{ start },
                goal{ goal },
                hfunc{ HEURISTICS.at(heuristic) },
                km{ 0 },
                q{ [this](Cell l, Cell r) { return Key{ at(l), km } < Key{ at(r), km }; } },
                old_keys { }
            {
                mark_bad_cells(bad_cells);
                mark_h_values_with(start);  //h value : start to current
                reset_statistics();
            }
            //
            //  data members
            //
            Matrix matrix;
            Cell const start, goal;
            function<int(Cell, Cell)> const hfunc;
            int km;
            PriorityQueue < Cell, function<bool(Cell, Cell)> > q;
            OldKeys old_keys;
            //
            //  statistics
            //
            size_t max_q_size;
            Cells expansions;
            string path;
            long long run_time;
        };
    }
}
