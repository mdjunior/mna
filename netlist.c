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
	char tipo, na[MAX_NOME], nb[MAX_NOME], nc[MAX_NOME], nd[MAX_NOME];

	frv = sscanf(txt, RESOLVE_ONE_STRING(MAX_NOME), current->nome);
	// Verificando se o nome foi atribuido e estava dentro dos limites
	if (frv != 1 || current->nome[MAX_NOME-1] != '\0') {
		printf("Nao foi possivel ler nome do elemento da %d linha. %d caracteres de tamanho maximo.\n", ne, MAX_NOME);
		return(EXCEEDED_MAX_NOME);
	}

	// Identificando tipo de elemento
	txt[0] = toupper(txt[0]);
	tipo = txt[0];
	p = txt + strlen(current->nome);
	/* Inicio dos parametros */
	if (tipo == 'R' || tipo == 'I' || tipo == 'V' || tipo == 'C' || tipo == 'L') {
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
	else if (tipo == 'G' || tipo == 'E' || tipo == 'F' || tipo == 'H') {
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
	else {
		printf("Elemento desconhecido: %s\n",txt);
		return(UNKNOWN_ELEMENT);
	}

	return 0;
}
