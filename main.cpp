#include "bitboard.hpp"
#include "utils.hpp"
#include "search.hpp"
#include<chrono>
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
    Board board("8/2k5/7R/6R1/4K3/8/8/8 w - - 0 1");
    auto start = chrono::high_resolution_clock::now();
    Move move = search(board,3);
    cout<<"Final move "<<pos_to_str(move.from)<<" "<<pos_to_str(move.to)<<endl;
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "Time taken: " << duration.count() << " ms" << endl;
    return 0;
}