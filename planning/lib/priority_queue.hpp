//
//  This file implemented a custom priority queue data structure. Compared with std::priority_queue, 
//  this one allows to access and change any item held in it by openning public interface to its 
//  underlying data structure. This feature is essential for "uniform cost search" and "A* with strict
//  expanded list".
//
//  Reference : Chapter 6, "Introduction to Algorithms 3rd ed", aka CLRS.
//

#pragma once
#include <vector>
#include <functional>
#include <stdexcept>
#include <algorithm>

namespace mai
{
    namespace container
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
        //  maintain heap's peroperties with a bottom up way
        //
        template<typename Iterator, typename CompareFunc>
        auto heapify(Iterator first, Iterator last, Iterator curr, CompareFunc && compare) -> void
        {
            while (true)
            {
                auto left = mai::container::left_child(first, last, curr);
                auto right = mai::container::right_child(first, last, curr);

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
        template<typename Iterator, typename CompareFunc >
        auto inline build_heap(Iterator first, Iterator last, CompareFunc && compare) -> void
        {
            auto size = last - first;
            for (auto curr = first + size / 2 - 1; /* */; --curr)
            {
                mai::container::heapify(first, last, curr, compare);
                if (curr == first) return;
            }
        }

        //
        //  PriorityQueue
        //
        template<typename T>
        class PriorityQueue
        {
        public:
            using ValueType = T;
            using Vector = std::vector < T > ;
            using SizeType = typename Vector::size_type;
            using Iterator = typename Vector::iterator;
            using CompareFunc = std::function < bool(ValueType const&, ValueType const&) > ;

            explicit PriorityQueue(CompareFunc && c)
                : seq_{}, compare_{ std::move(c) }
            {}

            explicit PriorityQueue(std::initializer_list<ValueType>&& list, CompareFunc&& c)
                : seq_(std::move(list)), compare_{ std::move(c) }
            {
                mai::container::build_heap(seq_.begin(), seq_.end(), compare_);
            }

            template<typename Iterator>
            PriorityQueue(Iterator first, Iterator last, CompareFunc&& c)
                : seq_(first, last), compare_{ std::move(c) }
            {
                mai::container::build_heap(seq_.begin(), seq_.end(), compare_);
            }

            auto data() -> Vector& { return seq_; }
            auto top() const -> ValueType const&{ return seq_.front(); }
            auto size() const -> SizeType { return seq_.size(); }
            auto empty() const -> bool{ return seq_.empty(); }

            auto push(ValueType const& new_val) -> void
            {
                //! find the right place for added
                seq_.resize(size() + 1);
                auto curr = seq_.end() - 1;
                for (; curr > seq_.begin() && compare_(new_val, *parent(seq_.begin(), curr)); curr = parent(seq_.begin(), curr))
                    *curr = *parent(seq_.begin(), curr);

                //! insert
                *curr = new_val;
            }

            auto pop() -> void
            {
                if (empty())
                    throw std::underflow_error{ "underflow." };

                seq_.front() = seq_.back();
                seq_.resize(seq_.size() - 1);
                mai::container::heapify(seq_.begin(), seq_.end(), seq_.begin(), compare_);
            }

        private:
            Vector seq_;
            CompareFunc compare_;
        };
    }
}
