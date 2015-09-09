#pragma once
#include <string>
#include <map>
#include <functional>
#include <vector>

using std::string;

namespace search
{
    using Size = int;
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
        Goes()
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

        Node() = default;

        Node(Path const& path, Coordinate start, Coordinate goal)
            : _path{ path }, _start{ start }, _goal{ goal }
        { }

        Node(Node const& other)
            : _path{ other._path }, _start{ other._start }, _goal{ other._goal }
        {   }

        auto path() const& -> Path const& 
        {
            return _path; 
        }

        auto start() const& -> Coordinate
        {
            return _start;
        }

        auto goal() const& -> Coordinate
        {
            return _goal;
        }

        auto coordinate() const -> Coordinate
        {
            Coordinate c = _start;
            for (auto direction : _path)
                c = GOES.at(direction)(c);
            return c;
        }

        auto to_string() const -> string
        {
            using std::to_string;
            auto y = [](Coordinate const& c) { return to_string(c.y); };
            auto x = [](Coordinate const& c) { return to_string(c.x); };
            return "[" + y(_start) + "," + x(_start) + "][" + _path + "][" + y(_goal) + "," + x(_goal) + "]";
        }

        //
        //  require ValidateFunc has interface operator()(Node)
        //
        template<typename ValidateFunc>
        auto children(ValidateFunc validate) const -> Children
        {
            Children children;
            for(auto direction = '1'; direction <= '8'; ++direction)
            {
                auto child = Node{ _path + direction, _start, _goal };
                if(validate(child))
                    children.push_back(child);
            }
            return children;
        }

    private:
        Path _path;
        Coordinate _start;
        Coordinate _goal;
    };
}//end of namespace

using namespace search;
using std::to_string;
namespace std
{
    template<>
    struct hash<Node>
    {
        //[start.to_string][path][goal.to_string]
        auto operator()(Node const& node) const -> size_t
        {
            auto const& n = node;
            using C = Coordinate;
            auto x = [](C const& c) { return to_string(c.x); };
            auto y = [](C const& c) { return to_string(c.y); };
            auto s = y(n.start()) + x(n.start()) + n.path() + y(n.goal()) + x(n.goal());
            return std::hash<string>{}(s);
        }
    };
}
