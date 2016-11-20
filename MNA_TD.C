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
#include <string.h> // strcpy strstr
#include <stdlib.h>
#include <ctype.h> // toupper 
#include <math.h>


#define MAX_LINHA 80
#define MAX_NOME 11
#define MAX_ELEM 50
#define MAX_NOS 50
#define MAX_FILENAME 50
#define TOLG 1e-9
#define DEBUG

// defines para sscanf
#define FORMAT_ONE_STRING(S) "%" #S "s"
#define RESOLVE_ONE_STRING(S) FORMAT_ONE_STRING(S)
#define FORMAT_TWO_TERMINALS(S) "%" #S "s%" #S "s%lg"
#define RESOLVE_TWO_TERMINALS(S) FORMAT_TWO_TERMINALS(S)
#define FORMAT_FOUR_TERMINALS(S) "%" #S "s%" #S "s%" #S "s%" #S "s%lg"
#define RESOLVE_FOUR_TERMINALS(S) FORMAT_FOUR_TERMINALS(S)
#define FORMAT_AMPOP(S) "%" #S "s%" #S "s%" #S "s%" #S "s"
#define RESOLVE_AMPOP(S) FORMAT_AMPOP(S)

#define EXCEEDED_MAX_ELEMENTS 1
#define EXCEEDED_MAX_NOME 2
#define EXCEEDED_MAX_NOS 3
#define UNKNOWN_ELEMENT 4

typedef struct configuration { /* Parametros da analise */
	int PRINT_NETLIST;
	int PRINT_INTERMEDIATE_MATRIX;
	int PRINT_INTERNAL_NETLIST;
	int PRINT_FINAL_MATRIX;
	int PRINT_INTERNAL_VARIABLES;
	int PRINT_RESUME;
} configuration;

typedef struct elemento { /* Elemento do netlist */
	char nome[MAX_NOME];
	double valor;
	int a,b,c,d,x,y;
} elemento;

elemento netlist[MAX_ELEM]; /* Netlist */
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
	txt[MAX_LINHA+1],
	*p;
FILE *arquivo;

double
	g,
	Yn[MAX_NOS+1][MAX_NOS+2];


/* Funcao que apaga a tela para compatibilidade Windows/Linus*/
void clear_screen()
{
	#ifdef WINDOWS
		std::system("cls");
	#else
		// Assume POSIX
		std::system ("clear");
	#endif
}

/* Resolucao de sistema de equacoes lineares.
	 Metodo de Gauss-Jordan com condensacao pivotal */
int resolversistema(void)
{
	int i,j,l, a;
	double t, p;

	for (i=1; i<=nv; i++) {
		t=0.0;
		a=i;
		for (l=i; l<=nv; l++) {
			if (fabs(Yn[l][i])>fabs(t)) {
	a=l;
	t=Yn[l][i];
			}
		}
		if (i!=a) {
			for (l=1; l<=nv+1; l++) {
	p=Yn[i][l];
	Yn[i][l]=Yn[a][l];
	Yn[a][l]=p;
			}
		}
		if (fabs(t)<TOLG) {
			printf("Sistema singular\n");
			return 1;
		}
		for (j=nv+1; j>0; j--) {  /* Basta j>i em vez de j>0 */
			Yn[i][j]/= t;
			p=Yn[i][j];
			if (p!=0)  /* Evita operacoes com zero */
				for (l=1; l<=nv; l++) {  
		if (l!=i)
			Yn[l][j]-=Yn[l][i]*p;
				}
		}
	}
	return 0;
}

/* Rotina que conta os nos e atribui numeros a eles */
int numero(char *nome)
{
	int i, achou;

	i=0;
	achou=0;
	while (!achou && i <= nv) {
		// Itera nos nos ate achar (quando acha, i tem o numero do no)
		if ( ! (achou = !strcmp(nome,lista[i]) ) ){
			i++;
		};
	}
	if ( !achou ) {
		// Verificou em todos os nos e nao achou
		if (nv == MAX_NOS) {
			printf("O programa so aceita ate %d nos.\n",nv);
			exit(EXCEEDED_MAX_NOS);
		}
		// Adicionando novo no
		nv++;
		strcpy(lista[nv],nome);
		return nv;
	}
	else {
		return i; /* no ja conhecido */
	}
}

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

		// Verificando numero maximo de elementos
		ne++; /* Nao usa o netlist[0] */
		if (ne > MAX_ELEM) {
			printf("O programa so aceita ate %d elementos\n", MAX_ELEM);
			exit(EXCEEDED_MAX_ELEMENTS);
		}

		// Colocando nome do elemento na estrutura de dados
		frv = sscanf(txt, RESOLVE_ONE_STRING(MAX_NOME), netlist[ne].nome);
		// Verificando se o nome foi atribuido e estava dentro dos limites
		if (frv != 1 || netlist[ne].nome[MAX_NOME-1] != '\0') {
			printf("Nao foi possivel ler nome do elemento da %d linha. %d caracteres de tamanho maximo.\n", ne, MAX_NOME);
			exit(EXCEEDED_MAX_NOME);
		}

		// Identificando tipo de elemento
		txt[0] = toupper(txt[0]);
		tipo = txt[0];
		p = txt + strlen(netlist[ne].nome);
		/* Inicio dos parametros */
		if (tipo == 'R' || tipo == 'I' || tipo == 'V' || tipo == 'C' || tipo == 'L') {
			frv = sscanf(p, RESOLVE_TWO_TERMINALS(MAX_NOME), na, nb, &netlist[ne].valor);
			// Verificando se os valores foram atribuidos e estavam dentro dos limites
			if (frv != 3 || na[MAX_NOME-1] != '\0' || nb[MAX_NOME-1] != '\0') {
				printf("Nao foi possivel ler elemento da %d linha. %d caracteres de tamanho maximo.\n", ne, MAX_NOME);
				exit(EXCEEDED_MAX_NOME);
			}

			if (config.PRINT_NETLIST) printf("%s %s %s %g\n", netlist[ne].nome, na, nb, netlist[ne].valor);
			netlist[ne].a = numero(na);
			netlist[ne].b = numero(nb);
		}
		else if (tipo == 'G' || tipo == 'E' || tipo == 'F' || tipo == 'H') {
			frv = sscanf(p, RESOLVE_FOUR_TERMINALS(MAX_NOME), na, nb, nc, nd, &netlist[ne].valor);
			// Verificando se os valores foram atribuidos e estavam dentro dos limites
			if (frv != 5 || na[MAX_NOME-1] != '\0' || nb[MAX_NOME-1] != '\0' || nc[MAX_NOME-1] != '\0' || nd[MAX_NOME-1] != '\0' ) {
				printf("Nao foi possivel ler elemento da %d linha. %d caracteres de tamanho maximo.\n", ne, MAX_NOME);
				exit(EXCEEDED_MAX_NOME);
			}

			if (config.PRINT_NETLIST) printf("%s %s %s %s %s %g\n", netlist[ne].nome, na, nb, nc, nd, netlist[ne].valor);
			netlist[ne].a = numero(na);
			netlist[ne].b = numero(nb);
			netlist[ne].c = numero(nc);
			netlist[ne].d = numero(nd);
		}
		else if (tipo == 'O') {
			frv = sscanf(p, RESOLVE_AMPOP(MAX_NOME), na, nb, nc, nd);
			// Verificando se os valores foram atribuidos e estavam dentro dos limites
			if (frv != 4 || na[MAX_NOME-1] != '\0' || nb[MAX_NOME-1] != '\0' || nc[MAX_NOME-1] != '\0' || nd[MAX_NOME-1] != '\0' ) {
				printf("Nao foi possivel ler elemento da %d linha. %d caracteres de tamanho maximo.\n", ne, MAX_NOME);
				exit(EXCEEDED_MAX_NOME);
			}

			if (config.PRINT_NETLIST) printf("%s %s %s %s %s\n", netlist[ne].nome, na, nb, nc, nd);
			netlist[ne].a = numero(na);
			netlist[ne].b = numero(nb);
			netlist[ne].c = numero(nc);
			netlist[ne].d = numero(nd);
		}
		else if (tipo == '*') { /* Comentario comeca com "*" */
			frv = sscanf(p, RESOLVE_ONE_STRING(MAX_LINHA), txt);
			printf("Comentario:\"%s\"\n", txt);
			ne--;

			// Verificando se os valores foram atribuidos e estavam dentro dos limites
			if (frv == 1) {
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
			}
		}
		else {
			printf("Elemento desconhecido: %s\n",txt);
			getchar();
			exit(UNKNOWN_ELEMENT);
		}
	}
	fclose(arquivo);

	/* Acrescenta variaveis de corrente acima dos nos, anotando no netlist */
	nn=nv;
	for (i=1; i<=ne; i++) {
		tipo=netlist[i].nome[0];
		if (tipo=='V' || tipo=='E' || tipo=='F' || tipo=='O') {
			nv++;
			if (nv>MAX_NOS) {
				printf("As correntes extra excederam o numero de variaveis permitido (%d)\n",MAX_NOS);
				exit(1);
			}
			strcpy(lista[nv],"j"); /* Tem espaco para mais dois caracteres */
			strcat(lista[nv],netlist[i].nome);
			netlist[i].x=nv;
		}
		else if (tipo=='H') {
			nv=nv+2;
			if (nv>MAX_NOS) {
				printf("As correntes extra excederam o numero de variaveis permitido (%d)\n",MAX_NOS);
				exit(1);
			}
			strcpy(lista[nv-1],"jx"); strcat(lista[nv-1],netlist[i].nome);
			netlist[i].x=nv-1;
			strcpy(lista[nv],"jy"); strcat(lista[nv],netlist[i].nome);
			netlist[i].y=nv;
		}
	}
	getchar();
	/* Lista tudo */
	printf("Variaveis internas: \n");
	for (i=0; i<=nv; i++)
		printf("%d -> %s\n",i,lista[i]);
	getchar();
	printf("Netlist interno final\n");
	for (i=1; i<=ne; i++) {
		tipo=netlist[i].nome[0];
		if (tipo=='R' || tipo=='I' || tipo=='V') {
			printf("%s %d %d %g\n",netlist[i].nome,netlist[i].a,netlist[i].b,netlist[i].valor);
		}
		else if (tipo=='G' || tipo=='E' || tipo=='F' || tipo=='H') {
			printf("%s %d %d %d %d %g\n",netlist[i].nome,netlist[i].a,netlist[i].b,netlist[i].c,netlist[i].d,netlist[i].valor);
		}
		else if (tipo=='O') {
			printf("%s %d %d %d %d\n",netlist[i].nome,netlist[i].a,netlist[i].b,netlist[i].c,netlist[i].d);
		}
		if (tipo=='V' || tipo=='E' || tipo=='F' || tipo=='O')
			printf("Corrente jx: %d\n",netlist[i].x);
		else if (tipo=='H')
			printf("Correntes jx e jy: %d, %d\n",netlist[i].x,netlist[i].y);
	}
	getchar();
	/* Monta o sistema nodal modificado */
	printf("O circuito tem %d nos, %d variaveis e %d elementos\n",nn,nv,ne);
	getchar();
	/* Zera sistema */
	for (i=0; i<=nv; i++) {
		for (j=0; j<=nv+1; j++)
			Yn[i][j]=0;
	}
	/* Monta estampas */
	for (i=1; i<=ne; i++) {
		tipo=netlist[i].nome[0];
		if (tipo=='R') {
			g=1/netlist[i].valor;
			Yn[netlist[i].a][netlist[i].a]+=g;
			Yn[netlist[i].b][netlist[i].b]+=g;
			Yn[netlist[i].a][netlist[i].b]-=g;
			Yn[netlist[i].b][netlist[i].a]-=g;
		}
		else if (tipo=='G') {
			g=netlist[i].valor;
			Yn[netlist[i].a][netlist[i].c]+=g;
			Yn[netlist[i].b][netlist[i].d]+=g;
			Yn[netlist[i].a][netlist[i].d]-=g;
			Yn[netlist[i].b][netlist[i].c]-=g;
		}
		else if (tipo=='I') {
			g=netlist[i].valor;
			Yn[netlist[i].a][nv+1]-=g;
			Yn[netlist[i].b][nv+1]+=g;
		}
		else if (tipo=='V') {
			Yn[netlist[i].a][netlist[i].x]+=1;
			Yn[netlist[i].b][netlist[i].x]-=1;
			Yn[netlist[i].x][netlist[i].a]-=1;
			Yn[netlist[i].x][netlist[i].b]+=1;
			Yn[netlist[i].x][nv+1]-=netlist[i].valor;
		}
		else if (tipo=='E') {
			g=netlist[i].valor;
			Yn[netlist[i].a][netlist[i].x]+=1;
			Yn[netlist[i].b][netlist[i].x]-=1;
			Yn[netlist[i].x][netlist[i].a]-=1;
			Yn[netlist[i].x][netlist[i].b]+=1;
			Yn[netlist[i].x][netlist[i].c]+=g;
			Yn[netlist[i].x][netlist[i].d]-=g;
		}
		else if (tipo=='F') {
			g=netlist[i].valor;
			Yn[netlist[i].a][netlist[i].x]+=g;
			Yn[netlist[i].b][netlist[i].x]-=g;
			Yn[netlist[i].c][netlist[i].x]+=1;
			Yn[netlist[i].d][netlist[i].x]-=1;
			Yn[netlist[i].x][netlist[i].c]-=1;
			Yn[netlist[i].x][netlist[i].d]+=1;
		}
		else if (tipo=='H') {
			g=netlist[i].valor;
			Yn[netlist[i].a][netlist[i].y]+=1;
			Yn[netlist[i].b][netlist[i].y]-=1;
			Yn[netlist[i].c][netlist[i].x]+=1;
			Yn[netlist[i].d][netlist[i].x]-=1;
			Yn[netlist[i].y][netlist[i].a]-=1;
			Yn[netlist[i].y][netlist[i].b]+=1;
			Yn[netlist[i].x][netlist[i].c]-=1;
			Yn[netlist[i].x][netlist[i].d]+=1;
			Yn[netlist[i].y][netlist[i].x]+=g;
		}
		else if (tipo=='O') {
			Yn[netlist[i].a][netlist[i].x]+=1;
			Yn[netlist[i].b][netlist[i].x]-=1;
			Yn[netlist[i].x][netlist[i].c]+=1;
			Yn[netlist[i].x][netlist[i].d]-=1;
		}
#ifdef DEBUG
		/* Opcional: Mostra o sistema apos a montagem da estampa */
		printf("Sistema apos a estampa de %s\n",netlist[i].nome);
		for (k=1; k<=nv; k++) {
			for (j=1; j<=nv+1; j++)
				if (Yn[k][j]!=0) printf("%+3.1f ",Yn[k][j]);
				else printf(" ... ");
			printf("\n");
		}
		getchar();
#endif
	}
	/* Resolve o sistema */
	if (resolversistema()) {
		getchar();
		exit;
	}
#ifdef DEBUG
	/* Opcional: Mostra o sistema resolvido */
	printf("Sistema resolvido:\n");
	for (i=1; i<=nv; i++) {
			for (j=1; j<=nv+1; j++)
				if (Yn[i][j]!=0) printf("%+3.1f ",Yn[i][j]);
				else printf(" ... ");
			printf("\n");
		}
	getchar();
#endif
	/* Mostra solucao */
	printf("Solucao:\n");
	strcpy(txt,"Tensao");
	for (i=1; i<=nv; i++) {
		if (i==nn+1) strcpy(txt,"Corrente");
		printf("%s %s: %g\n",txt,lista[i],Yn[i][nv+1]);
	}
	getchar();
	return 0;
}

