#include "search.hpp"
#include "moves.hpp"
#include "evaluation.hpp"
#include "utils.hpp"
#include<iostream>
using namespace std;
Move search(Board& board,int depth){
    int mx = -1e9;
    Move best_move(square_nb, square_nb, 0xFF);
    vector<Move> all_moves;
    board.generate_legal_moves(all_moves);
    for(auto move : all_moves){
        board.make_move(move);
        int score = -alpha_beta(board,depth - 1,-1e9,1e9);
        board.unmake_move();
        if(score >= mx){
            mx = score;
            best_move = move;
        }
    }
    return best_move;
}
int alpha_beta(Board& board,int depth,int alpha,int beta){
    if(depth == 0){
        return quiscence(board,alpha,beta);
    }
    vector<Move>legal_moves;
    int cnt = board.generate_legal_moves(legal_moves);
    if(cnt == 0){
        return evaluate(board,cnt);
    }
    int mx = -1e9;
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
int quiscence(Board& board,int alpha,int beta){
    vector<Move>legal_moves;
    int cnt = board.generate_legal_moves(legal_moves);
    if(cnt == 0){
        return evaluate(board,cnt);
    }
    bool in_check = board.checks > 0;
    int best_value;
    if(in_check){
        best_value = -1e9;
    }
    else{
        best_value = evaluate(board,cnt);
        if(best_value >= beta){
            return best_value;
        }
        if(best_value > alpha){
            alpha = best_value;
        }
    }
    for(auto i:legal_moves){
        if(in_check || i.flags == 4 || i.flags == 5 || i.flags >= 12){
            board.make_move(i);
            int score = -quiscence(board,-beta,-alpha);
            board.unmake_move();
            if(score >= beta){
                return score;
            }
            if(score > best_value){
                best_value = score;
            }
            if(score > alpha){
                alpha = score;
            }
        }
    }
    return best_value;
}