//
//  Reference : Chapter 6, "Introduction to Algorithms 3rd ed", aka CLRS.
//
#pragma once

#include <vector>
#include <functional>
#include <stdexcept>
#include <algorithm>

using std::move;
using std::find_if;

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

    template<typename Iterator, typename CompareFunc>
    auto inline sift_up(Iterator first, Iterator curr, CompareFunc && compare) -> bool
    {
        auto c = curr;
        auto p = [&] { return parent(first, c); };
        auto is_needed = [&] { return c != first && !compare(*p(), *c); };

        if (!is_needed()) return false;
        for (; is_needed(); c = p())  std::swap(*p(), *c);
        return true;
    }

    //
    //  PriorityQueue
    //
    template<typename Value, typename CompareFunc>
    class PriorityQueue
    {
    public:
        using Vector = std::vector < Value >;
        using SizeType = typename Vector::size_type;
        using Iterator = typename Vector::iterator;

        PriorityQueue() = default;

        PriorityQueue(CompareFunc c)
            : _seq{}, _compare{ c }
        {   }

        PriorityQueue(std::initializer_list<Value>&& list, CompareFunc&& c)
            : _seq(std::move(list)), _compare{ std::move(c) }
        {
            if (!empty())
                build_heap(_seq.begin(), _seq.end(), _compare);
        }

        template<typename Iterator>
        PriorityQueue(Iterator first, Iterator last, CompareFunc&& c)
            : _seq(first, last), _compare{ std::move(c) }
        {
            if (!empty())
                build_heap(_seq.begin(), _seq.end(), _compare);
        }

        auto top() const -> Value const&
        {
            return _seq.front();
        }

        auto size() const -> SizeType
        {
            return _seq.size();
        }

        auto empty() const -> bool
        {
            return _seq.empty();
        }

        auto contains(Value const& value) const -> bool
        {
            return _seq.cend() != std::find(_seq.cbegin(), _seq.cend(), value);
        }

        auto push(Value const& new_val) -> void
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

        auto remove(Value const& item) -> void
        {
            auto it = std::find(_seq.begin(), _seq.end(), item);
            if (_seq.end() != it)   
                remove(it);
        }
        //
        //  O(lg n)
        //
        auto substitute(Value const& old_value, Value const& new_value) -> void
        {
            remove(old_value);
            push(new_value);
        }

        template<typename Functor>
        auto update_if(Functor && functor)
        {
            auto iterator = find_if(_seq.begin(), _seq.end(), [this](Value const& val) {
                return functor(val);
            });

            if (iterator != _seq.end() && _compare(functor.value, *iterator))
                substitute(*iterator, functor.value);
        }

        void reset()
        {
            _seq.clear();
        }

        void reset(CompareFunc && c)
        {
            _compare = move(c);
            reset();
        }

    private:
        Vector _seq;
        CompareFunc _compare;
        //
        //  Make sure vector is not empty.
        //  O(lg n)
        //
        template<typename Iterator>
        auto remove(Iterator at) -> void
        {
            std::swap(*at, *(_seq.end() - 1));
            if (!sift_up(_seq.begin(), at, _compare))
                heapify(_seq.begin(), _seq.end() - 1, at, _compare);//avoid involving the last item.
            _seq.resize(size() - 1);
        }
    };
}