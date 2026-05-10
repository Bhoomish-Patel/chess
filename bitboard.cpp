#include "bitboard.hpp"
#include "utils.hpp"
#include "moves.hpp"
#include<string>
#include<iostream>
#include<random>
using namespace std;

Board:: Board(): Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"){}
Board:: Board(string fen){
    vector<string>partitions = split(fen);
    set_bitboard(partitions[0],bitboard);
    if(partitions[1]=="w"){
        active_color = white;
    }
    else{
        active_color = black;
    }
    castling_rights = 0;
    string castling_rights_str = partitions[2];
    for(int i=0;i<castling_rights_str.size();i++){
        if(castling_rights_str[i] == 'k'){
            castling_rights |= (4);
        }
        else if(castling_rights_str[i] == 'q'){
            castling_rights |= (8);
        }
        else if(castling_rights_str[i] == 'K'){
            castling_rights |= (1);
        }
        else if(castling_rights_str[i] == 'Q'){
            castling_rights |= (2);
        }
    }
    if(partitions[3]=="-"){
        en_passant = square_nb;
    }
    else{
    en_passant = str_to_pos(partitions[3]);
    }
    halfmove_clock = stoi(partitions[4]);
    fullmove_number = stoi(partitions[5]);
    zobrist_init();
}
int Board:: is_occupied(int pos){
    for(int i=0;i<12;i++){
        if((bitboard[i]>>pos)&1){
            return i;
        }
    }
    return -1;
}
unsigned long long int Board::generate_attack_squares(int pos,int &cnt_check,int &checking_piece_square){
    unsigned long  long int attack_squares = 0;
    vector<unsigned long long int>temp = bitboard;
    int cur_king = active_color== white?0:6 + W_KING;
    for(int i=0;i<64;i++){
        if((bitboard[cur_king]>>i)&1){
            bitboard[cur_king] = bitboard[cur_king] & (~(1ULL<<i));
            break;
        }
    }
    int opponent_pawn = (active_color == white) ? B_PAWN : W_PAWN;
    for(int i=0;i<64;i++){
        if((bitboard[opponent_pawn]>>i)&1){
            int row = i/8;
            int col = i%8;
            if(active_color == white){
                if(col == 7){
                    attack_squares |= (1ULL << (i - 9));
                    if(i - 9 == pos){
                        checking_piece_square = i;
                        cnt_check++;
                    }
                }
                else if(col == 0){
                    attack_squares |= (1ULL << (i - 7));
                    if(i - 7 == pos){
                        checking_piece_square = i;
                        cnt_check++;
                    }
                }
                else{
                    attack_squares |= (1ULL << (i - 9));
                    attack_squares |= (1ULL << (i - 7));
                    if(i - 9 == pos){
                        checking_piece_square = i;
                        cnt_check++;
                    }
                    else if(i - 7 == pos){
                        checking_piece_square = i;
                        cnt_check++;
                    }
                }
            }
            else{
                if(col == 7){
                    attack_squares |= (1ULL << (i + 7));
                    if(i + 7 == pos){
                        checking_piece_square = i;
                        cnt_check++;
                    }
                }
                else if(col == 0){
                    attack_squares |= (1ULL << (i + 9));
                    if(i + 9 == pos){
                        checking_piece_square = i;
                        cnt_check++;
                    }
                }
                else{
                    attack_squares |= (1ULL << (i + 9));
                    attack_squares |= (1ULL << (i + 7));
                    if(i + 9 == pos){
                        checking_piece_square = i;
                        cnt_check++;
                    }
                    else if(i + 7 == pos){
                        checking_piece_square = i;
                        cnt_check++;
                    }
                }
            }
        }
    }
    int opponent_knight = (active_color == white) ? B_KNIGHT : W_KNIGHT;
     for(int i=0;i<64;i++){
        if((bitboard[opponent_knight]>>i)&1){
            int row = i/8;
            int col = i%8;
            int dr[8] = {-2,-1,1,2,2,1,-1,-2};
            int dc[8] = {1,2,2,1,-1,-2,-2,-1};
            for(int j=0;j<8;j++){
                int new_row = row + dr[j];
                int new_col = col + dc[j];
                if(new_row>=0 && new_row<8 && new_col>=0 && new_col<8){
                    int new_pos = new_row*8 + new_col;
                    attack_squares |= (1ULL << new_pos);
                    if(new_pos == pos){
                        checking_piece_square = i;
                        cnt_check++;
                    }
                }
            }
        }
    }
    int opponent_bishop = (active_color == white) ? B_BISHOP : W_BISHOP;
    for(int i=0;i<64;i++){
        if((bitboard[opponent_bishop]>>i)&1){
            int row = i/8;
            int col = i%8;
            for(int dr=-1;dr<=1;dr++){
                for(int dc=-1;dc<=1;dc++){
                    if(dr==0 || dc==0) continue;
                    int new_row = row + dr;
                    int new_col = col + dc;
                    while(new_row>=0 && new_row<8 && new_col>=0 && new_col<8){
                        int new_pos = new_row*8 + new_col;
                        attack_squares |= (1ULL << new_pos);
                        if(new_pos == pos){
                            checking_piece_square = i;
                            cnt_check++;
                        }
                        if(is_occupied(new_pos) != -1){
                            break;
                        }
                        new_row += dr;
                        new_col += dc;
                    }
                }
            }
        }
    }
    int opponent_rook = (active_color == white) ? B_ROOK : W_ROOK;
    for(int i=0;i<64;i++){
        if((bitboard[opponent_rook]>>i)&1){
            int row = i/8;
            int col = i%8;
            for(int dr=-1;dr<=1;dr++){
                for(int dc=-1;dc<=1;dc++){
                    if((dr==0 && dc==0) || (dr!=0 && dc!=0)) continue;
                    int new_row = row + dr;
                    int new_col = col + dc;
                    while(new_row>=0 && new_row<8 && new_col>=0 && new_col<8){
                        int new_pos = new_row*8 + new_col;
                        attack_squares |= (1ULL << new_pos);
                        if(new_pos == pos){
                            checking_piece_square = i;
                            cnt_check++;
                        }
                        if(is_occupied(new_pos) != -1){
                            break;
                        }
                        new_row += dr;
                        new_col += dc;
                    }
                }
            }
        }
    }
    int opponent_queen = (active_color == white) ? B_QUEEN : W_QUEEN;
    for(int i=0;i<64;i++){
        if((bitboard[opponent_queen]>>i)&1){
            int row = i/8;
            int col = i%8;
            for(int dr=-1;dr<=1;dr++){
                for(int dc=-1;dc<=1;dc++){
                    if(dr==0 && dc==0) continue;
                    int new_row = row + dr;
                    int new_col = col + dc;
                    while(new_row>=0 && new_row<8 && new_col>=0 && new_col<8){
                        int new_pos = new_row*8 + new_col;
                        attack_squares |= (1ULL << new_pos);
                        if(new_pos == pos){
                            checking_piece_square = i;
                            cnt_check++;
                        }
                        if(is_occupied(new_pos) != -1){
                            break;
                        }
                        new_row += dr;
                        new_col += dc;
                    }
                }
            }
        }
    }
    int opponent_king = (active_color == white) ? B_KING : W_KING;
    for(int i=0;i<64;i++){
        if((bitboard[opponent_king]>>i)&1){
            int row = i/8;
            int col = i%8;
            for(int dr=-1;dr<=1;dr++){
                for(int dc=-1;dc<=1;dc++){
                    if(dr==0 && dc==0) continue;
                    int new_row = row + dr;
                    int new_col = col + dc;
                    if(new_row>=0 && new_row<8 && new_col>=0 && new_col<8){
                        int new_pos = new_row*8 + new_col;
                        attack_squares |= (1ULL << new_pos);
                        if(new_pos == pos){
                            checking_piece_square = i;
                            cnt_check++;
                        }
                    }
                }
            }
            break;
        }
    }
    bitboard = temp;
    return attack_squares;
}

vector<Move> Board:: generate_legal_moves(){
    vector<Move> all_moves = generate_pseudo_legal_moves();
    int cur_king = active_color== white?0:6 + W_KING;
    int cur_king_pos = find_active_pos(bitboard,cur_king)[0];
    int cnt_check = 0;
    int checking_piece_square = 0;
    unsigned long long int attack_squares = generate_attack_squares(cur_king_pos,cnt_check,checking_piece_square);
    vector<Move>final_moves;
    map<int,unsigned long long int>pinned_piece_moves = find_pinned_piece_moves(bitboard,cur_king_pos);
    checks = cnt_check;
    if(cnt_check == 0){
        for(int i=0;i<all_moves.size();i++){
            Move cur = all_moves[i];
            if(get_piece_at_pos(bitboard,cur.from) == ((active_color == white?0:6) + W_KING)){
                if((attack_squares>>cur.to)&1){
                    continue;
                }
            }
            if(pinned_piece_moves.find(cur.from) != pinned_piece_moves.end()){
                unsigned long long int slider_moves =  pinned_piece_moves[cur.from];
                if((!((slider_moves>>cur.to)&1))){
                    continue;
                }
            }
            if(cur.flags == 5){
                int r_cur_king = cur_king_pos/8;
                int c_cur_king = cur_king_pos%8;
                if(r_cur_king== cur.from/8){
                   bool is_discovered_check = false;
                    for(int c = c_cur_king+1;c<8;c++){
                        if(c == cur.to%8 || c == cur.from%8){
                            continue;
                        }
                        int piece_at_pos = get_piece_at_pos(bitboard,c + 8*r_cur_king);
                        if(piece_at_pos != -1){
                            if(get_piece_type(piece_at_pos) == active_color){
                                break;
                            }
                            else if(piece_at_pos == (active_color == white?6:0) + W_ROOK || piece_at_pos == (active_color == white?6:0) + W_QUEEN){
                                is_discovered_check = true;
                                break;
                            }
                            else{
                                break;
                            }
                        }
                    }
                    for(int c= c_cur_king-1;c>=0;c--){
                        if(c == cur.to%8 || c == cur.from%8){
                            continue;
                        }
                        int piece_at_pos = get_piece_at_pos(bitboard,c + 8*r_cur_king);
                        if(piece_at_pos != -1){
                            if(get_piece_type(piece_at_pos) == active_color){
                                break;
                            }
                            else if(piece_at_pos == (active_color == white?6:0) + W_ROOK || piece_at_pos == (active_color == white?6:0) + W_QUEEN){
                                is_discovered_check = true;
                                break;
                            }
                            else{
                                break;
                            }
                        }
                    }
                    if(is_discovered_check){
                        continue;
                    }
                }
            }
            if(cur.flags == 2){
                if(active_color == white){
                    if((attack_squares>>f1)&1){
                        continue;
                    }
                }
                else{
                    if((attack_squares>>f8)&1){
                        continue;
                    }
                }
            }
            if(cur.flags == 3){
                if(active_color == white){
                    if((attack_squares>>d1)&1){
                        continue;
                    }
                    if((attack_squares>>c1)&1){
                        continue;
                    }
                }
                else{
                    if((attack_squares>>d8)&1){
                        continue;
                    }
                    if((attack_squares>>c8)&1){
                        continue;
                    }
                }
            }
            final_moves.push_back(cur);
        }

    }
    else if(cnt_check == 1){
        unsigned  long long int slider_squares = find_slider_squares(checking_piece_square,cur_king_pos);
        for(int i=0;i<all_moves.size();i++){
            Move cur = all_moves[i];
            if(get_piece_at_pos(bitboard,cur.from) == ((active_color == white?0:6) + W_KING)){
                if(cur.flags == 2 || cur.flags == 3){
                    continue;
                }
                if(!((attack_squares>>cur.to)&1)){
                    final_moves.push_back(cur);
                    continue;
                }
            }
            else if(pinned_piece_moves.find(cur.from) != pinned_piece_moves.end()){
                unsigned long long int slider_moves =  pinned_piece_moves[cur.from];
                if(!((slider_moves>>cur.to)&1)){
                    continue;
                }
            }
            else if(cur.to == checking_piece_square ){
                final_moves.push_back(cur);
            }
            else if((slider_squares>>cur.to)&1){
                final_moves.push_back(cur);
            }
            else{
                int cur_piece = get_piece_at_pos(bitboard,cur.from);
                if(cur_piece != W_PAWN && cur_piece != B_PAWN){
                    continue;
                }
                if(get_piece_at_pos(bitboard,checking_piece_square) == (active_color == white       ?B_PAWN:W_PAWN)){
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
            if(get_piece_at_pos(bitboard,cur.from) != ((active_color == white?0:6) + W_KING)){
                continue;
            }
            else{
                if(cur.flags == 2 || cur.flags == 3){
                    continue;
                }
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
        if((castling_rights & 1) && is_occupied(f1) == -1 && is_occupied(g1) == -1){
            moves.push_back(Move(e1,g1,2)); 
        }
        if((castling_rights & 2) && is_occupied(d1) == -1 && is_occupied(c1) == -1 && is_occupied(b1) == -1){
            moves.push_back(Move(e1,c1,3)); 
        }
    }
    else{
        if((castling_rights & 4) && is_occupied(f8) == -1 && is_occupied(g8) == -1){
            moves.push_back(Move(e8,g8,2)); 
        }
        if((castling_rights & 8) && is_occupied(d8) == -1 && is_occupied(c8) == -1 && is_occupied(b8) == -1){
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
void Board::make_move(Move m){
    bitboard_history.push_back(bitboard);
    castling_rights_history.push_back(castling_rights);
    en_passant_history.push_back(en_passant);
    halfmove_clock_history.push_back(halfmove_clock);
    zobrist_history.push_back(zobrist_hash);
    int from = m.from;
    int to = m.to;
    int flags = m.flags;
    int piece = is_occupied(from);
    zobrist_hash^=zobrist_keys[piece*64 + from];
    zobrist_hash^=zobrist_keys[piece*64 + to];
    if(en_passant != square_nb){
        zobrist_hash ^= zobrist_keys[784 + en_passant%8];
    }
    if(active_color == black){
        zobrist_hash ^= zobrist_keys[792];
    }
    if(piece == W_PAWN || piece== B_PAWN || flags == 4 ){
        halfmove_clock = 0;
    }
    else{
        halfmove_clock++;
    }

    if(active_color == black){
        fullmove_number++;
    }
    zobrist_hash ^= zobrist_keys[768 + castling_rights];
    if((piece == W_KING || piece == B_KING) && (flags != 2 && flags != 3)){
        castling_rights &= ~(active_color == white?3:12);
    }
    else if(piece == W_ROOK){
        if(from == a1){
            castling_rights &= ~2;
        }
        else if(from == h1){
            castling_rights &= ~1;
        }
    }
    else if(piece == B_ROOK){
        if(from == a8){
            castling_rights &= ~8;
        }
        else if(from == h8){
            castling_rights &= ~4;
        }
    }
    if(flags == 4 || flags >= 12){
        int captured_piece = is_occupied(to);
        if(captured_piece == W_ROOK){
            if(to == a1){
                castling_rights &= ~2;
            }
            else if(to == h1){
                castling_rights &= ~1;
            }
        }
        else if(captured_piece == B_ROOK){
            if(to == a8){
                castling_rights &= ~8;
            }
            else if(to == h8){
                castling_rights &= ~4;
            }
        }
    }
    en_passant = square_nb;
    if(flags == 0){
        bitboard[piece] &= ~(1ULL << from);
        bitboard[piece] |= (1ULL << to);
    }
    else if(flags == 1){
        bitboard[piece] &= ~(1ULL << from);
        bitboard[piece] |= (1ULL << to);
        if(piece == W_PAWN){
            en_passant = from + 8;
        }
        else{
            en_passant = from - 8;
        }
    }
    else if(flags == 2){
        bitboard[piece] &= ~(1ULL << from);
        bitboard[piece] |= (1ULL << to);
        if(active_color == white){
            bitboard[W_ROOK] &= ~(1ULL << h1);
            bitboard[W_ROOK] |= (1ULL << f1);
            zobrist_hash ^= zobrist_keys[W_ROOK*64 + h1];
            zobrist_hash ^= zobrist_keys[W_ROOK*64 + f1];
        }
        else{
            bitboard[B_ROOK] &= ~(1ULL << h8);
            bitboard[B_ROOK] |= (1ULL << f8);
            zobrist_hash ^= zobrist_keys[B_ROOK*64 + h8];
            zobrist_hash ^= zobrist_keys[B_ROOK*64 + f8];
        }
        castling_rights &= ~(active_color == white?3:12);
    }
    else if(flags == 3){
        bitboard[piece] &= ~(1ULL << from);
        bitboard[piece] |= (1ULL << to);
        if(active_color == white){
            bitboard[W_ROOK] &= ~(1ULL << a1);
            bitboard[W_ROOK] |= (1ULL << d1);
            zobrist_hash ^= zobrist_keys[W_ROOK*64 + a1];
            zobrist_hash ^= zobrist_keys[W_ROOK*64 + d1];
        }
        else{
            bitboard[B_ROOK] &= ~(1ULL << a8);
            bitboard[B_ROOK] |= (1ULL << d8);
            zobrist_hash ^= zobrist_keys[B_ROOK*64 + a8];
            zobrist_hash ^= zobrist_keys[B_ROOK*64 + d8];
        }
        castling_rights &= ~(active_color == white?3:12);
    }
    else if(flags == 4){
        int captured_piece = is_occupied(to);
        bitboard[captured_piece] &= ~(1ULL << to);
        bitboard[piece] &= ~(1ULL << from);
        bitboard[piece] |= (1ULL << to);
        zobrist_hash ^= zobrist_keys[captured_piece*64 + to];
    }
    else if(flags == 5){
        bitboard[piece] &= ~(1ULL << from);
        bitboard[piece] |= (1ULL << to);
        if(active_color == white){
            bitboard[B_PAWN] &= ~(1ULL << (to - 8));
            zobrist_hash ^= zobrist_keys[B_PAWN*64 + (to-8)];
        }
        else{
            bitboard[W_PAWN] &= ~(1ULL << (to + 8));
            zobrist_hash ^= zobrist_keys[W_PAWN*64 + (to+8)];
        }
    }
    else if(flags >= 8 && flags <= 11){
        int promotion_piece = 0;
        if(flags == 8){
            promotion_piece = W_KNIGHT;
        }
        else if(flags == 9){
            promotion_piece = W_BISHOP;
        }
        else if(flags == 10){
            promotion_piece = W_ROOK;
        }
        else if(flags == 11){
            promotion_piece = W_QUEEN;
        }
        if(active_color == black){
            promotion_piece += 6;
        }
        bitboard[piece] &= ~(1ULL << from);
        bitboard[piece] |= (1ULL << to);
        bitboard[piece] &= ~(1ULL << to);
        bitboard[promotion_piece] |= (1ULL << to);
        zobrist_hash ^= zobrist_keys[piece*64 + to];
        zobrist_hash ^= zobrist_keys[promotion_piece*64 + to];
    }
    else if(flags >= 12 && flags <= 15){
        int promotion_piece =0;
        if(flags == 12){
            promotion_piece = W_KNIGHT;
        }
        else if(flags == 13){
            promotion_piece = W_BISHOP;
        }
        else if(flags == 14){
            promotion_piece = W_ROOK;
        }
        else if(flags == 15){
            promotion_piece = W_QUEEN;
        }
        if(active_color == black){
            promotion_piece += 6;
        }
        int captured_piece = is_occupied(to);
        bitboard[captured_piece] &= ~(1ULL << to);
        bitboard[piece] &= ~(1ULL << from);
        bitboard[piece] |= (1ULL << to);
        bitboard[piece] &= ~(1ULL << to);
        bitboard[promotion_piece] |= (1ULL << to);
        zobrist_hash ^= zobrist_keys[piece*64 + to];
        zobrist_hash ^= zobrist_keys[promotion_piece*64 + to];
        zobrist_hash ^= zobrist_keys[captured_piece*64 + to];
    }
    active_color ^= 1;
    zobrist_hash ^= zobrist_keys[768 + castling_rights];
    if(en_passant != square_nb){
        zobrist_hash ^= zobrist_keys[784 + en_passant%8];
    }
    if(active_color == black){
        zobrist_hash ^= zobrist_keys[792];
    }
}

void Board::unmake_move(Move m){
    bitboard = bitboard_history.back();
    bitboard_history.pop_back();
    castling_rights = castling_rights_history.back();
    castling_rights_history.pop_back();
    en_passant = en_passant_history.back();
    en_passant_history.pop_back();
    halfmove_clock = halfmove_clock_history.back();
    halfmove_clock_history.pop_back();
    active_color ^= 1;
    if(active_color == black){
        fullmove_number--;
    }
    zobrist_hash = zobrist_history.back();
    zobrist_history.pop_back();
}

void Board::zobrist_init(){
    zobrist_keys.resize(793);
    std::mt19937_64 rng(0xDEADBEEF);
    for(int i = 0; i < 793; i++) {
        zobrist_keys[i] = rng();
    }
    zobrist_hash = 0;
    for(int i=0;i<12;i++){
        for(int j=0;j<64;j++){
            if((bitboard[i]>>j)&1){
                zobrist_hash ^= zobrist_keys[i*64 + j];
            }
        }
    }
    zobrist_hash ^= zobrist_keys[castling_rights+768];
    if(en_passant != square_nb){
        zobrist_hash ^= zobrist_keys[784 + en_passant%8];
    }
    if(active_color == black){
        zobrist_hash ^= zobrist_keys[792];
    }
}
