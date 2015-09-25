#pragma once

#include "helpers.hpp"
#include "priority_queue.hpp"

namespace search
{
    namespace ds
    {
        class Dstar
        {
        public:
            //
            //  data members
            //
            Matrix matrix;
            Cell const start, goal;
            function<int(Cell, Cell)> const hfunc;
            PriorityQueue < Cell, function<bool(Cell, Cell)> > q;
            int km;
            //
            //  statistics
            //
            size_t max_q_size;
            Cells expansions;
            string path;
            long long run_time;
        };

    }
}
