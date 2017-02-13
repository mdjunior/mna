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
- Diodos ideais (TODO)
- Chaves ideais (TODO)

O programa recebe um netlist descrevendo o circuito e os parâmetros da simulação, e inicializa a análise com o cálculo do ponto de operação em t=0.
Após o ponto de operação, é feita a análise usando o tempo total e o tamanho do passo (definido no netlist).
Os resultados são salvos em um arquivo no mesmo diretório do arquivo de entrada, com o mesmo nome do arquivo de entrada, 
acrescido do timestamp do horário do início da simulação e da extenso .tsv (ex: testes/RLC.net.1487022829.tsv para o arquivo de entrada testes/RLC.net).

A primeira linha do arquivo de resultado contem a descrição das colunas, por exemplo: t	1	2	3	jL0300	jC0103	jV0200.

Esse programa está em desenvolvimento e foi baseado no programa MNA1 do Professor Carlos Moreirão, disponível em: http://www.coe.ufrj.br/~acmq/cursos/mna1.zip

### Referências
- http://www.coe.ufrj.br/~acmq/cursos/mna1.zip
- http://www.coe.ufrj.br/~acmq/cursos/Trab22016.pdf
- https://pdfs.semanticscholar.org/9594/5bd9c050f9a67a20abf3f09c3ce62844db9f.pdf
