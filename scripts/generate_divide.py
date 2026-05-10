import chess
import time
def perft(board, depth):
    if depth == 0:
        return 1

    nodes = 0

    for move in board.legal_moves:
        board.push(move)
        nodes += perft(board, depth - 1)
        board.pop()

    return nodes


def divide(board, depth):
    total = 0

    for move in board.legal_moves:

        board.push(move)

        nodes = perft(board, depth - 1)

        board.pop()

        print(f"{move}: {nodes}")

        total += nodes

    print()
    print("Total:", total)


board = chess.Board("8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1")

# Example:
# board.push_uci("d8c7")
# board.push_uci("e8f7")
# board.push_uci("f3h5")
depth = 4
start = time.time()
divide(board, depth)
end = time.time()
print("Time taken",(end-start)*1000,"ms")