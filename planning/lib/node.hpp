//
//  Node
//
#pragma once

#include <string>
#include <map>
#include <functional>
#include <vector>
#include <algorithm>

using std::string;
using std::to_string;
using std::function;
using std::max;
using std::min;
using std::vector;

namespace search
{
    using Integer = int;
    //
    //  State
    //      That is coordinate.
    //
    struct State
    {
        Integer y, x;
        auto to_string() const -> string
        {
            return "[" + std::to_string(y) + "," + std::to_string(x) + "]";
        }

        auto is_within_grid(State s1, State s2) const -> bool
        {
            auto ymax = max(s1.y, s2.y);
            auto ymin = min(s1.y, s2.y);
            auto xmax = max(s1.x, s2.x);
            auto xmin = min(s1.x, s2.x);

            return x >= xmin && x <= xmax && y >= ymin && y <= ymax;
        }
    };
    inline auto operator==(State lhs, State rhs) -> bool
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }
    inline auto operator!=(State lhs, State rhs) -> bool
    {
        return !(lhs == rhs);
    }
    //
    //  Function map for 8 directions
    //
    using Functions = std::map< char, function< State(State) >>;
    struct Goes : public Functions
    {
        Goes()
        {
            (*this)['1'] = [](State c) -> State { return{ c.y - 1, c.x - 1 }; };
            (*this)['2'] = [](State c) -> State { return{ c.y - 1, c.x - 0 }; };
            (*this)['3'] = [](State c) -> State { return{ c.y - 1, c.x + 1 }; };
            (*this)['4'] = [](State c) -> State { return{ c.y - 0, c.x - 1 }; };
            (*this)['5'] = [](State c) -> State { return{ c.y + 0, c.x + 1 }; };
            (*this)['6'] = [](State c) -> State { return{ c.y + 1, c.x - 1 }; };
            (*this)['7'] = [](State c) -> State { return{ c.y + 1, c.x - 0 }; };
            (*this)['8'] = [](State c) -> State { return{ c.y + 1, c.x + 1 }; };
        }
    } const GOES;
    //
    //  Class Node
    //  Storing start, goal State and path moved so far as well as a few methods.    
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
        Node(Path const& path, State start, State goal)
            : _path{ path }, _start{ start }, _goal{ goal }
        {   }
        //
        //  Copy Ctor
        //
        Node(Node const& other)
            : _path{ other._path }, _start{ other._start }, _goal{ other._goal }
        {   }
        //
        //  Return the path this node has traveled.
        //
        auto path() const& -> Path const&
        {
            return _path;
        }
        //
        //  Return start State
        //
        auto start() const& -> State
        {
            return _start;
        }
        //
        //  Return goal State
        //
        auto goal() const& -> State
        {
            return _goal;
        }
        //
        //  Return current State for this node
        //
        auto state() const -> State
        {
            State c = _start;
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
            return _start.to_string() + "[" + _path + "]" + _goal.to_string();
        }
        //
        //  Implementing hash function using std::hash<string>
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
            for (auto direction = '1'; direction <= '8'; ++direction)
            {
                auto child = Node{ _path + direction, _start, _goal };
                if (validate(child))
                    children.push_back(child);
            }
            return children;
        }

    private:

        Path _path;
        State _start;
        State _goal;
    };

    //
    //  Decode path to states
    //
    auto inline static path_to_states(State start, string const& path) -> vector<State>
    {
        vector<State> states;
        for (auto direction : path)
            states.push_back(start = GOES.at(direction)(start));
        return states;
    }
}//end of search namespace

//
//  Openning std to implement std::hash<Node> specialization
//
namespace std
{
    using namespace search;

    template<>
    struct hash<State>
    {
        auto operator()(State c) const -> size_t
        {
            return std::hash<string>{}(c.to_string());
        }
    };

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
