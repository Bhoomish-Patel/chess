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
long long int Board::generate_attack_squares(){
    for(int i=0;i<12;i++){
        if(get_piece_type(i) != active_color){
            //pawn
            //king
            //queen
            //rook
            //knight
            //bishop
        }
    }
}
vector<Move> Board:: generate_legal_moves(){
    vector<Move> all_moves = generate_pseudo_legal_moves();
    long long int attack_squares = generate_attack_squares();
    return all_moves;
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
    // noraml moves 
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