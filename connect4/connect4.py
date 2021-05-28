import enum
import random
import readline  # noqa: F401
import time

WIDTH = 7
HEIGHT = 6


def play():
    board = [[" " for _ in range(HEIGHT)] for _ in range(WIDTH)]
    while True:
        print_board(board)
        human_move = get_human_move(board)
        make_move(board, human_move, "X")
        result = check_board(board)
        if result == GameResult.WIN:
            print("You win!")
            break
        elif result == GameResult.DRAW:
            print("It is a draw.")
            break

        print_board(board)
        print("Computer is thinking.")
        time.sleep(0.5)
        computer_move = get_computer_move(board)
        make_move(board, computer_move, "O")
        result = check_board(board)
        if result == GameResult.WIN:
            print_board(board)
            print("The computer wins.")
            break
        elif result == GameResult.DRAW:
            print_board(board)
            print("It is a draw.")
            break


def get_human_move(board):
    while True:
        x = input("? ")
        try:
            x = int(x)
        except ValueError:
            continue

        if not (1 <= x <= WIDTH):
            continue

        if board[x - 1][0] != " ":
            continue

        return x - 1


def get_computer_move(board):
    while True:
        x = random.randint(1, WIDTH) - 1
        if board[x][0] != " ":
            continue
        return x


def make_move(board, move, mark):
    column = board[move]
    for i in reversed(range(len(column))):
        if column[i] == " ":
            column[i] = mark
            return

    raise ConnectFourError("Column is already full.")


def check_board(board):
    unfilled = 0
    for row in range(HEIGHT):
        for column in range(WIDTH):
            mark = board[column][row]
            if mark == " ":
                unfilled += 1
                continue

            if check_square(board, column, row):
                return GameResult.WIN

    return GameResult.DRAW if unfilled == 0 else GameResult.UNFINISHED


def check_square(board, column, row):
    return (
        check_square_horizontal(board, column, row)
        or check_square_vertical(board, column, row)
        or check_square_diagonal_up(board, column, row)
        or check_square_diagonal_down(board, column, row)
    )


def check_square_horizontal(board, column, row):
    if column + 3 >= WIDTH:
        return False

    mark = board[column][row]
    for i in range(1, 4):
        if board[column + i][row] != mark:
            return False

    return True


def check_square_vertical(board, column, row):
    if row < 3:
        return False

    mark = board[column][row]
    for i in range(1, 4):
        if board[column][row - i] != mark:
            return False

    return True


def check_square_diagonal_up(board, column, row):
    if row + 3 >= HEIGHT or column + 3 >= WIDTH:
        return False

    mark = board[column][row]
    for i in range(1, 4):
        if board[column + i][row + i] != mark:
            return False

    return True


def check_square_diagonal_down(board, column, row):
    if row < 3 or column + 3 >= WIDTH:
        return False

    mark = board[column][row]
    for i in range(1, 4):
        if board[column + i][row - i] != mark:
            return False

    return True


def print_board(board):
    decorative_row = "o" + ("-" * (WIDTH * 2 - 1)) + "o"
    print(decorative_row)
    for row in range(HEIGHT):
        print("|", end="")
        for column in range(WIDTH):
            mark = board[column][row]
            if mark == "O":
                mark = "\033[33mO\033[0m"
            elif mark == "X":
                mark = "\033[31mO\033[0m"
            print(mark + "|", end="")
        print()
    print(decorative_row)
    # Note: This will only display correctly if WIDTH is a single-digit number.
    print(" " + " ".join(map(str, range(1, WIDTH + 1))))
    print()


class GameResult(enum.Enum):
    WIN = enum.auto()
    DRAW = enum.auto()
    UNFINISHED = enum.auto()


class ConnectFourError(Exception):
    pass


play()
