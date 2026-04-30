#ifndef UTILS_HPP
#define UTILS_HPP
#include<string>
#include<vector>
using namespace std;
vector<string> split(string fen);
string pos_to_str(int pos);
int str_to_pos(string str);
void set_bitboard(string board_string,vector<unsigned long long int> &bitboard);
void print_bitboard(vector<unsigned long long int> &bitboard);
int get_piece_type(int piece);
#endif