#include "defines.h"
#include "utils.h"
#include "netlist.h"

#include <stdio.h> // printf sscanf
#include <string.h> // strcpy strstr strlen
#include <ctype.h> // toupper 

int process_device(char txt[], int ne, int *nv, char lista_int[][MAX_NOME+2], device *current, int debug)
{
	int frv = 0;
	char *p;
	char tipo, na[MAX_NOME], nb[MAX_NOME], nc[MAX_NOME], nd[MAX_NOME], subtipo[MAX_NOME];
	na[MAX_NOME-1] = NULL;
	nb[MAX_NOME-1] = NULL;
	nc[MAX_NOME-1] = NULL;
	nd[MAX_NOME-1] = NULL;
	subtipo[MAX_NOME-1] = NULL;

	// Identificando tipo de elemento
	txt[0] = toupper(txt[0]);
	tipo = txt[0];

	frv = sscanf(txt, RESOLVE_ONE_STRING(MAX_NOME), current->nome);
	// Vamos fazer a validacao
	// Verificando se o nome foi atribuido e estava dentro dos limites
	if (frv != 1 || current->nome[MAX_NOME-1] != '\0') {
		// Verificando se comentario ou configuracao de analise
		if (tipo == '.' || tipo == '*') {
			return(SPECIAL_LINE);
		} else {
			printf("Nao foi possivel ler nome do elemento da %d linha. %d caracteres de tamanho maximo (%i).\n", ne, MAX_NOME, frv);
			return(EXCEEDED_MAX_NOME);
		}
	}

	p = txt + strlen(current->nome);
	/* Inicio dos parametros */
	// R0605 6 5 1
	// C0405 4 5 1
	// L0200 2 0 0.25E-3
	if (tipo == 'R' || tipo == 'C' || tipo == 'L') {
		frv = sscanf(p, RESOLVE_TWO_TERMINALS(MAX_NOME), na, nb, &current->valor);
		// Verificando se os valores foram atribuidos e estavam dentro dos limites
		if (frv != 3 || na[MAX_NOME-1] != '\0' || nb[MAX_NOME-1] != '\0') {
			printf("Nao foi possivel ler elemento da %d linha. %d caracteres de tamanho maximo.\n", ne, MAX_NOME);
			return(EXCEEDED_MAX_NOME);
		}

		if (debug) printf("%s %s %s %g\n", current->nome, na, nb, current->valor);
		current->a = numero(na, lista_int, nv);
		current->b = numero(nb, lista_int, nv);
	}
	// VCC 3 0 DC 6
	// V0600 6 0 SIN 0 0.01 10 0 0 1 5
	// V0200 2 0 PULSE 10 0.1 0 0 0 2 2 1
	else if (tipo == 'V' || tipo == 'I') {
		frv = sscanf(p, RESOLVE_SOURCE(MAX_NOME), na, nb, subtipo);
		// Verificando se os valores foram atribuidos e estavam dentro dos limites
		if (frv == 3) {
			if (na[MAX_NOME-1] != '\0' || nb[MAX_NOME-1] != '\0' || subtipo[MAX_NOME-1] != '\0') {
				printf("Nao foi possivel ler elemento da %d linha. %d caracteres de tamanho maximo(%c).\n", ne, MAX_NOME, subtipo[MAX_NOME-1]);
				return(EXCEEDED_MAX_NOME);
			}
			if (subtipo[0] == 'D' || subtipo[0] == 'S' || subtipo[0] == 'P') {
				current->subtipo = subtipo[0];
				current->a = numero(na, lista_int, nv);
				current->b = numero(nb, lista_int, nv);
			} else {
				printf("Nao foi possivel ler fonte da %d linha. Tipo nao reconhecido (%s).\n", ne, subtipo);
				return(UNKNOWN_SOURCE);
			}

			if (current->subtipo == 'D') {
				frv = sscanf(p, RESOLVE_SOURCE_DC(MAX_NOME), na, nb, &current->valor);
			} else if (current->subtipo == 'S') {
				frv = sscanf(p, RESOLVE_SOURCE_SIN(MAX_NOME), na, nb,
					&current->nv_continuo,&current->amplitude,&current->frequencia,
					&current->atraso,&current->amortecimento,&current->defasagem,&current->ciclos);
				current->valor = current->nv_continuo;

				if (current->amplitude == 0 || current->frequencia == 0 || current->ciclos == 0) {
					printf("Nao foi possivel ler fonte da %d linha, sua amplitude, frequencia ou numero de ciclos e zero.\n", ne);
					return(UNKNOWN_SOURCE);
				}

			} else if (current->subtipo == 'P') {
				frv = sscanf(p, RESOLVE_SOURCE_PULSE(MAX_NOME), na, nb,
					&current->amplitude1,&current->amplitude2,&current->atraso,&current->t_subida,
					&current->t_descida,&current->t_ligada,&current->periodo,&current->ciclos);
				current->valor = current->amplitude1;
			}

		}

		if (debug) printf("%s %s %s %g %c\n", current->nome, na, nb, current->valor, current->subtipo);
	}
	else if (tipo == 'G' || tipo == 'E' || tipo == 'F' || tipo == 'H' || tipo == '$') {
		frv = sscanf(p, RESOLVE_FOUR_TERMINALS(MAX_NOME), na, nb, nc, nd, &current->valor);
		// Verificando se os valores foram atribuidos e estavam dentro dos limites
		if (frv != 5 || na[MAX_NOME-1] != '\0' || nb[MAX_NOME-1] != '\0' || nc[MAX_NOME-1] != '\0' || nd[MAX_NOME-1] != '\0' ) {
			printf("Nao foi possivel ler elemento da %d linha. %d caracteres de tamanho maximo.\n", ne, MAX_NOME);
			return(EXCEEDED_MAX_NOME);
		}

		if (debug) printf("%s %s %s %s %s %g\n", current->nome, na, nb, nc, nd, current->valor);
		current->a = numero(na, lista_int, nv);
		current->b = numero(nb, lista_int, nv);
		current->c = numero(nc, lista_int, nv);
		current->d = numero(nd, lista_int, nv);
	}
	else if (tipo == 'O') {
		frv = sscanf(p, RESOLVE_AMPOP(MAX_NOME), na, nb, nc, nd);
		// Verificando se os valores foram atribuidos e estavam dentro dos limites
		if (frv != 4 || na[MAX_NOME-1] != '\0' || nb[MAX_NOME-1] != '\0' || nc[MAX_NOME-1] != '\0' || nd[MAX_NOME-1] != '\0' ) {
			printf("Nao foi possivel ler elemento da %d linha. %d caracteres de tamanho maximo.\n", ne, MAX_NOME);
			return(EXCEEDED_MAX_NOME);
		}

		if (debug) printf("%s %s %s %s %s\n", current->nome, na, nb, nc, nd);
		current->a = numero(na, lista_int, nv);
		current->b = numero(nb, lista_int, nv);
		current->c = numero(nc, lista_int, nv);
		current->d = numero(nd, lista_int, nv);
	}
	else if (tipo == 'D') {
		frv = sscanf(p, RESOLVE_DIODE(MAX_NOME), na, nb);
		// Verificando se os valores foram atribuidos e estavam dentro dos limites
		if (frv != 2 || na[MAX_NOME-1] != '\0' || nb[MAX_NOME-1] != '\0') {
			printf("Nao foi possivel ler elemento da %d linha. %d caracteres de tamanho maximo.\n", ne, MAX_NOME);
			return(EXCEEDED_MAX_NOME);
		}

		if (debug) printf("%s %s %s\n", current->nome, na, nb);
		current->a = numero(na, lista_int, nv);
		current->b = numero(nb, lista_int, nv);
	}
	else if (tipo == '*' || tipo == '.') {
		return(SPECIAL_LINE);
	}
	else {
		printf("Elemento desconhecido: %s\n",txt);
		return(UNKNOWN_ELEMENT);
	}

	return 0;
}
