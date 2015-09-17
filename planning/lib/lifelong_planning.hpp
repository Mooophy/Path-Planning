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
        constexpr auto infinity()
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
            Cell cell;
            int g, r, h;
            bool bad;

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
                        at(curr).g = at(curr).r = infinity();
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
                        result += at({ c, r }).to_string();
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
    //        auto filter(vector<Coordinate> && cs)
    //        {
    //            vector<Coordinate> result;
    //            for (auto && c : cs)
    //                if (c.x >= 0 && c.x < (int)matrix.cols() && c.y >= 0 && c.y < (int)matrix.rows())
    //                    result.push_back(move(c));
    //            return result;
    //        }

    //        auto initialize()
    //        {
    //            q.reset();
    //            matrix.at(start).r = 0;
    //            q.push(matrix.at(start));
    //        }

    //        auto update_vertex(LpState& s)
    //        {
    //            if (s.coordinate != start)
    //            {
    //                auto minimum = infinity();
    //                for (auto n : filter(s.coordinate.neighbours()))
    //                {
    //                    auto& vertex = matrix.at(n);
    //                    //if (!vertex.is_blocked)
    //                    minimum = min(minimum, (vertex.g + cost()));
    //                }
    //                s.r = minimum;
    //            }
    //            q.remove(s);
    //            if (s.g != s.r) q.push(s);
    //        }

    //        auto compute_shortest_path()
    //        {
    //            auto top_key = [this] { return q.empty() ? Key{ infinity(), infinity() } : Key{ matrix.at(q.top().coordinate), h, goal };  };

    //            while (top_key() < Key{ matrix.at(goal), h, goal } || matrix.at(goal).r != matrix.at(goal).g)
    //            {
    //                auto c = q.pop().coordinate;

    //                if (matrix.at(c).g > matrix.at(c).r)
    //                {
    //                    matrix.at(c).g = matrix.at(c).r;
    //                    for (auto n : filter(c.neighbours()))
    //                        if (!matrix.at(n).is_blocked)
    //                            update_vertex(matrix.at(n));
    //                }
    //                else
    //                {
    //                    matrix.at(c).g = infinity();
    //                    for (auto n : filter(c.neighbours()))
    //                        if (!matrix.at(n).is_blocked)
    //                            update_vertex(matrix.at(n));
    //                    update_vertex(matrix.at(c));
    //                }
    //            }
    //        }

        public:

            //
            //  Constructor
            //
            LpAstarCore(unsigned rows, unsigned cols, Cell start, Cell goal, string heuristic, unordered_set<Cell> const& bad_cells) :
                matrix{ rows, cols },
                start{ start },
                goal{ goal },
                hfunc{ HEURISTICS.at(heuristic) },
                q{ [this](Cell lft, Cell rht) { return Key{ matrix.at(lft) } < Key{ matrix.at(rht) }; } }
            {
                //mark all bad cells
                for (auto cell : bad_cells)
                    matrix.at(cell).bad = true;

                //mark all h value
                for (auto r = 0; r != matrix.rows(); ++r)
                    for (auto c = 0; c != matrix.cols(); ++c)
                        matrix.at({ r, c }).h = hfunc({ r, c }, goal);
            }

    //        auto run()
    //        {
    //            initialize();
    //            compute_shortest_path();
    //        }

    //        auto run_with_changes(vector<Coordinate> const& coordinates_to_toggle)
    //        {
    //            for (auto c : coordinates_to_toggle)
    //            {
    //                auto& s = matrix.at(c);
    //                s.is_blocked = !s.is_blocked;
    //                if (!s.is_blocked)
    //                    update_vertex(s);
    //                else
    //                    s.r = s.g = infinity();
    //                for (auto neighbour : filter(s.coordinate.neighbours()))
    //                    update_vertex(matrix.at(neighbour));
    //            }
    //            compute_shortest_path();
    //        }

            Matrix matrix;
            Cell const start, goal;
            function<int(Cell, Cell)> const hfunc;
            PriorityQueue < Cell, function<bool(Cell, Cell)> > q;
        };
    }
}//end of namespace search