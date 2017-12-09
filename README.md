# mna
Modified Nodal Analysis Simulator in Time Domain (only in portuguese)

A ideia do MNA é ser um simulador que analise circuitos no domínio do tempo, contendo elementos lineares e não lineares, 
usando análise nodal modificada e o método dos trapézios junto com o método de Newton-Raphson.

O programa suporta os seguintes elementos:
- Fontes de corrente e de tensão independentes
- Resistores, capacitores e indutores
- As quatro fontes controladas
- Amplificadores operacionais ideais, com 4 terminais
- Transformadores ideais
- Resistores lineares por partes
- Chave resistiva
- Diodos ideais (TODO)
- Chaves ideais (TODO)

O programa recebe um netlist descrevendo o circuito e os parâmetros da simulação, e inicializa a análise com o cálculo do ponto de operação em t=0.
Após o ponto de operação, é feita a análise usando o tempo total e o tamanho do passo (definido no netlist).
Os resultados são salvos em um arquivo no mesmo diretório do arquivo de entrada, com o mesmo nome do arquivo de entrada, acrescido do timestamp do horário do início da simulação e da extenso .tsv (ex: testes/RLC.net.1487022829.tsv para o arquivo de entrada testes/RLC.net).

A primeira linha do arquivo de resultado contem a descrição das colunas, por exemplo: t	1	2	3	jL0300	jC0103	jV0200.

Esse programa está em desenvolvimento e foi baseado no programa MNA1 do Professor Carlos Moreirão, disponível em: http://www.coe.ufrj.br/~acmq/cursos/mna1.zip

### Compilando o programa

O programa foi desenvolvido em ambiente *NIX. Para compilar ele, basta executar:

    $ g++ MNA_TD.C utils.c netlist.c 

### Executando o programa

Basta executar o arquivo:

    ./a.out

A seguinte tela será mostrada:

```
Programa demonstrativo de analise nodal modificada
Por Antonio Carlos M. de Queiroz - acmq@coe.ufrj.br
Versao 1.0j - 26/11/2015
Nome do arquivo com o netlist (ex: mna.net): 
```

Basta preencher o caminho do arquivo:

```
Programa demonstrativo de analise nodal modificada
Por Antonio Carlos M. de Queiroz - acmq@coe.ufrj.br
Versao 1.0j - 26/11/2015
Nome do arquivo com o netlist (ex: mna.net): testes/fonte_senoidal.net
```

Após o início do programa, ele produzirá as seguintes mensagens:

```
Arquivo do netlist aberto com sucesso.
Titulo (primeira linha): FONTE SENOIDAL
Dados sendo armazenados em: testes/fonte_senoidal.net.1512847996.tsv
```

### Comentários úteis

O programa consegue ler alguns tipos de comentários no netlist de maneira a fornecer uma saída mais detalhada de sua execução. A seguir listamos os comentários que podem ser utilizados (eles podem ser utilizados ao mesmo tempo):

- **PRINT_NETLIST**: Imprime o netlist durante a execução do programa

```
Comentario:"* PRINT_NETLIST
"
V0100 1 0 0 S
R0200 1 0 1
```

- **PRINT_INTERMEDIATE_MATRIX**: Imprime as metrizes intermediárias durante a execução do programa

```
Comentario:"* PRINT_INTERMEDIATE_MATRIX
"
Dados sendo armazenados em: testes/fonte_senoidal.net.1512848131.tsv
Sistema apos a estampa de V0100
 ... +1.0  ... 
-1.0  ...  ... 
Sistema apos a estampa de R0200
+1.0 +1.0  ... 
-1.0  ...  ... 
Sistema apos a estampa de V0100
 ... +1.0  ... 
-1.0  ...  ... 
Sistema apos a estampa de R0200
+1.0 +1.0  ... 
-1.0  ...  ...
[...]
```
- **PRINT_INTERNAL_NETLIST**: Imprime o netlist interno do programa

```
Comentario:"* PRINT_INTERNAL_NETLIST 
"
Netlist interno final
V0100 1 0 0
Corrente jx: 2
R0200 1 0 1
```

- **PRINT_FINAL_MATRIX**: Imprime o sistema resolvido para cada passo

```
Comentario:"* PRINT_FINAL_MATRIX 
"
Dados sendo armazenados em: testes/fonte_senoidal.net.1512848278.tsv
Sistema resolvido:
+1.0  ...  ... 
 ... +1.0  ... 
Sistema resolvido:
+1.0  ... +0.0 
 ... +1.0 -0.0 
Sistema resolvido:
+1.0  ... +0.0 
 ... +1.0 -0.0 
Sistema resolvido:
+1.0  ... +0.0 
 ... +1.0 -0.0 
[...]
```
- **PRINT_INTERNAL_VARIABLES**: Imprime as variáveis internas usadas no programa

```
Comentario:"* PRINT_INTERNAL_VARIABLES 
"
Variaveis internas: 
0 -> 0
1 -> 1
2 -> jV0100
```

- **PRINT_RESUME**: Imprime um resumo sobre o sistema de equações

```
Comentario:"* PRINT_RESUME
"
O circuito tem 1 nos, 2 variaveis e 2 elementos
Dados sendo armazenados em: testes/fonte_senoidal.net.1512847980.tsv
```
- **PRINT_SOLUTION**: Imprime a solução do sistema para cada passo

```
Comentario:"* PRINT_SOLUTION 
"
Dados sendo armazenados em: testes/fonte_senoidal.net.1512848386.tsv
t	1	jV0100	
0	0	-0	
0.001	0.00628314	-0.00628314	
0.002	0.012566	-0.012566	
0.003	0.0188484	-0.0188484	
0.004	0.0251301	-0.0251301	
0.005	0.0314108	-0.0314108	
0.006	0.0376902	-0.0376902	
0.007	0.0439681	-0.0439681	
0.008	0.0502443	-0.0502443	
0.009	0.0565185	-0.0565185	
0.01	0.0627905	-0.0627905	
0.011	0.06906	-0.06906
[...]
```
- **PRINT_ANALYSIS_DATA**: Imprime os dados de análise fornecidos pelo netlist

```
Comentario:"* PRINT_ANALYSIS_DATA 
"
Parametros da analise no tempo: 
Tipo: TRAP
Tempo Final: 7.000000
Passo: 0.001000
Passos por ponto na tabela: 1.000000
```

Os comentários mais recomendados são:
```
* PRINT_RESUME
* PRINT_ANALYSIS_DATA
* PRINT_NETLIST
* PRINT_INTERNAL_NETLIST
* PRINT_INTERNAL_VARIABLES
```

Na execução de um netlist, eles fornecem os seguintes dados:

```
Nome do arquivo com o netlist (ex: mna.net): testes/fonte_senoidal.net
Arquivo do netlist aberto com sucesso.
Titulo (primeira linha): FONTE SENOIDAL
Comentario:"* PRINT_RESUME
"
Comentario:"* PRINT_ANALYSIS_DATA
"
Comentario:"* PRINT_NETLIST
"
Comentario:"* PRINT_INTERNAL_NETLIST
"
Comentario:"* PRINT_INTERNAL_VARIABLES
"
V0100 1 0 0 S
R0200 1 0 1
Parametros da analise no tempo: 
Tipo: TRAP
Tempo Final: 7.000000
Passo: 0.001000
Passos por ponto na tabela: 1.000000
Variaveis internas: 
0 -> 0
1 -> 1
2 -> jV0100
Netlist interno final
V0100 1 0 0
Corrente jx: 2
R0200 1 0 1
O circuito tem 1 nos, 2 variaveis e 2 elementos
Dados sendo armazenados em: testes/fonte_senoidal.net.1512848640.tsv
```

### Gerando gráficos a partir dos resultados

É possível gerar gráficos a partir dos exemplos fornecidos usando o [gnuplot](). Para isso basta utilizar os arquivos `.plot` fornecidos, editando os nomes dos arquivos `.tsv` dentro deles.

A execução é conforme o exemplo a seguir:

```
$ gnuplot -c testes/fonte_senoidal.plot
```

Dependendo do número de pontos, a execução pode demorar alguns minutos. No exemplo acima, demora cerca de 35 segundos.

### Referências
- http://www.coe.ufrj.br/~acmq/cursos/mna1.zip
- http://www.coe.ufrj.br/~acmq/cursos/Trab22016.pdf
- http://www.coe.ufrj.br/~acmq/cursos/Trab22017.pdf
- https://pdfs.semanticscholar.org/9594/5bd9c050f9a67a20abf3f09c3ce62844db9f.pdf
