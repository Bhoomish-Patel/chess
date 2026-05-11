#include "evaluation.hpp"

#include "evaluation.hpp"

int evaluate(Board board){

    if(board.is_checkmate()){
        return -INT32_MAX;
    }
    else if(board.is_stalemate()){
        return 0;
    }
    else if(board.is_fifty_move_draw()){
        return 0;
    }

    int material_score = 0;

    int piece_type_scores[6] = {0,9,5,3,3,1};

    for(int i=0;i<12;i++){

        for(int j=0;j<64;j++){

            if((board.bitboard[i] >> j) & 1ULL){

                int cur = piece_type_scores[i%6];

                if(i < 6){
                    material_score += cur;
                }
                else{
                    material_score -= cur;
                }
            }
        }
    }

    if(board.is_three_fold_repetition() && material_score <= 0){
        return 0;
    }

    return board.active_color == white ? material_score : -material_score;
}