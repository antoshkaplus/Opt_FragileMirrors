#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include "mirrors/board/board_i5.hpp"
#include "mirrors/solver/bs_restore_i1.hpp"
#include "mirrors/solver/bs_restore_i2.hpp"
#include "mirrors/common/solver_util.hpp"
#include "mirrors/score/score_i1.hpp"
#include "mirrors/score/score_i2.hpp"
#include "mirrors/score/score_i3.hpp"
#include "mirrors/score/score_i4.hpp"
#include "fragile_mirrors.hpp"


namespace po = boost::program_options;

struct Params {
    size_t beam_width;
    double restore_destroyed_ratio;
    size_t score_version;
    size_t board_version;
    double score_param;
};

Params parse_params(const std::vector<std::string>& cmd_args) {
    std::vector<const char*> cs;
    // Add mandatory first command line argument.
    cs.push_back("program");
    for(auto& arg: cmd_args) {
        cs.push_back(arg.c_str());
    }

    po::options_description desc;
    Params params;
    desc.add_options()
            ("beam-width", po::value<size_t>(&params.beam_width)->default_value(300))
            ("restore-destroyed-ratio", po::value<double>(&params.restore_destroyed_ratio)->default_value(0.5))
            ("score", po::value<size_t>(&params.score_version)->default_value(1))
            ("score-param", po::value<double>(&params.score_param))
            ("board", po::value<size_t>(&params.board_version)->default_value(5));
    po::variables_map vm;
    auto parsed_options = po::parse_command_line(cs.size(), cs.data(), desc);
    po::store(parsed_options, vm);
    po::notify(vm);

    std::cerr << "beam width: " << params.beam_width << '\n';
    std::cerr << "restore-destroyed-ratio: " << params.restore_destroyed_ratio << '\n';
    std::cerr << "score version: " << params.score_version << '\n';
    if (vm.contains("score-param")) {
        std::cerr << "score param: " << params.score_param << '\n';
    }
    return params;
}

template <template<template<class> class> class Solver, class Board>
std::vector<int> destroy(const Params& params, const Board& bb) {
    switch (params.score_version) {
        case 1: {
            Solver<mirrors::Score_i1> solver(params.beam_width, params.restore_destroyed_ratio);
            auto cast_history = solver.Destroy(bb);
            return mirrors::ToSolution(cast_history);
        }
        case 2: {
            Solver<mirrors::Score_i2> solver(params.beam_width, params.restore_destroyed_ratio);
            auto cast_history = solver.Destroy(bb);
            return mirrors::ToSolution(cast_history);
        }
        case 3: {
            Solver<mirrors::Score_i3> solver(params.beam_width, params.restore_destroyed_ratio);
            auto cast_history = solver.Destroy(bb);
            return mirrors::ToSolution(cast_history);
        }
        case 4: {
            mirrors::Score_i4<Board> score{params.score_param};
            Solver solver(params.beam_width, params.restore_destroyed_ratio, score);
            auto cast_history = solver.Destroy(bb);
            return mirrors::ToSolution(cast_history);
        }
        default:
            auto msg = boost::format("Unexpected score version: %1%") % params.score_version;
            throw std::runtime_error(msg.str());
    }
}

std::vector<int> FragileMirrors::destroy(const std::vector<std::string>& board) {
    auto params = parse_params(cmd_args);
    auto gg = mirrors::ToMirrorsGrid(board);
    switch (params.board_version) {
        case 5: {
            mirrors::Board_i5 bb(gg);
            return ::destroy <mirrors::BS_Restore_i2, mirrors::Board_i5> (params, bb);
        }
        default:
            auto msg = boost::format("Unexpected board version %1%") % params.board_version;
            throw std::runtime_error(msg.str());
    }
}
