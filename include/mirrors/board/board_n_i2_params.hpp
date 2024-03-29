#pragma once
#include "mirrors/common/types.hpp"


namespace mirrors {

struct Board_n_i2_Params {
    hash_value_t hash {};
    cell_count_t destroyed_count_ {};
    board_size_t empty_rows_ {};
    board_size_t empty_cols_ {};

    Board_n_i2_Params() = default;
    Board_n_i2_Params(board_size_t size) {}

    cell_count_t destroyed_count() const {
        return destroyed_count_;
    }
    board_size_t empty_rows() const {
        return empty_rows_;
    }
    board_size_t empty_cols() const {
        return empty_cols_;
    }
    cell_count_t empty_lines() const {
        return empty_rows_ + empty_cols_;
    }

    template<class Board>
    bool board_empty(const Board& b) const {
        return destroyed_count_ == b.cell_count();
    }
};

}