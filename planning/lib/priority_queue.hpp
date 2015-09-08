//
//  Reference : Chapter 6, "Introduction to Algorithms 3rd ed", aka CLRS.
//

#pragma once
#include <vector>
#include <functional>
#include <stdexcept>
#include <algorithm>

namespace search
{
    //
    //  parent
    //  O(1)
    //
    template<typename Iterator> 
    auto inline parent(Iterator first, Iterator it) -> Iterator
    {
        return first + (it - first - 1) / 2;
    }
    //
    //  left_child
    //  O(1)
    //
    template<typename Iterator>
    auto inline left_child(Iterator first, Iterator last, Iterator it) -> Iterator
    {
        auto size = last - first;
        auto offset = 2 * (it - first) + 1;
        return offset > size ? last : first + offset;
    }
    //
    //  right_child
    //  O(1)
    //
    template<typename Iterator>
    auto inline right_child(Iterator first, Iterator last, Iterator it) -> Iterator
    {
        auto left = left_child(first, last, it);
        return left != last ? left + 1 : last;
    }
    //
    //  heapify
    //  O(lg n)
    //  maintain heap's peroperties with a float down way
    //
    template<typename Iterator, typename CompareFunc>
    auto heapify(Iterator first, Iterator last, Iterator curr, CompareFunc && compare) -> void
    {
        while (true)
        {
            auto left = left_child(first, last, curr);
            auto right = right_child(first, last, curr);

            //! find max or min amoung curr, left and right children, depending on the CommpareFunc passed in.
            auto max_min = (left != last && compare(*left, *curr)) ? left : curr;
            if (right != last && compare(*right, *max_min))	max_min = right;

            //!	exchange.
            if (max_min != curr)
            {
                std::swap(*max_min, *curr);
                curr = max_min;
            }
            else
            {
                return;
            }
        }
    }
    //
    //  build_heap
    //  O(n)
    //
    template<typename Iterator, typename CompareFunc>
    auto inline build_heap(Iterator first, Iterator last, CompareFunc && compare) -> void
    {
        auto size = last - first;
        for (auto curr = first + size / 2 - 1; /* */; --curr)
        {
            heapify(first, last, curr, compare);
            if (curr == first) return;
        }
    }

    template<template Iterator, typename CompareFunc>
    auto inline sift_up(Iterator const first, Iterator curr, CompareFunc && compare) -> void
    {
        auto c = curr;
        auto p = [&]{ return parent(first, c); };
        for( ; c != first && !compare(*p(), *c); c = p())
            std::swap(*p(), *c);
    }

    //
    //  PriorityQueue
    //
    template<typename T, typename CompareFunc>
    class PriorityQueue
    {
    public:
        using ValueType = T;
        using Vector = std::vector < T > ;
        using SizeType = typename Vector::size_type;
        using Iterator = typename Vector::iterator;

        PriorityQueue(CompareFunc c)
            : _seq{ }, _compare{ c }
        {   }

        PriorityQueue(std::initializer_list<ValueType>&& list, CompareFunc&& c)
            : _seq(std::move(list)), _compare{ std::move(c) }
        {
            if(!empty())
                build_heap(_seq.begin(), _seq.end(), _compare);
        }

        template<typename Iterator>
        PriorityQueue(Iterator first, Iterator last, CompareFunc&& c)
            : _seq(first, last), _compare{ std::move(c) }
        {
            if (!empty())
                build_heap(_seq.begin(), _seq.end(), _compare);
        }

        auto data() -> Vector const& { return _seq; }
        auto top() const -> ValueType const&{ return _seq.front(); }
        auto size() const -> SizeType { return _seq.size(); }
        auto empty() const -> bool{ return _seq.empty(); }

        auto push(ValueType const& new_val) -> void
        {
            // find the right place for new_val
            _seq.resize(size() + 1);
            auto curr = _seq.end() - 1;
            for (; curr > _seq.begin() && _compare(new_val, *parent(_seq.begin(), curr)); curr = parent(_seq.begin(), curr))
                *curr = *parent(_seq.begin(), curr);

            // insert
            *curr = new_val;
        }

        auto pop() -> void
        {
            if (empty())
                throw std::underflow_error{ "underflow." };
            _seq.front() = _seq.back();
            _seq.resize(_seq.size() - 1);
            heapify(_seq.begin(), _seq.end(), _seq.begin(), _compare);
        }
        //
        //  Make sure vector is not empty.
        //  O(lg n)
        //
        template<typename Iterator>
        auto remove(Iterator at) -> void
        {
            std::wap(*at, *(_seq.end() - 1));
            //not finished.
        }
        //
        //  O(lg n)
        //
        template<typename Iterator, typename ValueType>
        auto substitue(Iterator at, ValueType value) -> void
        {
            remove(at);
            push(value);
        }

    private:
        Vector _seq;
        CompareFunc _compare;
    };
}
