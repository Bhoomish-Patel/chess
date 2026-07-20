#include "bitboard.hpp"
#include "utils.hpp"
#include "moves.hpp"
#include <filesystem>
#include <stdexcept>
using namespace std;

array<uint64_t,64> rook_masks = {
282578800148862ULL, 565157600297596ULL, 1130315200595066ULL, 2260630401190006ULL,
4521260802379886ULL, 9042521604759646ULL, 18085043209519166ULL, 36170086419038334ULL,
282578800180736ULL, 565157600328704ULL, 1130315200625152ULL, 2260630401218048ULL,
4521260802403840ULL, 9042521604775424ULL, 18085043209518592ULL, 36170086419037696ULL,
282578808340736ULL, 565157608292864ULL, 1130315208328192ULL, 2260630408398848ULL,
4521260808540160ULL, 9042521608822784ULL, 18085043209388032ULL, 36170086418907136ULL,
282580897300736ULL, 565159647117824ULL, 1130317180306432ULL, 2260632246683648ULL,
4521262379438080ULL, 9042522644946944ULL, 18085043175964672ULL, 36170086385483776ULL,
283115671060736ULL, 565681586307584ULL, 1130822006735872ULL, 2261102847592448ULL,
4521664529305600ULL, 9042787892731904ULL, 18085034619584512ULL, 36170077829103616ULL,
420017753620736ULL, 699298018886144ULL, 1260057572672512ULL, 2381576680245248ULL,
4624614895390720ULL, 9110691325681664ULL, 18082844186263552ULL, 36167887395782656ULL,
35466950888980736ULL, 34905104758997504ULL, 34344362452452352ULL, 33222877839362048ULL,
30979908613181440ULL, 26493970160820224ULL, 17522093256097792ULL, 35607136465616896ULL,
9079539427579068672ULL, 8935706818303361536ULL, 8792156787827803136ULL, 8505056726876686336ULL,
7930856604974452736ULL, 6782456361169985536ULL, 4485655873561051136ULL, 9115426935197958144ULL
};
array<uint64_t,64> bishop_masks = {
18049651735527936ULL, 70506452091904ULL, 275415828992ULL, 1075975168ULL,
38021120ULL, 8657588224ULL, 2216338399232ULL, 567382630219776ULL,
9024825867763712ULL, 18049651735527424ULL, 70506452221952ULL, 275449643008ULL,
9733406720ULL, 2216342585344ULL, 567382630203392ULL, 1134765260406784ULL,
4512412933816832ULL, 9024825867633664ULL, 18049651768822272ULL, 70515108615168ULL,
2491752130560ULL, 567383701868544ULL, 1134765256220672ULL, 2269530512441344ULL,
2256206450263040ULL, 4512412900526080ULL, 9024834391117824ULL, 18051867805491712ULL,
637888545440768ULL, 1135039602493440ULL, 2269529440784384ULL, 4539058881568768ULL,
1128098963916800ULL, 2256197927833600ULL, 4514594912477184ULL, 9592139778506752ULL,
19184279556981248ULL, 2339762086609920ULL, 4538784537380864ULL, 9077569074761728ULL,
562958610993152ULL, 1125917221986304ULL, 2814792987328512ULL, 5629586008178688ULL,
11259172008099840ULL, 22518341868716544ULL, 9007336962655232ULL, 18014673925310464ULL,
2216338399232ULL, 4432676798464ULL, 11064376819712ULL, 22137335185408ULL,
44272556441600ULL, 87995357200384ULL, 35253226045952ULL, 70506452091904ULL,
567382630219776ULL, 1134765260406784ULL, 2832480465846272ULL, 5667157807464448ULL,
11333774449049600ULL, 22526811443298304ULL, 9024825867763712ULL, 18049651735527936ULL
};
array<uint32_t,64>RBits = {
  12, 11, 11, 11, 11, 11, 11, 12,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  12, 11, 11, 11, 11, 11, 11, 12
};

array<uint32_t,64>BBits = {
  6, 5, 5, 5, 5, 5, 5, 6,
  5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 7, 7, 7, 7, 5, 5,
  5, 5, 7, 9, 9, 7, 5, 5,
  5, 5, 7, 9, 9, 7, 5, 5,
  5, 5, 7, 7, 7, 7, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5,
  6, 5, 5, 5, 5, 5, 5, 6
};

Board:: Board(): Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"){}
Board:: Board(string fen){
    bitboard.fill(0);
    piece_at.fill(square_nb);
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
    for(uint8_t i=0;i<castling_rights_str.size();i++){
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
    our_pieces = 0;
    their_pieces = 0;
    for(uint8_t i=0;i<12;i++){
        uint64_t bb = bitboard[i];
        while(bb){
            uint8_t j = __builtin_ctzll(bb);
            bb &= bb - 1;
            piece_at[j] = i;
            if((i < 6) == (active_color == white)){
                our_pieces |= (1ull<<j);
            }
            else{
                their_pieces |= (1ull<<j);
            }
        }
    }
    zobrist_init();
    undo_history.reserve(512);
}
int Board::is_occupied(int pos){
    return (int)piece_at[pos];
}
uint64_t Board::generate_attack_squares(uint8_t pos,uint8_t &cnt_check,uint8_t &checking_piece_square){
    uint64_t attack_squares = 0;
    uint8_t cur_king = active_color== white ? W_KING : B_KING;
    uint8_t king_sq = pos;
    bitboard[cur_king] = 0;
    piece_at[king_sq] = square_nb;
    our_pieces &= ~(1ULL << king_sq);
    uint8_t opponent_pawn = (active_color == white) ? B_PAWN : W_PAWN;
    uint64_t pawn_bb = bitboard[opponent_pawn];
    while(pawn_bb){
        uint8_t i = __builtin_ctzll(pawn_bb);
        pawn_bb &= pawn_bb - 1;
        {
            uint8_t row = i/8;
            uint8_t col = i%8;
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
    uint8_t opponent_knight = (active_color == white) ? B_KNIGHT : W_KNIGHT;
    uint64_t knight_bb = bitboard[opponent_knight];
    while(knight_bb){
        uint8_t i = __builtin_ctzll(knight_bb);
        knight_bb &= knight_bb - 1;
        attack_squares |= knight_tables[i];
        if((knight_tables[i] >> pos) & 1){
            checking_piece_square = i;
            cnt_check++;
        }
    }
    uint64_t occupancy = our_pieces | their_pieces;
    uint8_t opponent_bishop = (active_color == white) ? B_BISHOP : W_BISHOP;
    uint64_t bishop_bb = bitboard[opponent_bishop];
    while(bishop_bb){
        uint8_t i = __builtin_ctzll(bishop_bb);
        bishop_bb &= bishop_bb - 1;

        uint64_t blockers = occupancy & bishop_masks[i];
        uint32_t idx = (blockers * bishop_magic_numbers[i]) >> (64 - BBits[i]);
        attack_squares |= bishop_tables[i][idx];

        if((bishop_tables[i][idx] >> pos) & 1){
            checking_piece_square = i;
            cnt_check++;
        }
    }
    uint8_t opponent_rook = (active_color == white) ? B_ROOK : W_ROOK;
    uint64_t rook_bb = bitboard[opponent_rook];
    while(rook_bb){
        uint8_t i = __builtin_ctzll(rook_bb);
        rook_bb &= rook_bb - 1;
        uint64_t blockers = occupancy & rook_masks[i];
        uint32_t idx = (blockers * rook_magic_numbers[i]) >> (64 - RBits[i]);
        attack_squares |= rook_tables[i][idx];
        if((rook_tables[i][idx] >> pos) & 1){
            checking_piece_square = i;
            cnt_check ++;
        }
    }
    uint8_t opponent_queen = (active_color == white) ? B_QUEEN : W_QUEEN;
    uint64_t queen_bb = bitboard[opponent_queen];
    while(queen_bb){
        uint8_t i = __builtin_ctzll(queen_bb);
        queen_bb &= queen_bb - 1;

        uint64_t rook_blockers = occupancy & rook_masks[i];
        uint32_t rook_idx = (rook_blockers * rook_magic_numbers[i]) >> (64 - RBits[i]);
        uint64_t rook_attacks = rook_tables[i][rook_idx];

        uint64_t bishop_blockers = occupancy & bishop_masks[i];
        uint32_t bishop_idx = (bishop_blockers * bishop_magic_numbers[i]) >> (64 - BBits[i]);
        uint64_t bishop_attacks = bishop_tables[i][bishop_idx];

        uint64_t queen_attacks = rook_attacks | bishop_attacks;

        attack_squares |= queen_attacks;

        if((queen_attacks >> pos) & 1){
            checking_piece_square = i;
            cnt_check++;
        }
    }
    uint8_t opponent_king = (active_color == white) ? B_KING : W_KING;
    if(bitboard[opponent_king]){
        uint8_t i = __builtin_ctzll(bitboard[opponent_king]);
        attack_squares |= king_tables[i];
        if((king_tables[i] >> pos) & 1){ 
            checking_piece_square = i;
            cnt_check++;
        }
    }
    bitboard[cur_king] |= (1ULL<<king_sq);
    piece_at[king_sq] = (uint8_t)cur_king;
    our_pieces |= (1ULL << king_sq);
    return attack_squares;
}

int Board:: generate_legal_moves(vector<Move>& final_moves){
    static vector<Move> all_moves;
    all_moves.clear();
    all_moves.reserve(64);
    generate_pseudo_legal_moves(all_moves);
    uint8_t cur_king = active_color== white?0:6 + W_KING;
    uint8_t cur_king_pos = __builtin_ctzll(bitboard[cur_king]);
    uint8_t cnt_check = 0;
    uint8_t checking_piece_square = 0;
    uint64_t attack_squares = generate_attack_squares(cur_king_pos,cnt_check,checking_piece_square);
    array<uint64_t,64>pinned_piece_moves = find_pinned_piece_moves(cur_king_pos);
    checks = cnt_check;
    if(cnt_check == 0){
        for(uint8_t i=0;i<all_moves.size();i++){
            Move cur = all_moves[i];
            if(is_occupied(cur.from) == ((active_color == white?0:6) + W_KING)){
                if((attack_squares>>cur.to)&1){
                    continue;
                }
            }
            if(pinned_piece_moves[cur.from] != UINT64_MAX){
                uint64_t slider_moves =  pinned_piece_moves[cur.from];
                if((!((slider_moves>>cur.to)&1))){
                    continue;
                }
            }
            if(cur.flags == 5){
                uint8_t r_cur_king = cur_king_pos/8;
                uint8_t c_cur_king = cur_king_pos%8;
                if(r_cur_king== cur.from/8){
                   bool is_discovered_check = false;
                    for(uint8_t c = c_cur_king+1;c<8;c++){
                        if(c == cur.to%8 || c == cur.from%8){
                            continue;
                        }
                        int piece_at_pos = is_occupied(c + 8*r_cur_king);
                        if(piece_at_pos != square_nb){
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
                    for(int8_t c= (int8_t)c_cur_king-1;c>=0;c--){
                        if(c == cur.to%8 || c == cur.from%8){
                            continue;
                        }
                        int piece_at_pos = is_occupied(c + 8*r_cur_king);
                        if(piece_at_pos != square_nb){
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
        uint64_t slider_squares = find_slider_squares(checking_piece_square,cur_king_pos);
        for(int i=0;i<all_moves.size();i++){
            Move cur = all_moves[i];
            if(is_occupied(cur.from) == ((active_color == white?0:6) + W_KING)){
                if(cur.flags == 2 || cur.flags == 3){
                    continue;
                }
                if(!((attack_squares>>cur.to)&1)){
                    final_moves.push_back(cur);
                    continue;
                }
            }
            else if(pinned_piece_moves[cur.from] != UINT64_MAX){
                uint64_t slider_moves =  pinned_piece_moves[cur.from];
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
                uint8_t cur_piece = is_occupied(cur.from);
                if(cur_piece != W_PAWN && cur_piece != B_PAWN){
                    continue;
                }
                if(is_occupied(checking_piece_square) == (active_color == white       ?B_PAWN:W_PAWN)){
                    if(cur.to == en_passant){
                        final_moves.push_back(cur);
                    }
                }
            }
        }
    }
    else if(cnt_check >= 2){
        for(uint8_t i=0;i<all_moves.size();i++){
            Move cur = all_moves[i];
            if(is_occupied(cur.from) != ((active_color == white?0:6) + W_KING)){
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
    return final_moves.size();
}
int Board:: generate_pseudo_legal_moves(vector<Move>& moves){
    for(uint8_t i=0;i<12;i++){
        if(active_color != get_piece_type(i)){
            continue;
        }
        uint64_t bb = bitboard[i];
        while(bb){
            uint8_t j = __builtin_ctzll(bb);
            bb &= bb - 1;
            if(i%6 == 0){
                generate_king_moves(j,moves);
            }
            else if(i%6 == 1){
                generate_queen_moves(j,moves);
            }
            else if(i%6 == 2){
                generate_rook_moves(j,moves);
            }
            else if(i%6 == 3){
                generate_bishop_moves(j,moves);
            }
            else if(i%6 == 4){
                generate_knight_moves(j,moves);
            }
            else if(i%6 == 5){
                generate_pawn_moves(j,moves);
            }
        }
    }
    return moves.size();
}
int Board:: generate_king_moves(uint8_t pos,vector<Move>& moves){
    uint64_t attacks = king_tables[pos] & ~our_pieces;
    while(attacks){
        uint64_t new_pos = __builtin_ctzll(attacks);
        attacks &= attacks - 1;
        if((their_pieces>>new_pos) & 1){
            moves.push_back(Move(pos,new_pos,4));
        }
        else{
            moves.push_back(Move(pos,new_pos,0));
        }
    }

    if(active_color == white){
        if((castling_rights & 1) && is_occupied(f1) == square_nb && is_occupied(g1) == square_nb){
            moves.push_back(Move(e1,g1,2)); 
        }
        if((castling_rights & 2) && is_occupied(d1) == square_nb && is_occupied(c1) == square_nb && is_occupied(b1) == square_nb){
            moves.push_back(Move(e1,c1,3)); 
        }
    }
    else{
        if((castling_rights & 4) && is_occupied(f8) == square_nb && is_occupied(g8) == square_nb){
            moves.push_back(Move(e8,g8,2)); 
        }
        if((castling_rights & 8) && is_occupied(d8) == square_nb && is_occupied(c8) == square_nb && is_occupied(b8) == square_nb){
            moves.push_back(Move(e8,c8,3)); 
        }
    }
    return moves.size();
}

int Board:: generate_queen_moves(uint8_t pos,vector<Move>& moves){
    uint64_t occupancy = our_pieces | their_pieces;
    uint64_t rook_blockers = occupancy & rook_masks[pos];
    uint32_t rook_idx = (rook_blockers * rook_magic_numbers[pos]) >> (64 - RBits[pos]);
    uint64_t bishop_blockers = occupancy & bishop_masks[pos];
    uint32_t bishop_idx = (bishop_blockers * bishop_magic_numbers[pos]) >> (64 - BBits[pos]);
    uint64_t attacks = (rook_tables[pos][rook_idx] | bishop_tables[pos][bishop_idx]) & ~our_pieces;
    while(attacks){
        uint8_t new_pos = __builtin_ctzll(attacks);
        attacks &= attacks - 1;
        if((their_pieces>>new_pos) & 1){
            moves.push_back(Move(pos,new_pos,4));
        }
        else{
            moves.push_back(Move(pos,new_pos,0));
        }
    }
    return moves.size();
}

int Board:: generate_rook_moves(uint8_t pos,vector<Move>& moves){
    uint64_t occupancy = our_pieces | their_pieces;
    uint64_t blockers = occupancy & rook_masks[pos];
    uint32_t idx = (blockers * rook_magic_numbers[pos]) >> (64 - RBits[pos]);
    uint64_t attacks = rook_tables[pos][idx] & ~our_pieces;
    while(attacks){
        uint8_t new_pos = __builtin_ctzll(attacks);
        attacks &= attacks - 1;
        if((their_pieces>>new_pos) & 1){
            moves.push_back(Move(pos,new_pos,4));
        }
        else{
            moves.push_back(Move(pos,new_pos,0));
        }
    }
    return moves.size();
}
int Board:: generate_bishop_moves(uint8_t pos,vector<Move>& moves){
    uint64_t occupancy = our_pieces | their_pieces;
    uint64_t blockers = occupancy & bishop_masks[pos];
    uint32_t idx = (blockers * bishop_magic_numbers[pos]) >> (64 - BBits[pos]);
    uint64_t attacks = bishop_tables[pos][idx] & ~our_pieces;
    while(attacks){
        uint8_t new_pos = __builtin_ctzll(attacks);
        attacks &= attacks - 1;
        if((their_pieces>>new_pos) & 1){
            moves.push_back(Move(pos,new_pos,4));
        }
        else{
            moves.push_back(Move(pos,new_pos,0));
        }
    }
    return moves.size();
}
int Board:: generate_knight_moves(uint8_t pos,vector<Move>& moves){
    uint64_t attacks = knight_tables[pos] & ~our_pieces;
    while(attacks){
        uint8_t new_pos = __builtin_ctzll(attacks);
        attacks &= attacks - 1;
        if((their_pieces>>new_pos) & 1){
            moves.push_back(Move(pos,new_pos,4));
        }
        else{
            moves.push_back(Move(pos,new_pos,0));
        }
    }
    return moves.size();
}
int Board::generate_pawn_moves(int pos,vector<Move>& moves){
    uint8_t row = pos / 8;
    uint8_t col = pos % 8;

    if(active_color == white){
        if(row < 7 && is_occupied(pos + 8) == square_nb){
            if(row == 6){
                moves.push_back(Move(pos,pos+8,8));
                moves.push_back(Move(pos,pos+8,9));
                moves.push_back(Move(pos,pos+8,10));
                moves.push_back(Move(pos,pos+8,11));
            } else {
                moves.push_back(Move(pos,pos+8,0));
            }
        }

        if(row == 1 && is_occupied(pos+8) == square_nb && is_occupied(pos+16) == square_nb){
            moves.push_back(Move(pos,pos+16,1));
        }
        if(row < 7 && col > 0){
            uint8_t target = pos + 7;
            if(is_occupied(target) != square_nb && get_piece_type(is_occupied(target)) == black){
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
            uint8_t target = pos + 9;
            if(is_occupied(target) != square_nb && get_piece_type(is_occupied(target)) == black){
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
        if(row > 0 && is_occupied(pos - 8) == square_nb){
            if(row == 1){
                moves.push_back(Move(pos,pos-8,8));
                moves.push_back(Move(pos,pos-8,9));
                moves.push_back(Move(pos,pos-8,10));
                moves.push_back(Move(pos,pos-8,11));
            } else {
                moves.push_back(Move(pos,pos-8,0));
            }
        }
        if(row == 6 && is_occupied(pos-8) == square_nb && is_occupied(pos-16) == square_nb){
            moves.push_back(Move(pos,pos-16,1));
        }
        if(row > 0 && col > 0){
            uint8_t target = pos - 9;
            if(is_occupied(target) != square_nb && get_piece_type(is_occupied(target)) == white){
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
            uint8_t target = pos - 7;
            if(is_occupied(target) != square_nb && get_piece_type(is_occupied(target)) == white){
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
    return moves.size();
}
void Board::make_move(Move m){
    uint8_t from = m.from;
    uint8_t to = m.to;
    uint8_t flags = m.flags;
    uint8_t piece = is_occupied(from);

    Undo undo;
    undo.castling_rights = castling_rights;
    undo.en_passant = en_passant;
    undo.halfmove_clock = halfmove_clock;
    undo.zobrist_hash = zobrist_hash;
    undo.from = from;
    undo.to = to;
    undo.flags = flags;
    undo.moved_piece = piece;
    undo.captured_piece = 255;
    undo.promotion_piece = 255;

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
        undo.captured_piece = captured_piece;
    }
    en_passant = square_nb;
    if(flags == 0){
        bitboard[piece] &= ~(1ULL << from);
        bitboard[piece] |= (1ULL << to);
        piece_at[from] = square_nb;
        piece_at[to] = piece;
        our_pieces |= (1ULL << to);
        our_pieces &= ~(1ULL << from);
    }
    else if(flags == 1){
        bitboard[piece] &= ~(1ULL << from);
        bitboard[piece] |= (1ULL << to);
        piece_at[from] = square_nb;
        piece_at[to] = piece;
        our_pieces |= (1ULL << to);
        our_pieces &= ~(1ULL << from);
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
        piece_at[from] = square_nb;
        piece_at[to] = piece;
        our_pieces |= (1ULL << to);
        our_pieces &= ~(1ULL << from);
        if(active_color == white){
            bitboard[W_ROOK] &= ~(1ULL << h1);
            bitboard[W_ROOK] |= (1ULL << f1);
            piece_at[h1] =square_nb;
            piece_at[f1] = W_ROOK;
            our_pieces &= ~(1ULL << h1);
            our_pieces |= (1UL << f1);
            zobrist_hash ^= zobrist_keys[W_ROOK*64 + h1];
            zobrist_hash ^= zobrist_keys[W_ROOK*64 + f1];
        }
        else{
            bitboard[B_ROOK] &= ~(1ULL << h8);
            bitboard[B_ROOK] |= (1ULL << f8);
            piece_at[h8] =square_nb;
            piece_at[f8] = B_ROOK;
            our_pieces &= ~(1ULL << h8);
            our_pieces |= (1ULL << f8);
            zobrist_hash ^= zobrist_keys[B_ROOK*64 + h8];
            zobrist_hash ^= zobrist_keys[B_ROOK*64 + f8];
        }
        castling_rights &= ~(active_color == white?3:12);
    }
    else if(flags == 3){
        bitboard[piece] &= ~(1ULL << from);
        bitboard[piece] |= (1ULL << to);
        piece_at[from] = square_nb;
        piece_at[to] = piece;
        our_pieces &= ~(1ULL << from);
        our_pieces |= (1ULL << to);
        if(active_color == white){
            bitboard[W_ROOK] &= ~(1ULL << a1);
            bitboard[W_ROOK] |= (1ULL << d1);
            piece_at[a1] =square_nb;
            piece_at[d1] = W_ROOK;
            our_pieces &= ~(1ULL << a1);
            our_pieces |= (1ULL << d1);
            zobrist_hash ^= zobrist_keys[W_ROOK*64 + a1];
            zobrist_hash ^= zobrist_keys[W_ROOK*64 + d1];
        }
        else{
            bitboard[B_ROOK] &= ~(1ULL << a8);
            bitboard[B_ROOK] |= (1ULL << d8);
            piece_at[a8] =square_nb;
            piece_at[d8] = B_ROOK;
            our_pieces &= ~(1ULL << a8);
            our_pieces |= (1ULL << d8);
            zobrist_hash ^= zobrist_keys[B_ROOK*64 + a8];
            zobrist_hash ^= zobrist_keys[B_ROOK*64 + d8];
        }
        castling_rights &= ~(active_color == white?3:12);
    }
    else if(flags == 4){
        int8_t captured_piece = is_occupied(to);
        bitboard[captured_piece] &= ~(1ULL << to);
        bitboard[piece] &= ~(1ULL << from);
        bitboard[piece] |= (1ULL << to);
        piece_at[from] = square_nb;
        piece_at[to] = piece;
        their_pieces &= ~(1ULL << to);
        our_pieces &= ~(1ULL << from);
        our_pieces |= (1ULL << to);
        zobrist_hash ^= zobrist_keys[captured_piece*64 + to];
    }
    else if(flags == 5){
        bitboard[piece] &= ~(1ULL << from);
        bitboard[piece] |= (1ULL << to);
        piece_at[from] = square_nb;
        piece_at[to] = piece;
        our_pieces &= ~(1ULL << from);
        our_pieces |= (1ULL << to);
        if(active_color == white){
            bitboard[B_PAWN] &= ~(1ULL << (to - 8));
            piece_at[to-8] =square_nb;
            their_pieces &= ~(1ULL << (to - 8));
            zobrist_hash ^= zobrist_keys[B_PAWN*64 + (to-8)];
            undo.captured_piece = B_PAWN;
        }
        else{
            bitboard[W_PAWN] &= ~(1ULL << (to + 8));
            piece_at[to+8] = square_nb;
            their_pieces &= ~(1ULL << (to + 8));
            zobrist_hash ^= zobrist_keys[W_PAWN*64 + (to+8)];
            undo.captured_piece = W_PAWN;
        }
    }
    else if(flags >= 8 && flags <= 11){
        uint8_t promotion_piece = 0;
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
        piece_at[from] = square_nb;
        piece_at[to] = promotion_piece;
        our_pieces &= ~(1ULL << from);
        our_pieces |= (1ULL << to);
        zobrist_hash ^= zobrist_keys[piece*64 + to];
        zobrist_hash ^= zobrist_keys[promotion_piece*64 + to];
        undo.promotion_piece = promotion_piece;
    }
    else if(flags >= 12 && flags <= 15){
        uint8_t promotion_piece =0;
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
        piece_at[from] = square_nb;
        piece_at[to] = promotion_piece;
        our_pieces &= ~(1ULL << from);
        our_pieces |= (1ULL << to);
        their_pieces &= ~(1ULL << to);
        zobrist_hash ^= zobrist_keys[piece*64 + to];
        zobrist_hash ^= zobrist_keys[promotion_piece*64 + to];
        zobrist_hash ^= zobrist_keys[captured_piece*64 + to];
        undo.promotion_piece = promotion_piece;
    }
    active_color ^= 1;
    zobrist_hash ^= zobrist_keys[768 + castling_rights];
    if(en_passant != square_nb){
        zobrist_hash ^= zobrist_keys[784 + en_passant%8];
    }
    if(active_color == black){
        zobrist_hash ^= zobrist_keys[792];
    }
    undo_history.push_back(undo);
    swap(our_pieces,their_pieces);
}

void Board::unmake_move(){
    Undo undo = undo_history.back();
    undo_history.pop_back();
    castling_rights = undo.castling_rights;
    en_passant = undo.en_passant;
    halfmove_clock = undo.halfmove_clock;
    zobrist_hash = undo.zobrist_hash;
    uint8_t moved_piece = undo.moved_piece;
    uint8_t captured_piece = undo.captured_piece;
    uint8_t promotion_piece = undo.promotion_piece;
    uint8_t from = undo.from;
    uint8_t to = undo.to;
    uint8_t flags = undo.flags;
    if(flags == 2){
        if(moved_piece == W_KING){
            bitboard[W_KING] &= ~(1ULL<<g1);
            bitboard[W_KING] |= (1ULL<<e1);
            bitboard[W_ROOK] &= ~(1ULL<<f1);
            bitboard[W_ROOK] |= (1ULL<<h1);
            piece_at[g1] = square_nb;
            piece_at[e1] = W_KING;
            piece_at[f1] = square_nb;
            piece_at[h1] = W_ROOK;
            their_pieces &= ~(1ULL<<g1);
            their_pieces |= (1ULL<<e1);
            their_pieces &= ~(1ULL<<f1);
            their_pieces |= (1ULL<<h1);
        }
        else{
            bitboard[B_KING] &= ~(1ULL<<g8);
            bitboard[B_KING] |= (1ULL<<e8);
            bitboard[B_ROOK] &= ~(1ULL<<f8);
            bitboard[B_ROOK] |= (1ULL<<h8);
            piece_at[g8] = square_nb;
            piece_at[e8] = B_KING;
            piece_at[f8] = square_nb;
            piece_at[h8] = B_ROOK;
            their_pieces &= ~(1ULL<<g8);
            their_pieces |= (1ULL<<e8);
            their_pieces &= ~(1ULL<<f8);
            their_pieces |= (1ULL<<h8);
        }
    }
    else if(flags == 3){
        if(moved_piece == W_KING){
            bitboard[W_KING] &= ~(1ULL<<c1);
            bitboard[W_KING] |= (1ULL<<e1);
            bitboard[W_ROOK] &= ~(1ULL<<d1);
            bitboard[W_ROOK] |= (1ULL<<a1);
            piece_at[c1] = square_nb;
            piece_at[e1] = W_KING;
            piece_at[d1] = square_nb;
            piece_at[a1] = W_ROOK;
            their_pieces &= ~(1ULL<<c1);
            their_pieces |= (1ULL<<e1);
            their_pieces &= ~(1ULL<<d1);
            their_pieces |= (1ULL<<a1);
        }
        else{
            bitboard[B_KING] &= ~(1ULL<<c8);
            bitboard[B_KING] |= (1ULL<<e8);
            bitboard[B_ROOK] &= ~(1ULL<<d8);
            bitboard[B_ROOK] |= (1ULL<<a8);
            piece_at[c8] = square_nb;
            piece_at[e8] = B_KING;
            piece_at[d8] = square_nb;
            piece_at[a8] = B_ROOK;
            their_pieces &= ~(1ULL<<c8);
            their_pieces |= (1ULL<<e8);
            their_pieces &= ~(1ULL<<d8);
            their_pieces |= (1ULL<<a8);
        }
    }
    else if(flags == 5){
        bitboard[moved_piece] &= ~(1ULL<<to);
        bitboard[moved_piece] |= (1ULL<<from);
        piece_at[to] = square_nb;
        piece_at[from] = moved_piece;
        their_pieces &= ~(1ULL<<to);
        their_pieces |= (1ULL<<from);
        if(moved_piece == W_PAWN){
            bitboard[B_PAWN] |= (1ULL<<(to-8));
            piece_at[to-8] = B_PAWN;
            our_pieces |= (1ULL<<(to-8));
        }
        else{
            bitboard[W_PAWN] |= (1ULL<<(to+8));
            piece_at[to+8] = W_PAWN;
            our_pieces |= (1ULL<<(to+8));
        }
    }
    else{
        bitboard[moved_piece] &= ~(1ULL<<to);
        bitboard[moved_piece] |= (1ULL<<from);
        piece_at[to] = square_nb;
        piece_at[from] = moved_piece;
        their_pieces &= ~(1ULL<<to);
        their_pieces |= (1ULL<<from);
        if(captured_piece != 255){
            bitboard[captured_piece] |= (1ULL<<to);
            piece_at[to] = captured_piece;
            our_pieces |= (1ULL<<to);
        }
        if(promotion_piece != 255){
            bitboard[promotion_piece] &= ~(1ULL<<to);
            their_pieces &= ~(1ULL<<to);
        }
    }

    active_color ^= 1;
    if(active_color == black){
        fullmove_number--;
    }
    swap(our_pieces,their_pieces);
}

string Board::to_fen(){
    const char piece_chars[12] = {'K','Q','R','B','N','P','k','q','r','b','n','p'};
    string fen = "";
    for(int8_t row = 7; row >= 0; row--){
        int empty = 0;
        for(uint8_t col = 0; col < 8; col++){
            uint8_t p = piece_at[row*8 + col];
            if(p >= 12){
                empty++;
            } else {
                if(empty > 0){ fen += ('0' + empty); empty = 0; }
                fen += piece_chars[p];
            }
        }
        if(empty > 0) fen += ('0' + empty);
        if(row > 0) fen += '/';
    }
    fen += active_color == white ? " w " : " b ";
    string cr = "";
    if(castling_rights & 1) cr += 'K';
    if(castling_rights & 2) cr += 'Q';
    if(castling_rights & 4) cr += 'k';
    if(castling_rights & 8) cr += 'q';
    fen += cr.empty() ? "-" : cr;
    fen += ' ';
    fen += (en_passant == square_nb) ? "-" : pos_to_str(en_passant);
    fen += ' ';
    fen += to_string(halfmove_clock);
    fen += ' ';
    fen += to_string(fullmove_number);
    return fen;
}
void Board::zobrist_init(){
    std::mt19937_64 rng(0xDEADBEEF);
    for(int i = 0; i < 793; i++) {
        zobrist_keys[i] = rng();
    }
    zobrist_hash = 0;
    for(uint8_t i=0;i<12;i++){
        uint64_t bb = bitboard[i];
        while(bb){
            uint8_t j = __builtin_ctzll(bb);
            bb &= bb - 1;
            zobrist_hash ^= zobrist_keys[i*64 + j];
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
bool Board::is_stalemate(){
    vector<Move>moves;
    if(checks == 0){
        if(generate_legal_moves(moves) == 0){
            return 1;
        }
    }
    return 0;
}
bool Board::is_checkmate(){
    vector<Move>moves;
    return (generate_legal_moves(moves) == 0) && checks;
}
bool Board::is_fifty_move_draw(){
    return halfmove_clock >= 100;
}
bool Board::is_three_fold_repetition(){
    uint8_t count = 0;
    for(const auto& undo : undo_history) {
        if(undo.zobrist_hash == zobrist_hash) count++;
        if(count >= 3) return true;
    }
    return false;
}
array<uint64_t,64> Board::find_pinned_piece_moves(uint8_t king_pos){
    static const int8_t r_incs[3] = {-1,0,1};
    static const int8_t c_incs[3] = {-1,0,1};
    uint8_t r_king = king_pos/8;
    uint8_t c_king = king_pos%8;
    uint8_t cur_king = is_occupied(king_pos);
    array<uint64_t,64>ans;
    ans.fill(UINT64_MAX);
    for(uint8_t i=0;i<3;i++){
        for(uint8_t j=0;j<3;j++){
            int8_t r_inc = r_incs[i];
            int8_t c_inc = c_incs[j];
            uint8_t r_cur = r_king + r_inc;
            uint8_t c_cur = c_king + c_inc;
            if(r_inc == 0 && c_inc ==0)continue;
            int attack_piece_pos = -1 ,pinned_piece_pos = -1;
            while(r_cur<8 && r_cur>=0 && c_cur>=0 && c_cur<8){
                int cur_piece = is_occupied(r_cur*8 + c_cur);
                if(cur_piece != (int)square_nb){
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
            int piece_type = is_occupied(attack_piece_pos);
            bool local_color = active_color;
            // cout<<pos_to_str(attack_piece_pos)<<" "<<pos_to_str(pinned_piece_pos)<<endl;
            // cout<<active_color<<" "<<piece_type<<endl;
            // cout<<r_inc<<" "<<c_inc<<endl;
            // cout<<piece_type<<" "<<(active_color ==0 ? B_QUEEN : W_QUEEN)<<endl;
            if(r_inc == 0 || c_inc ==0){
                if((piece_type !=  (local_color == 0 ? B_ROOK : W_ROOK)) && (piece_type != (local_color == 0 ? B_QUEEN : W_QUEEN))){
                    continue;
                }
            }
            else{
                if((piece_type != (local_color == 0 ? B_BISHOP : W_BISHOP)) && (piece_type != (local_color == 0 ? B_QUEEN : W_QUEEN))){
                    continue;
                }
            }
            uint64_t slider_moves = find_slider_squares(king_pos,attack_piece_pos);
            slider_moves |=(1ull<<attack_piece_pos);
            ans[pinned_piece_pos] = slider_moves;
        }
    }
    return ans;
}

array<uint64_t,64>knight_tables;
array<uint64_t,64>king_tables;
array<uint64_t,64>rook_magic_numbers;
array<uint64_t,64>bishop_magic_numbers;
array<array<uint64_t,4096>,64> rook_tables;
array<array<uint64_t,512>,64> bishop_tables;
void init_attack_tables(const string& executable_path){
    for(uint8_t pos=0;pos<64;pos++){
        uint64_t attacks = 0;
        uint8_t row = pos/8;
        uint8_t col = pos%8;
        int8_t dr[8] = {-2,-1,1,2,2,1,-1,-2};
        int8_t dc[8] = {1,2,2,1,-1,-2,-2,-1};
        for(uint8_t i=0;i<8;i++){
            int8_t new_row = row + dr[i];
            int8_t new_col = col + dc[i];
            if(new_row>=0 && new_row<8 && new_col>=0 && new_col<8){
                uint8_t new_pos = new_row*8 + new_col;
                attacks |= (1ULL<<new_pos);
            }
        }
        knight_tables[pos] = attacks;
    }

    for(uint8_t pos=0;pos<64;pos++){
        uint8_t row = pos/8;
        uint8_t col = pos%8;
        uint64_t attacks = 0;
        for(int8_t dr=-1;dr<=1;dr++){
            for(int8_t dc=-1;dc<=1;dc++){
                if(dr==0 && dc==0) continue;
                int8_t new_row = row + dr;
                int8_t new_col = col + dc;
                if(new_row>=0 && new_row<8 && new_col>=0 && new_col<8){
                    uint8_t new_pos = new_row*8 + new_col;
                    attacks |= (1ULL<<new_pos);
                }
            }
        }
        king_tables[pos] = attacks;
    }

    namespace fs = std::filesystem;
    vector<fs::path> candidates;
    if (!executable_path.empty()) {
        fs::path executable = fs::absolute(executable_path).lexically_normal();
        candidates.push_back(executable.parent_path());
        candidates.push_back(executable.parent_path().parent_path());
    }
    candidates.push_back(fs::current_path());

    fs::path resource_dir;
    for (const fs::path& candidate : candidates) {
        if (fs::exists(candidate / "rook_magics.txt") &&
            fs::exists(candidate / "bishop_magics.txt") &&
            fs::exists(candidate / "rook_table.txt") &&
            fs::exists(candidate / "bishop_table.txt")) {
            resource_dir = candidate;
            break;
        }
    }
    if (resource_dir.empty()) {
        throw runtime_error("attack-table files were not found beside the executable");
    }

    ifstream rook_magic_file(resource_dir / "rook_magics.txt");
    ifstream bishop_magic_file(resource_dir / "bishop_magics.txt");
    ifstream rook_table_file(resource_dir / "rook_table.txt");
    ifstream bishop_table_file(resource_dir / "bishop_table.txt");

    for(int i=0;i<64;i++){
        if (!(rook_magic_file >> rook_magic_numbers[i]))
            throw runtime_error("rook_magics.txt is incomplete or invalid");
    }

    for(int i=0;i<64;i++){
        if (!(bishop_magic_file >> bishop_magic_numbers[i]))
            throw runtime_error("bishop_magics.txt is incomplete or invalid");
    }

    for(int i=0;i<64;i++){
        for(int j=0;j<4096;j++){
            if (!(rook_table_file >> rook_tables[i][j]))
                throw runtime_error("rook_table.txt is incomplete or invalid");
        }
    }

    for(int i=0;i<64;i++){
        for(int j=0;j<512;j++){
            if (!(bishop_table_file >> bishop_tables[i][j]))
                throw runtime_error("bishop_table.txt is incomplete or invalid");
        }
    }
}
