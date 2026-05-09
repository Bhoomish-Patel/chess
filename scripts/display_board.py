def display_bitboard(board):
    """
    Display a 64-bit board as an 8x8 grid.
    Bit 0 represents the bottom-left corner (a1 in chess notation).
    """
    for rank in range(7, -1, -1):  # From rank 8 down to rank 1
        for file in range(8):  # From file a to file h
            bit_index = rank * 8 + file
            bit_value = (board >> bit_index) & 1
            print(bit_value, end=' ')
        print()

if __name__ == "__main__":
    # Example usage - read a 64-bit number from user
    board = int(input("Enter a 64-bit number (0-18446744073709551615): "))
    display_bitboard(board)
