#include "utils.hpp"
#include "types.hpp"
#include<iostream>
vector<string> split(string fen){
    vector<string> partitions;
    string temp = "";
    for(char c : fen){
        if(c == ' '){
            partitions.push_back(temp);
            temp = "";
        } else {
            temp += c;
        }
    }
    partitions.push_back(temp);
    return partitions;
}

string pos_to_str(int pos){
    string str = "";
    str += ('a' + pos%8);
    str += ('1' + pos/8);
    return str;
}

int str_to_pos(string str){
    int pos = 0;
    pos += str[0] -'a';
    pos += (str[1]  - '0' - 1)*8;
    return pos;
}   

void set_bitboard(string board_str, vector<unsigned long long int>& bitboard){
    bitboard.assign(12, 0);

    int row = 7;
    int col = 0;

    for(int i = 0; i < board_str.size(); i++){
        if(board_str[i] == '/'){
            row--;
            col = 0;
        }
        else if(board_str[i] >= '1' && board_str[i] <= '8'){
            col += (board_str[i] - '0');
        }
        else{
            int pos = row * 8 + col;

            if(board_str[i] == 'K') bitboard[0] |= (1LL << pos);
            else if(board_str[i] == 'Q') bitboard[1] |= (1LL << pos);
            else if(board_str[i] == 'R') bitboard[2] |= (1LL << pos);
            else if(board_str[i] == 'B') bitboard[3] |= (1LL << pos);
            else if(board_str[i] == 'N') bitboard[4] |= (1LL << pos);
            else if(board_str[i] == 'P') bitboard[5] |= (1LL << pos);
            else if(board_str[i] == 'k') bitboard[6] |= (1LL << pos);
            else if(board_str[i] == 'q') bitboard[7] |= (1LL << pos);
            else if(board_str[i] == 'r') bitboard[8] |= (1LL << pos);
            else if(board_str[i] == 'b') bitboard[9] |= (1LL << pos);
            else if(board_str[i] == 'n') bitboard[10] |= (1LL << pos);
            else if(board_str[i] == 'p') bitboard[11] |= (1LL << pos);
            col++; 
        }
    }
}

void print_bitboard(vector<unsigned long long int> &bitboard){
    char board[64];
    for(int i = 0; i < 64; i++) board[i] = '.';
    char pieces[12] = {
        'K','Q','R','B','N','P',
        'k','q','r','b','n','p'
    };
    for(int p = 0; p < 12; p++){
        for(int sq = 0; sq < 64; sq++){
            if(bitboard[p] & (1ULL << sq)){
                board[sq] = pieces[p];
            }
        }
    }
    for(int row = 7; row >= 0; row--){
        cout << row + 1 << " ";
        for(int col = 0; col < 8; col++){
            int sq = row * 8 + col;
            cout << board[sq] << " ";
        }
        cout << endl;
    }
    cout << "  a b c d e f g h" << endl;
}

int get_piece_type(int piece){
    if(piece/6 == 1) return black;
    else return white;
}