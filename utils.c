#include "utils.h"
#include "defines.h"

#include <cstdlib>
#include <string.h> // strcpy strstr
#include <stdio.h> // printf sscanf
#include <math.h> // fabs

/*
*  Funcao que apaga a tela para compatibilidade Windows/Linus
*/
void clear_screen()
{
	#ifdef WINDOWS
		std::system("cls");
	#else
		// Assume POSIX
		std::system ("clear");
	#endif
}

/* 
* Rotina que conta os nos e atribui numeros a eles
*/
int numero(char *nome, char lista_int[][MAX_NOME+2], int *nv)
{
	int i = 0;
	int achou = 0;

	while (!achou && i <= *nv) {
		// Itera nos nos ate achar (quando acha, i tem o numero do no)
		if ( ! (achou = !strcmp(nome, lista_int[i]) ) ){
			i++;
		};
	}
	if ( !achou ) {
		// Verificou em todos os nos e nao achou
		if (*nv == MAX_NOS) {
			printf("O programa so aceita ate %d nos.\n", *nv);
			exit(EXCEEDED_MAX_NOS);
		}
		// Adicionando novo no
		(*nv)++;
		strcpy(lista_int[*nv],nome);
		return *nv;
	}
	else {
		return i; /* no ja conhecido */
	}
}

/*
* Resolucao de sistema de equacoes lineares.
* Metodo de Gauss-Jordan com condensacao pivotal
* Pag 20 - http://www.coe.ufrj.br/~acmq/cursos/CEII.pdf
*/ 
int resolversistema(double Yn[][MAX_NOS+2], int *nv)
{
	int i, j, l, a;
	double t, p;

	for (i=1; i <= *nv; i++) {
		t = 0.0;
		a = i;
		for (l=i; l <= *nv; l++) {
			if (fabs(Yn[l][i]) > fabs(t)) {
				a = l;
				t = Yn[l][i];
			}
		}
		if (i != a) {
			for (l=1; l <= *nv+1; l++) {
				p = Yn[i][l];
				Yn[i][l] = Yn[a][l];
				Yn[a][l] = p;
			}
		}
		if (fabs(t) < TOLG) {
			printf("Sistema singular\n");
			return 1;
		}
		for (j = *nv+1; j > 0; j--) {  /* Basta j>i em vez de j>0 */
			Yn[i][j] /= t;
			p = Yn[i][j];
			if (p != 0)  /* Evita operacoes com zero */
				for (l=1; l <= *nv; l++) {
					if (l != i)
						Yn[l][j] -= Yn[l][i] * p;
				}
		}
	}
	return 0;
}



