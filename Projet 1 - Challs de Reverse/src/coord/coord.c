#include <ctype.h>
#include <stddef.h>

#include "coord.h"

int coord_from_string(const char *coord)
{
    if(coord == NULL)
        return -1;

    char file = tolower(coord[0]);
    char rank = coord[1];

    if(file < 'a' || file > 'h')
        return -1;

    if(rank < '1' || rank > '8')
        return -1;

    return (rank - '1') * 16 + (file - 'a');
}

void coord_to_string(int square, char out[3])
{
    out[0] = 'a' + (square & 7);
    out[1] = '1' + (square >> 4);
    out[2] = '\0';
}

int coord_is_valid(int square)
{
    return !(square & 0x88);
}
