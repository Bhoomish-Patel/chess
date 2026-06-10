#ifndef UTILS_HPP
#define UTILS_HPP
#include<string>
#include<vector>
#include "bitboard.hpp"
using namespace std;
vector<string> split(string fen);
string pos_to_str(int pos);
int str_to_pos(string str);
void set_bitboard(string board_string,vector<unsigned  long long int> &bitboard);
void print_bitboard(vector<unsigned  long long int> &bitboard);
int get_piece_type(int piece);
int get_piece_at_pos(vector<unsigned  long long int>bitboard,int pos);
vector<int> find_active_pos(vector<unsigned  long long int>bitboard,int piece_type);
unsigned  long long int find_slider_squares(int start_square,int end_square);
long long int perft(Board &board,int depth,int root_depth);
#endif