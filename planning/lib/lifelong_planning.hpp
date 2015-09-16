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

        struct LpState
        {
            struct Key
            {
                const int first, second;

                friend auto operator== (Key l, Key r)
                {
                    return l.first == r.first && l.second == r.second;
                }
                friend auto operator < (Key l, Key r)
                {
                    return (l.first < r.first) || (l.first == r.first && l.second < r.second);
                }
            };

            Coordinate coordinate;
            int g, r;

            template<typename Hfunc>
            auto key(Hfunc h) const
            {
                return Key{ min(g, r + h(coordinate)), min(g, r) };
            }
        };

        struct LpManhattanDistance
        {
            const Coordinate goal;
            auto operator()(Coordinate c) const
            {
                return max(abs(goal.x - c.x), abs(goal.y - c.y));
            }
        };

        struct LpEuclideanDistance
        {
            const Coordinate goal;
            auto operator()(Coordinate c) const
            {
                auto result = hypot(abs(goal.x - c.x), abs(goal.y - c.y));
                return static_cast<int>(round(result));
            }
        };

        class Matrix
        {
        public:
            Matrix(unsigned height, unsigned width)
                : _data{ height, vector<LpState>(width)}
            {   }

            auto at(Coordinate c) -> LpState&
            {
                return{ _data[c.y][c.x] };
            }

            auto at(Coordinate c) const -> LpState const&
            {
                return{ _data[c.y][c.x] };
            }

        private:
            vector<vector<LpState>> _data;
        };
    }
}