set title  "Fonte senoidal com resistor nao linear"
set xlabel "tempo"
set ylabel "tensão"
set term png size 1024,768
set output "testes/fonte_senoidal_com_resistornaolinear.net.1512861838.png"
plot 'testes/fonte_senoidal_com_resistornaolinear.net.1512861838.tsv' using 1:2 title 'saida fonte', 'testes/fonte_senoidal_com_resistornaolinear.net.1512861838.tsv' using 1:3 title 'tensao no resistor'
set ylabel "corrente"
set output "testes/fonte_senoidal_com_resistornaolinear.net.1512861838_current.png"
plot 'testes/fonte_senoidal_com_resistornaolinear.net.1512861838.tsv' using 1:4 title 'corrente'
