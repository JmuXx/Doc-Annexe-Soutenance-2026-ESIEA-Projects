#ifndef COORD_H
#define COORD_H

int coord_from_string(const char *coord);

void coord_to_string(int square, char out[3]);

int coord_is_valid(int square);

#endif
