#pragma once

#include <limits>
#include <algorithm>
#include <vector>
#include <map>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <string>

using std::max;
using std::make_pair;
using std::min;
using std::abs;
using std::hypot;
using std::vector;
using std::map;
using std::function;
using std::unordered_map;
using std::unordered_set;
using std::string;
using std::to_string;
using std::hash;

namespace search
{
    constexpr auto huge()
    {
        return 10'000;
    }
    constexpr auto cost()
    {
        return 1;
    }

    struct Cell
    {
        int row, col;

        friend auto operator== (Cell l, Cell r)
        {
            return l.row == r.row && l.col == r.col;
        }
        friend auto operator!= (Cell l, Cell r)
        {
            return !(l == r);
        }

        auto to_string() const
        {
            using std::to_string;
            return string{ "[r=" + to_string(row) + ",c=" + to_string(col) + "]" };
        }

        struct Hasher
        {
            auto operator()(Cell c) const
            {
                return hash<string>{}(c.to_string());
            }
        };
    };

    const static map< char, function< Cell(Cell) >> DIRECTIONS
    {
        { '1', [](Cell c) { return Cell{ c.row - 1, c.col - 1 }; } },
        { '2', [](Cell c) { return Cell{ c.row - 1, c.col - 0 }; } },
        { '3', [](Cell c) { return Cell{ c.row - 1, c.col + 1 }; } },
        { '4', [](Cell c) { return Cell{ c.row - 0, c.col - 1 }; } },
        { '5', [](Cell c) { return Cell{ c.row + 0, c.col + 1 }; } },
        { '6', [](Cell c) { return Cell{ c.row + 1, c.col - 1 }; } },
        { '7', [](Cell c) { return Cell{ c.row + 1, c.col + 0 }; } },
        { '8', [](Cell c) { return Cell{ c.row + 1, c.col + 1 }; } }
    };

    using Cells = unordered_set<Cell, Cell::Hasher>;

    struct LpState
    {
        Cell cell; int g, r, h; bool bad;
        auto to_string() const
        {
            using std::to_string;
            return "{" + cell.to_string() + "|g:" + to_string(g) + "|r:" + to_string(r) + "|h:" + to_string(h) + "|b:" + (bad ? "t" : "f") + "}";
        }
        friend auto operator==(LpState const& l, LpState const& r)
        {
            return l.cell == r.cell && l.g == r.g && l.r == r.r  && l.h == r.h && l.bad == r.bad;
        }
    };

    class Matrix
    {
    public:
        auto rows() const
        {
            return _data.size();
        }
        auto cols() const
        {
            return _data.front().size();
        }

        template<typename Func>
        auto each_cell(Func && func)
        {
            for (auto r = 0; r != rows(); ++r)
                for (auto c = 0; c != cols(); ++c)
                    func(Cell{ r, c });
        }
        template<typename Func>
        auto each_cell(Func && func) const
        {
            for (auto r = 0; r != rows(); ++r)
                for (auto c = 0; c != cols(); ++c)
                    func(Cell{ r, c });
        }

        auto at(Cell c) -> LpState&
        {
            return{ _data[c.row][c.col] };
        }
        auto at(Cell c) const -> LpState const&
        {
            return{ _data[c.row][c.col] };
        }

        auto to_string() const
        {
            string result;
            for (auto r = 0; r != rows(); ++r)
            {
                for (auto c = 0; c != cols(); ++c)
                    result += at({ r, c }).to_string();
                result += "+++";
            }
            return result;
        }
        //
        //  Ctor
        //
        Matrix(unsigned rows, unsigned cols)
            : _data{ rows, vector<LpState>(cols) }
        {
            auto init_state = [this](Cell c) { at(c).cell = c, at(c).g = at(c).r = huge(); };
            each_cell(init_state);
        }

    private:
        vector<vector<LpState>> _data;
    };

    const static unordered_map < string, function<int(Cell, Cell)> > HEURISTICS
    {
        {
            "manhattan",
            [](Cell l, Cell r) { return max(abs(l.row - r.row), abs(l.col - r.col)); }
        },

        {
            "euclidean",
            [](Cell l, Cell r) { return static_cast<int>(round(hypot(abs(l.row - r.row), abs(l.col - r.col)))); }
        }
    };
}
