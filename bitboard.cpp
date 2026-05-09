#include "bitboard.hpp"
#include "utils.hpp"
#include "moves.hpp"
#include<string>
#include<iostream>
using namespace std;

Board:: Board(): Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"){}
Board:: Board(string fen){
    vector<string>partitions = split(fen);
    //bitboard
    set_bitboard(partitions[0],bitboard);
    //active color
    if(partitions[1]=="w"){
        active_color = white;
    }
    else{
        active_color = black;
    }
    //castling rights
    castling_rights = 0;
    string castling_rights_str = partitions[2];
    for(int i=0;i<castling_rights_str.size();i++){
        if(castling_rights_str[i] == 'k'){
            castling_rights |= (1);
        }
        else if(castling_rights_str[i] == 'q'){
            castling_rights |= (2);
        }
        else if(castling_rights_str[i] == 'K'){
            castling_rights |= (4);
        }
        else if(castling_rights_str[i] == 'Q'){
            castling_rights |= (8);
        }
    }
    //enpassant 
    if(partitions[3]=="-"){
        en_passant = square_nb;
    }
    else{
    en_passant = str_to_pos(partitions[3]);
    }
    //halfmove clock 
    halfmove_clock = stoi(partitions[4]);
    //fullmove clock
    fullmove_number = stoi(partitions[5]);
}
int Board:: is_occupied(int pos){
    for(int i=0;i<12;i++){
        if((bitboard[i]>>pos)&1){
            return i;
        }
    }
    return -1;
}
long long int Board::generate_attack_squares(int pos,int &cnt_check,int &checking_piece_square){
    vector<unsigned long long int>temp = bitboard;
    if(active_color == 0){
        for (int i=0;i<64;i++){
            if((bitboard[W_KING]>>i) & 1){
                (bitboard[W_KING]) &=  ~(1ULL << i);
            }
        }
    }
    else{
        for (int i=0;i<64;i++){
            if((bitboard[B_KING]>>i) & 1){
                (bitboard[B_KING]) &=  ~(1ULL << i);
            }
        }
    }
    active_color^=1;
    vector<Move>attack_moves =  generate_pseudo_legal_moves();
    active_color^=1;
    bitboard = temp;
    long long int attack_squares = 0;
    for(auto i:attack_moves){
        int row = i.from/8;
        int col = i.from%8;
        int opponent_pawn = (active_color == 0) ? B_PAWN : W_PAWN;
        if (get_piece_at_pos(bitboard, i.from) == opponent_pawn){
            if(active_color == 0){
                if(col == 7){
                    if(i.from - 9 >= 0){
                        attack_squares |= (1ULL << (i.from - 9));
                        if(i.from - 9 == pos){
                            checking_piece_square = i.from;
                            cnt_check++;
                        }
                    }
                }
                else if(col == 0){
                    if(i.from - 7 >= 0){
                        attack_squares |= (1ULL << (i.from - 7));
                        if(i.from - 7 == pos){
                            checking_piece_square = i.from;
                            cnt_check ++;
                        }
                    }
                }
                else{
                    if(i.from - 9 >= 0){
                        attack_squares |= (1ULL << (i.from - 9));
                        if(i.from - 9 == pos){
                            checking_piece_square = i.from;
                            cnt_check++;
                        }
                    }
                    if(i.from - 7 >= 0){
                        attack_squares |= (1ULL << (i.from - 7));
                        if(i.from - 7 == pos){
                            checking_piece_square = i.from;
                            cnt_check++;
                        }
                    }
                }
            }
            else{
                if(col == 7){
                    if(i.from + 7 < 64){
                        attack_squares |= (1ULL << (i.from + 7));
                        if(i.from + 7 == pos){
                            checking_piece_square = i.from;
                            cnt_check ++;
                        }
                    }
                }
                else if(col == 0){
                    if(i.from + 9 < 64){
                        attack_squares |= (1ULL << (i.from + 9));
                        if(i.from + 9 == pos){
                            checking_piece_square = i.from;
                            cnt_check ++ ;
                        }
                    }
                }
                else{
                    if(i.from + 9 < 64){
                        attack_squares |= (1ULL << (i.from + 9));
                        if(i.from + 9 == pos){
                            checking_piece_square = i.from;
                            cnt_check ++ ;
                        }
                    }
                    if(i.from + 7 < 64){
                        attack_squares |= (1ULL << (i.from + 7));
                        if(i.from + 7 == pos){
                            checking_piece_square =i.from;
                            cnt_check ++;
                        }
                    }                 
                }
            }
        }
        else{
            attack_squares |= (1ULL << i.to);
            if(i.to  == pos){
                checking_piece_square = i .from;
                cnt_check++;
            }
        }
    }
    // for(int i=0;i<64;i++){
    //     if((attack_squares>>i)&1){
    //         cout<<i<<endl;
    //     }
    // }
    return attack_squares;
}

//checks:
//1.king move to an attacked squares
//2.Check - i) double check - only king moves
//          ii)single check = move  king,block the check,capture the checking piece
//3.pinned pieces
//4.other moves
vector<Move> Board:: generate_legal_moves(){
    vector<Move> all_moves = generate_pseudo_legal_moves();
    int cur_king = active_color== 0?0:6 + W_KING;
    int cur_king_pos = find_active_pos(bitboard,cur_king)[0];
    int cnt_check = 0;
    int checking_piece_square = 0;
    long long int attack_squares = generate_attack_squares(cur_king_pos,cnt_check,checking_piece_square);
    vector<Move>final_moves;
    map<int,long long int>pinned_piece_moves = find_pinned_piece_moves(bitboard,cur_king_pos);
    if(cnt_check == 0){
        for(int i=0;i<all_moves.size();i++){
            Move cur = all_moves[i];
            if(get_piece_at_pos(bitboard,cur.from) == ((active_color == 0?0:6) + W_KING)){
                if((attack_squares>>cur.to)&1){
                    continue;
                }
            }
            if(pinned_piece_moves.find(cur.from) != pinned_piece_moves.end()){
                long long int slider_moves =  pinned_piece_moves[cur.from];
                if(!((slider_moves>>cur.to)&1)){
                    continue;
                }
            }
            //TODO : enpassant discovered check
            final_moves.push_back(cur);
        }

    }
    else if(cnt_check == 1){
        unsigned long long int slider_squares = find_slider_squares(checking_piece_square,cur_king_pos);
        for(int i=0;i<all_moves.size();i++){
            Move cur = all_moves[i];
            if(get_piece_at_pos(bitboard,cur.from) == ((active_color == 0?0:6) + W_KING)){
                if(!((attack_squares>>cur.to)&1)){
                    final_moves.push_back(cur);
                    continue;
                }
            }
            if(pinned_piece_moves.find(cur.from) != pinned_piece_moves.end()){
                long long int slider_moves =  pinned_piece_moves[cur.from];
                if(!((slider_moves>>cur.to)&1)){
                    continue;
                }
            }
            if(cur.to == checking_piece_square){
                final_moves.push_back(cur);
            }
            else if((slider_squares>>cur.to)&1){
                final_moves.push_back(cur);
            }
            else{
                int cur_piece = get_piece_at_pos(bitboard,cur.from);
                if(cur_piece != W_PAWN || cur_piece != B_PAWN){
                    continue;
                }
                if(get_piece_at_pos(bitboard,checking_piece_square) == (active_color == 0?B_PAWN:W_PAWN)){
                    if(cur.to == en_passant){
                        final_moves.push_back(cur);
                    }
                }
            }
        }
    }
    else if(cnt_check >= 2){
        for(int i=0;i<all_moves.size();i++){
            Move cur = all_moves[i];
            if(get_piece_at_pos(bitboard,cur.from) != ((active_color == 0?0:6) + W_KING)){
                continue;
            }
            else{
                if((attack_squares>>cur.to)&1){
                    continue;
                }
            }
            final_moves.push_back(cur);
        }
    }
    return final_moves;
}
vector<Move> Board:: generate_pseudo_legal_moves(){
    vector<Move>moves;
    for(int i=0;i<12;i++){
        if(active_color != get_piece_type(i)){
            continue;
        }
        for(int j=0;j<64;j++){
            if((bitboard[i]>>j)&1){
                if(i%6 == 0){
                    vector<Move>king_moves = generate_king_moves(j);
                    moves.insert(moves.end(),king_moves.begin(),king_moves.end());
                }
                else if(i%6 == 1){
                    vector<Move>queen_moves = generate_queen_moves(j);
                    moves.insert(moves.end(),queen_moves.begin(),queen_moves.end());
                }
                else if(i%6 == 2){
                    vector<Move>rook_moves = generate_rook_moves(j);
                    moves.insert(moves.end(),rook_moves.begin(),rook_moves.end());
                }
                else if(i%6 == 3){
                    vector<Move>bishop_moves = generate_bishop_moves(j);
                    moves.insert(moves.end(),bishop_moves.begin(),bishop_moves.end());
                }
                else if(i%6 == 4){
                    vector<Move>knight_moves = generate_knight_moves(j);
                    moves.insert(moves.end(),knight_moves.begin(),knight_moves.end());
                }
                else if(i%6 == 5){
                    vector<Move>pawn_moves = generate_pawn_moves(j);
                    moves.insert(moves.end(),pawn_moves.begin(),pawn_moves.end());
                }
            }
        }
    }
    return moves;
}
vector<Move> Board:: generate_king_moves(int pos){
    vector<Move>moves;
    int row = pos/8;
    int col = pos%8;
    for(int dr=-1;dr<=1;dr++){
        for(int dc=-1;dc<=1;dc++){
            if(dr==0 && dc==0) continue;
            int new_row = row + dr;
            int new_col = col + dc;
            if(new_row>=0 && new_row<8 && new_col>=0 && new_col<8){
                int new_pos = new_row*8 + new_col;
                if(is_occupied(new_pos) == -1){
                    moves.push_back(Move(pos,new_pos,0));
                }
                else if(get_piece_type(is_occupied(new_pos)) != active_color){
                    moves.push_back(Move(pos,new_pos,4)); 
                }
            }
        }
    }

    if(active_color == white){
        if((castling_rights & 4) && is_occupied(f1) == -1 && is_occupied(g1) == -1){
            moves.push_back(Move(e1,g1,2)); 
        }
        if((castling_rights & 8) && is_occupied(d1) == -1 && is_occupied(c1) == -1 && is_occupied(b1) == -1){
            moves.push_back(Move(e1,c1,3)); 
        }
    }
    else{
        if((castling_rights & 1) && is_occupied(f8) == -1 && is_occupied(g8) == -1){
            moves.push_back(Move(e8,g8,2)); 
        }
        if((castling_rights & 2) && is_occupied(d8) == -1 && is_occupied(c8) == -1 && is_occupied(b8) == -1){
            moves.push_back(Move(e8,c8,3)); 
        }
    }
    return moves;
}

vector<Move> Board:: generate_queen_moves(int pos){
    vector<Move>moves;

    int row = pos/8;
    int col = pos%8;
    for(int dr=-1;dr<=1;dr++){
        for(int dc=-1;dc<=1;dc++){
            if(dr==0 && dc==0) continue;
            int new_row = row + dr;
            int new_col = col + dc;
            while(new_row>=0 && new_row<8 && new_col>=0 && new_col<8){
                int new_pos = new_row*8 + new_col;
                if(is_occupied(new_pos) == -1){
                    moves.push_back(Move(pos,new_pos,0));
                }
                else{
                    if(get_piece_type(is_occupied(new_pos)) != active_color){
                        moves.push_back(Move(pos,new_pos,4)); 
                    }
                    break;
                }
                new_row += dr;
                new_col += dc;
            }
        }
    }
    return moves;
}

vector<Move> Board:: generate_rook_moves(int pos){
    vector<Move>moves;
    for(int dc = -1;dc<=1;dc++){
        for(int dr = -1;dr<=1;dr++){
            if((dc==0 && dr==0) || (dc!=0 && dr!=0)) continue;
            int new_row = pos/8 + dr;
            int new_col = pos%8 + dc;
            while(new_row>=0 && new_row<8 && new_col>=0 && new_col<8){
                int new_pos = new_row*8 + new_col;
                if(is_occupied(new_pos) == -1){
                    moves.push_back(Move(pos,new_pos,0));
                }
                else{
                    if(get_piece_type(is_occupied(new_pos)) != active_color){
                        moves.push_back(Move(pos,new_pos,4)); 
                    }
                    break;
                }
                new_row += dr;
                new_col += dc;
            }
        }
    }
    return moves;
}
vector<Move> Board:: generate_bishop_moves(int pos){
    vector<Move>moves;
    for(int dc = -1;dc<=1;dc++){
        for(int dr = -1;dr<=1;dr++){
            if(dr==0 || dc==0) continue;
            int new_row = pos/8 + dr;
            int new_col = pos%8 + dc;
            while(new_row>=0 && new_row<8 && new_col>=0 && new_col<8){
                int new_pos = new_row*8 + new_col;
                if(is_occupied(new_pos) == -1){
                    moves.push_back(Move(pos,new_pos,0));
                }
                else{
                    if(get_piece_type(is_occupied(new_pos)) != active_color){
                        moves.push_back(Move(pos,new_pos,4)); 
                    }
                    break;
                }
                new_row += dr;
                new_col += dc;
            }
        }
    }
    return moves;
}
vector<Move> Board:: generate_knight_moves(int pos){
    vector<Move>moves;
    int row = pos/8;
    int col = pos%8;
    int dr[8] = {-2,-1,1,2,2,1,-1,-2};
    int dc[8] = {1,2,2,1,-1,-2,-2,-1};
    for(int i=0;i<8;i++){
        int new_row = row + dr[i];
        int new_col = col + dc[i];
        if(new_row>=0 && new_row<8 && new_col>=0 && new_col<8){
            int new_pos = new_row*8 + new_col;
            if(is_occupied(new_pos) == -1){
                moves.push_back(Move(pos,new_pos,0));
            }
            else if(get_piece_type(is_occupied(new_pos)) != active_color){
                moves.push_back(Move(pos,new_pos,4)); 
            }
        }
    }
    return moves;
}
vector<Move> Board::generate_pawn_moves(int pos){
    vector<Move> moves;
    int row = pos / 8;
    int col = pos % 8;

    if(active_color == white){
        if(row < 7 && is_occupied(pos + 8) == -1){
            if(row == 6){
                moves.push_back(Move(pos,pos+8,8));
                moves.push_back(Move(pos,pos+8,9));
                moves.push_back(Move(pos,pos+8,10));
                moves.push_back(Move(pos,pos+8,11));
            } else {
                moves.push_back(Move(pos,pos+8,0));
            }
        }

        if(row == 1 && is_occupied(pos+8) == -1 && is_occupied(pos+16) == -1){
            moves.push_back(Move(pos,pos+16,1));
        }
        if(row < 7 && col > 0){
            int target = pos + 7;
            if(is_occupied(target) != -1 && get_piece_type(is_occupied(target)) == black){
                if(row == 6){
                    moves.push_back(Move(pos,target,12));
                    moves.push_back(Move(pos,target,13));
                    moves.push_back(Move(pos,target,14));
                    moves.push_back(Move(pos,target,15));
                } else {
                    moves.push_back(Move(pos,target,4));
                }
            }
        }
        if(row < 7 && col < 7){
            int target = pos + 9;
            if(is_occupied(target) != -1 && get_piece_type(is_occupied(target)) == black){
                if(row == 6){
                    moves.push_back(Move(pos,target,12));
                    moves.push_back(Move(pos,target,13));
                    moves.push_back(Move(pos,target,14));
                    moves.push_back(Move(pos,target,15));
                } else {
                    moves.push_back(Move(pos,target,4));
                }
            }
        }
        if(row == 4){
            if(col > 0 && en_passant == pos + 7){
                moves.push_back(Move(pos,pos+7,5));
            }
            if(col < 7 && en_passant == pos + 9){
                moves.push_back(Move(pos,pos+9,5));
            }
        }
    }
    else{
        if(row > 0 && is_occupied(pos - 8) == -1){
            if(row == 1){
                moves.push_back(Move(pos,pos-8,8));
                moves.push_back(Move(pos,pos-8,9));
                moves.push_back(Move(pos,pos-8,10));
                moves.push_back(Move(pos,pos-8,11));
            } else {
                moves.push_back(Move(pos,pos-8,0));
            }
        }
        if(row == 6 && is_occupied(pos-8) == -1 && is_occupied(pos-16) == -1){
            moves.push_back(Move(pos,pos-16,1));
        }
        if(row > 0 && col > 0){
            int target = pos - 9;
            if(is_occupied(target) != -1 && get_piece_type(is_occupied(target)) == white){
                if(row == 1){
                    moves.push_back(Move(pos,target,12));
                    moves.push_back(Move(pos,target,13));
                    moves.push_back(Move(pos,target,14));
                    moves.push_back(Move(pos,target,15));
                } else {
                    moves.push_back(Move(pos,target,4));
                }
            }
        }
        if(row > 0 && col < 7){
            int target = pos - 7;
            if(is_occupied(target) != -1 && get_piece_type(is_occupied(target)) == white){
                if(row == 1){
                    moves.push_back(Move(pos,target,12));
                    moves.push_back(Move(pos,target,13));
                    moves.push_back(Move(pos,target,14));
                    moves.push_back(Move(pos,target,15));
                } else {
                    moves.push_back(Move(pos,target,4));
                }
            }
        }
        if(row == 3){
            if(col > 0 && en_passant == pos - 9){
                moves.push_back(Move(pos,pos-9,5));
            }
            if(col < 7 && en_passant == pos - 7){
                moves.push_back(Move(pos,pos-7,5));
            }
        }
    }
    return moves;
}