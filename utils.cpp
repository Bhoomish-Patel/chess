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

void set_bitboard(string board_str, vector<unsigned  long long int>& bitboard){
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

void print_bitboard(vector<unsigned  long long int> &bitboard){
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
int get_piece_at_pos(vector<unsigned  long long int>bitboard,int pos){
    for(int i=0;i<12;i++){
        if((bitboard[i]>>pos)&1LL){
            return i;
        }
    }
    return -1;
}
vector<int> find_active_pos(vector<unsigned  long long int>bitboard,int piece_type){
    vector<int>pos;
    for(int i=0;i<64;i++){
        if((bitboard[piece_type]>>i)&1){
            pos.push_back(i);
        }
    }
    return pos;
}
unsigned  long long int find_slider_squares(int start_square,int end_square){
    int s_row = start_square/8;
    int s_col = start_square%8;
    int e_row = end_square/8;
    int e_col = end_square%8;
    unsigned  long long int slider_squares = 0;

    if(s_row == e_row){
        for(int i=min(s_col,e_col);i<=max(s_col,e_col);i++){
            slider_squares = slider_squares|(1ull<<(i+s_row*8));
        }
        return slider_squares;
    }

    if(s_col == e_col){
        for(int i=min(s_row,e_row);i<=max(s_row,e_row);i++){
            slider_squares = slider_squares|(1ull<<(s_col + i*8));
        }
        return slider_squares;
    }

    int r_inc = -1 ,c_inc = -1;
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
map<int,unsigned  long long int> find_pinned_piece_moves(vector<unsigned  long long int>bitboard,int king_pos){
    vector<int> r_incs = {-1,0,1};
    vector<int> c_incs = {-1,0,1};
    int r_king = king_pos/8;
    int c_king = king_pos%8;
    int cur_king = get_piece_at_pos(bitboard,king_pos);
    map<int,unsigned long long int>ans;
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            int r_inc = r_incs[i];
            int c_inc = c_incs[j];
            int r_cur = r_king + r_inc;
            int c_cur = c_king + c_inc;
            if(r_inc == 0 && c_inc ==0)continue;
            int attack_piece_pos = -1 ,pinned_piece_pos = -1;
            while(r_cur<8 && r_cur>=0 && c_cur>=0 && c_cur<8){
                int cur_piece = get_piece_at_pos(bitboard,r_cur*8 + c_cur);
                if(cur_piece!=-1){
                    if(pinned_piece_pos == -1 && get_piece_type(cur_piece) == get_piece_type(cur_king)){
                        pinned_piece_pos = r_cur*8 + c_cur;
                    }
                    else if(pinned_piece_pos!=-1 &&  get_piece_type(cur_piece) == get_piece_type(cur_king)){
                        break;
                    }
                    else{
                        attack_piece_pos = r_cur*8 + c_cur;
                        break;
                    }
                }
                r_cur += r_inc;
                c_cur += c_inc;
            }
            if(pinned_piece_pos == -1 || attack_piece_pos == -1){
                continue;
            }
            int piece_type = get_piece_at_pos(bitboard,attack_piece_pos);
            int active_color = get_piece_type(cur_king);
            // cout<<pos_to_str(attack_piece_pos)<<" "<<pos_to_str(pinned_piece_pos)<<endl;
            // cout<<active_color<<" "<<piece_type<<endl;
            // cout<<r_inc<<" "<<c_inc<<endl;
            // cout<<piece_type<<" "<<(active_color ==0 ? B_QUEEN : W_QUEEN)<<endl;
            if(r_inc == 0 || c_inc ==0){
                if((piece_type !=  (active_color == 0 ? B_ROOK : W_ROOK)) && (piece_type != (active_color == 0 ? B_QUEEN : W_QUEEN))){
                    continue;
                }
            }
            else{
                if((piece_type != (active_color == 0 ? B_BISHOP : W_BISHOP)) && (piece_type != (active_color == 0 ? B_QUEEN : W_QUEEN))){
                    continue;
                }
            }
            unsigned long long int slider_moves = find_slider_squares(king_pos,attack_piece_pos);
            slider_moves |=(1ull<<attack_piece_pos);
            ans[pinned_piece_pos] = slider_moves;
        }
    }
    return ans;
}

long long int perft(Board &board,int depth,int root_depth){
    if(depth == 0){
        return 1;
    }
    vector<Move> moves = board.generate_legal_moves();
    long long int nodes = 0;
    for(Move m : moves){
        board.make_move(m);
        long long int child = perft(board,depth-1,root_depth);
        if(depth == root_depth){
            cout << pos_to_str(m.from)<< pos_to_str(m.to) << ": " << child << endl;
        }
        nodes += child;
        board.unmake_move(m);
    }
    return nodes;
}