#pragma once

using std::make_pair;
using std::min;
using std::abs;
using std::hypot;

namespace search
{
    //
    //  Lifelong planning 
    //
    namespace lp
    {
        struct Key
        {
            int first, second;
            friend auto operator== (Key l, Key r)
            {
                return l.first == r.first && l.second == r.second;
            }
        };

        struct Coordinate
        {
            int x, y;
            friend auto operator== (Coordinate l, Coordinate r)
            {
                return l.x == r.x && l.y == r.y;
            }
        };

        struct LpState
        {
            Coordinate c;
            int g, r;

            template<typename Hfunc>
            auto key(Hfunc h) const -> Key
            {
                return{ min(g, r + h(c)), min(g, r) };
            }
        };

        struct LpManhattanDistance
        {
            Coordinate goal;
            auto operator()(Coordinate c) const -> int
            {
                return max(abs(goal.x - c.x), abs(goal.y - c.y));
            }
        };

        struct LpEuclideanDistance
        {
            Coordinate goal;
            auto operator()(Coordinate c) const -> int
            {
                auto result = hypot(abs(goal.x - c.x), abs(goal.y - c.y));
                return static_cast<int>(round(result));
            }
        };
    }
}