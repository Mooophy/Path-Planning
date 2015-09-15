#pragma once

#include <limits>
#include <algorithm>
#include <vector>
#include <map>
#include <functional>

using std::max;
using std::make_pair;
using std::min;
using std::abs;
using std::hypot;
using std::vector;
using std::map;
using std::function;

namespace search
{
    //
    //  Lifelong planning 
    //
    namespace lp
    {
        static auto infinity() -> int
        {
            return std::numeric_limits<int>::max();
        }

        struct Key
        {
            const int first, second;

            friend auto operator== (Key l, Key r) -> bool
            {
                return l.first == r.first && l.second == r.second;
            }
            friend auto operator < (Key l, Key r) -> bool
            {
                return (l.first < r.first) || (l.first == r.first && l.second < r.second);
            }
        };

        struct Coordinate
        {
            const int x, y;
            
            friend auto operator== (Coordinate l, Coordinate r)
            {
                return l.x == r.x && l.y == r.y;
            }
            friend auto operator!= (Coordinate l, Coordinate r)
            {
                return !(l == r);
            }

            auto neighbours() const -> vector<Coordinate>
            {
                struct Directions : public map< char, function< Coordinate(Coordinate) >>
                {
                    Directions()
                    {
                        (*this)['1'] = [](Coordinate c) -> Coordinate { return{ c.x - 1, c.y - 1 }; };
                        (*this)['2'] = [](Coordinate c) -> Coordinate { return{ c.x - 0, c.y - 1 }; };
                        (*this)['3'] = [](Coordinate c) -> Coordinate { return{ c.x + 1, c.y - 1 }; };
                        (*this)['4'] = [](Coordinate c) -> Coordinate { return{ c.x - 1, c.y - 0 }; };
                        (*this)['5'] = [](Coordinate c) -> Coordinate { return{ c.x + 1, c.y + 0 }; };
                        (*this)['6'] = [](Coordinate c) -> Coordinate { return{ c.x - 1, c.y + 1 }; };
                        (*this)['7'] = [](Coordinate c) -> Coordinate { return{ c.x - 0, c.y + 1 }; };
                        (*this)['8'] = [](Coordinate c) -> Coordinate { return{ c.x + 1, c.y + 1 }; };
                    }
                } static const directions;

                vector<Coordinate> result;
                for (auto n = '1'; n != '9'; ++n)
                    result.push_back(directions.at(n)(*this));
                return result;
            }
        };



        struct LpState
        {
            const Coordinate c;
            int g, r;

            template<typename Hfunc>
            auto key(Hfunc h) const -> Key
            {
                return{ min(g, r + h(c)), min(g, r) };
            }
        };

        struct LpManhattanDistance
        {
            const Coordinate goal;
            auto operator()(Coordinate c) const -> int
            {
                return max(abs(goal.x - c.x), abs(goal.y - c.y));
            }
        };

        struct LpEuclideanDistance
        {
            const Coordinate goal;
            auto operator()(Coordinate c) const -> int
            {
                auto result = hypot(abs(goal.x - c.x), abs(goal.y - c.y));
                return static_cast<int>(round(result));
            }
        };
    }
}