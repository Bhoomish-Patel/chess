#ifndef UTILS_HPP
#define UTILS_HPP
#include<string>
#include<vector>
#include<array>
#include "bitboard.hpp"
using namespace std;
vector<string> split(string fen);
string pos_to_str(int pos);
int str_to_pos(string str);
void set_bitboard(string board_str, array<uint64_t,12>& bitboard);
void print_bitboard(array<uint64_t,12> &bitboard);
bool get_piece_type(uint8_t piece);
uint8_t get_piece_at_pos(array<uint64_t,12>bitboard,uint8_t pos);
vector<int> find_active_pos(array<uint64_t,12>bitboard,uint8_t piece_type);
uint64_t find_slider_squares(uint8_t start_square,uint8_t end_square);
uint64_t perft(Board &board,int depth,int root_depth);
#endif