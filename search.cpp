#include "search.hpp"
#include "moves.hpp"
#include "evaluation.hpp"
#include<iostream>
using namespace std;
Move search(Board& board,int depth){
    int mx = INT32_MIN;
    Move best_move(square_nb,square_nb,-1);
    vector<Move> all_moves = board.generate_legal_moves();
    for(auto move : all_moves){
        board.make_move(move);
        int score = -alpha_beta(board,depth - 1,INT32_MIN,INT32_MAX);
        cout<<pos_to_str(move.from)<<" "<<pos_to_str(move.to)<<" "<<score<<endl;
        board.unmake_move(move);
        if(score > mx){
            mx = score;
            best_move = move;
        }
    }
    return best_move;
}

int negamax(Board& board,int depth){
    if(depth == 0){
        return evaluate(board);
    }
    vector<Move> all_moves = board.generate_legal_moves();
    if(all_moves.size() == 0){
        return evaluate(board);
    }
    int mx = INT32_MIN;
    for(auto move : all_moves){
        board.make_move(move);
        int score = -negamax(board,depth - 1);
        board.unmake_move(move);
        mx = max(mx,score);
    }
    return mx;
}
// 
int alpha_beta(Board& board,int depth,int alpha,int beta){
    if(depth == 0){
        return evaluate(board);
    }
    vector<Move>legal_moves = board.generate_legal_moves();
    if(legal_moves.size() == 0){
        return evaluate(board);
    }
    int mx = INT32_MIN;
    for(auto i:legal_moves){
        board.make_move(i);
        int score = -alpha_beta(board,depth-1,-beta,-alpha);
        board.unmake_move(i);
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