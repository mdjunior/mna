/*
Programa de demonstracao de analise nodal modificada
Por Antonio Carlos M. de Queiroz acmq@coe.ufrj.br
Versao 1.0 - 6/9/2000
Versao 1.0a - 8/9/2000 Ignora comentarios
Versao 1.0b - 15/9/2000 Aumentado Yn, retirado Js
Versao 1.0c - 19/2/2001 Mais comentarios
Versao 1.0d - 16/2/2003 Tratamento correto de nomes em minusculas
Versao 1.0e - 21/8/2008 Estampas sempre somam. Ignora a primeira linha
Versao 1.0f - 21/6/2009 Corrigidos limites de alocacao de Yn
Versao 1.0g - 15/10/2009 Le as linhas inteiras
Versao 1.0h - 18/6/2011 Estampas correspondendo a modelos
Versao 1.0i - 03/11/2013 Correcoes em *p e saida com sistema singular.
Versao 1.0j - 26/11/2015 Evita operacoes com zero.
*/

/*
Elementos aceitos e linhas do netlist:

Resistor:				R<nome> <no+> <no-> <resistencia>
Fonte I:				I<nome> <io+> <io-> <corrente>
Fonte V:				V<nome> <vo+> <vo-> <tensao>

VCVC:					E<nome> <vo+> <vo-> <vi+> <vi-> <ganho de tensao>
CCCS:					F<nome> <io+> <io-> <ii+> <ii-> <ganho de corrente>
VCCS:					G<nome> <io+> <io-> <vi+> <vi-> <transcondutancia>
CCVS:					H<nome> <vo+> <vo-> <ii+> <ii-> <transresistencia>

Amp. op.:				O<nome> <vo1> <vo2> <vi1> <vi2>

As fontes F e H tem o ramo de entrada em curto
O amplificador operacional ideal tem a saida suspensa
Os nos podem ser nomes
*/

#define versao "1.0j - 26/11/2015"
#include <stdio.h> // printf sscanf
#include <cstdlib>
#include <string.h> // strcpy strstr strlen
#include <stdlib.h>
#include <ctype.h> // toupper 

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
	int configured;
	double t_final;
	double t_passo;
	double passos_por_ponto;
} configuration;

device netlist[MAX_ELEM]; /* Netlist - elemento vem do netlist.h */

configuration config; /* Parametros de configuracao interna */

int
	ne, /* Elementos */
	nv, /* Variaveis */
	nn, /* Nos */
	i,j,k;

char
/* Foram colocados limites nos formatos de leitura para alguma protecao
	 contra excesso de caracteres nestas variaveis */
	nomearquivo[MAX_LINHA+1],
	tipo,
	na[MAX_NOME],
	nb[MAX_NOME],
	nc[MAX_NOME],
	nd[MAX_NOME],
	lista[MAX_NOS+1][MAX_NOME+2], // Lista com o numero do no e o nome (Tem que caber jx antes do nome)
	txt[MAX_LINHA+1], // Variavel temporaria que sera usada para processar cada linha do netlist
	*p;
FILE *arquivo;

double
	g, // Variavel intermediaria/temporaria para ser usada nas estampas
	Yn[MAX_NOS+1][MAX_NOS+2]; // Matriz onde o sistema fica com as estampas

int main(void)
{
	int frv; // functions return values

	clear_screen();
	printf("Programa demonstrativo de analise nodal modificada\n");
	printf("Por Antonio Carlos M. de Queiroz - acmq@coe.ufrj.br\n");
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
				}
			}

			// Verificacao de configuracao (refazer)
			if (txt[0] == '.') {
				// default values
				config.configured = 1;
				config.t_final = 0.5;
				config.t_passo = 0.0001;
				config.passos_por_ponto = 1;
			// 	char *p;
			// 	p = txt + strlen(&txt[5]);
			// 	char string[MAX_NOME];
			// 	frv = sscanf(p,"%lg%lg%10s%lg",&(config.t_final),&(config.t_passo),string,&(config.passos_por_ponto));
			// 	printf("Parametros da analise no tempo(%i): \n",frv);
			// 	printf("TEMPO FINAL: %f\n", config.t_final);
			// 	printf("PASSO: %f\n", config.t_passo);
			// 	printf("PASSOS POR PONTO NA TABELA: %f\n", config.passos_por_ponto);
			// 	config.configured = 1;
			}
		}
	}
	fclose(arquivo);

	// Verificando se configuracao para analise esta ok
	if (config.configured != 1) {
		printf("Nao foi possivel ler carcteristicas da analise. Por favor, revise o arquivo.\n");
		exit(INCORRECT_ANALYSIS_SETUP);
	}

	/* Acrescenta variaveis de corrente acima dos nos, anotando no netlist */
	// nv = 0 - definido acima
	nn = nv;
	for (i=1; i <= ne; i++) {
		tipo = netlist[i].nome[0];
		// Fonte de tensao, Fonte de tensao controlada a tensao, Fonte de corrente controlada a corrente e amplificador ideal
		//   calculamos a corrente neles
		if (tipo == 'V' || tipo == 'E' || tipo == 'F' || tipo == 'O') {
			nv++;
			if (nv > MAX_NOS) {
				printf("As correntes extra excederam o numero de variaveis permitido (%d)\n", MAX_NOS);
				exit(EXCEEDED_MAX_NOS);
			}
			strcpy(lista[nv], "j"); /* Tem espaco para mais dois caracteres */
			strcat(lista[nv], netlist[i].nome);
			netlist[i].x=nv;
		}
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
	}

	if (config.PRINT_ANALYSIS_DATA) {
		printf("Parametros da analise no tempo: \n");
		printf("TEMPO FINAL: %f\n", config.t_final);
		printf("PASSO: %f\n", config.t_passo);
		printf("PASSOS POR PONTO NA TABELA: %f\n", config.passos_por_ponto);
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

	/* Monta o sistema nodal modificado */
	/* Zera sistema */
	for (i=0; i<=nv; i++) {
		for (j=0; j<=nv+1; j++)
			Yn[i][j]=0;
	}
	/* Monta estampas */
	for (i=1; i <= ne; i++) {
		tipo = netlist[i].nome[0];
		if (tipo == 'R') {
			g = 1/netlist[i].valor;
			Yn[netlist[i].a][netlist[i].a] += g;
			Yn[netlist[i].b][netlist[i].b] += g;
			Yn[netlist[i].a][netlist[i].b] -= g;
			Yn[netlist[i].b][netlist[i].a] -= g;
		}
		else if (tipo == 'G') {
			g = netlist[i].valor;
			Yn[netlist[i].a][netlist[i].c] += g;
			Yn[netlist[i].b][netlist[i].d] += g;
			Yn[netlist[i].a][netlist[i].d] -= g;
			Yn[netlist[i].b][netlist[i].c] -= g;
		}
		else if (tipo == 'I') {
			g = netlist[i].valor;
			Yn[netlist[i].a][nv+1] -= g;
			Yn[netlist[i].b][nv+1] += g;
		}
		else if (tipo == 'V') {
			Yn[netlist[i].a][netlist[i].x] += 1;
			Yn[netlist[i].b][netlist[i].x] -= 1;
			Yn[netlist[i].x][netlist[i].a] -= 1;
			Yn[netlist[i].x][netlist[i].b] += 1;
			Yn[netlist[i].x][nv+1] -= netlist[i].valor;
		}
		else if (tipo == 'E') {
			g = netlist[i].valor;
			Yn[netlist[i].a][netlist[i].x] += 1;
			Yn[netlist[i].b][netlist[i].x] -= 1;
			Yn[netlist[i].x][netlist[i].a] -= 1;
			Yn[netlist[i].x][netlist[i].b] += 1;
			Yn[netlist[i].x][netlist[i].c] += g;
			Yn[netlist[i].x][netlist[i].d] -= g;
		}
		else if (tipo == 'F') {
			g = netlist[i].valor;
			Yn[netlist[i].a][netlist[i].x] += g;
			Yn[netlist[i].b][netlist[i].x] -= g;
			Yn[netlist[i].c][netlist[i].x] += 1;
			Yn[netlist[i].d][netlist[i].x] -= 1;
			Yn[netlist[i].x][netlist[i].c] -= 1;
			Yn[netlist[i].x][netlist[i].d] += 1;
		}
		else if (tipo == 'H') {
			g = netlist[i].valor;
			Yn[netlist[i].a][netlist[i].y] += 1;
			Yn[netlist[i].b][netlist[i].y] -= 1;
			Yn[netlist[i].c][netlist[i].x] += 1;
			Yn[netlist[i].d][netlist[i].x] -= 1;
			Yn[netlist[i].y][netlist[i].a] -= 1;
			Yn[netlist[i].y][netlist[i].b] += 1;
			Yn[netlist[i].x][netlist[i].c] -= 1;
			Yn[netlist[i].x][netlist[i].d] += 1;
			Yn[netlist[i].y][netlist[i].x] += g;
		}
		else if (tipo == 'O') {
			Yn[netlist[i].a][netlist[i].x] += 1;
			Yn[netlist[i].b][netlist[i].x] -= 1;
			Yn[netlist[i].x][netlist[i].c] += 1;
			Yn[netlist[i].x][netlist[i].d] -= 1;
		}


		if (config.PRINT_INTERMEDIATE_MATRIX) {
			/* Opcional: Mostra o sistema apos a montagem da estampa */
			printf("Sistema apos a estampa de %s\n", netlist[i].nome);
			for (k=1; k <= nv; k++) {
				for (j=1; j <= nv+1; j++)
					if (Yn[k][j]!=0) printf("%+3.1f ", Yn[k][j]);
					else printf(" ... ");
				printf("\n");
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////
	/* Resolve o sistema */
	///////////////////////////////////////////////////////////////////////////
	if (resolversistema(Yn, &nv)) {
		getchar();
		exit;
	}

	if (config.PRINT_FINAL_MATRIX) {
		/* Opcional: Mostra o sistema resolvido */
		printf("Sistema resolvido:\n");
		for (i=1; i <= nv; i++) {
			for (j=1; j<=nv+1; j++)
				if (Yn[i][j] != 0) printf("%+3.1f ", Yn[i][j]);
				else printf(" ... ");
			printf("\n");
		}
	}

	if (config.PRINT_SOLUTION) {
		/* Mostra solucao */
		printf("Solucao:\n");
		strcpy(txt, "Tensao");
		for (i=1; i <= nv; i++) {
			if (i == nn+1) strcpy(txt, "Corrente");
			printf("%s %s: %g\n", txt, lista[i], Yn[i][nv+1]);
		}
	}

	return 0;
}

