/*
Programa de analise circuitos no dominio do tempo, contendo elementos lineares
 e nao lineares, usando analise nodal modificada e o metodo dos trapezios
 junto com o metodo de Newton-Raphson.

 Por Manoel Domingues Junior - mdjunior@ufrj.br

Baseado no programa de demonstracao de analise nodal modificada (por Antonio
 Carlos M. de Queiroz acmq@coe.ufrj.br) versao 1.0j disponivel em:
 http://www.coe.ufrj.br/~acmq/cursos/mna1.zip
*/

/*
Elementos aceitos e linhas do netlist:

Resistor:				R<nome> <no+> <no-> <resistencia>
Indutor:				L<nome> <no1> <no2> <indutancia>
Capacitor:				C<nome> <no1> <no2> <capacitancia>
Fonte I:				I<nome> <io+> <io-> <corrente>
Fonte V:				V<nome> <vo+> <vo-> <tensao>

VCVC:					E<nome> <vo+> <vo-> <vi+> <vi-> <ganho de tensao>
CCCS:					F<nome> <io+> <io-> <ii+> <ii-> <ganho de corrente>
VCCS:					G<nome> <io+> <io-> <vi+> <vi-> <transcondutancia>
CCVS:					H<nome> <vo+> <vo-> <ii+> <ii-> <transresistencia>

Amp. op.:				O<nome> <vo1> <vo2> <vi1> <vi2>

Transformador ideal:	K<nome> <noa> <nob> <noc> <nod> <n>

As fontes F e H tem o ramo de entrada em curto
O amplificador operacional ideal tem a saida suspensa
Os nos podem ser nomes
*/

#define versao "1.1a - 09/12/2017"
#include <stdio.h> // printf sscanf
#include <stdlib.h>
#include <string.h> // strcpy strstr strlen
#include <stdlib.h>
#include <ctype.h> // toupper 
#include <math.h> // fabs

#include "defines.h"
#include "utils.h"
#include "netlist.h"


typedef struct configuration { /* Parametros da analise */
	int PRINT_NETLIST;
	int PRINT_INTERMEDIATE_MATRIX;
	int PRINT_INTERNAL_NETLIST;
	int PRINT_FINAL_MATRIX;
	int PRINT_INTERNAL_VARIABLES;
	int PRINT_RESUME;
	int PRINT_SOLUTION;
	int PRINT_ANALYSIS_DATA;
	int PRINT_GMIN_DATA;
	int newton_raphson;
	int configured;
	int gmin_enabled;
	char tipo[MAX_NOME];
	double t_atual;
	double t_final;
	double t_passo;
	double passos_por_ponto;
	double gmin_value;
	double gmin_last_value;
	double gmin_min;
	double gmin_max;
	double gmin_factor;
	double gmin_factor_min;
} configuration;

device netlist[MAX_ELEM]; /* Netlist - elemento vem do netlist.h */

configuration config; /* Parametros de configuracao interna */

int
	ne, /* Elementos */
	nv, /* Variaveis */
	nn, /* Nos */
	i,j,k; // variaveis para iteracoes

char
/* Foram colocados limites nos formatos de leitura para alguma protecao
	 contra excesso de caracteres nestas variaveis */
	nomearquivo[MAX_FILENAME],
	tipo,
	na[MAX_NOME],
	nb[MAX_NOME],
	nc[MAX_NOME],
	nd[MAX_NOME],
	lista[MAX_NOS+1][MAX_NOME+2], // Lista com o numero do no e o nome (Tem que caber jx antes do nome)
	txt[MAX_LINHA+1]; // Variavel temporaria que sera usada para processar cada linha do netlist

FILE *arquivo;

int main(void)
{
	int frv; // functions return values

	clear_screen();
	printf("Programa demonstrativo de analise nodal modificada\n");
	printf("Originalmente por Antonio Carlos M. de Queiroz - acmq@coe.ufrj.br\n");
	printf("Adaptado por Manoel Domingues Junior - mdjunior@ufrj.br\n");
	printf("             João Ricardo da Silva Feitosa - diego.neves@poli.ufrj.br\n");
	printf("             Diego Neves Marinho - diego.neves@poli.ufrj.br\n");
	printf("Versao %s\n", versao);

	/* Leitura do netlist */
	ne=0;
	nv=0;
	strcpy(lista[0],"0");

	while (arquivo == 0) {
		printf("Nome do arquivo com o netlist (ex: mna.net): ");
		frv = scanf(RESOLVE_ONE_STRING(MAX_FILENAME), nomearquivo);
		if (frv != 1 || nomearquivo[MAX_FILENAME-1] != '\0') {
			printf("Nao foi possivel ler nome do arquivo. %d caracteres de tamanho maximo.\n", MAX_FILENAME);
		} else {
			arquivo = fopen(nomearquivo, "r");
			if (arquivo == 0) {
				printf("Arquivo %s inexistente.\n", nomearquivo);
			}
		}
	}

	// Definindo paramentros do GminStep (serão usados em alguns elementos do netlist)
	config.gmin_enabled = 0;
	config.gmin_value = 0;
	config.gmin_factor = GMIN_FACTOR;
	config.gmin_max = GMIN_MAX;
	config.gmin_min = GMIN_MIN;
	config.gmin_factor_min = GMIN_FACTOR_MIN;

	printf("Arquivo do netlist aberto com sucesso.\n");

	// Pegando a primeira linha do arquivo (se for numero, eh o numero de nos)
	fgets(txt, MAX_LINHA, arquivo);
	printf("Titulo (primeira linha): %s", txt);

	// Lendo as outras linhas
	while ( fgets(txt, MAX_LINHA, arquivo) ) {
		ne++;
		int result;
		result = process_device(txt, ne, &nv, lista, &netlist[ne], config.PRINT_NETLIST);
		if (result) {
			// Nao e elemento (decrementando)
			ne--;

			// Verificando se e comentario
			if (txt[0] == '*') { /* Comentario comeca com "*" */
				char *p;
				p = txt + strlen(&txt[0]);
				frv = sscanf(p, RESOLVE_ONE_STRING(MAX_LINHA), txt);
				printf("Comentario:\"%s\"\n", txt);

				// Verificando se os valores foram atribuidos e estavam dentro dos limites
				// Verificar porque está dando -1
				if (frv) {
					// Verificando se eh configuracao
					if ( strstr(txt, "PRINT_NETLIST") != NULL ){
						config.PRINT_NETLIST = 1;
					}
					if ( strstr(txt, "PRINT_INTERMEDIATE_MATRIX") != NULL ){
						config.PRINT_INTERMEDIATE_MATRIX = 1;
					}
					if ( strstr(txt, "PRINT_INTERNAL_NETLIST") != NULL ){
						config.PRINT_INTERNAL_NETLIST = 1;
					}
					if ( strstr(txt, "PRINT_FINAL_MATRIX") != NULL ){
						config.PRINT_FINAL_MATRIX = 1;
					}
					if ( strstr(txt, "PRINT_INTERNAL_VARIABLES") != NULL ){
						config.PRINT_INTERNAL_VARIABLES = 1;
					}
					if ( strstr(txt, "PRINT_RESUME") != NULL ){
						config.PRINT_RESUME = 1;
					}
					if ( strstr(txt, "PRINT_SOLUTION") != NULL ){
						config.PRINT_SOLUTION = 1;
					}
					if ( strstr(txt, "PRINT_ANALYSIS_DATA") != NULL ){
						config.PRINT_ANALYSIS_DATA = 1;
					}
					if ( strstr(txt, "PRINT_GMIN_DATA") != NULL ){
						config.PRINT_GMIN_DATA = 1;
					}
				}
			}

			// Verificacao de configuracao (refazer)
			if (txt[0] == '.') {
				char *p;
				p = &txt[6];
				char type[MAX_NOME];
				frv = sscanf(p,"%lg%lg%10s%lg", &(config.t_final), &(config.t_passo), config.tipo, &(config.passos_por_ponto));
				// Verificando
				if ( strstr(config.tipo, "TRAP") == NULL ){
					printf("Tipo de analise nao suportada (%s). Por favor, revise o arquivo.\n", config.tipo);
					fclose(arquivo);
					exit(INCORRECT_ANALYSIS_SETUP);
				}
				if ( config.t_final == 0 || config.t_final < config.t_passo ) {
					printf("Tempo de simulação insuficiente (%f). Por favor, revise o arquivo.\n", config.t_final);
					fclose(arquivo);
					exit(INCORRECT_ANALYSIS_SETUP);
				}
				if ( config.t_passo <= 0 ) {
					printf("Tempo de passo insuficiente (%f). Por favor, revise o arquivo.\n", config.t_passo);
					fclose(arquivo);
					exit(INCORRECT_ANALYSIS_SETUP);
				}
				if ( config.passos_por_ponto < 1 ) {
					printf("Passos por ponto na tabela invalido (%f). Por favor, revise o arquivo.\n", config.passos_por_ponto);
					fclose(arquivo);
					exit(INCORRECT_ANALYSIS_SETUP);
				}
				config.configured = 1;
			}
		}
		if (result != 0 && result != SPECIAL_LINE) {
			printf("Erro no processamento do netlist. Por favor, revise o arquivo.\n");
			fclose(arquivo);
			exit(result);
		}
	}
	fclose(arquivo);

	// Verificando se configuracao para analise esta ok
	if (config.configured != 1) {
		printf("Nao foi possivel ler carcteristicas da analise. Por favor, revise o arquivo.\n");
		exit(INCORRECT_ANALYSIS_SETUP);
	}

	/* Acrescenta variaveis de corrente acima dos nos, anotando no netlist */
	// Colocando configuracoes de analise de acordo com os componentes
	// nv = 0 - definido acima
	nn = nv;
	for (i=1; i <= ne; i++) {
		tipo = netlist[i].nome[0];
		// Fonte de tensao, Fonte de tensao controlada a tensao, Fonte de corrente controlada a corrente, amplificador ideal
		//   indutores e capacitores
		//   calculamos a corrente neles
		if (tipo == 'V' || tipo == 'E' || tipo == 'F' || tipo == 'O' || tipo == 'L' || tipo == 'C') {
			nv++;
			if (nv > MAX_NOS) {
				printf("As correntes extra excederam o numero de variaveis permitido (%d)\n", MAX_NOS);
				exit(EXCEEDED_MAX_NOS);
			}
			strcpy(lista[nv], "j"); /* Tem espaco para mais dois caracteres */
			strcat(lista[nv], netlist[i].nome);
			netlist[i].x=nv;
		}
		// Fonte de tensao controlada a corrente
		else if (tipo == 'H') {
			nv = nv+2;
			if (nv > MAX_NOS) {
				printf("As correntes extra excederam o numero de variaveis permitido (%d)\n", MAX_NOS);
				exit(EXCEEDED_MAX_NOS);
			}
			strcpy(lista[nv-1], "jx");
			strcat(lista[nv-1], netlist[i].nome);
			netlist[i].x = nv-1;
			strcpy(lista[nv], "jy");
			strcat(lista[nv], netlist[i].nome);
			netlist[i].y = nv;
		}

		if (tipo == 'D') {
			config.newton_raphson = 1;
		} else {
			config.newton_raphson = 0;
		}
	}

	if (config.PRINT_ANALYSIS_DATA) {
		printf("Parametros da analise no tempo: \n");
		printf("Tipo: %s\n", config.tipo);
		printf("Tempo Final: %f\n", config.t_final);
		printf("Passo: %f\n", config.t_passo);
		printf("Passos por ponto na tabela: %f\n", config.passos_por_ponto);
	}

	if (config.PRINT_INTERNAL_VARIABLES) {
		printf("Variaveis internas: \n");
		for (i=0; i <= nv; i++)
			printf("%d -> %s\n", i, lista[i]);
	}

	if (config.PRINT_INTERNAL_NETLIST) {
		printf("Netlist interno final\n");
		for (i=1; i <= ne; i++) {
			tipo = netlist[i].nome[0];
			if (tipo == 'R' || tipo == 'I' || tipo == 'V') {
				printf("%s %d %d %g\n", netlist[i].nome, netlist[i].a, netlist[i].b, netlist[i].valor);
			}
			else if (tipo == 'G' || tipo == 'E' || tipo == 'F' || tipo == 'H') {
				printf("%s %d %d %d %d %g\n", netlist[i].nome, netlist[i].a, netlist[i].b, netlist[i].c, netlist[i].d, netlist[i].valor);
			}
			else if (tipo == 'O') {
				printf("%s %d %d %d %d\n", netlist[i].nome, netlist[i].a, netlist[i].b, netlist[i].c, netlist[i].d);
			}
			if (tipo == 'V' || tipo == 'E' || tipo == 'F' || tipo == 'O')
				printf("Corrente jx: %d\n", netlist[i].x);
			else if (tipo == 'H')
				printf("Correntes jx e jy: %d, %d\n", netlist[i].x, netlist[i].y);
		}
	}

	if (config.PRINT_RESUME) {
		printf("O circuito tem %d nos, %d variaveis e %d elementos\n",nn,nv,ne);
	}

	// Preparando arquivo de resultado
	char arquivo_saida[MAX_FILENAME+15];
	frv = create_filename(nomearquivo,arquivo_saida);
	FILE *arquivo_saida_handler;
	arquivo_saida_handler = fopen(arquivo_saida, "w");
	if (arquivo_saida_handler == NULL) {
		printf("Nao foi possivel abrir o arquivo: %s\n", arquivo_saida);
		exit(OPEN_OUTPUT_FILE_ERROR);
	} else {
		printf("Dados sendo armazenados em: %s\n",arquivo_saida);
	}

	// Marcando inicio
	config.t_atual = 0;
	double solucao_anterior[MAX_NOS+2];
	// inicializando solucao_anterior
	for (j=0; j <= MAX_NOS+1; j++) {
		solucao_anterior[j] = 0;
	}

	int printed_title; // controlando impressao da legenda dos resultados

	// loop de tempo
	while (config.t_atual <= config.t_final) {

		// loop de passos
		int current_step;
		for (current_step = 0; current_step < config.passos_por_ponto; current_step++) {

			int randomizations = 0;
			int convergence = 1;
			int tries = 0;

			while (convergence) {
				// atingiu numero maximo de tentativas, vamos aleatorizar valores
				if ( (tries == MAX_TRIES) && (randomizations < MAX_RANDOMIZATIONS) ) {
					randomizations++;
					tries = 0;
					for (i = 1; i <= nv; i++) {
						solucao_anterior[i] = (rand()%100) / 100.0;
					}
				}
				else if (randomizations == MAX_RANDOMIZATIONS) {
					printf("Os calculos nao convergem com %i aleatorizacoes (%i tentativas cada).\n", MAX_RANDOMIZATIONS, MAX_TRIES);
					printf("Ativando Gmin step\n");
					config.gmin_enabled = 1;
					config.gmin_last_value = config.gmin_value;
					config.gmin_value = config.gmin_max;

					//exit(EXCEEDED_MAX_RANDOMIZATIONS);
				} else {
					tries++;
				}

				///////////////////////////////////////////////////////////////////////////
				/* Monta sistema nodal */
				///////////////////////////////////////////////////////////////////////////
				double solucao_atual[MAX_NOS+1][MAX_NOS+2];

				// inicializando solucao_atual
				for (i=0; i <= MAX_NOS; i++) {
					for (j=0; j <= MAX_NOS+1; j++) {
						solucao_atual[i][j] = 0;
					}
				}
				frv = build_nodal_system(ne, &nv, netlist, solucao_anterior, solucao_atual, config.t_passo, config.t_atual, config.passos_por_ponto, config.gmin_value, config.PRINT_INTERMEDIATE_MATRIX);
				if (frv) {
					printf("Não foi possível montar o sistema nodal.\n");
					exit(IMPOSSIBLE_BUILD_NODAL_SYSTEM);
				}

				///////////////////////////////////////////////////////////////////////////
				/* Resolve o sistema */
				///////////////////////////////////////////////////////////////////////////
				frv = resolversistema(solucao_atual, &nv);
				if (frv) {
					exit(frv);
				}
				// controle de convergencia padrao
				convergence = 0;

				// Se tem diodo, vamos verificar o tempo de Newton-Raphson
				if (config.newton_raphson) {
					for(i = 1; i <= nv; i++) {
						if( fabs(solucao_anterior[i] - solucao_atual[i][nv+1]) > MAX_ERROR ) {
							convergence = 1;
						}
					}
					tries++;
				}

				// Verificando erro antes de atualizar solução
                for (int cont = 1; cont <= nv; cont++ ) {
                    double error = fabs( solucao_atual[i][nv+1] - solucao_anterior[cont] );
                    if (error > MAX_ERROR) {
                        if ( config.gmin_enabled ) {
							if (config.PRINT_GMIN_DATA) {
								printf("Fator: %g, Fator Mínimo: %g\n", config.gmin_factor, config.gmin_factor_min);
								printf ("Entrou no MAX_ERROR. Tentativas: %i de %i, Aleatorizações: %i de %i\n", tries, MAX_TRIES,randomizations, MAX_RANDOMIZATIONS);
							}

                            if (config.gmin_factor < config.gmin_factor_min) {
                                printf("Os calculos nao convergiram com fator %g. Diminua o valor mínino no defines.h em GMIN_FACTOR_MIN.\n",config.gmin_factor);
                                exit(EXCEEDED_MAX_RANDOMIZATIONS);
                            }
                            else {
								config.gmin_value = config.gmin_last_value;
								config.gmin_factor = sqrt(config.gmin_factor);
								config.gmin_value = config.gmin_value/config.gmin_factor;
                            }
                        }
			        } else {
                        if (config.gmin_enabled) {
                            if(config.gmin_value <= config.gmin_min) {
                                convergence = 0;
                            } else {
                                config.gmin_last_value = config.gmin_value;
                                config.gmin_value = config.gmin_value/config.gmin_factor;
                                tries = 0;
                                randomizations = 0;
                            }
				        }
			        }
		        }

				// Atualizando ultima solucao
				for (i=1; i <= nv; i++) {
					solucao_anterior[i] = solucao_atual[i][nv+1];
				}

				if (config.PRINT_FINAL_MATRIX) {
					/* Opcional: Mostra o sistema resolvido */
					printf("Sistema resolvido:\n");
					for (i=1; i <= nv; i++) {
						for (j=1; j<=nv+1; j++)
							if (solucao_atual[i][j] != 0) printf("%+3.1f ", solucao_atual[i][j]);
							else printf(" ... ");
						printf("\n");
					}
				}

				// Salvando solucao no arquivo
				if (printed_title != 1) {
					fprintf(arquivo_saida_handler, "t\t");
					if (config.PRINT_SOLUTION) printf("t\t");
					strcpy(txt, "T");
					for (i=1; i <= nv; i++) {
						if (i == nn+1)
							strcpy(txt, "I");
						fprintf(arquivo_saida_handler, "%s\t", lista[i]);
						if (config.PRINT_SOLUTION) printf("%s\t", lista[i]);
					}
					fprintf(arquivo_saida_handler, "\n");
					if (config.PRINT_SOLUTION) printf("\n");
					printed_title = 1;
				}
				fprintf(arquivo_saida_handler, "%g\t", config.t_atual);
				if (config.PRINT_SOLUTION) printf("%g\t", config.t_atual);
				for (i=1; i <= nv; i++) {
					fprintf(arquivo_saida_handler, "%g\t", solucao_atual[i][nv+1]);
					if (config.PRINT_SOLUTION) printf("%g\t", solucao_atual[i][nv+1]);
				}
				fprintf(arquivo_saida_handler, "\n");
				if (config.PRINT_SOLUTION) printf("\n");

			}
		}
		config.t_atual += config.t_passo/config.passos_por_ponto;
	}
	fclose(arquivo_saida_handler);
	return 0;
}

