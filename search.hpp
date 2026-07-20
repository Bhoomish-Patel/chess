#ifndef SEARCH_H
#define SEARCH_H

#include "bitboard.hpp"
#include "moves.hpp"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <vector>

enum TTFlag { TT_EXACT, TT_LOWER, TT_UPPER };

struct TTEntry {
    int depth;
    int value;
    uint8_t flag;
    Move best;
};

constexpr int MAX_PLY = 128;

struct SearchLimits {
    int max_depth = MAX_PLY - 1;
    bool has_deadline = false;
    std::chrono::steady_clock::time_point deadline;
};

struct SearchInfo {
    int depth = 0;
    int score = 0;
    uint64_t nodes = 0;
    uint64_t elapsed_ms = 0;
    Move best_move = Move(square_nb, square_nb, 0xFF);
};

struct SearchResult {
    Move best_move = Move(square_nb, square_nb, 0xFF);
    int score = 0;
    int completed_depth = 0;
    uint64_t nodes = 0;
    uint64_t elapsed_ms = 0;
    bool has_move = false;
};

using SearchInfoCallback = std::function<void(const SearchInfo&)>;

SearchResult iterative_search(Board board,
                              const SearchLimits& limits,
                              const std::atomic<bool>& stop_requested,
                              SearchInfoCallback on_info = {});

// Fixed-depth compatibility wrapper.
Move search(Board& board, int depth);

std::vector<int> get_score(Board& board, const std::vector<Move>& moves);
void order_moves(Board& board, std::vector<Move>& moves, Move tt_move, int ply);
void clear_search_state();

inline std::unordered_map<uint64_t, TTEntry> transposition_table;
inline Move killer_moves[MAX_PLY][2];

#endif
