#include <stdio.h>
#include <string.h>

#include "game/game2.h"

#include "attack/attack.h"
#include "security/antidebug.h"
#include "board/board.h"
#include "challenge/challenge2.h"
#include "coord/coord.h"
#include "move/legalmove.h"
#include "move/makemove.h"
#include "move/move.h"
#include "move/movelist.h"
#include "piece/piece.h"
#include "search/search.h"


#define HISTORY_SIZE 3
#define AI_DEPTH 6


typedef struct
{
    char move_text[8];

    char event_text[64];

    PieceColor color;

    int move_number;

} HistoryEntry;


static const char *color_name(PieceColor color)
{
    if (color == COLOR_WHITE)
    {
        return "Blanc";
    }


    if (color == COLOR_BLACK)
    {
        return "Bleu";
    }


    return "Inconnu";
}



static void append_event(
    char event_text[64],
    const char *text)
{
    if (event_text[0] != '\0')
    {
        strcat(
            event_text,
            " | "
        );
    }


    strcat(
        event_text,
        text
    );
}


static void build_move_event(
    Board *board,
    const Move *move,
    char event_text[64])
{
    event_text[0] =
        '\0';


    if (move_is_castle(move))
    {
        if (move->to ==
                coord_from_string("g1") ||
            move->to ==
                coord_from_string("g8"))
        {
            append_event(
                event_text,
                "PETIT ROQUE"
            );
        }
        else
        {
            append_event(
                event_text,
                "GRAND ROQUE"
            );
        }
    }


    if (move_is_capture(move))
    {
        append_event(
            event_text,
            "CAPTURE"
        );
    }


    if (move_is_enpassant(move))
    {
        append_event(
            event_text,
            "EN PASSANT"
        );
    }


    if (move_is_promotion(move))
    {
        switch (move->promotion)
        {
            case PROMOTION_QUEEN:

                append_event(
                    event_text,
                    "PROMOTION DAME"
                );

                break;


            case PROMOTION_ROOK:

                append_event(
                    event_text,
                    "PROMOTION TOUR"
                );

                break;


            case PROMOTION_BISHOP:

                append_event(
                    event_text,
                    "PROMOTION FOU"
                );

                break;


            case PROMOTION_KNIGHT:

                append_event(
                    event_text,
                    "PROMOTION CAVALIER"
                );

                break;


            default:

                append_event(
                    event_text,
                    "PROMOTION"
                );

                break;
        }
    }


    if (board_is_checkmate(
            board
        ))
    {
        append_event(
            event_text,
            "ECHEC ET MAT"
        );
    }


    else if (board_is_in_check(
                 board,
                 board->side_to_move
             ))
    {
        append_event(
            event_text,
            "ECHEC"
        );
    }
}


static void history_add(
    HistoryEntry history[HISTORY_SIZE],
    int *history_count,
    int *total_moves,
    const char *move_text,
    const char *event_text,
    PieceColor color)
{
    (*total_moves)++;


    if (*history_count <
        HISTORY_SIZE)
    {
        HistoryEntry *entry =
            &history[*history_count];


        strcpy(
            entry->move_text,
            move_text
        );


        strcpy(
            entry->event_text,
            event_text
        );


        entry->color =
            color;


        entry->move_number =
            *total_moves;


        (*history_count)++;


        return;
    }


    history[0] =
        history[1];


    history[1] =
        history[2];


    strcpy(
        history[2].move_text,
        move_text
    );


    strcpy(
        history[2].event_text,
        event_text
    );


    history[2].color =
        color;


    history[2].move_number =
        *total_moves;
}


static void print_history(
    const HistoryEntry history[HISTORY_SIZE],
    int history_count)
{
    const char *WHITE_COLOR =
        "\033[1;97m";

    const char *BLUE_COLOR =
        "\033[0;34m";

    const char *EVENT_COLOR =
        "\033[1;33m";

    const char *RESET_COLOR =
        "\033[0m";


    printf(
        "Derniers coups :\n"
    );


    if (history_count == 0)
    {
        printf(
            "  Aucun coup joue.\n\n"
        );


        return;
    }


    for (int i = 0;
         i < history_count;
         i++)
    {
        const HistoryEntry *entry =
            &history[i];


        const char *color_code;
        const char *color_text;


        if (entry->color ==
            COLOR_WHITE)
        {
            color_code =
                WHITE_COLOR;

            color_text =
                "Blanc";
        }
        else
        {
            color_code =
                BLUE_COLOR;

            color_text =
                "Bleu";
        }


        printf(
            "  #%d  %s%-5s%s : %s",
            entry->move_number,
            color_code,
            color_text,
            RESET_COLOR,
            entry->move_text
        );


        if (entry->event_text[0] != '\0')
        {
            printf(
                "   -> %s%s%s",
                EVENT_COLOR,
                entry->event_text,
                RESET_COLOR
            );
        }


        printf(
            "\n"
        );
    }


    printf(
        "\n"
    );
}


static void clear_screen(void)
{
    printf(
        "\033[2J\033[H"
    );


    fflush(
        stdout
    );
}


static int read_line(
    char *buffer,
    int size)
{
    if (fgets(
            buffer,
            size,
            stdin
        ) == NULL)
    {
        return 0;
    }


    char *newline =
        strchr(
            buffer,
            '\n'
        );


    if (newline != NULL)
    {
        *newline =
            '\0';
    }
    else
    {
        int c;


        while ((c = getchar()) != '\n' &&
               c != EOF)
        {
        }
    }


    return 1;
}


static void print_game_header(void)
{
    printf(
        "========================================\n"
    );


    printf(
        "      PolymorphiChess Challenge 2\n"
    );


    printf(
        "========================================\n\n"
    );


    printf(
        "Tu joues les Blancs contre l'IA.\n"
    );


    printf(
        "La vulnerabilite du canal de communication\n"
        "du precedent challenge a ete corrigee.\n\n"
    );


    printf(
        "Le nouveau systeme surveille maintenant\n"
        "directement le comportement de la partie.\n\n"
    );


    printf(
        "Format des coups : e2e4\n"
    );


    printf(
        "En cas de promotion, un choix "
        "te sera demande.\n\n"
    );


    printf(
        "Commande :\n"
    );


    printf(
        "  ff : abandonner\n\n"
    );
}


static int parse_move_input(
    const char *text,
    int *from,
    int *to)
{
    if (text == NULL ||
        from == NULL ||
        to == NULL)
    {
        return 0;
    }


    if (strlen(text) != 4)
    {
        return 0;
    }


    char from_text[3] =
    {
        text[0],
        text[1],
        '\0'
    };


    char to_text[3] =
    {
        text[2],
        text[3],
        '\0'
    };


    int parsed_from =
        coord_from_string(
            from_text
        );


    int parsed_to =
        coord_from_string(
            to_text
        );


    if (parsed_from == -1 ||
        parsed_to == -1)
    {
        return 0;
    }


    *from =
        parsed_from;


    *to =
        parsed_to;


    return 1;
}


static void move_to_text(
    const Move *move,
    char text[8])
{
    char from[3];
    char to[3];


    coord_to_string(
        move->from,
        from
    );


    coord_to_string(
        move->to,
        to
    );


    snprintf(
        text,
        8,
        "%s%s",
        from,
        to
    );
}


static const Move *find_legal_move(
    const MoveList *list,
    int from,
    int to)
{
    for (int i = 0;
         i < movelist_count(list);
         i++)
    {
        const Move *move =
            movelist_get(
                list,
                i
            );


        if (move->from == from &&
            move->to == to &&
            !move_is_promotion(move))
        {
            return move;
        }
    }


    return NULL;
}

static int has_promotion_move(
    const MoveList *list,
    int from,
    int to)
{
    for (int i = 0;
         i < movelist_count(list);
         i++)
    {
        const Move *move =
            movelist_get(
                list,
                i
            );


        if (move->from == from &&
            move->to == to &&
            move_is_promotion(move))
        {
            return 1;
        }
    }


    return 0;
}

static const Move *find_promotion_move(
    const MoveList *list,
    int from,
    int to,
    PromotionPiece promotion)
{
    for (int i = 0;
         i < movelist_count(list);
         i++)
    {
        const Move *move =
            movelist_get(
                list,
                i
            );


        if (move->from == from &&
            move->to == to &&
            move_is_promotion(move) &&
            move->promotion == promotion)
        {
            return move;
        }
    }


    return NULL;
}

static PromotionPiece ask_promotion(void)
{
    char input[32];


    while (1)
    {
        printf(
            "\nPromotion :\n"
            "  q = Dame\n"
            "  r = Tour\n"
            "  b = Fou\n"
            "  n = Cavalier\n"
            "\n"
            "Votre choix > "
        );


        if (!read_line(
                input,
                sizeof(input)
            ))
        {
            return PROMOTION_QUEEN;
        }


        if (strlen(input) != 1)
        {
            printf(
                "Choix invalide. Reessayez.\n"
            );


            continue;
        }


        switch (input[0])
        {
            case 'q':
            case 'Q':

                return PROMOTION_QUEEN;


            case 'r':
            case 'R':

                return PROMOTION_ROOK;


            case 'b':
            case 'B':

                return PROMOTION_BISHOP;


            case 'n':
            case 'N':

                return PROMOTION_KNIGHT;


            default:

                printf(
                    "Choix invalide. Reessayez.\n"
                );


                break;
        }
    }
}


static void admin_panel(
    Challenge2State *challenge)
{
    char password[128];


    printf(
        "\n"
        "========================================\n"
        "       POLYMORPHICHESS SECURITY\n"
        "========================================\n"
        "\n"
    );


    printf(
        "Anomalie comportementale detectee.\n\n"
    );


    printf(
        "Selon le modele de l'IA,\n"
        "la partie aurait deja du etre terminee.\n\n"
    );


    printf(
        "Pour continuer, une authentification\n"
        "administrateur est requise.\n\n"
    );


    printf(
        "Admin password > "
    );


    if (!read_line(
            password,
            sizeof(password)
        ))
    {
        printf(
            "\nErreur de lecture.\n"
        );


        return;
    }


    if (!challenge2_check_password(
            challenge,
            password
        ))
    {
        printf(
            "\n"
            "ACCESS DENIED\n"
            "\n"
        );


        return;
    }


    printf(
        "\n"
        "ACCESS GRANTED\n"
        "\n"
    );


    challenge2_print_reward(
        challenge
    );
}


static int play_ai_move(
    Board *board,
    HistoryEntry history[HISTORY_SIZE],
    int *history_count,
    int *total_moves,
    Challenge2State *challenge)
{
    Move calculated_move;


    printf(
        "L'ordinateur reflechit...\n"
    );


    fflush(
        stdout
    );


    if (!search_best_move(
            board,
            AI_DEPTH,
            &calculated_move
        ))
    {
        return 0;
    }


    PieceColor ai_color =
        board->side_to_move;


    challenge2_record_move(
        challenge,
        calculated_move.from,
        calculated_move.to
    );


    Undo undo;


    make_move(
        board,
        &calculated_move,
        &undo
    );


    char move_text[8];


    move_to_text(
        &calculated_move,
        move_text
    );


    char event_text[64];


    build_move_event(
        board,
        &calculated_move,
        event_text
    );


    history_add(
        history,
        history_count,
        total_moves,
        move_text,
        event_text,
        ai_color
    );


    return 1;
}


void game2_start(void)
{
    Board board;


    board_init(
        &board
    );

    Challenge2State challenge;


    challenge2_init(
        &challenge
    );


    HistoryEntry history[HISTORY_SIZE] =
    {
        0
    };


    int history_count =
        0;


    int total_moves =
        0;

    while (1)
    {
        clear_screen();


        print_game_header();


        board_print(
            &board
        );


        print_history(
            history,
            history_count
        );


        if (board_is_checkmate(
                &board
            ))
        {
            PieceColor winner =
                board.side_to_move ==
                    COLOR_WHITE
                ? COLOR_BLACK
                : COLOR_WHITE;


            printf(
                "\nEchec et mat.\n"
            );


            printf(
                "%s gagne la partie.\n",
                color_name(winner)
            );


            return;
        }


        if (board_is_stalemate(
                &board
            ))
        {
            printf(
                "\nPat.\n"
            );


            return;
        }

        if (board_is_in_check(
                &board,
                board.side_to_move
            ))
        {
            printf(
                "%s est en echec.\n\n",
                color_name(
                    board.side_to_move
                )
            );
        }


        if (board.side_to_move ==
            COLOR_BLACK)
        {
            if (!play_ai_move(
                    &board,
                    history,
                    &history_count,
                    &total_moves,
                    &challenge
                ))
            {
                printf(
                    "\nErreur pendant le tour "
                    "de l'ordinateur.\n"
                );


                return;
            }

            if (challenge2_is_ready(
                    &challenge
                ))
            {
                clear_screen();


                print_game_header();


                board_print(
                    &board
                );


                print_history(
                    history,
                    history_count
                );

                if (antidebug_detected())
                {
                    printf(
                        "\nRuntime integrity violation.\n"
                    );

                    return;
                }
                admin_panel(
                    &challenge
                );


                return;
            }


            continue;
        }

        MoveList legal_moves;


        legalmove_generate(
            &board,
            &legal_moves
        );


        int from =
            -1;


        int to =
            -1;


        char input[64];

        while (1)
        {
            printf(
                "\nBlanc > "
            );


            if (!read_line(
                    input,
                    sizeof(input)
                ))
            {
                return;
            }


            if (strcmp(
                    input,
                    "ff"
                ) == 0)
            {
                printf(
                    "\nTu abandonnes.\n"
                    "L'ordinateur gagne.\n"
                );


                return;
            }


            if (parse_move_input(
                    input,
                    &from,
                    &to
                ))
            {
                break;
            }


            printf(
                "Format invalide.\n"
                "Utilise par exemple : e2e4\n"
                "Reessaie.\n"
            );
        }

        const Move *selected_move =
            NULL;


        if (has_promotion_move(
                &legal_moves,
                from,
                to
            ))
        {
            PromotionPiece promotion =
                ask_promotion();


            selected_move =
                find_promotion_move(
                    &legal_moves,
                    from,
                    to,
                    promotion
                );
        }
        else
        {
            selected_move =
                find_legal_move(
                    &legal_moves,
                    from,
                    to
                );
        }

        if (selected_move == NULL)
        {
            printf(
                "\nCoup illegal : %s\n",
                input
            );


            printf(
                "Partie arretee.\n"
            );


            return;
        }

        Move move =
            *selected_move;


        PieceColor player_color =
            board.side_to_move;

        challenge2_record_move(
            &challenge,
            move.from,
            move.to
        );

        Undo undo;

        make_move(
            &board,
            &move,
            &undo
        );

        char event_text[64];

        build_move_event(
            &board,
            &move,
            event_text
        );

        history_add(
            history,
            &history_count,
            &total_moves,
            input,
            event_text,
            player_color
        );

        if (challenge2_is_ready(
                &challenge
            ))
        {
            clear_screen();


            print_game_header();


            board_print(
                &board
            );


            print_history(
                history,
                history_count
            );
            if (antidebug_detected())
            {
                printf(
                    "\nRuntime integrity violation.\n"
                );

                return;
            }
            admin_panel(
                &challenge
            );


            return;
        }
    }
}
