#include "search.hpp"
#include "moves.hpp"
#include "evaluation.hpp"
#include<iostream>
using namespace std;
Move search(Board& board,int depth){
    int mx = INT32_MIN;
    Move best_move(square_nb, square_nb, 0xFF);
    vector<Move> all_moves;
    board.generate_legal_moves(all_moves);
    for(auto move : all_moves){
        board.make_move(move);
        int score = -alpha_beta(board,depth - 1,INT32_MIN,INT32_MAX);
        board.unmake_move();
        if(score > mx){
            mx = score;
            best_move = move;
        }
    }
    return best_move;
}
int alpha_beta(Board& board,int depth,int alpha,int beta){
    if(depth == 0){
        return evaluate(board);
    }
    vector<Move>legal_moves;
    board.generate_legal_moves(legal_moves);
    if(legal_moves.size() == 0){
        return evaluate(board);
    }
    int mx = INT32_MIN;
    for(auto i:legal_moves){
        board.make_move(i);
        int score = -alpha_beta(board,depth-1,-beta,-alpha);
        board.unmake_move();
        if(score > mx){
            mx = score;
            if(score > alpha){
                alpha = score;
            }
        }
        if(score >= beta){
            break;
        }
    }
    return mx;
}