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

    assert(check_piece(b.bitboard, 0, str_to_pos("e1"))); // K
    assert(check_piece(b.bitboard, 6, str_to_pos("e8"))); // k

    cout << "Start position OK\n";
}

void test_empty_board(){
    Board b("8/8/8/8/8/8/8/8 w - - 0 1");

    for(int i = 0; i < 12; i++){
        assert(b.bitboard[i] == 0);
    }

    cout << "Empty board OK\n";
}
void test_pawn_structure() {
    // Testing a specific setup: White pawns on a2, b2; Black pawns on g7, h7
    Board b("8/6pp/8/8/8/8/PP6/8 w - - 0 1");

    // Check White Pawns (assuming index 5 for P)
    assert(check_piece(b.bitboard, 5, str_to_pos("a2")));
    assert(check_piece(b.bitboard, 5, str_to_pos("b2")));
    assert(!check_piece(b.bitboard, 5, str_to_pos("c2")));

    // Check Black Pawns (assuming index 11 for p)
    assert(check_piece(b.bitboard, 11, str_to_pos("g7")));
    assert(check_piece(b.bitboard, 11, str_to_pos("h7")));

    cout << "Pawn structure OK\n";
}

void test_complex_fen() {
    // A mid-game position with multiple pieces and specific active color
    // FEN: r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 3 3
    Board b("r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 3 3");

    assert(check_piece(b.bitboard, 1, str_to_pos("d1"))); // White Queen (Q)
    assert(check_piece(b.bitboard, 4, str_to_pos("f3"))); // White Knight (N)
    assert(check_piece(b.bitboard, 10, str_to_pos("c6"))); // Black Knight (n)
    
    // Check for side to move (if your Board class stores this)
    // assert(b.side_to_move == WHITE); 

    cout << "Complex FEN OK\n";
}

void test_occupancy_bitboards() {
    Board b; // Start position
    
    // If you maintain a combined bitboard for all white/black pieces
    // This is crucial for move generation (sliding pieces)
    unsigned long long white_occupancy = 0;
    for(int i = 0; i < 6; i++) white_occupancy |= b.bitboard[i];
    
    assert(white_occupancy == 0xFFFFULL); // Ranks 1 and 2 full
    
    cout << "Occupancy bitboards OK\n";
}

void test_edge_of_board() {
    // Pieces at the very corners (index 0 and 63)
    Board b("R6k/8/8/8/8/8/8/K6r w - - 0 1");
    
    assert(check_piece(b.bitboard, 2, str_to_pos("a8"))); // White Rook
    assert(check_piece(b.bitboard, 6, str_to_pos("h8"))); // Black King
    assert(check_piece(b.bitboard, 0, str_to_pos("a1"))); // White King
    assert(check_piece(b.bitboard, 8, str_to_pos("h1"))); // Black Rook

    cout << "Board corners OK\n";
}
void test_legal_moves_generation() {
    // -------------------------------------------------------
    // ORIGINAL TESTS (1-7)
    // -------------------------------------------------------

    // Test 1: r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3 2
    Board b1("r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3 2");
    assert(b1.generate_legal_moves().size() == 8);
    cout << "Test 1 (8 moves) OK\n";

    // Test 2: 8/8/8/2k5/2pP4/8/B7/4K3 b - d3 0 3
    Board b2("8/8/8/2k5/2pP4/8/B7/4K3 b - d3 0 3");
    assert(b2.generate_legal_moves().size() == 8);
    cout << "Test 2 (8 moves) OK\n";

    // Test 3: r1bqkbnr/pppppppp/n7/8/8/P7/1PPPPPPP/RNBQKBNR w KQkq - 2 2
    Board b3("r1bqkbnr/pppppppp/n7/8/8/P7/1PPPPPPP/RNBQKBNR w KQkq - 2 2");
    assert(b3.generate_legal_moves().size() == 19);
    cout << "Test 3 (19 moves) OK\n";

    // Test 4: r3k2r/p1pp1pb1/bn2Qnp1/2qPN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQkq - 3 2
    Board b4("r3k2r/p1pp1pb1/bn2Qnp1/2qPN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQkq - 3 2");
    assert(b4.generate_legal_moves().size() == 5);
    cout << "Test 4 (5 moves) OK\n";

    // Test 5: 2kr3r/p1ppqpb1/bn2Qnp1/3PN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQ - 3 2
    Board b5("2kr3r/p1ppqpb1/bn2Qnp1/3PN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQ - 3 2");
    assert(b5.generate_legal_moves().size() == 44);
    cout << "Test 5 (44 moves) OK\n";

    // Test 6: rnb2k1r/pp1Pbppp/2p5/q7/2B5/8/PPPQNnPP/RNB1K2R w KQ - 3 9
    Board b6("rnb2k1r/pp1Pbppp/2p5/q7/2B5/8/PPPQNnPP/RNB1K2R w KQ - 3 9");
    assert(b6.generate_legal_moves().size() == 39);
    cout << "Test 6 (39 moves) OK\n";

    // Test 7: 2r5/3pk3/8/2P5/8/2K5/8/8 w - - 5 4
    Board b7("2r5/3pk3/8/2P5/8/2K5/8/8 w - - 5 4");
    assert(b7.generate_legal_moves().size() == 9);
    cout << "Test 7 (9 moves) OK\n";

    // -------------------------------------------------------
    // STARTING POSITION
    // -------------------------------------------------------

    // Test 8: Baseline — 16 pawn moves + 4 knight moves
    Board b8("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    assert(b8.generate_legal_moves().size() == 20);
    cout << "Test 8 (starting position, 20 moves) OK\n";

    // -------------------------------------------------------
    // CASTLING EDGE CASES
    // -------------------------------------------------------

    // Test 9: Kiwipete — hits castling, captures, checks all at once
    // Verified by Chess Programming Wiki: depth 1 = 48 moves
    Board b9("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    assert(b9.generate_legal_moves().size() == 48);
    cout << "Test 9 (Kiwipete, 48 moves) OK\n";

    // Test 10: Castling rights present, king not in check
    // White can castle queenside only, rook a1 active
    Board b10("4k2r/8/8/8/8/8/8/R3K3 w Q - 0 1");
    assert(b10.generate_legal_moves().size() == 16);
    cout << "Test 10 (castling rights, 16 moves) OK\n";

    // -------------------------------------------------------
    // EN PASSANT EDGE CASES
    // -------------------------------------------------------

    // Test 11: En passant available and legal
    // Black pawn c4 can push to c3 or capture en passant on d3
    // Black king h1: g1, g2, h2 = 3 king moves
    // Total: 2 pawn + 3 king = 5
    Board b11("8/8/8/8/2pP4/8/8/4K2k b - d3 0 1");
    assert(b11.generate_legal_moves().size() == 5);
    cout << "Test 11 (en passant available, 5 moves) OK\n";

    // Test 12: En passant ILLEGAL — would expose king to rook (pin)
    // White pawn c5, black pawn d5 just moved, white king a5, black rook h5
    // Taking en passant dxc6 removes both pawns from rank 5, exposing Ka5 to Rh5
    Board b12("8/8/8/K1Pp3r/8/8/8/7k w - d6 0 1");
    assert(b12.generate_legal_moves().size() == 6);
    cout << "Test 12 (en passant pin — illegal, 6 moves) OK\n";

    // -------------------------------------------------------
    // PROMOTION EDGE CASES
    // -------------------------------------------------------

    // Test 13: Pawn promotion, no captures available
    // White pawn e7 promotes: 4 moves (Q, R, B, N)
    // White king e1: d1, d2, e2, f1, f2 = 5 moves
    // Total: 9
    Board b13("4k3/4P3/8/8/8/8/8/4K3 w - - 0 1");
    assert(b13.generate_legal_moves().size() == 5);
    cout << "Test 13 (promotion push only, 5 moves) OK\n";

    // Test 14: Promotion with capture available
    // White pawn g7 can push to g8 (4) or capture Rh8 (4) = 8 pawn moves
    // White king a1: a2, b1, b2 = 3 moves
    // Total: 11
    Board b14("7r/6P1/8/8/8/8/8/K6k w - - 0 1");
    assert(b14.generate_legal_moves().size() == 11);
    cout << "Test 14 (promotion with capture, 11 moves) OK\n";

    // -------------------------------------------------------
    // CHECK EVASION
    // -------------------------------------------------------

    // Test 15: King in check from rook — only 4 evasions
    // White king e1, black rook e8 giving check
    // King can go: d1, d2, f1, f2 (not e2, still on e-file)
    Board b15("4r3/8/8/8/8/8/8/4K3 w - - 0 1");
    assert(b15.generate_legal_moves().size() == 4);
    cout << "Test 15 (king in check, 4 evasions) OK\n";

    // Test 16: Double check — ONLY king moves are legal
    // King on e1, in double check from Rd1 and Bf3
    // Must move king, cannot block or capture both attackers
    // Board b16("8/8/8/8/8/5b2/8/3rK2k w - - 0 1");
    // auto moves16 = b16.generate_legal_moves();
    // // King on e1: f2 only (d1 occupied by rook, e2/f1 attacked by bishop f3)
    // // All moves must be king moves — verify count > 0 and all are king moves
    // for (const Move& m : moves16) {
    //     int from = m.from();
    //     assert(from == 4); // every move must be the king moving
    // }
    // cout << "Test 16 (double check, only king moves) OK\n";

    // -------------------------------------------------------
    // STALEMATE
    // -------------------------------------------------------

    // Test 17: Stalemate — must return 0 legal moves, not crash
    // Black king a8 stalemated by white queen b6
    Board b17("k7/8/1Q6/8/8/8/8/7K b - - 0 1");
    assert(b17.generate_legal_moves().size() == 0);
    cout << "Test 17 (stalemate, 0 moves) OK\n";

    // -------------------------------------------------------
    // CPW POSITION 3 — verified reference position
    // -------------------------------------------------------

    // Test 18: Chess Programming Wiki Position 3
    // Verified depth 1 = 14 moves
    Board b18("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
    assert(b18.generate_legal_moves().size() == 14);
    cout << "Test 18 (CPW position 3, 14 moves) OK\n";

    cout << "\nAll tests passed!\n";
}
void run_all_tests(){
    test_start_position();
    test_empty_board();
    test_pawn_structure();
    test_complex_fen();
    test_occupancy_bitboards();
    test_edge_of_board();
    test_legal_moves_generation();
    
    cout << "\n====================\n";
    cout << "All tests passed!";
    cout << "\n====================\n";
}