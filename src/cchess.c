#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "argparse.h"
#include "cchess.h"
#include "helpers.h"
#ifdef _WIN32
#include <windows.h>
#endif

int round_number = 1;

int main(int argc, char const *argv[])
{
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif
    argparse(argc, argv);
    return 0;
}

void start_game(team first_player)
{
    if (!is_board_valid())
        generate_board();
    game_loop(first_player);
}

void end_game(team winner)
{
    printf("THE GAME IS OVER! ");
    if (winner != NONE)
        printf("THE WINNER IS\n");
    else
        printf("IT'S A DRAW.\n");

    // The ASCII texts are generated from https://budavariam.github.io/asciiart-text/.
    if (winner == WHITE)
    {
        printf(" _    _ _   _ _____ _____ _____\n \
| |  | | | | |_   _|_   _|  ___|\n \
| |  | | |_| | | |   | | | |__  \n \
| |/\\| |  _  | | |   | | |  __| \n \
\\  /\\  / | | |_| |_  | | | |___ \n \
 \\/  \\/\\_| |_/\\___/  \\_/ \\____/ \n\n");
    }
    else if (winner == BLACK)
    {
        printf("______ _       ___  _____  _   __\n \
| ___ \\ |     / _ \\/  __ \\| | / /\n \
| |_/ / |    / /_\\ \\ /  \\/| |/ / \n \
| ___ \\ |    |  _  | |    |    \\ \n \
| |_/ / |____| | | | \\__/\\| |\\  \\ \n \
\\____/\\_____/\\_| |_/\\____/\\_| \\_/ \n\n");
    }
}

void stop_game(void)
{
    printf("The game stopped.\n");
    exit(0);
}

void game_loop(team first_player)
{
    while (1)
    {
        team current_player = round_number % 2 == 0 ? BLACK : WHITE;
        if (first_player == BLACK)
            current_player = opposite_team(current_player);

        printf("\n");
        print_board(current_player);
        printf("\n");
        position current_pos, new_pos;
        if (is_over())
        {
            end_game(get_winner_team());
            break;
        }
        while (ask_for_move(current_player, &current_pos, &new_pos) != 0)
            ;

        make_move(current_pos, new_pos);
        position p;
        if ((p = get_promotable(current_player)).x != -1)
        {
            char promote_to;
            while ((promote_to = ask_for_promotion(current_player)) == -1)
                ;
            promote_to = tolower(promote_to);
            switch (promote_to)
            {
            case 'k':
                replace_position(p, current_player == WHITE ? W_KNIGHT : B_KNIGHT);
                break;
            case 'b':
                replace_position(p, current_player == WHITE ? W_BISHOP : B_BISHOP);
                break;
            case 'r':
                replace_position(p, current_player == WHITE ? W_ROOK : B_ROOK);
                break;
            case 'q':
                replace_position(p, current_player == WHITE ? W_QUEEN : B_QUEEN);
                break;

            default:
                break;
            }
        }
        round_number++;
    }
}

void save_game()
{
    printf("This will create the file 'cchess_game.txt' in the current directory or overwrite it if it exists. Are you sure? [y/N]\n");
    char c = getchar();
    if (c != 'y')
        return;
    FILE *file = fopen("cchess_game.txt", "w");
    extract_board(file);
    fprintf(file, "%d", round_number);
    printf("The game has been saved to the file cchess_game.txt\nTo resume, use the '-r' flag and type the file name next time you run the program.\n");
    exit(0);
}

void resume_game(char const *filename)
{
    FILE *file = fopen(filename, "r");
    char next_player[10];
    if (!file || load_board(file) != 0 || !fgets(next_player, 10, file))
    {
        printf("Couldn't load the file.\n");
        return;
    }
    char *endptr;
    round_number = strtol(next_player, &endptr, 10);
    if (endptr == next_player)
        round_number = 1;
}

position parse_position(char *str)
{
    if (((str[0] < 'a' || str[0] > 'h') && (str[0] < 'A' || str[0] > 'H')) || str[1] < '1' || str[1] > '8')
        return POS(-1, -1);
    return POS((str[0] >= 'a' && str[0] <= 'h') ? str[0] - 'a' : str[0] - 'A', str[1] - '1');
}

int ask_for_move(team player, position *current_pos, position *new_pos)
{
    printf("%s player, enter your move: ", player == BLACK ? "Black" : "White");
    char buffer[7];
    if (!fgets(buffer, sizeof(buffer), stdin))
        stop_game();
    *current_pos = parse_position(buffer);
    *new_pos = parse_position(buffer[2] == ' ' ? buffer + 3 : buffer + 2);

    if (str_equal(buffer, "save\n"))
    {
        save_game();
        while (getchar() != '\n')
            ;
        return -1;
    }

    if (current_pos->x == -1 || new_pos->x == -1 || buffer[strlen(buffer) - 1] != '\n')
    {
        printf("Incorrect syntax. Enter your move in the 'current-position new-position' format. For example, 'e2 e4'.\n");
        return -1;
    }
    if (get_piece_team(*current_pos) != player)
    {
        printf("Invalid current-position. You must choose a current-position that contains one of your pieces.\n");
        return -1;
    }
    if (!can_move_to(*current_pos, *new_pos))
    {
        printf("This isn't a valid move in chess. Please choose a valid move. \nYou can learn the rules of chess at https://en.wikipedia.org/wiki/Rules_of_chess.\n");
        return -1;
    }
    return 0;
}

char ask_for_promotion(team player)
{
    printf("%s player, which piece will you promote your pawn to [k/b/r/q]: ", player == BLACK ? "Black" : "White");
    char buffer[3];
    if (!fgets(buffer, sizeof(buffer), stdin))
        stop_game();
    if (buffer[1] != '\n')
    {
        printf("Incorrect syntax. Enter one of the possible options [k/b/r/q] (k = knight, b = bishop, r = rook, q = queen).\n");
        return -1;
    }
    return buffer[0];
}