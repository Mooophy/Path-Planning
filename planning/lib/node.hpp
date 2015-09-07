#pragma once

#include <string>
#include <map>
#include <functional>
#include <vector>

namespace search
{
    using Size = std::size_t;
    //
    //  Struct Coordinate
    //
    struct Coordinate { Size y, x; };
    inline auto operator==(Coordinate lhs, Coordinate rhs) -> bool
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }
    //
    //  Function map for 8 directions
    //
    using Functions = std::map< char, std::function< Coordinate(Coordinate) >>;
    struct Goes : public Functions
    {
        explicit Goes()
        {
            (*this)['1'] = [](Coordinate c) -> Coordinate { return{ c.y - 1, c.x - 1 }; };
            (*this)['2'] = [](Coordinate c) -> Coordinate { return{ c.y - 1, c.x - 0 }; };
            (*this)['3'] = [](Coordinate c) -> Coordinate { return{ c.y - 1, c.x + 1 }; };
            (*this)['4'] = [](Coordinate c) -> Coordinate { return{ c.y - 0, c.x - 1 }; };
            (*this)['5'] = [](Coordinate c) -> Coordinate { return{ c.y + 0, c.x + 1 }; };
            (*this)['6'] = [](Coordinate c) -> Coordinate { return{ c.y + 1, c.x - 1 }; };
            (*this)['7'] = [](Coordinate c) -> Coordinate { return{ c.y + 1, c.x - 0 }; };
            (*this)['8'] = [](Coordinate c) -> Coordinate { return{ c.y + 1, c.x + 1 }; };
        }
    } const GOES;
    //
    //  Class Node
    //
    class Node
    {
        friend auto operator==(Node const& lhs, Node const& rhs) -> bool
        {
            return lhs._path == rhs._path && lhs._start == rhs._start && lhs._goal == rhs._goal;
        }

    public:
        using Path = std::string;
        using Children = std::vector<Node>;
        Node(Path const& path, Coordinate start, Coordinate goal)
            : _path{ path }, _start{ start }, _goal{ goal }
        { }

        auto path() const -> Path const& 
        {
            return _path; 
        }

        auto coordinate() const -> Coordinate
        {
            Coordinate c = _start;
            for (auto direction : _path)
                c = GOES.at(direction)(c);
            return c;
        }

        template<typename ValidateFunc>
        auto children() const -> Children
        {
            Children children;
            ValidateFunc validate;
            for(auto direction = '1'; direction <= '8'; ++direction)
            {
                auto child = Node{ _path + direction, _start, _goal };
                if(validate(child))
                    children.push_back(child);
            }
            return children;
        }

    private:
        Path const  _path;
        Coordinate const _start;
        Coordinate const _goal;
    };
}//end of namespace