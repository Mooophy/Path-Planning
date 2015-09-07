#pragma once

#include <string>
#include <map>
#include <functional>
#include <vector>

namespace search
{
    class Node
    {
        friend auto operator==(Node, Node) -> bool;
    public:
        using Size = std::size_t;
        using Path = std::string;
        using Children = std::vector<Node>;
        struct Coordinate { Size y, x; };
        using Functions = std::map< char, std::function< Coordinate(Coordinate) >>;

        //
        //  ctor
        //
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
                c = Node::goes.at(direction)(c);
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

        const static Functions goes;
    private:
        Path const  _path;
        Coordinate const _start;
        Coordinate const _goal;
    };

    auto operator==(Node::Coordinate lhs, Node::Coordinate rhs) -> bool
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    auto operator==(Node lhs, Node rhs) -> bool
    {
        return (lhs._path == rhs._path) && (lhs._start == rhs._start) && (lhs._goal == rhs._goal);
    }

    //  0,0     0,1     0,2     ||  1   2   3
    //  1,0     1,1     1,2     ||  4       5
    //  2,0     2,1     2,2     ||  6   7   8
    Node::Functions const Node::goes 
    {
        { '1', [](Coordinate c) -> Coordinate{ return{ c.y - 1, c.x - 1 }; } },
        { '2', [](Coordinate c) -> Coordinate{ return{ c.y - 1, c.x - 0 }; } },
        { '3', [](Coordinate c) -> Coordinate{ return{ c.y - 1, c.x + 1 }; } },
        { '4', [](Coordinate c) -> Coordinate{ return{ c.y - 0, c.x - 1 }; } },
        { '5', [](Coordinate c) -> Coordinate{ return{ c.y + 0, c.x + 1 }; } },
        { '6', [](Coordinate c) -> Coordinate{ return{ c.y + 1, c.x - 1 }; } },
        { '7', [](Coordinate c) -> Coordinate{ return{ c.y + 1, c.x - 0 }; } },
        { '8', [](Coordinate c) -> Coordinate{ return{ c.y + 1, c.x + 1 }; } }
    };
}
