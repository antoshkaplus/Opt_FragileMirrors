#pragma once
#include <vector>
#include "common/types.hpp"
#include "common/position.hpp"
#include "common/grid.hpp"


namespace mirrors {

inline std::vector<std::string> ReadBoard(std::istream& cin) {
    int n;
    cin >> n;
    std::vector<std::string> str_board(n);
    for (int i = 0; i < n; i++) {
        cin >> str_board[i];
    }
    return str_board;
}

inline Grid<Mirror> ToMirrorsGrid(const std::vector<std::string>& board) {
    std::function<Mirror(char)> convert = ConvertLetterMirrorToMirror;
    auto any = board[0][0];
    if (any == '\\' || any == '/') {
        convert = ConvertChMirrorToMirror;
    }
    Grid<Mirror> res(board.size());
    for (auto row = 0; row < board.size(); ++row) {
        for (auto col = 0; col < board.size(); ++col) {
            res(row, col) = convert(board[row][col]);
        }
    }
    return res;
}

inline std::vector<int> ToSolution(const std::vector<Position>& ps) {
    std::vector<int> res;
    res.reserve(2*ps.size());
    for (auto& p : ps) {
        res.push_back(p.row);
        res.push_back(p.col);
    }
    return res;
}

inline void PrintSolution(std::ostream& cout, const std::vector<Position>& sol) {
    int n = 2 * sol.size();
    cout << n << std::endl;
    for (auto& p : sol) {
        cout << p.row << std::endl
             << p.col << std::endl;
    }
}

inline void PrintSolution(std::ostream& cout, const std::vector<int>& sol) {
    cout << sol.size() << std::endl;
    for (auto i : sol) {
        cout << i << std::endl;
    }
}

}