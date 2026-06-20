#ifndef EVALUATION_H
#define EVALUATION_H
#include "bitboard.hpp"
#include "utils.hpp"
#include "moves.hpp"
int evaluate(Board& board,int legal_move_count);
Move search(Board& board,int depth);
#endif