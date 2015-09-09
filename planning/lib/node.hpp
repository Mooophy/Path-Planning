#pragma once
#include <string>
#include <map>
#include <functional>
#include <vector>

using std::string;
using std::to_string;
using std::function;

namespace search
{
    using Size = int;
    //
    //  Coordinate used in Node
    //
    struct Coordinate 
    { 
        Size y, x;
        auto to_string() const -> string
        {
            return "[" + std::to_string(y) + "," + std::to_string(x) + "]";
        }
    };

    inline auto operator==(Coordinate lhs, Coordinate rhs) -> bool
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }
    //
    //  Function map for 8 directions
    //
    using Functions = std::map< char, function< Coordinate(Coordinate) >>;
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
    //  Storing start, goal coordinate and path moved so far as well as a few methods.    
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
        //
        //  Defualt Ctor
        //
        Node() = default;
        //
        //  Ctor
        //
        Node(Path const& path, Coordinate start, Coordinate goal)
            : _path{ path }, _start{ start }, _goal{ goal }
        {   }
        //
        //  Copy Ctor
        //
        Node(Node const& other)
            : _path{ other._path }, _start{ other._start }, _goal{ other._goal }
        {   }
        //
        //  Return path has moved so far
        //
        auto path() const& -> Path const& 
        {
            return _path; 
        }
        //
        //  Return start coordinate
        //
        auto start() const& -> Coordinate
        {
            return _start;
        }
        //
        //  Return goal coordinate
        //
        auto goal() const& -> Coordinate
        {
            return _goal;
        }
        //
        //  Return current coordinate for this node
        //
        auto coordinate() const -> Coordinate
        {
            Coordinate c = _start;
            for (auto direction : _path)
                c = GOES.at(direction)(c);
            return c;
        }
        //  
        //  Format:
        //      [start.to_string][path][goal.to_string]
        //
        auto to_string() const -> string
        {
            return _start.to_string() +  "[" + _path + "]" + _goal.to_string();
        }
        //
        //  Implementing hash function with std::hash<string>
        //
        auto hash() const -> size_t
        {
            return std::hash<string>{}(to_string());
        }
        //
        //  Require ValidateFunc implements interface operator()(Node)
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
}//end of search namespace

//
//  Openning std to implement std::hash<Node> specialization
//
namespace std
{
    using namespace search;

    template<>
    struct hash<Node>
    {
        //[start.to_string][path][goal.to_string]
        auto operator()(Node const& node) const -> size_t
        {
            return node.hash();
        }
    };
}//end of std namespace