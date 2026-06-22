#include "search.hpp"
#include "moves.hpp"
#include "evaluation.hpp"
#include "utils.hpp"
#include<iostream>
#include<algorithm>
using namespace std;
vector<int> get_score(Board &board,const vector<Move>&moves){
    static constexpr int MVV_LVA[6][6] = {
        {0, 50, 40, 30, 20, 10},
        {0, 51, 41, 31, 21, 11},
        {0, 52, 42, 32, 22, 12},
        {0, 53, 43, 33, 23, 13},
        {0, 54, 44, 34, 24, 14},
        {0, 55, 45, 35, 25, 15}
    };
    vector<int>score(moves.size());
    for(size_t i=0;i<moves.size();i++){
        const Move&move = moves[i];
        if(move.flags <= 3){
            score[i] = 0;
        }
        else if(move.flags == 4){
            score[i] = MVV_LVA[board.piece_at[move.from]%6][board.piece_at[move.to]%6];
        }
        else if(move.flags == 5){
            score[i] = 15;
        }
        else if(move.flags <= 11){
            score[i] = 56 + (move.flags - 8);
        }
        else {
            score[i] = 56 + (move.flags - 8) + MVV_LVA[board.piece_at[move.from]%6][board.piece_at[move.to]%6];
        }
    }
    return score;
}
void order_moves(Board& board,vector<Move>&moves,Move tt_move,int ply){
    vector<int>score = get_score(board,moves);
    for(size_t i=0;i<moves.size();i++){
        if(moves[i].from == tt_move.from && moves[i].to == tt_move.to && moves[i].flags == tt_move.flags){
            score[i] = 1000000;
        }
        else if(ply < MAX_PLY && moves[i].from == killer_moves[ply][0].from && moves[i].to == killer_moves[ply][0].to && moves[i].flags == killer_moves[ply][0].flags){
            score[i] = 9;
        }
        else if(ply < MAX_PLY && moves[i].from == killer_moves[ply][1].from && moves[i].to == killer_moves[ply][1].to && moves[i].flags == killer_moves[ply][1].flags){
            score[i] = 8;
        }
    }
    vector<pair<int,Move>>temp;
    temp.reserve(moves.size());
    for(size_t i=0;i<moves.size();i++){
        temp.push_back({score[i],moves[i]});
    }
    sort(temp.begin(),temp.end(),[](const pair<int,Move>&a,const pair<int,Move>&b){
        return a.first > b.first;
    });
    for(size_t i=0;i<moves.size();i++){
        moves[i] = temp[i].second;
    }
}
Move search(Board& board,int depth){
    Move best_move(square_nb, square_nb, 0xFF);
    for(int d=1;d<=depth;d++){
        int mx = -1e9;
        vector<Move> all_moves;
        board.generate_legal_moves(all_moves);
        order_moves(board,all_moves,best_move,0);
        Move iter_best(square_nb, square_nb, 0xFF);
        for(auto move : all_moves){
            board.make_move(move);
            int score = -alpha_beta(board,d - 1,-1e9,1e9,1);
            board.unmake_move();
            if(score > mx){
                mx = score;
                iter_best = move;
            }
        }
        best_move = iter_best;
    }
    return best_move;
}
int alpha_beta(Board& board,int depth,int alpha,int beta,int ply){
    if(depth == 0){
        return quiscence(board,alpha,beta);
    }
    int alpha_orig = alpha;
    Move hash_move(square_nb, square_nb, 0xFF);
    auto it = transposition_table.find(board.zobrist_hash);
    if(it != transposition_table.end()){
        const TTEntry& e = it->second;
        hash_move = e.best;
        if(e.depth >= depth){
            if(e.flag == TT_EXACT){
                return e.value;
            }
            else if(e.flag == TT_LOWER && e.value > alpha){
                alpha = e.value;
            }
            else if(e.flag == TT_UPPER && e.value < beta){
                beta = e.value;
            }
            if(alpha >= beta){
                return e.value;
            }
        }
    }
    vector<Move>legal_moves;
    int cnt = board.generate_legal_moves(legal_moves);
    if(cnt == 0){
        return evaluate(board,cnt);
    }
    int mx = -1e9;
    Move best(square_nb, square_nb, 0xFF);
    order_moves(board,legal_moves,hash_move,ply);
    for(auto i:legal_moves){
        board.make_move(i);
        int score = -alpha_beta(board,depth-1,-beta,-alpha,ply+1);
        board.unmake_move();
        if(score > mx){
            mx = score;
            best = i;
            if(score > alpha){
                alpha = score;
            }
        }
        if(score >= beta){
            if(i.flags <= 3 && ply < MAX_PLY){
                if(!(killer_moves[ply][0].from == i.from && killer_moves[ply][0].to == i.to && killer_moves[ply][0].flags == i.flags)){
                    killer_moves[ply][1] = killer_moves[ply][0];
                    killer_moves[ply][0] = i;
                }
            }
            break;
        }
    }
    uint8_t flag;
    if(mx <= alpha_orig){
        flag = TT_UPPER;
    }
    else if(mx >= beta){
        flag = TT_LOWER;
    }
    else{
        flag = TT_EXACT;
    }
    transposition_table[board.zobrist_hash] = TTEntry{depth, mx, flag, best};
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
