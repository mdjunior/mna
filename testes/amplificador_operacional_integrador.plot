set title  "Fonte senoidal"
set xlabel "tempo"
set ylabel "tensão"
set term png size 1024,768
set output "testes/amplificador_operacional_integrador.net.1481771695.png"
plot 'testes/amplificador_operacional_integrador.net.1481771695.tsv' using 1:4 title 'saida da fonte'
