import chess

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


board = chess.Board("3k4/3p4/8/K1P4r/8/8/8/8 b - - 0 1")

# Example:
# board.push_uci("d8c7")
# board.push_uci("e8f7")
# board.push_uci("f3h5")
depth = 1

divide(board, depth)