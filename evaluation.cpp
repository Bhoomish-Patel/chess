#include "evaluation.hpp"
#include "evaluation.hpp"
#include <iostream>
int evaluate(Board board){
    if(board.is_checkmate()){
        return INT32_MIN;
    }
    else if(board.is_stalemate()){
        return 0;
    }
    else if(board.is_fifty_move_draw()){
        return 0;
    }
    int material_score = 0;
    const int piece_type_scores[6] = {0,9,5,3,3,1};
    for(uint8_t i=0;i<12;i++){
        int cnt = __builtin_popcountll(board.bitboard[i]);
        int cur = piece_type_scores[i%6] * cnt;
        if(i < 6) material_score += cur;
        else       material_score -= cur;
    }
    if(board.is_three_fold_repetition() && material_score <= 0){
        return 0;
    }
    return board.active_color == white ? material_score : -material_score;
}