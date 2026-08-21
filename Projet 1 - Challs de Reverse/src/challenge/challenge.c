#include <stdio.h>
#include <string.h>

#include "challenge/challenge.h"

#include "coord/coord.h"
#include "move/legalmove.h"
#include "move/movelist.h"


static const unsigned char payload_blob[] =
{
    0x59,
    0x55,
    0x69,
    0xDA,
    0xB7,
    0x93,
    0xFE,
    0x76,
    0x21,
    0xAB,
    0xB0,
    0xCA,
    0x62,
    0x7A,
    0xF6,
    0x07,
    0x93,
    0xB0,
    0x62,
    0x81
};


#define PAYLOAD_SIZE \
    ((int)sizeof(payload_blob))


#if defined(__GNUC__) || defined(__clang__)
__attribute__((noinline))
#endif
static void transform_payload(char *output)
{
    unsigned int state = 0x6D2B79F5u;

    for (int i = 0; i < PAYLOAD_SIZE; i++)
    {
        state ^=
            state << 13;

        state ^=
            state >> 17;

        state ^=
            state << 5;

        output[i] = (char)(payload_blob[i] ^ (state & 0xFF));
    }
    output[PAYLOAD_SIZE] = '\0';
}


int challenge_reset_move_file(void)
{
    FILE *file =
        fopen(
            CHALLENGE_MOVE_FILE,
            "w"
        );


    if (file == NULL)
    {
        return 0;
    }


    fclose(file);


    return 1;
}


int challenge_write_ai_move(const Move *move)
{
    if (move == NULL)
    {
        return 0;
    }
    FILE *file = fopen(CHALLENGE_MOVE_FILE,"a");
    if (file == NULL)
    {
        return 0;
    }
    char from[3];
    char to[3];
    coord_to_string(move->from,from);
    coord_to_string(move->to,to);
    long position =
        ftell(file);
    
    if (position > 0)
    {
        fprintf(
            file,
            " "
        );
    }
    fprintf(
        file,
        "%s%s",
        from,
        to);
    fclose(file);

    return 1;
}


static const Move *find_move_by_coords(
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


        if (move->from != from ||
            move->to != to)
        {
            continue;
        }


        if (move_is_promotion(move))
        {
            if (move->promotion ==
                PROMOTION_QUEEN)
            {
                return move;
            }


            continue;
        }


        return move;
    }


    return NULL;
}


int challenge_read_ai_move(Board *board,
                           Move *move)
{
    if (board == NULL || move == NULL)
    {
        return 0;
    }
    FILE *file = fopen(CHALLENGE_MOVE_FILE,"r");

    if (file == NULL)
    {
        return 0;
    }

    char text[512];

    if (fgets(
            text,
            sizeof(text),
            file
        ) == NULL)
    {
        fclose(file);
        return 0;
    }

    fclose(file);

    text[
        strcspn(
            text,
            "\r\n"
        )
    ] = '\0';


    char *last_move =
        strrchr(
            text,
            ' '
        );


    if (last_move != NULL)
    {
        last_move++;
    }
    else
    {
        last_move =
            text;
    }


    if (strlen(last_move) != 4)
    {
        return 0;
    }


    char from_text[3] =
    {
        last_move[0],
        last_move[1],
        '\0'
    };


    char to_text[3] =
    {
        last_move[2],
        last_move[3],
        '\0'
    };


    int from =
        coord_from_string(
            from_text
        );


    int to =
        coord_from_string(
            to_text
        );


    if (from == -1 ||
        to == -1)
    {
        return 0;
    }


    MoveList legal_moves;


    legalmove_generate(
        board,
        &legal_moves
    );


    const Move *legal_move =
        find_move_by_coords(
            &legal_moves,
            from,
            to
        );


    if (legal_move == NULL)
    {
        return 0;
    }


    *move =
        *legal_move;


    return 1;
}


void challenge_print_reward(void)
{
    char output[PAYLOAD_SIZE + 1];


    transform_payload(
        output
    );


    puts(
        output
    );


    for (int i = 0;
         i < PAYLOAD_SIZE;
         i++)
    {
        output[i] =
            '\0';
    }
}