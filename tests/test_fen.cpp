#include "../bitboard.hpp"
#include "../utils.hpp"
#include <cassert>
#include <iostream>
using namespace std;

bool check_piece(vector<unsigned long long> &bb, int piece, int pos){
    return (bb[piece] & (1ULL << pos)) != 0;
}

void test_start_position(){
    Board b;

    assert(check_piece(b.bit_board, 0, str_to_pos("e1"))); // K
    assert(check_piece(b.bit_board, 6, str_to_pos("e8"))); // k

    cout << "Start position OK\n";
}

void test_empty_board(){
    Board b("8/8/8/8/8/8/8/8 w - - 0 1");

    for(int i = 0; i < 12; i++){
        assert(b.bit_board[i] == 0);
    }

    cout << "Empty board OK\n";
}
void test_pawn_structure() {
    // Testing a specific setup: White pawns on a2, b2; Black pawns on g7, h7
    Board b("8/6pp/8/8/8/8/PP6/8 w - - 0 1");

    // Check White Pawns (assuming index 5 for P)
    assert(check_piece(b.bit_board, 5, str_to_pos("a2")));
    assert(check_piece(b.bit_board, 5, str_to_pos("b2")));
    assert(!check_piece(b.bit_board, 5, str_to_pos("c2")));

    // Check Black Pawns (assuming index 11 for p)
    assert(check_piece(b.bit_board, 11, str_to_pos("g7")));
    assert(check_piece(b.bit_board, 11, str_to_pos("h7")));

    cout << "Pawn structure OK\n";
}

void test_complex_fen() {
    // A mid-game position with multiple pieces and specific active color
    // FEN: r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 3 3
    Board b("r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 3 3");

    assert(check_piece(b.bit_board, 1, str_to_pos("d1"))); // White Queen (Q)
    assert(check_piece(b.bit_board, 4, str_to_pos("f3"))); // White Knight (N)
    assert(check_piece(b.bit_board, 10, str_to_pos("c6"))); // Black Knight (n)
    
    // Check for side to move (if your Board class stores this)
    // assert(b.side_to_move == WHITE); 

    cout << "Complex FEN OK\n";
}

void test_occupancy_bitboards() {
    Board b; // Start position
    
    // If you maintain a combined bitboard for all white/black pieces
    // This is crucial for move generation (sliding pieces)
    unsigned long long white_occupancy = 0;
    for(int i = 0; i < 6; i++) white_occupancy |= b.bit_board[i];
    
    assert(white_occupancy == 0xFFFFULL); // Ranks 1 and 2 full
    
    cout << "Occupancy bitboards OK\n";
}

void test_edge_of_board() {
    // Pieces at the very corners (index 0 and 63)
    Board b("R6k/8/8/8/8/8/8/K6r w - - 0 1");
    
    assert(check_piece(b.bit_board, 2, str_to_pos("a8"))); // White Rook
    assert(check_piece(b.bit_board, 6, str_to_pos("h8"))); // Black King
    assert(check_piece(b.bit_board, 0, str_to_pos("a1"))); // White King
    assert(check_piece(b.bit_board, 8, str_to_pos("h1"))); // Black Rook

    cout << "Board corners OK\n";
}
void run_all_tests(){
    test_start_position();
    test_empty_board();
    test_pawn_structure();
    test_complex_fen();
    test_occupancy_bitboards();
    test_edge_of_board();
    
    cout << "\n====================\n";
    cout << "All tests passed!";
    cout << "\n====================\n";
}