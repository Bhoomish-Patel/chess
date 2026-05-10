#include "bitboard.hpp"
#include "utils.hpp"
#include<bits/stdc++.h>
using namespace std;

long long int perft(Board &board,int depth,int root_depth){
    if(depth == 0){
        return 1;
    }
    vector<Move> moves = board.generate_legal_moves();
    long long int nodes = 0;
    for(Move m : moves){
        board.make_move(m);
        long long int child = perft(board,depth-1,root_depth);
        if(depth == root_depth){
            cout << pos_to_str(m.from)<< pos_to_str(m.to) << ": " << child << endl;
        }
        nodes += child;
        board.unmake_move(m);
    }
    return nodes;
}

int main(){
    Board board("8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1");
    int depth;
    cin >> depth;
    cout << perft(board,depth,depth) << endl;
    return 0;
}