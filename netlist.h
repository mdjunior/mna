#ifndef _NETLIST_H_
#define _NETLIST_H_

#include "defines.h"

typedef struct device { /* Elemento do netlist */
	char nome[MAX_NOME];
	char subtipo; // usado nas fontes
	double valor;
	int a,b,c,d,x,y;
	double atraso, ciclos; // comum as fontes SIN/PULSE
	double nv_continuo, amplitude, frequencia, amortecimento, defasagem; // SIN
	double amplitude1, amplitude2, t_subida, t_descida, t_ligada, periodo; // PULSE
} device;

int process_device(char txt[], int ne, int *nv, char lista_int[][MAX_NOME+2], device *current, int debug);

#endif