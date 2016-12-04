#ifndef _UTILS_H_
#define _UTILS_H_

#include "defines.h"

void clear_screen();

int resolversistema(double Yn[][MAX_NOS+2], int *nv);

int numero(char *nome, char lista_int[][MAX_NOME+2], int *nv);

int create_filename(char filename[MAX_FILENAME], char filename_result[MAX_FILENAME]);

#endif