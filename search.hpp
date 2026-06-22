#ifndef SEARCH_H
#define SEARCH_H
#include "bitboard.hpp"
#include "utils.hpp"
#include "moves.hpp"
#include<unordered_map>
Move search(Board &board,int depth);
int alpha_beta(Board& board,int depth,int alpha,int beta,int ply);
int quiscence(Board& board,int alpha,int beta);
vector<int> get_score(Board& board,const vector<Move>&moves);
void order_moves(Board& board,vector<Move>&moves,Move tt_move,int ply);
enum TTFlag{TT_EXACT,TT_LOWER,TT_UPPER};
struct TTEntry{
    int depth;
    int value;
    uint8_t flag;
    Move best;
};
inline std::unordered_map<uint64_t,TTEntry>transposition_table;
constexpr int MAX_PLY = 128;
inline Move killer_moves[MAX_PLY][2];
#endif
