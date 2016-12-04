#include "defines.h"
#include "utils.h"
#include "netlist.h"

#include <stdio.h> // printf sscanf
#include <string.h> // strcpy strstr strlen
#include <ctype.h> // toupper 
#include <math.h> // M_PI fmod

int process_device(char txt[], int ne, int *nv, char lista_int[][MAX_NOME+2], device *current, int debug)
{
	int frv = 0;
	char *p;
	char tipo, na[MAX_NOME], nb[MAX_NOME], nc[MAX_NOME], nd[MAX_NOME], subtipo[MAX_NOME];

	na[MAX_NOME-1] = '\0';
	nb[MAX_NOME-1] = '\0';
	nc[MAX_NOME-1] = '\0';
	nd[MAX_NOME-1] = '\0';
	subtipo[MAX_NOME-1] = '\0';

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

				if (current->t_subida + current->t_ligada + current->t_descida > current->periodo ) {
					printf("Os tempos (subida+ligada+descida) nao correspondem ao periodo configurado (%g > %g s). Por favor, revise a configuracao da fonte.\n",
						current->t_subida + current->t_ligada + current->t_descida, current->periodo);
					return(INVALID_PULSE_SOURCE);
				}
			}

		}

		if (debug) printf("%s %s %s %g %c\n", current->nome, na, nb, current->valor, current->subtipo);
	}
	else if (tipo == 'G' || tipo == 'E' || tipo == 'F' || tipo == 'H' || tipo == '$' || tipo == 'K') {
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


int build_nodal_system(int ne, int *nv, device netlist[], double solucao_anterior[MAX_NOS+2], double nodal_matrix[][MAX_NOS+2], double t_passo, double t_atual, double passos_por_ponto, int debug)
{
	int i, j, k;
	char tipo;

	// Se estiver em t=0, apagando variavel nodal_matrix
	if (t_atual == 0) {
		for (i=0; i <= *nv; i++) {
			for (j=0; j <= *nv+1; j++)
				nodal_matrix[i][j] = 0;
		}
	}

	/* Zera sistema */
	for (i=0; i <= *nv; i++) {
		for (j=0; j <= *nv+1; j++)
			nodal_matrix[i][j] = 0;
	}

	/* Monta estampas */
	for (i=1; i <= ne; i++) {
		tipo = netlist[i].nome[0];
		if (tipo == 'R') {
			double g = 1/netlist[i].valor;
			nodal_matrix[netlist[i].a][netlist[i].a] += g;
			nodal_matrix[netlist[i].b][netlist[i].b] += g;
			nodal_matrix[netlist[i].a][netlist[i].b] -= g;
			nodal_matrix[netlist[i].b][netlist[i].a] -= g;
		}
		else if (tipo == 'C')
		{
			nodal_matrix[netlist[i].a][netlist[i].x] = 1;
			nodal_matrix[netlist[i].b][netlist[i].x] = -1;
			nodal_matrix[netlist[i].x][netlist[i].a] = 1;
			nodal_matrix[netlist[i].x][netlist[i].b] = -1;
			nodal_matrix[netlist[i].x][netlist[i].x] -= capacitor_resistance(&netlist[i], solucao_anterior, t_passo, t_atual, passos_por_ponto);
			nodal_matrix[netlist[i].x][*nv+1] += capacitor_current(&netlist[i], solucao_anterior, t_passo, t_atual, passos_por_ponto);
		}
		else if (tipo=='L')
		{
			nodal_matrix[netlist[i].a][netlist[i].x] = 1;
			nodal_matrix[netlist[i].b][netlist[i].x] = -1;
			nodal_matrix[netlist[i].x][netlist[i].a] = 1;
			nodal_matrix[netlist[i].x][netlist[i].b] = -1;
			nodal_matrix[netlist[i].x][netlist[i].x] = -inductor_resistance(&netlist[i], solucao_anterior, t_passo, t_atual, passos_por_ponto);
			nodal_matrix[netlist[i].x][*nv+1] = -inductor_voltage(&netlist[i], solucao_anterior, t_passo, t_atual, passos_por_ponto);
		}
		else if (tipo == 'G') {
			double g = netlist[i].valor;
			nodal_matrix[netlist[i].a][netlist[i].c] += g;
			nodal_matrix[netlist[i].b][netlist[i].d] += g;
			nodal_matrix[netlist[i].a][netlist[i].d] -= g;
			nodal_matrix[netlist[i].b][netlist[i].c] -= g;
		}
		else if (tipo == 'I') {
			double g = 0;

			if (netlist[i].subtipo == 'S') {
				g = source_sin(&netlist[i], solucao_anterior, t_passo, t_atual, passos_por_ponto);
			}
			else if (netlist[i].subtipo == 'P') {
				g = source_pulse(&netlist[i], solucao_anterior, t_passo, t_atual, passos_por_ponto);
			}
			else { // default eh fonte DC
				g = netlist[i].valor;
			}
			nodal_matrix[netlist[i].a][*nv+1] -= -g;
			nodal_matrix[netlist[i].b][*nv+1] += -g;
		}
		else if (tipo == 'V') {
			double g = 0;

			if (netlist[i].subtipo == 'S') {
				g = source_sin(&netlist[i], solucao_anterior, t_passo, t_atual, passos_por_ponto);
			}
			else if (netlist[i].subtipo == 'P') {
				g = source_pulse(&netlist[i], solucao_anterior, t_passo, t_atual, passos_por_ponto);
			}
			else { // default eh fonte DC
				g = netlist[i].valor;
			}
			nodal_matrix[netlist[i].a][netlist[i].x] += 1;
			nodal_matrix[netlist[i].b][netlist[i].x] -= 1;
			nodal_matrix[netlist[i].x][netlist[i].a] -= 1;
			nodal_matrix[netlist[i].x][netlist[i].b] += 1;
			nodal_matrix[netlist[i].x][*nv+1] = -g;
		}
		else if (tipo == 'E') {
			double g = netlist[i].valor;
			nodal_matrix[netlist[i].a][netlist[i].x] += 1;
			nodal_matrix[netlist[i].b][netlist[i].x] -= 1;
			nodal_matrix[netlist[i].x][netlist[i].a] -= 1;
			nodal_matrix[netlist[i].x][netlist[i].b] += 1;
			nodal_matrix[netlist[i].x][netlist[i].c] += g;
			nodal_matrix[netlist[i].x][netlist[i].d] -= g;
		}
		else if (tipo == 'F') {
			double g = netlist[i].valor;
			nodal_matrix[netlist[i].a][netlist[i].x] += g;
			nodal_matrix[netlist[i].b][netlist[i].x] -= g;
			nodal_matrix[netlist[i].c][netlist[i].x] += 1;
			nodal_matrix[netlist[i].d][netlist[i].x] -= 1;
			nodal_matrix[netlist[i].x][netlist[i].c] -= 1;
			nodal_matrix[netlist[i].x][netlist[i].d] += 1;
		}
		else if (tipo == 'H') {
			nodal_matrix[netlist[i].a][netlist[i].y] += 1;
			nodal_matrix[netlist[i].b][netlist[i].y] -= 1;
			nodal_matrix[netlist[i].c][netlist[i].x] += 1;
			nodal_matrix[netlist[i].d][netlist[i].x] -= 1;
			nodal_matrix[netlist[i].y][netlist[i].a] -= 1;
			nodal_matrix[netlist[i].y][netlist[i].b] += 1;
			nodal_matrix[netlist[i].x][netlist[i].c] -= 1;
			nodal_matrix[netlist[i].x][netlist[i].d] += 1;
			nodal_matrix[netlist[i].y][netlist[i].x] += netlist[i].valor;
		}
		else if (tipo == 'O') {
			nodal_matrix[netlist[i].a][netlist[i].x] += 1;
			nodal_matrix[netlist[i].b][netlist[i].x] -= 1;
			nodal_matrix[netlist[i].x][netlist[i].c] += 1;
			nodal_matrix[netlist[i].x][netlist[i].d] -= 1;
		}

		if (debug) {
			/* Opcional: Mostra o sistema apos a montagem da estampa */
			printf("Sistema apos a estampa de %s\n", netlist[i].nome);
			for (k=1; k <= *nv; k++) {
				for (j=1; j <= *nv+1; j++)
					if (nodal_matrix[k][j]!=0)
						printf("%+3.1f ", nodal_matrix[k][j]);
					else
						printf(" ... ");
				printf("\n");
			}
		}
	}
	return 0;
}

// Calcula resistencia do capacitor
double capacitor_resistance(device *elemento, double solucao_anterior[MAX_NOS+2], double t_passo, double t_atual, double passos_por_ponto)
{
	// Ponto de operacao deltaT/C
	if (t_atual == 0) {
		return ( t_passo/DIVISOR_DE_PASSO ) / elemento->valor;
	}

	// deltaT/2C
	return (t_passo/passos_por_ponto) / ( 2.0*(elemento->valor) );
}

// Calcula corrente do capacitor
double capacitor_current(device *elemento, double solucao_anterior[MAX_NOS+2], double t_passo, double t_atual, double passos_por_ponto)
{
	if (t_atual == 0) {
		return 0;
	}
	// corrente anterior * deltaT/2C + (tensao_anterior)
	return ( ( solucao_anterior[elemento->x] ) * (t_passo/passos_por_ponto) / (2.0*(elemento->valor)) )
		+ (solucao_anterior[elemento->a] - solucao_anterior[elemento->b]);
}

// Calcula resistencia do indutor
double inductor_resistance(device *elemento, double solucao_anterior[MAX_NOS+2], double t_passo, double t_atual, double passos_por_ponto)
{
	if (t_atual == 0) {
		// L/deltaT
		return ( elemento->valor / (t_passo/DIVISOR_DE_PASSO) );
	}

	// 2L/deltaT
	return ( (2.0 * elemento->valor) / (t_passo/DIVISOR_DE_PASSO) );
}

// Calcula tensao do indutor
double inductor_voltage(device *elemento, double solucao_anterior[MAX_NOS+2], double t_passo, double t_atual, double passos_por_ponto)
{
	// Curto em t=0
	//if (t_atual == 0) {
	//	return 0;
	//}

	// corrente anterior * (2L/deltaT) + tensao anterior
	// pag 96
	return ( ((2.0 * elemento->valor) / (t_passo/passos_por_ponto)) * solucao_anterior[elemento->x]
		+ (solucao_anterior[elemento->a] - solucao_anterior[elemento->b]) );
}

// Calcula saida da fonte senoidal
double source_sin(device *elemento, double solucao_anterior[MAX_NOS+2], double t_passo, double t_atual, double passos_por_ponto)
{
	double vSin = 0;
	if (t_atual < elemento->atraso) {
		vSin = elemento->nv_continuo;
	} else {
		// Verificando se os ciclos que já passaram (tempo de funcionamento * frequencia) sao menor que o configurados
		if ( (t_atual - elemento->atraso) * elemento->frequencia <= elemento->ciclos ) {
			vSin = elemento->nv_continuo
				+ ( elemento->amplitude * exp( -elemento->amortecimento * (t_atual - elemento->atraso) ))
				* sin( 2 * M_PI * elemento->frequencia * (t_atual - elemento->atraso) + elemento->defasagem * M_PI/180 );
		} else {
			vSin = elemento->nv_continuo;
		}
	}
	return vSin;
}

// Calcula saida da fonte pulse
double source_pulse(device *elemento, double solucao_anterior[MAX_NOS+2], double t_passo, double t_atual, double passos_por_ponto)
{
	// Tratando t_subida ou t_descida como zeros (usando tempo do passo)
	if (elemento->t_subida == 0.0) {
		elemento->t_subida = t_passo/passos_por_ponto;
	}
	if (elemento->t_descida == 0.0) {
		elemento->t_descida = t_passo/passos_por_ponto;
	}

	if (t_atual > elemento->atraso) {

		// Verifica se eh um ciclo de atividade
		if ( ( (t_atual - elemento->atraso) / elemento->periodo ) < elemento->ciclos ) {

			//  Obtem a posicao dentro do ciclo
			double t_position = fmod((t_atual - elemento->atraso), elemento->periodo);

			// Ciclo na subida
			if (t_position < elemento->t_subida) {
				return ( ((elemento->amplitude2 - elemento->amplitude1) * (t_position / elemento->t_subida))
					+ elemento->amplitude1 );
			}
			// Ciclo ligado
			if (t_position <= (elemento->t_subida + elemento->t_ligada)) {
				return (elemento->amplitude2);
			}
			// Ciclo na descida
			if (t_position < (elemento->t_subida + elemento->t_ligada + elemento->t_descida) )
				return (elemento->amplitude2
					- (elemento->amplitude2 - elemento->amplitude1) * (t_position - elemento->t_subida - elemento->t_ligada) / elemento->t_descida );
		}
	}
	// Nao eh um ciclo de atividade ou ja passou do tempo de descida
	return elemento->amplitude1;
}
