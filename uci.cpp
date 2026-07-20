#include "uci.hpp"

#include "bitboard.hpp"
#include "search.hpp"
#include "utils.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cctype>
#include <cstdint>
#include <iostream>
#include <limits>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

using namespace std;

namespace {

vector<string> tokenize(const string& line) {
    istringstream input(line);
    vector<string> tokens;
    string token;
    while (input >> token) tokens.push_back(token);
    return tokens;
}

bool parse_nonnegative(const string& text, int64_t& value) {
    try {
        size_t used = 0;
        unsigned long long parsed = stoull(text, &used);
        if (used != text.size() || parsed > static_cast<unsigned long long>(numeric_limits<int64_t>::max()))
            return false;
        value = static_cast<int64_t>(parsed);
        return true;
    } catch (...) {
        return false;
    }
}

bool valid_square_text(const string& square) {
    return square.size() == 2 && square[0] >= 'a' && square[0] <= 'h' &&
           square[1] >= '1' && square[1] <= '8';
}

bool valid_fen_fields(const vector<string>& fields, string& error) {
    if (fields.size() != 6) {
        error = "FEN must contain six fields";
        return false;
    }

    int ranks = 1;
    int files = 0;
    int white_kings = 0;
    int black_kings = 0;
    for (char c : fields[0]) {
        if (c == '/') {
            if (files != 8) {
                error = "FEN rank does not contain eight squares";
                return false;
            }
            ranks++;
            files = 0;
        } else if (c >= '1' && c <= '8') {
            files += c - '0';
        } else if (string("KQRBNPkqrbnp").find(c) != string::npos) {
            files++;
            if (c == 'K') white_kings++;
            if (c == 'k') black_kings++;
        } else {
            error = "FEN board contains an invalid character";
            return false;
        }
        if (files > 8) {
            error = "FEN rank contains too many squares";
            return false;
        }
    }
    if (ranks != 8 || files != 8) {
        error = "FEN board must contain eight complete ranks";
        return false;
    }
    if (white_kings != 1 || black_kings != 1) {
        error = "FEN board must contain exactly one king of each color";
        return false;
    }
    if (fields[1] != "w" && fields[1] != "b") {
        error = "FEN active color must be w or b";
        return false;
    }
    if (fields[2] != "-") {
        bool seen[256] = {};
        for (unsigned char c : fields[2]) {
            if (string("KQkq").find(c) == string::npos || seen[c]) {
                error = "FEN castling rights are invalid";
                return false;
            }
            seen[c] = true;
        }
    }
    if (fields[3] != "-" &&
        (!valid_square_text(fields[3]) ||
         (fields[3][1] != '3' && fields[3][1] != '6'))) {
        error = "FEN en-passant square is invalid";
        return false;
    }
    int64_t halfmove = 0;
    int64_t fullmove = 0;
    if (!parse_nonnegative(fields[4], halfmove) || halfmove > 255) {
        error = "FEN halfmove clock is invalid";
        return false;
    }
    if (!parse_nonnegative(fields[5], fullmove) || fullmove < 1 ||
        fullmove > numeric_limits<uint32_t>::max()) {
        error = "FEN fullmove number is invalid";
        return false;
    }
    return true;
}

char promotion_character(const Move& move) {
    static constexpr char promotions[4] = {'n', 'b', 'r', 'q'};
    if (move.flags >= 8 && move.flags <= 15) return promotions[(move.flags - 8) % 4];
    return '\0';
}

string move_to_uci(const Move& move) {
    if (move.from >= square_nb || move.to >= square_nb) return "0000";
    string text = pos_to_str(move.from) + pos_to_str(move.to);
    char promotion = promotion_character(move);
    if (promotion) text += promotion;
    return text;
}

optional<Move> parse_legal_move(Board& board, const string& text) {
    if ((text.size() != 4 && text.size() != 5) ||
        !valid_square_text(text.substr(0, 2)) ||
        !valid_square_text(text.substr(2, 2))) {
        return nullopt;
    }
    int from = str_to_pos(text.substr(0, 2));
    int to = str_to_pos(text.substr(2, 2));
    vector<Move> legal_moves;
    board.generate_legal_moves(legal_moves);
    for (const Move& move : legal_moves) {
        if (move.from != from || move.to != to) continue;
        char promotion = promotion_character(move);
        if ((!promotion && text.size() == 4) ||
            (promotion && text.size() == 5 &&
             static_cast<char>(tolower(static_cast<unsigned char>(text[4]))) == promotion)) {
            return move;
        }
    }
    return nullopt;
}

class UciEngine {
public:
    ~UciEngine() { stop_search(false); }

    int loop() {
        string line;
        while (getline(cin, line)) {
            vector<string> tokens = tokenize(line);
            if (tokens.empty()) continue;
            const string& command = tokens[0];
            if (command == "uci") {
                send("id name ChessEngine");
                send("id author Bhoomish");
                send("uciok");
            } else if (command == "isready") {
                send("readyok");
            } else if (command == "ucinewgame") {
                stop_search(false);
                clear_search_state();
            } else if (command == "position") {
                set_position(tokens);
            } else if (command == "go") {
                start_search(tokens);
            } else if (command == "stop") {
                stop_search(true);
            } else if (command == "quit") {
                stop_search(false);
                return 0;
            }
        }
        stop_search(false);
        return 0;
    }

private:
    Board board_;
    thread search_thread_;
    atomic<bool> stop_requested_{false};
    atomic<bool> publish_result_{false};
    mutex output_mutex_;

    void send(const string& message) {
        lock_guard<mutex> lock(output_mutex_);
        cout << message << endl;
    }

    void stop_search(bool publish_result) {
        if (!search_thread_.joinable()) return;
        publish_result_.store(publish_result, memory_order_relaxed);
        stop_requested_.store(true, memory_order_relaxed);
        search_thread_.join();
    }

    void set_position(const vector<string>& tokens) {
        stop_search(false);
        if (tokens.size() < 2) {
            send("info string position requires startpos or fen");
            return;
        }

        Board candidate;
        size_t index = 0;
        if (tokens[1] == "startpos") {
            candidate = Board();
            index = 2;
        } else if (tokens[1] == "fen") {
            if (tokens.size() < 8) {
                send("info string invalid position: FEN must contain six fields");
                return;
            }
            vector<string> fields(tokens.begin() + 2, tokens.begin() + 8);
            string error;
            if (!valid_fen_fields(fields, error)) {
                send("info string invalid position: " + error);
                return;
            }
            string fen;
            for (size_t i = 0; i < fields.size(); i++) {
                if (i) fen += ' ';
                fen += fields[i];
            }
            candidate = Board(fen);
            index = 8;
        } else {
            send("info string position requires startpos or fen");
            return;
        }

        if (index < tokens.size()) {
            if (tokens[index] != "moves") {
                send("info string unexpected token in position command: " + tokens[index]);
                return;
            }
            index++;
        }
        for (; index < tokens.size(); index++) {
            optional<Move> move = parse_legal_move(candidate, tokens[index]);
            if (!move) {
                send("info string illegal move in position command: " + tokens[index]);
                return;
            }
            candidate.make_move(*move);
        }
        board_ = std::move(candidate);
    }

    void start_search(const vector<string>& tokens) {
        stop_search(false);

        optional<int64_t> depth;
        optional<int64_t> movetime;
        optional<int64_t> wtime, btime, winc, binc, movestogo;
        bool infinite = false;
        bool unsupported = false;

        for (size_t i = 1; i < tokens.size(); i++) {
            const string& token = tokens[i];
            if (token == "infinite") {
                infinite = true;
                continue;
            }
            if (token == "ponder" || token == "nodes" || token == "mate" ||
                token == "searchmoves") {
                send("info string unsupported go token: " + token);
                unsupported = true;
                break;
            }

            optional<int64_t>* destination = nullptr;
            if (token == "depth") destination = &depth;
            else if (token == "movetime") destination = &movetime;
            else if (token == "wtime") destination = &wtime;
            else if (token == "btime") destination = &btime;
            else if (token == "winc") destination = &winc;
            else if (token == "binc") destination = &binc;
            else if (token == "movestogo") destination = &movestogo;
            else {
                send("info string unsupported go token: " + token);
                unsupported = true;
                break;
            }
            if (i + 1 >= tokens.size()) {
                send("info string missing value for go token: " + token);
                unsupported = true;
                break;
            }
            int64_t value = 0;
            if (!parse_nonnegative(tokens[++i], value)) {
                send("info string invalid value for go token: " + token);
                unsupported = true;
                break;
            }
            *destination = value;
        }

        SearchLimits limits;
        bool has_clock = board_.active_color == white ? wtime.has_value() : btime.has_value();
        bool has_time_limit = movetime.has_value() || has_clock;
        if (depth) limits.max_depth = static_cast<int>(clamp<int64_t>(*depth, 1, MAX_PLY - 1));
        else if (has_time_limit || infinite) limits.max_depth = MAX_PLY - 1;
        else limits.max_depth = 6;
        if (unsupported && !depth && !has_time_limit) limits.max_depth = 6;

        int64_t budget_ms = 0;
        if (movetime) {
            budget_ms = max<int64_t>(1, *movetime);
        } else if (has_clock) {
            int64_t remaining = board_.active_color == white ? *wtime : *btime;
            int64_t increment = board_.active_color == white ? winc.value_or(0) : binc.value_or(0);
            int64_t moves = max<int64_t>(1, movestogo.value_or(30));
            int64_t safety = max<int64_t>(10, remaining / 50);
            int64_t usable = max<int64_t>(1, remaining - safety);
            int64_t calculated = remaining / moves + increment * 4 / 5;
            budget_ms = clamp<int64_t>(calculated, 1, usable);
        }
        if (budget_ms > 0) {
            limits.has_deadline = true;
            limits.deadline = chrono::steady_clock::now() + chrono::milliseconds(budget_ms);
        }

        Board search_board = board_;
        stop_requested_.store(false, memory_order_relaxed);
        publish_result_.store(true, memory_order_relaxed);
        search_thread_ = thread([this, search_board = std::move(search_board), limits]() mutable {
            SearchResult result = iterative_search(
                std::move(search_board), limits, stop_requested_,
                [this](const SearchInfo& info) {
                    if (!publish_result_.load(memory_order_relaxed)) return;
                    uint64_t nps = info.elapsed_ms == 0
                                       ? info.nodes * 1000
                                       : info.nodes * 1000 / info.elapsed_ms;
                    send("info depth " + to_string(info.depth) + " nodes " +
                         to_string(info.nodes) + " time " + to_string(info.elapsed_ms) +
                         " nps " + to_string(nps) + " pv " + move_to_uci(info.best_move));
                });
            if (publish_result_.load(memory_order_relaxed)) {
                send("bestmove " + (result.has_move ? move_to_uci(result.best_move) : "0000"));
            }
        });
    }
};

}  // namespace

int run_uci() {
    UciEngine engine;
    return engine.loop();
}
