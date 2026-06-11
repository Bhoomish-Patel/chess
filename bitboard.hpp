#ifndef BITBOARD_H
#define BITBOARD_H
#include<vector>
#include<string>
#include<map>
#include<array>
#include<string>
#include<iostream>
#include<random>
#include "types.hpp"
#include "pieces.hpp"
#include "moves.hpp"
using namespace std;
struct Undo{
    uint8_t from,to,flags;
    uint8_t moved_piece,captured_piece,promotion_piece;
    uint8_t castling_rights,en_passant,halfmove_clock;
    uint64_t zobrist_hash;
};
class Board{
    public:
        array<uint64_t,12>bitboard;
        array<uint8_t,64>piece_at;
        uint8_t castling_rights;
        uint8_t en_passant;
        bool active_color;
        uint8_t halfmove_clock;
        uint32_t fullmove_number;
        uint8_t checks;
        Board();
        Board(string fen);
        vector<Move> generate_pseudo_legal_moves();
        int is_occupied(int pos);
        void zobrist_init();
        vector<Undo> undo_history;
        uint64_t zobrist_hash;
        vector<uint64_t>zobrist_keys;
        vector<Move> generate_king_moves(uint8_t pos);
        vector<Move> generate_queen_moves(uint8_t pos);
        vector<Move> generate_rook_moves(uint8_t pos);
        vector<Move> generate_bishop_moves(uint8_t pos);
        vector<Move> generate_knight_moves(uint8_t pos);
        vector<Move> generate_pawn_moves(int pos);
        vector<Move> generate_legal_moves();
        void make_move(Move m);
        void unmake_move();
        uint64_t generate_attack_squares(uint8_t pos,uint8_t&cnt_check,uint8_t &checking_piece_square);
        string to_fen();
        bool is_stalemate();
        bool is_checkmate();
        bool is_fifty_move_draw();
        bool is_three_fold_repetition();
        map<uint8_t,uint64_t> find_pinned_piece_moves(uint8_t king_pos);
};
#endif