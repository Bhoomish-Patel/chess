#ifndef SEARCH_H
#define SEARCH_H
#include "bitboard.hpp"
#include "utils.hpp"
#include "moves.hpp"
Move search(Board &board,int depth);
int negamax(Board& board,int depth);
#endif