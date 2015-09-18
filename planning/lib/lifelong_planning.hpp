#pragma once

#include <limits>
#include <algorithm>
#include <vector>
#include <map>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include "priority_queue.hpp"

using std::max;
using std::make_pair;
using std::min;
using std::abs;
using std::hypot;
using std::vector;
using std::map;
using std::function;
using std::unordered_map;
using std::unordered_set;
using std::string;
using std::to_string;
using std::hash;

namespace search
{
    //
    //  Lifelong planning 
    //
    namespace lp
    {
        constexpr auto huge()
        {
            return 10'000;
        }
        constexpr auto cost()
        {
            return 1;
        }

        struct Cell
        {
            int row, col;

            friend auto operator== (Cell l, Cell r)
            {
                return l.row == r.row && l.col == r.col;
            }
            friend auto operator!= (Cell l, Cell r)
            {
                return !(l == r);
            }

            auto to_string() const
            {
                using std::to_string;
                return string{ "[r=" + to_string(row) + ",c=" + to_string(col) + "]" };
            }
            auto to_hash() const
            {
                return hash<string>{}(to_string());
            }

            auto neighbours() const
            {
                struct Directions : public map< char, function< Cell(Cell) >>
                {
                    Directions()
                    {
                        (*this)['1'] = [](Cell c) { return Cell{ c.row - 1, c.col - 1 }; };
                        (*this)['2'] = [](Cell c) { return Cell{ c.row - 1, c.col - 0 }; };
                        (*this)['3'] = [](Cell c) { return Cell{ c.row - 1, c.col + 1 }; };
                        (*this)['4'] = [](Cell c) { return Cell{ c.row - 0, c.col - 1 }; };
                        (*this)['5'] = [](Cell c) { return Cell{ c.row + 0, c.col + 1 }; };
                        (*this)['6'] = [](Cell c) { return Cell{ c.row + 1, c.col - 1 }; };
                        (*this)['7'] = [](Cell c) { return Cell{ c.row + 1, c.col + 0 }; };
                        (*this)['8'] = [](Cell c) { return Cell{ c.row + 1, c.col + 1 }; };
                    }
                } static const directions;

                vector<Cell> result;
                for (auto n = '1'; n != '9'; ++n)
                    result.push_back(directions.at(n)(*this));
                return result;
            }
        };
    }
}


namespace std
{
    using namespace search::lp;
    template<>
    struct hash<Cell>
    {
        auto operator()(Cell c) const
        {
            return c.to_hash();
        }
    };
}


namespace search
{
    namespace lp
    {
        struct LpState
        {
            Cell cell; int g, r, h; bool bad;
            auto to_string() const
            {
                using std::to_string;
                return "{" + cell.to_string() + "|g:" + to_string(g) + "|r:" + to_string(r) + "|h:" + to_string(h) + "|b:" + (bad ? "t" : "f") + "}";
            }
            friend auto operator==(LpState const& l, LpState const& r)
            {
                return l.cell == r.cell && l.g == r.g && l.r == r.r  && l.h == r.h && l.bad == r.bad;
            }
        };

        class Matrix
        {
        public:
            Matrix(unsigned rows, unsigned cols)
                : _data{ rows, vector<LpState>(cols) }
            {
                for (auto r = 0; r != rows; ++r)
                {
                    for (auto c = 0; c != cols; ++c)
                    {
                        Cell curr{ r, c };
                        at(curr).cell = curr;
                        at(curr).g = at(curr).r = huge();
                    }
                }
            }

            auto at(Cell c) -> LpState&
            {
                return{ _data[c.row][c.col] };
            }
            auto at(Cell c) const -> LpState const&
            {
                return{ _data[c.row][c.col] };
            }

            auto rows() const
            {
                return _data.size();
            }
            auto cols() const
            {
                return _data.front().size();
            }

            auto to_string() const
            {
                string result;
                for (auto r = 0; r != rows(); ++r)
                {
                    for (auto c = 0; c != cols(); ++c)
                        result += at({ r, c }).to_string();
                    result += "+++";
                }
                return result;
            }

        private:
            vector<vector<LpState>> _data;
        };

        struct Heuristics : public unordered_map < string, function<int(Cell, Cell)> >
        {
            Heuristics()
            {
                (*this)["manhattan"] =
                    [](Cell curr, Cell goal)
                {
                    return max(abs(goal.row - curr.row), abs(goal.col - curr.col));
                };
                (*this)["euclidean"] =
                    [](Cell curr, Cell goal)
                {
                    return static_cast<int>(round(hypot(abs(goal.row - curr.row), abs(goal.col - curr.col))));
                };
            }
        };

        const static Heuristics HEURISTICS;

        struct Key
        {
            const int fst, snd;

            Key(int fst, int snd)
                : fst{ fst }, snd{ snd }
            {   }
            Key(LpState const& s)
                : Key{ min(s.g, s.r) + s.h, min(s.g, s.r) }// i.e. CalculateKey in paper
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

            auto filter(vector<Cell> && cells)
            {
                vector<Cell> result;
                for (auto && c : cells)
                    if (c.row >= 0 && c.row < (int)matrix.rows() && c.col >= 0 && c.col < (int)matrix.cols())
                        result.push_back(move(c));
                return result;
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
                    for (auto neighbour : filter(s.cell.neighbours()))
                        minimum = min(minimum, (at(neighbour).g + cost()));
                    s.r = minimum;
                }
                q.remove(s.cell);
                if (s.g != s.r) q.push(s.cell);
            }
            auto update_neighbours_of(Cell cell)
            {
                for (auto neighbour : filter(cell.neighbours()))
                    if (!at(neighbour).bad)
                        update_vertex(at(neighbour));
            }
            auto compute_shortest_path()
            {
                while ( !q.empty() && (Key{ at(q.top()) } < Key{ at(goal) } || at(goal).r != at(goal).g))
                {
                    auto c = q.pop();
                    if (at(c).g > at(c).r)
                        at(c).g = at(c).r;
                    else
                        at(c).g = huge(), update_vertex(at(c));
                    update_neighbours_of(c);
                }
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
            auto mark_bad_cells(unordered_set<Cell> const& bad_cells)
            {
                for (auto cell : bad_cells) at(cell).bad = true;
            }
            auto mark_h_values()
            {
                for (auto r = 0; r != matrix.rows(); ++r)
                    for (auto c = 0; c != matrix.cols(); ++c)
                        at({ r, c }).h = hfunc({ r, c }, goal);
            }

        public:

            //
            //  Constructor
            //
            LpAstarCore(unsigned rows, unsigned cols, Cell start, Cell goal, string heuristic, unordered_set<Cell> const& bad_cells) :
                matrix{ rows, cols },
                start{ start },
                goal{ goal },
                hfunc{ HEURISTICS.at(heuristic) },
                q{ [this](Cell lft, Cell rht) { return Key{ at(lft) } < Key{ at(rht) }; } }
            {
                mark_bad_cells(bad_cells);
                mark_h_values();
            }

            auto plan()
            {
                initialize();
                compute_shortest_path();
            }
            auto replan(unordered_set<Cell> const& cells_to_toggle = {})
            {
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
        };
    }
}//end of namespace search