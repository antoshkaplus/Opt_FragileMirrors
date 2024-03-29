#pragma once
#include <tuple>
#include <mirrors/common/types.hpp>

namespace mirrors {

template <class Board_>
struct Score_i1 {
    using Board = Board_;
    using Value = std::tuple<cell_count_t, cell_count_t>;

    std::tuple<cell_count_t, cell_count_t> operator()(const Board& b) const {
        return {b.destroyed_count(), b.empty_cols() + b.empty_rows()};
    }
};

}