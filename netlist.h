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

int build_nodal_system(int ne, int *nv, device netlist[], double Yn[][MAX_NOS+2], double t_passo, double t_atual, double passos_por_ponto, int debug);

double capacitor_current(device *elemento, double solucao_anterior[][MAX_NOS+2], double t_passo, double t_atual, double passos_por_ponto);

double capacitor_resistance(device *elemento, double solucao_anterior[][MAX_NOS+2], double t_passo, double t_atual, double passos_por_ponto);

double inductor_resistance(device *elemento, double solucao_anterior[][MAX_NOS+2], double t_passo, double t_atual, double passos_por_ponto);

double inductor_voltage(device *elemento, double solucao_anterior[][MAX_NOS+2], double t_passo, double t_atual, double passos_por_ponto);

#endif