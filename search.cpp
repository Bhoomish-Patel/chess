#include "search.hpp"

#include "evaluation.hpp"

#include <algorithm>
#include <limits>
#include <utility>

using namespace std;

namespace {

constexpr int INF = 1000000000;

struct SearchContext {
    const SearchLimits& limits;
    const atomic<bool>& stop_requested;
    uint64_t nodes = 0;
    bool aborted = false;
};

bool should_stop(SearchContext& context) {
    if (context.stop_requested.load(memory_order_relaxed)) {
        context.aborted = true;
        return true;
    }
    if (context.limits.has_deadline &&
        chrono::steady_clock::now() >= context.limits.deadline) {
        context.aborted = true;
        return true;
    }
    return false;
}

int quiescence(Board& board, int alpha, int beta, SearchContext& context);

int alpha_beta(Board& board, int depth, int alpha, int beta, int ply,
               SearchContext& context) {
    context.nodes++;
    if (should_stop(context)) return 0;
    if (depth == 0) return quiescence(board, alpha, beta, context);

    int alpha_orig = alpha;
    Move hash_move(square_nb, square_nb, 0xFF);
    auto it = transposition_table.find(board.zobrist_hash);
    if (it != transposition_table.end()) {
        const TTEntry& entry = it->second;
        hash_move = entry.best;
        if (entry.depth >= depth) {
            if (entry.flag == TT_EXACT) return entry.value;
            if (entry.flag == TT_LOWER && entry.value > alpha) alpha = entry.value;
            if (entry.flag == TT_UPPER && entry.value < beta) beta = entry.value;
            if (alpha >= beta) return entry.value;
        }
    }

    vector<Move> legal_moves;
    int count = board.generate_legal_moves(legal_moves);
    if (count == 0) return evaluate(board, count);

    int best_score = -INF;
    Move best(square_nb, square_nb, 0xFF);
    order_moves(board, legal_moves, hash_move, ply);
    for (const Move& move : legal_moves) {
        board.make_move(move);
        int score = -alpha_beta(board, depth - 1, -beta, -alpha, ply + 1, context);
        board.unmake_move();
        if (context.aborted) return 0;

        if (score > best_score) {
            best_score = score;
            best = move;
        }
        if (score > alpha) alpha = score;
        if (score >= beta) {
            if (move.flags <= 3 && ply < MAX_PLY &&
                !(killer_moves[ply][0].from == move.from &&
                  killer_moves[ply][0].to == move.to &&
                  killer_moves[ply][0].flags == move.flags)) {
                killer_moves[ply][1] = killer_moves[ply][0];
                killer_moves[ply][0] = move;
            }
            break;
        }
    }

    uint8_t flag = TT_EXACT;
    if (best_score <= alpha_orig) flag = TT_UPPER;
    else if (best_score >= beta) flag = TT_LOWER;
    transposition_table[board.zobrist_hash] = TTEntry{depth, best_score, flag, best};
    return best_score;
}

int quiescence(Board& board, int alpha, int beta, SearchContext& context) {
    context.nodes++;
    if (should_stop(context)) return 0;

    vector<Move> legal_moves;
    int count = board.generate_legal_moves(legal_moves);
    if (count == 0) return evaluate(board, count);

    bool in_check = board.checks > 0;
    int best_value = in_check ? -INF : evaluate(board, count);
    if (!in_check) {
        if (best_value >= beta) return best_value;
        if (best_value > alpha) alpha = best_value;
    }

    for (const Move& move : legal_moves) {
        if (in_check || move.flags == 4 || move.flags == 5 || move.flags >= 12) {
            board.make_move(move);
            int score = -quiescence(board, -beta, -alpha, context);
            board.unmake_move();
            if (context.aborted) return 0;
            if (score >= beta) return score;
            if (score > best_value) best_value = score;
            if (score > alpha) alpha = score;
        }
    }
    return best_value;
}

}  // namespace

vector<int> get_score(Board& board, const vector<Move>& moves) {
    static constexpr int MVV_LVA[6][6] = {
        {0, 50, 40, 30, 20, 10}, {0, 51, 41, 31, 21, 11},
        {0, 52, 42, 32, 22, 12}, {0, 53, 43, 33, 23, 13},
        {0, 54, 44, 34, 24, 14}, {0, 55, 45, 35, 25, 15}
    };
    vector<int> score(moves.size());
    for (size_t i = 0; i < moves.size(); i++) {
        const Move& move = moves[i];
        if (move.flags <= 3) score[i] = 0;
        else if (move.flags == 4)
            score[i] = MVV_LVA[board.piece_at[move.from] % 6][board.piece_at[move.to] % 6];
        else if (move.flags == 5) score[i] = 15;
        else if (move.flags <= 11) score[i] = 56 + (move.flags - 8);
        else
            score[i] = 56 + (move.flags - 8) +
                       MVV_LVA[board.piece_at[move.from] % 6][board.piece_at[move.to] % 6];
    }
    return score;
}

void order_moves(Board& board, vector<Move>& moves, Move tt_move, int ply) {
    vector<int> score = get_score(board, moves);
    for (size_t i = 0; i < moves.size(); i++) {
        if (moves[i].from == tt_move.from && moves[i].to == tt_move.to &&
            moves[i].flags == tt_move.flags) {
            score[i] = 1000000;
        } else if (ply < MAX_PLY && moves[i].from == killer_moves[ply][0].from &&
                   moves[i].to == killer_moves[ply][0].to &&
                   moves[i].flags == killer_moves[ply][0].flags) {
            score[i] = 9;
        } else if (ply < MAX_PLY && moves[i].from == killer_moves[ply][1].from &&
                   moves[i].to == killer_moves[ply][1].to &&
                   moves[i].flags == killer_moves[ply][1].flags) {
            score[i] = 8;
        }
    }
    vector<pair<int, Move>> ranked;
    ranked.reserve(moves.size());
    for (size_t i = 0; i < moves.size(); i++) ranked.push_back({score[i], moves[i]});
    stable_sort(ranked.begin(), ranked.end(),
                [](const auto& a, const auto& b) { return a.first > b.first; });
    for (size_t i = 0; i < moves.size(); i++) moves[i] = ranked[i].second;
}

SearchResult iterative_search(Board board, const SearchLimits& limits,
                              const atomic<bool>& stop_requested,
                              SearchInfoCallback on_info) {
    const auto started = chrono::steady_clock::now();
    SearchContext context{limits, stop_requested};
    SearchResult result;

    vector<Move> root_moves;
    board.generate_legal_moves(root_moves);
    if (root_moves.empty()) return result;

    result.best_move = root_moves.front();
    result.has_move = true;

    const int max_depth = max(1, min(limits.max_depth, MAX_PLY - 1));
    for (int depth = 1; depth <= max_depth; depth++) {
        if (should_stop(context)) break;

        order_moves(board, root_moves, result.best_move, 0);
        int best_score = -INF;
        Move iteration_best = root_moves.front();
        for (const Move& move : root_moves) {
            board.make_move(move);
            int score = -alpha_beta(board, depth - 1, -INF, INF, 1, context);
            board.unmake_move();
            if (context.aborted) break;
            if (score > best_score) {
                best_score = score;
                iteration_best = move;
            }
        }
        if (context.aborted) break;

        result.best_move = iteration_best;
        result.score = best_score;
        result.completed_depth = depth;
        result.nodes = context.nodes;
        result.elapsed_ms = chrono::duration_cast<chrono::milliseconds>(
                                chrono::steady_clock::now() - started)
                                .count();
        if (on_info) {
            on_info(SearchInfo{depth, best_score, result.nodes, result.elapsed_ms,
                               iteration_best});
        }
    }

    result.nodes = context.nodes;
    result.elapsed_ms = chrono::duration_cast<chrono::milliseconds>(
                            chrono::steady_clock::now() - started)
                            .count();
    return result;
}

Move search(Board& board, int depth) {
    atomic<bool> stop_requested{false};
    SearchLimits limits;
    limits.max_depth = depth;
    return iterative_search(board, limits, stop_requested).best_move;
}

void clear_search_state() {
    transposition_table.clear();
    for (auto& ply : killer_moves) {
        ply[0] = Move();
        ply[1] = Move();
    }
}
