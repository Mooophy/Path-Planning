//
//  Functors
//
#pragma once

#include <cmath>
#include <algorithm>

using std::max;
using std::abs;
using std::hypot;
using std::find;

namespace search
{
    template<typename Node>
    inline auto dy(Node const& node) -> Size
    {
        return abs(node.goal().y - node.coordinate().y);
    }
    template<typename Node>
    inline auto dx(Node const& node) -> Size
    {
        return abs(node.goal().x - node.coordinate().x);
    }

    template<typename Node> 
    struct Heuristc{};

    template<typename Node>
    struct ManhattanDistance : public Heuristc<Node>
    {
        auto operator()(Node const& node) const -> Size
        {
            return max(dy(node), dx(node));
        }
    };

    template<typename Node>
    struct EuclideanDistance : public Heuristc<Node>
    {
        auto operator()(Node const& node) const -> Size
        {
            return static_cast<Size>(round(hypot(dy(node), dx(node))));
        }
    };
    
    template<typename Node>
    struct Cost
    {
        auto operator()(Node const& node) const -> Size
        {
            return node.path().size();
        }
    };
    
    template<typename Node, typename Hfunc, typename Cfunc = Cost<Node>>
    struct Less
    {
        Hfunc h;
        Cfunc c;
        auto operator()(Node const& lhs, Node const& rhs) const -> bool
        {
            return h(lhs) + c(lhs) < h(rhs) + c(rhs);
        }
    };
}//end of namespace
