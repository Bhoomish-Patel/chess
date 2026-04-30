#ifndef BITBOARD_H
#define BITBOARD_H
#include<vector>
#include<string>
#include "types.hpp"
#include "pieces.hpp"
#include "moves.hpp"
using namespace std;
class Board{
    public:
        vector<unsigned long long int>bitboard;
        int castling_rights;
        int en_passant;
        bool active_color;
        int halfmove_clock;
        int fullmove_number;
        Board();
        Board(string fen);
        vector<Move> generate_pseudo_legal_moves();
        int is_occupied(int pos);
        vector<Move> generate_king_moves(int pos);
        vector<Move> generate_queen_moves(int pos);
        vector<Move> generate_rook_moves(int pos);
        vector<Move> generate_bishop_moves(int pos);
        vector<Move> generate_knight_moves(int pos);
        vector<Move> generate_pawn_moves(int pos);
        vector<Move> generate_legal_moves();
        long long int generate_attack_squares();
};
#endif