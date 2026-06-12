#include "utils.hpp"
#include "types.hpp"
#include "pieces.hpp"
#include "bitboard.hpp"
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

void set_bitboard(string board_str, array<uint64_t,12>& bitboard){
    uint8_t row = 7;
    uint8_t col = 0;

    for(uint8_t i = 0; i < board_str.size(); i++){
        if(board_str[i] == '/'){
            row--;
            col = 0;
        }
        else if(board_str[i] >= '1' && board_str[i] <= '8'){
            col += (board_str[i] - '0');
        }
        else{
            uint8_t pos = row * 8 + col;

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

void print_bitboard(array<uint64_t,12> &bitboard){
    char board[64];
    for(int i = 0; i < 64; i++) board[i] = '.';
    char pieces[12] = {
        'K','Q','R','B','N','P',
        'k','q','r','b','n','p'
    };
    for(uint8_t p = 0; p < 12; p++){
        for(uint8_t sq = 0; sq < 64; sq++){
            if(bitboard[p] & (1ULL << sq)){
                board[sq] = pieces[p];
            }
        }
    }
    for(int8_t row = 7; row >= 0; row--){
        cout << row + 1 << " ";
        for(uint8_t col = 0; col < 8; col++){
            uint8_t sq = row * 8 + col;
            cout << board[sq] << " ";
        }
        cout << endl;
    }
    cout << "  a b c d e f g h" << endl;
}

bool get_piece_type(uint8_t piece){
    if(piece/6 == 1) return black;
    else return white;
}
uint8_t get_piece_at_pos(array<uint64_t,12>bitboard,uint8_t pos){
    for(uint8_t i=0;i<12;i++){
        if((bitboard[i]>>pos)&1LL){
            return i;
        }
    }
    return -1;
}
vector<int> find_active_pos(array<uint64_t,12>bitboard,uint8_t piece_type){
    vector<int>pos;
    uint64_t bb = bitboard[piece_type];
    while(bb){
        pos.push_back(__builtin_ctzll(bb));
        bb &= bb - 1;
    }
    return pos;
}
uint64_t find_slider_squares(uint8_t start_square,uint8_t end_square){
    uint8_t s_row = start_square/8;
    uint8_t s_col = start_square%8;
    uint8_t e_row = end_square/8;
    uint8_t e_col = end_square%8;
    uint64_t slider_squares = 0;

    if(s_row == e_row){
        for(uint8_t i=min(s_col,e_col);i<=max(s_col,e_col);i++){
            slider_squares = slider_squares|(1ull<<(i+s_row*8));
        }
        return slider_squares;
    }

    if(s_col == e_col){
        for(uint8_t i=min(s_row,e_row);i<=max(s_row,e_row);i++){
            slider_squares = slider_squares|(1ull<<(s_col + i*8));
        }
        return slider_squares;
    }

    int8_t r_inc = -1 ,c_inc = -1;
    if(s_col < e_col ){
        c_inc = 1;
    }
    if(s_row < e_row){
        r_inc = 1;
    }
    slider_squares = slider_squares|(1ull<<(start_square));
    while(s_row != e_row && s_col != e_col){
        slider_squares = slider_squares|(1ull<<(s_col + 8*s_row));
        s_col += c_inc;
        s_row +=r_inc;
    }
    return slider_squares;
    
}
uint64_t perft(Board &board,int depth,int root_depth){
    if(depth == 0){
        return 1;
    }
    vector<Move> moves;
    board.generate_legal_moves(moves);
    if(depth == 1){
        return (int)moves.size();
    }
    uint64_t nodes = 0;
    for(Move m : moves){
        board.make_move(m);
        uint64_t child = perft(board,depth-1,root_depth);
        // if(depth == root_depth){
        //     cout << pos_to_str(m.from)<< pos_to_str(m.to) << ": " << child << endl;
        // }
        nodes += child;
        board.unmake_move();
    }
    return nodes;
}