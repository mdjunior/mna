#ifndef _NETLIST_H_
#define _NETLIST_H_

#include "defines.h"

typedef struct device { /* Elemento do netlist */
	char nome[MAX_NOME];
	double valor;
	int a,b,c,d,x,y;
} device;

int process_device(char txt[], int ne, int *nv, char lista_int[][MAX_NOME+2], device *current, int debug);

#endif