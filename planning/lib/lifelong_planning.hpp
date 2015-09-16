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
            return std::numeric_limits<int>::max();
        }
        constexpr auto cost()
        {
            return 1;
        }

        struct Coordinate
        {
            int x, y;

            friend auto operator== (Coordinate l, Coordinate r)
            {
                return l.x == r.x && l.y == r.y;
            }
            friend auto operator!= (Coordinate l, Coordinate r)
            {
                return !(l == r);
            }

            auto to_string() const
            {
                using std::to_string;
                return string{ "[x = " + to_string(x) + ", y = " + to_string(y) + "]" };
            }
            auto to_hash() const
            {
                return hash<string>{}(to_string());
            }

            auto neighbours() const
            {
                struct Directions : public map< char, function< Coordinate(Coordinate) >>
                {
                    Directions()
                    {
                        (*this)['1'] = [](Coordinate c) { return Coordinate{ c.x - 1, c.y - 1 }; };
                        (*this)['2'] = [](Coordinate c) { return Coordinate{ c.x - 0, c.y - 1 }; };
                        (*this)['3'] = [](Coordinate c) { return Coordinate{ c.x + 1, c.y - 1 }; };
                        (*this)['4'] = [](Coordinate c) { return Coordinate{ c.x - 1, c.y - 0 }; };
                        (*this)['5'] = [](Coordinate c) { return Coordinate{ c.x + 1, c.y + 0 }; };
                        (*this)['6'] = [](Coordinate c) { return Coordinate{ c.x - 1, c.y + 1 }; };
                        (*this)['7'] = [](Coordinate c) { return Coordinate{ c.x - 0, c.y + 1 }; };
                        (*this)['8'] = [](Coordinate c) { return Coordinate{ c.x + 1, c.y + 1 }; };
                    }
                } static const directions;

                vector<Coordinate> result;
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
    struct hash<Coordinate>
    {
        auto operator()(Coordinate c) const
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
            Coordinate coordinate;
            int g, r;
            bool is_blocked;
            friend auto operator==(LpState const& l, LpState const& r)
            {
                return l.coordinate == r.coordinate && l.g == r.g && l.r == r.r && l.is_blocked == r.is_blocked;
            }
        };

        class Matrix
        {
        public:
            Matrix(unsigned height, unsigned width)
                : _data{ height, vector<LpState>(width) }
            {
                for (auto y = 0; y != height; ++y)
                {
                    for (auto x = 0; x != width; ++x)
                    {
                        Coordinate curr{ x, y };
                        at(curr).coordinate = curr;
                        at(curr).g = at(curr).r = infinity();
                    }
                }
            }

            auto at(Coordinate c) -> LpState&
            {
                return{ _data[c.y][c.x] };
            }
            auto at(Coordinate c) const -> LpState const&
            {
                return{ _data[c.y][c.x] };
            }

            auto rows() const
            {
                return _data.size();
            }
            auto cols() const
            {
                return _data.front().size();
            }

        private:
            vector<vector<LpState>> _data;
        };

        struct HeuristcFuncs : public unordered_map < string, function<int(Coordinate, Coordinate)> >
        {
            HeuristcFuncs()
            {
                (*this)["manhattan"] =
                    [](Coordinate curr, Coordinate goal)
                {
                    return max(abs(goal.x - curr.x), abs(goal.y - curr.y));
                };
                (*this)["euclidean"] =
                    [](Coordinate curr, Coordinate goal)
                {
                    return static_cast<int>(round(hypot(abs(goal.x - curr.x), abs(goal.y - curr.y))));
                };
            }
        };

        struct Key
        {
            const int first, second;

            Key(int fst, int snd)
                : first{ fst }, second{ snd }
            {   }
            Key(LpState s, function<int(Coordinate, Coordinate)> h, Coordinate g)
                : Key{ min(s.g, s.r + h(s.coordinate, g)), min(s.g, s.r) }
            {   }

            friend auto operator== (Key l, Key r)
            {
                return l.first == r.first && l.second == r.second;
            }
            friend auto operator < (Key l, Key r)
            {
                return (l.first < r.first) || (l.first == r.first && l.second < r.second);
            }
        };
        //
        //  Lifelong A*
        //
        class LpAstarCore
        {
            auto initialize()
            {
                q.reset();
                matrix.at(start).r = 0;
                q.push(matrix.at(start));
            }

            auto update_vetex(LpState state)
            {
                //if (state.coordinate != start)
                //{

                //}
            }

            auto run()
            {
                initialize();
            }

        public:
            //
            //  Constructor
            //
            LpAstarCore(unsigned rows, unsigned cols, Coordinate start, Coordinate goal, string heuristic, unordered_set<Coordinate> const& blockeds):
                heuristics{},
                matrix{ rows, cols },
                start{ start }, 
                goal{ goal },
                h{ heuristics.at(heuristic) },
                q{ [&](LpState const& lft, LpState const& rht) { return Key{ lft, h, goal } < Key{ rht, h, goal }; } }
            {
                for (auto blocked : blockeds)
                    matrix.at(blocked).is_blocked = true;
            }

            auto operator()()
            {
                run();
            }

            HeuristcFuncs const heuristics;
            Matrix matrix;
            Coordinate const start, goal;
            function<int(Coordinate, Coordinate)> const h;
            PriorityQueue < LpState, function<bool(LpState, LpState)>> q;
        };
    }
}//end of namespace search