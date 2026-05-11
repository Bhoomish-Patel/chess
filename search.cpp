#include "bitboard.hpp"
#include "utils.hpp"
#include "moves.hpp"
#include "evaluation.hpp"
using namespace std;
Move search(Board& board,int depth){

    int mx = -2147483648;

    Move best_move(square_nb,square_nb,-1);

    vector<Move> all_moves = board.generate_legal_moves();

    for(auto move : all_moves){

        board.make_move(move);

        int score = -negamax(board,depth - 1);

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

    int mx = -2147483648;

    vector<Move> all_moves = board.generate_legal_moves();

    for(auto move : all_moves){

        board.make_move(move);

        int score = -negamax(board,depth - 1);

        board.unmake_move(move);

        mx = max(mx,score);
    }

    return mx;
}