#ifndef APSC143__PARSER_HELPERS_H
#define APSC143__PARSER_HELPERS_H

#include "tools.h"


bool is_file(char);
bool is_rank(char);
bool is_piece(char);

void skip_spaces();
void expect_char(char);

int parse_square();
int parse_promotion();
void parse_castle(struct chess_move *move);
bool parse_capture();

void reset_fields(struct chess_move *move);

int parse_disambiguation(char *from_file, char *from_rank);

#endif