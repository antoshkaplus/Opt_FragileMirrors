#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include "mirrors/board/board_n_i5.hpp"
#include "mirrors/solver/bs_restore_i3.hpp"
#include "mirrors/common/solver_util.hpp"
#include "mirrors/score/score_i8.hpp"
#include "mirrors/param/empty_lines_param.hpp"
#include "legacy/score.hpp"
#include "fragile_mirrors.hpp"


namespace po = boost::program_options;

struct Params {
    size_t solver_version;
    size_t beam_width;
    size_t score_version;
    double score_empty_param;
    double score_even_param;
};

Params parse_params(const std::vector<std::string>& cmd_args, mirrors::board_size_t board_size) {
    std::vector<const char*> cs;
    // Add mandatory first command line argument.
    cs.push_back("program");
    for(auto& arg: cmd_args) {
        cs.push_back(arg.c_str());
    }

    po::options_description desc;
    Params params;
    std::string score_empty_param;
    desc.add_options()
            ("solver-version", po::value(&params.solver_version)->default_value(3))
            ("beam-width", po::value<size_t>(&params.beam_width)->default_value(300))
            ("score", po::value<size_t>(&params.score_version)->default_value(8))
            ("score-empty-param", po::value<std::string>(&score_empty_param))
            ("score-even-param", po::value<double>(&params.score_even_param)->default_value(0));
    po::variables_map vm;
    auto parsed_options = po::parse_command_line(cs.size(), cs.data(), desc);
    po::store(parsed_options, vm);
    po::notify(vm);

    std::cerr << "solver version: " << params.solver_version << '\n';
    std::cerr << "beam width: " << params.beam_width << '\n';
    std::cerr << "score version: " << params.score_version << '\n';
    const auto kMinBoardSize = 50;
    if (vm.contains("score-empty-param")) {
        if (score_empty_param == "legacy") {
            params.score_empty_param = legacy::EMPTY_LINES_PARAM[board_size-kMinBoardSize];
        } else {
            params.score_empty_param = std::stod(score_empty_param);
        }
    } else {
        params.score_empty_param = mirrors::EMPTY_LINES_PARAM[board_size-kMinBoardSize];
    }
    std::cerr << "score empty param: " << params.score_empty_param << '\n';
    std::cerr << "score even param: " << params.score_even_param << '\n';
    return params;
}

namespace mirrors {

inline OriginGrid<Grid<Mirror>> MakeBoardGrid(const Grid<Mirror> &mirrors_param) {
    OriginGrid<Grid<Mirror>> bg{{-1, -1}, mirrors_param.size() + 2};
    bg.grid().fill(Mirror::Border);
    for (auto row = 0; row < mirrors_param.size(); ++row) {
        for (auto col = 0; col < mirrors_param.size(); ++col) {
            bg[{row, col}] = mirrors_param(row, col);
        }
    }
    return bg;
}

template<template <template<class> class, class, class> class Solver, class Board>
std::vector<int> destroy(const Params& params, OriginGrid<Grid<Mirror>>&ggg) {
    switch (params.score_version) {
        case 8: {
            mirrors::Score_i8<Board_n_i5_Params> score(params.score_empty_param,
                                                       params.score_even_param);
            Solver<mirrors::Score_i8, Board, Board_n_i5_Params> solver(params.beam_width, ggg, score);
            auto cast_history = solver.Destroy();
            return mirrors::ToSolution(cast_history);
        }
        default: {
            auto msg = boost::format("Unexpected score version: %1%") % params.score_version;
            throw std::runtime_error(msg.str());
        }
    }
}

}


std::vector<int> FragileMirrors::destroy(const std::vector<std::string>& board) {
    auto params = parse_params(cmd_args, board.size());
    auto gg = mirrors::ToMirrorsGrid(board);
    auto ggg = mirrors::MakeBoardGrid(gg);
    switch (params.solver_version) {
        case 3:
            return mirrors::destroy<mirrors::BS_Restore_i3, mirrors::Board_n_i5>(params, ggg);
        default:
            auto msg = boost::format("Unexpected solver version: %1%") % params.score_version;
            throw std::runtime_error(msg.str());
    }
}
