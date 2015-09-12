//
//  Functors
//
#pragma once

#include <cmath>
#include <algorithm>
#include <vector>
#include "node.hpp"

using std::max;
using std::abs;
using std::hypot;
using std::find;

namespace search
{
    template<typename Node>
    inline auto dy(Node const& node) -> Integer
    {
        return abs(node.goal().y - node.state().y);
    }
    template<typename Node>
    inline auto dx(Node const& node) -> Integer
    {
        return abs(node.goal().x - node.state().x);
    }

    template<typename Node>
    struct ManhattanDistance
    {
        auto operator()(Node const& node) const -> Integer
        {
            return max(dy(node), dx(node));
        }
    };

    template<typename Node>
    struct EuclideanDistance
    {
        auto operator()(Node const& node) const -> Integer
        {
            return static_cast<Integer>(round(hypot(dy(node), dx(node))));
        }
    };

    template<typename Node>
    struct Cost
    {
        auto operator()(Node const& node) const -> Integer
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
